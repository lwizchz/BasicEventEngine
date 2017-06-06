/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME
#define _BEE_GAME 1

#include "game.hpp" // Include the engine headers

MetaResourceList* BEE::resource_list;
bool BEE::is_initialized = false;

BEE::BEE(int argc, char** argv, const std::list<ProgramFlags*>& new_flags, Room** new_first_room, GameOptions* new_options) :
	flags(),
	options(new_options),

	quit(false),
	is_ready(false),
	is_paused(false),

	first_room(nullptr),
	current_room(nullptr),

	width(DEFAULT_WINDOW_WIDTH),
	height(DEFAULT_WINDOW_HEIGHT),
	cursor(nullptr),

	renderer(new Renderer(this)),

	color(new RGBA()),
	font_default(nullptr),

	has_mouse(false),
	has_focus(false),

	tickstamp(0),
	new_tickstamp(0),
	fps_ticks(0),
	tick_delta(0),

	net(new NetworkData()),

	volume(1.0),

	fps_goal(DEFAULT_GAME_FPS),
	fps_max(300),
	//fps_max(fps_goal),
	fps_unfocused(fps_max/20),
	fps_count(0),
	frame_number(0),

	texture_before(nullptr),
	texture_after(nullptr),
	transition_type(bee::E_TRANSITION::NONE),
	transition_speed(1024.0/DEFAULT_GAME_FPS),
	transition_custom_func(nullptr),

	keystrings_keys(),
	keystrings_strings(),

	commandline_input(),
	commandline_current(0),

	recipients(),
	protected_tags({"engine", "console"}),
	messages(),
	messenger_output_level(bee::E_OUTPUT::NORMAL),

	console(nullptr),

	fps_stable(0)
{
	messenger_send({"engine", "init"}, bee::E_MESSAGE::INFO,
		"Initializing BasicEventEngine v" +
		std::to_string(BEE_VERSION_MAJOR) + "." + std::to_string(BEE_VERSION_MINOR) + "." + std::to_string(BEE_VERSION_RELEASE)
	);

	handle_flags(new_flags, true);

	if (options->should_assert) {
		if (!verify_assertions(argc, argv)) {
			throw std::string("Couldn't verify assertions\n");
		}
	}

	net_init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		throw "Couldn't init SDL: " + get_sdl_error() + "\n";
	}

	// Use the highest version of OpenGL available
	switch (options->renderer_type) {
		case bee::E_RENDERER::OPENGL4: {
			if (GL_VERSION_4_1) { // FIXME: Properly test for opengl support
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
				break;
			}
		}
		case bee::E_RENDERER::OPENGL3: {
			if (GL_VERSION_3_3) {
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
				break;
			}
		}
		case bee::E_RENDERER::SDL:
		default: {
			options->renderer_type = bee::E_RENDERER::SDL;
		}
	}

	int window_flags = SDL_WINDOW_OPENGL;
	if (options->is_fullscreen) {
		window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (options->is_borderless) {
		window_flags |= SDL_WINDOW_BORDERLESS;
	}
	if (options->is_resizable) {
		window_flags |= SDL_WINDOW_RESIZABLE;
	}
	if (options->is_maximized) {
		window_flags |= SDL_WINDOW_MAXIMIZED;
	}
	if (options->is_highdpi) {
		window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
	}
	if (options->is_visible) {
		window_flags |= SDL_WINDOW_SHOWN;
	}
	renderer->window = SDL_CreateWindow("BasicEventEngine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
	if (renderer->window == nullptr) {
		throw "Couldn't create SDL window: " + get_sdl_error() + "\n";
	}
	if (options->is_minimized) {
		SDL_MinimizeWindow(renderer->window);
	}

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	keystate = SDL_GetKeyboardState(nullptr);
	keystrings_populate();

	try {
		if (options->renderer_type != bee::E_RENDERER::SDL) {
			renderer->opengl_init();
		} else { // if not OpenGL, init an SDL renderer
			renderer->sdl_renderer_init();
		}
	} catch (const std::string& e) {
		messenger_send({"engine", "init"}, bee::E_MESSAGE::ERROR, e);
		handle_messages();
		std::rethrow_exception(std::current_exception());
	}

	int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		throw std::string("Couldn't init SDL_image: ") + IMG_GetError() + "\n";
	}

	if (TTF_Init() == -1) {
		throw std::string("Couldn't init SDL_ttf: ") + TTF_GetError() + "\n";
	}

	Mix_SetSoundFonts(""); // This will disable MIDI but fixes a small error when not including a soundfont
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
		throw std::string("Couldn't init SDL_mixer: ") + Mix_GetError() + "\n";
	}
	Mix_ChannelFinished(sound_finished);
	Mix_AllocateChannels(128); // Probably overkill

	if (!is_initialized) {
		resource_list = new MetaResourceList(this);
		if (init_resources()) {
			throw std::string("Couldn't init resources\n");
		}
	}

	texture_before = new Sprite();
	texture_after = new Sprite();

	font_default = new Font("font_default", "liberation_mono.ttf", 16, false);
		font_default->load();

	handle_messages();
	console = new Console(); // Initialize the default console commands
	console_init_commands();
	handle_messages();

	if (*new_first_room != nullptr) {
		if (change_room(*new_first_room, false)) {
			throw std::string("Couldn't load first room\n");
		}
	}

	handle_flags(new_flags, false);
}
BEE::~BEE() {
	close();
}
int BEE::handle_flags(const std::list<ProgramFlags*>& new_flags, bool pre_init) {
	flags = new_flags;

	if (flags.empty()) {
		return 0;
	}

	int l = 0;
	struct option* long_options = new struct option[flags.size()+1];
	std::string optstring = "";
	for (auto& f : flags) {
		if (f->shortopt > 0) {
			optstring += f->shortopt;
			if (f->has_arg == optional_argument) {
				optstring += "::";
			} else if (f->has_arg == required_argument) {
				optstring += ":";
			}
		}

		long_options[l].name = f->longopt.c_str();
		long_options[l].has_arg = f->has_arg;
		long_options[l].flag = nullptr;
		long_options[l].val = f->shortopt;
		l++;
	}
	long_options[l++] = {0, 0, 0, 0};

	optind = 1;
	int index = -1;
	int c = -1;
	int amount = 0;
	while ((c = getopt_long(argc, argv, optstring.c_str(), long_options, &index)) != -1) {
		for (auto& f : flags) {
			if (((c != 0)&&(c == f->shortopt))||((c == 0)&&(strcmp(long_options[index].name, f->longopt.c_str()) == 0))) {
				if (f->pre_init == pre_init) {
					if (f->func != nullptr) {
						if ((f->has_arg != no_argument)&&(optarg)) {
							f->func(this, optarg);
						} else {
							f->func(this, (char*)nullptr);
						}
					}
					amount++;
				}
				break;
			}
		}
	}

	delete[] long_options;

	return amount;
}
int BEE::loop() {
	tickstamp = get_ticks();
	fps_ticks = get_ticks();

	messenger_send({"engine"}, bee::E_MESSAGE::START, "gameloop");
	handle_messages();

	// Register the logging system
	messenger_register_protected("cmdlog", {"engine", "commandline"}, true, [] (BEE* g, std::shared_ptr<MessageContents> msg) {
		std::cout << "[" << msg->descr << "]\n";
	});

	while (!quit) {
		if (current_room == nullptr) {
			messenger_send({"engine"}, bee::E_MESSAGE::ERROR, "Aborted event loop because current_room == nullptr");
			return 1;
		}

		try {
			current_room->step_begin();
			current_room->check_alarms();

			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT: {
						quit = true;
						break;
					}
					case SDL_WINDOWEVENT: {
						switch (event.window.event) {
							case SDL_WINDOWEVENT_SHOWN: {
								render_set_camera(nullptr);
								renderer->render();
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_HIDDEN: {
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_EXPOSED: {
								renderer->render();
								break;
							}
							case SDL_WINDOWEVENT_MOVED: {
								break;
							}
							case SDL_WINDOWEVENT_RESIZED: {
								width = event.window.data1;
								height = event.window.data2;
								render_set_camera(nullptr);
								renderer->render();
								break;
							}
							case SDL_WINDOWEVENT_SIZE_CHANGED: {
								renderer->render();
								break;
							}
							case SDL_WINDOWEVENT_MINIMIZED: {
								options->is_minimized = true;
								has_mouse = false;
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_MAXIMIZED: {
								options->is_minimized = false;
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_RESTORED: {
								options->is_minimized = false;
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_ENTER: {
								has_mouse = true;
								break;
							}
							case SDL_WINDOWEVENT_LEAVE: {
								has_mouse = false;
								break;
							}
							case SDL_WINDOWEVENT_FOCUS_GAINED: {
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_FOCUS_LOST: {
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_CLOSE: {
								SDL_Event qe;
								qe.type = SDL_QUIT;
								SDL_PushEvent(&qe);
								break;
							}
							#if SDL_VERSION_ATLEAST(2, 0, 5)
								case SDL_WINDOWEVENT_TAKE_FOCUS:
									SDL_SetWindowInputFocus(renderer->window);
									break;
								case SDL_WINDOWEVENT_HIT_TEST:
									break;
							#endif
							default: {
								messenger_send({"engine"}, bee::E_MESSAGE::WARNING, "Unknown window event: " + bee_itos(event.window.event));
								break;
							}
						}
						current_room->window(&event);
						break;
					}

					case SDL_KEYDOWN: {
						console_handle_input(&event);

						if (event.key.repeat == 0) {
							current_room->keyboard_press(&event);
						}
						current_room->keyboard_input(&event);

						break;
					}
					case SDL_KEYUP: {
						current_room->keyboard_release(&event);
						break;
					}
					case SDL_MOUSEMOTION:
					case SDL_MOUSEWHEEL: {
						current_room->mouse_input(&event);
						break;
					}
					case SDL_MOUSEBUTTONDOWN: {
						current_room->mouse_press(&event);
						break;
					}
					case SDL_MOUSEBUTTONUP: {
						current_room->mouse_release(&event);
						break;
					}
					case SDL_CONTROLLERAXISMOTION: {
						current_room->controller_axis(&event);
						break;
					}
					case SDL_CONTROLLERBUTTONDOWN: {
						current_room->controller_press(&event);
						break;
					}
					case SDL_CONTROLLERBUTTONUP: {
						current_room->controller_release(&event);
						break;
					}
					case SDL_CONTROLLERDEVICEADDED:
					case SDL_CONTROLLERDEVICEREMOVED:
					case SDL_CONTROLLERDEVICEREMAPPED: {
						current_room->controller_modify(&event);
						break;
					}

					#if SDL_VERSION_ATLEAST(2, 0, 4)
						case SDL_AUDIODEVICEADDED:
						case SDL_AUDIODEVICEREMOVED:
					#endif
					case SDL_DOLLARGESTURE:
					case SDL_DOLLARRECORD:
					case SDL_DROPFILE:
					case SDL_FINGERMOTION:
					case SDL_FINGERDOWN:
					case SDL_FINGERUP:
					case SDL_JOYAXISMOTION:
					case SDL_JOYBALLMOTION:
					case SDL_JOYHATMOTION:
					case SDL_JOYBUTTONDOWN:
					case SDL_JOYBUTTONUP:
					case SDL_JOYDEVICEADDED:
					case SDL_JOYDEVICEREMOVED:
					case SDL_MULTIGESTURE:
					case SDL_SYSWMEVENT:
					case SDL_TEXTEDITING:
					case SDL_TEXTINPUT:
					case SDL_USEREVENT: {
						// For events which we don't care about or currently support
						break;
					}
					default:
						messenger_send({"engine"}, bee::E_MESSAGE::WARNING, "Unknown event type: " + bee_itos(event.type));
						break;
				}
			}
			if (bee_has_commandline_input()) {
				commandline_input.push_back("");
				std::getline(std::cin, commandline_input[commandline_current]);
				if (commandline_input[commandline_current] == "") {
					commandline_input.pop_back();
				} else {
					messenger_send({"engine", "commandline"}, bee::E_MESSAGE::INFO, commandline_input[commandline_current]);
					current_room->commandline_input(commandline_input[commandline_current++]);
				}
			}

			current_room->step_mid();
			current_room->check_paths();
			current_room->outside_room();
			current_room->intersect_boundary();
			current_room->collision();

			current_room->step_end();
			current_room->draw();
			current_room->animation_end();
			current_room->destroy();

			net_handle_events();
			handle_messages();

			fps_count++;
			frame_number++;
			new_tickstamp = get_ticks();
			unsigned int fps_desired = fps_max;
			if (!has_focus) {
				fps_desired = fps_unfocused;
			}
			if (new_tickstamp - tickstamp < 1000/fps_desired) {
				if ((!options->is_vsync_enabled)||(!has_focus)) {
					SDL_Delay((1000/fps_desired) - (new_tickstamp - tickstamp));
				}
			}
			update_delta();

			if (tickstamp - fps_ticks >= 1000) {
				fps_stable = fps_count / ((tickstamp-fps_ticks)/1000);
				fps_count = 0;
				fps_ticks = tickstamp;
			}

			if (options->single_run) {
				current_room->draw();
				save_screenshot("single_run.bmp");
				quit = true;
			}
		} catch (int e) {
			switch (e) {
				case -1: { // Resource error
					messenger_send({"engine"}, bee::E_MESSAGE::ERROR, "Aborting due to resource error");
					return 2;
				}
				case 0: { // Jump to loop end, e.g. change room
					break;
				}
				case 1: { // Quit
					quit = true;
					break;
				}
				case 2: { // Restart game
					change_room(first_room, false);
					break;
				}
				case 3: { // Restart room
					change_room(current_room, false);
					break;
				}
				default: {
					messenger_send({"engine"}, bee::E_MESSAGE::ERROR, "Unknown error code: " + bee_itos(e));
					return e;
				}
			}
		} catch (...) {
			close();

			bee_commandline_color(9);
			std::cout << "Unknown error\n";
			bee_commandline_color_reset();
			std::flush(std::cout);

			std::rethrow_exception(std::current_exception());
		}
	}
	messenger_send({"engine"}, bee::E_MESSAGE::END, "gameloop");

	current_room->room_end();
	current_room->game_end();
	change_room(nullptr, false);
	is_ready = false;

	return 0;
}
int BEE::close() {
	handle_messages();

	Mix_AllocateChannels(0);

	if (is_initialized) {
		free_media();
		close_resources();
	}

	if (font_default != nullptr) {
		delete font_default;
		font_default = nullptr;
	}

	if (texture_before != nullptr) {
		texture_before->free();
		delete texture_before;
		texture_before = nullptr;
	}
	if (texture_after != nullptr) {
		texture_after->free();
		delete texture_after;
		texture_after = nullptr;
	}

	if (color != nullptr) {
		delete color;
		color = nullptr;
	}

	if (renderer != nullptr) {
		delete renderer;
		renderer = nullptr;
	}
	if (cursor != nullptr) {
		SDL_FreeCursor(cursor);
		cursor = nullptr;
	}

	if (console != nullptr) {
		delete console;
		console = nullptr;
	}

	Mix_CloseAudio();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	if (net != nullptr) {
		net_close();
		delete net;
		net = nullptr;
	}

	if (resource_list != nullptr) {
		resource_list->reset(nullptr);
		delete resource_list;
		resource_list = nullptr;
	}

	BEE::free_standard_flags();

	return 0;
}

#ifndef _WIN32
#include "core/resources.cpp"
#endif // _WIN32

int BEE::update_delta() {
	tick_delta = get_ticks() - tickstamp;
	tickstamp = get_ticks();
	return 0;
}
Uint32 BEE::get_ticks() const {
	return SDL_GetTicks();
}
Uint32 BEE::get_seconds() const {
	return get_ticks()/1000;
}
Uint32 BEE::get_frame() const {
	return frame_number;
}
double BEE::get_delta() const {
	return max<double>((double)get_tick_delta(), 1.0) / 1000.0;
}
Uint32 BEE::get_tick_delta() const {
	return tick_delta;
}
unsigned int BEE::get_fps_goal() const {
	return fps_goal;
}

int BEE::restart_game() const {
	throw 2;
	return 0;
}
int BEE::end_game() const {
	throw 1;
	return 0;
}

#include "core/sidp.cpp" // Include the structs which are used in other classes
#include "render/rgba.cpp"

#endif // _BEE_GAME
