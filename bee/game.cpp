/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME
#define _BEE_GAME 1

#include "game.hpp"

MetaResourceList* BEE::resource_list;
bool BEE::is_initialized = false;

BEE::BEE(int new_argc, char** new_argv, const std::list<ProgramFlags*>& new_flags, Room** new_first_room, GameOptions* new_options) {
	argc = new_argc;
	argv = new_argv;
	is_ready = false;

	has_mouse = false;
	has_focus = false;

	fps_goal = DEFAULT_GAME_FPS;
	fps_max = 300;
	//fps_max = fps_goal;
	fps_unfocused = fps_max/20;
	fps_count = 0;
	fps_stable = 0;

	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;

	options = new_options;
	handle_flags(new_flags, true);

	if (options->should_assert) {
		if (!verify_assertions(argc, argv)) {
			throw std::string("Couldn't verify assertions\n");
		}
	}

	net = new NetworkData();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
		throw std::string("Couldn't init SDL: ") + SDL_GetError() + "\n";
	}

	// Use the highest version of OpenGL available
	switch (options->renderer_type) {
		case BEE_RENDERER_OPENGL4: {
			if (GL_VERSION_4_1) {
				options->renderer_type = BEE_RENDERER_OPENGL4;
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
				//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // Currently the compatibility profile must be used because there are unknown uses of the deprecated functions in the code base
				break;
			}
		}
		case BEE_RENDERER_OPENGL3: {
			if (GL_VERSION_3_3) {
				options->renderer_type = BEE_RENDERER_OPENGL3;
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
				//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // Currently the compatibility profile must be used because there are unknown uses of the deprecated functions in the code base
				break;
			}
		}
		case BEE_RENDERER_SDL:
		default: {
			options->renderer_type = BEE_RENDERER_SDL;
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
	window = SDL_CreateWindow("Basic Event Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
	if (window == nullptr) {
		throw std::string("Couldn't create SDL window: ") + SDL_GetError() + "\n";
	}
	if (options->is_minimized) {
		SDL_MinimizeWindow(window);
	}

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	keystate = SDL_GetKeyboardState(nullptr);
	console_input.clear();

	color = new RGBA();
	if (options->renderer_type != BEE_RENDERER_SDL) {
		opengl_init();
	} else { // if not OpenGL, init an SDL renderer
		sdl_renderer_init();
	}

	int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		throw std::string("Couldn't init SDL_image: ") + IMG_GetError() + "\n";
	}

	if (TTF_Init() == -1) {
		throw std::string("Couldn't init SDL_ttf: ") + TTF_GetError() + "\n";
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
		throw std::string("Couldn't init SDL_mixer: ") + Mix_GetError() + "\n";
	}
	Mix_ChannelFinished(sound_finished);
	Mix_AllocateChannels(128); // Probably overkill

	if (options->is_network_enabled) {
		if (!network_init()) {
			net->is_initialized = true;
		}
	}

	if (!is_initialized) {
		resource_list = new MetaResourceList(this);
		if (init_resources()) {
			throw std::string("Couldn't init resources\n");
		}
	}

	texture_before = new Sprite();
	texture_after = new Sprite();

	quit = false;
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
	while (!quit) {
		if (current_room == nullptr) {
			std::cerr << "Aborted event loop because current_room == nullptr\n";
			return 1;
		}

		try {
			current_room->step_begin();
			current_room->check_alarms();

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT: {
						quit = true;
						break;
					}
					case SDL_WINDOWEVENT: {
						switch (event.window.event) {
							case SDL_WINDOWEVENT_SHOWN: {
								render();
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_HIDDEN: {
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_EXPOSED: {
								render();
								break;
							}
							case SDL_WINDOWEVENT_MOVED: {
								break;
							}
							case SDL_WINDOWEVENT_RESIZED: {
								width = event.window.data1;
								height = event.window.data2;
								render();
								break;
							}
							case SDL_WINDOWEVENT_SIZE_CHANGED: {
								render();
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
							default: {
								std::cerr << "Unknown window event: " << event.window.event << "\n";
								break;
							}
						}
						current_room->window(&event);
						break;
					}

					case SDL_KEYDOWN: {
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
						std::cerr << "Unknown event type: " << event.type << "\n";
						break;
				}
			}
			if (bee_has_console_input()) {
				console_input.push_back("");
				std::cin >> console_input[console_line];
				current_room->console_input(console_input[console_line++]);
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
					std::cerr << "Aborting due to resource error\n";
					return 2;
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
				case 4: { // Jump to loop end, e.g. change room
					break;
				}
				default: {
					std::cerr << "Unknown error: " << e << "\n";
					return e;
				}
			}
		}
	}

	current_room->room_end();
	current_room->game_end();
	change_room(nullptr, false);
	is_ready = false;

	return 0;
}
int BEE::close() {
	if (is_initialized) {
		free_media();
		close_resources();
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

	delete color;

	if (context != nullptr) {
		opengl_close();
	}
	if (renderer != nullptr) {
		sdl_renderer_close();
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	if (options->is_network_enabled) {
		if (!network_close()) {
			net->is_initialized = false;
		}
	}

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	if (resource_list != nullptr) {
		resource_list->reset(nullptr);
		delete resource_list;
		resource_list = nullptr;
	}

	return 0;
}

#ifndef _WIN32
#include "game/resources.cpp"
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
	return SDL_GetTicks()/1000;
}
Uint32 BEE::get_frame() const {
	return frame_number;
}
double BEE::get_delta() const {
	return get_fps_goal()/1000.0*tick_delta;
}
unsigned int BEE::get_fps_goal() const {
	return fps_goal;
}

BEE::GameOptions BEE::get_options() const {
	return *options;
}
int BEE::set_options(const GameOptions& new_options) {
	if (options->is_fullscreen != new_options.is_fullscreen) {
		// Change fullscreen state
		options->is_fullscreen = new_options.is_fullscreen;

		bool b = 0;
		if (options->is_fullscreen) {
			b = SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		SDL_SetWindowFullscreen(window, b);
	}
	if (options->is_borderless != new_options.is_borderless) {
		// Change borderless state
		options->is_borderless = new_options.is_borderless;

		SDL_bool b = SDL_TRUE;
		if (options->is_borderless) {
			b = SDL_FALSE;
		}
		SDL_SetWindowBordered(window, b);
	}
	if (options->is_resizable != new_options.is_resizable) {
		// Change resizable state
		options->is_resizable = new_options.is_resizable;

		if (options->is_resizable) {
			SDL_SetWindowMaximumSize(window, 16384, 16384);
			SDL_SetWindowMinimumSize(window, 128, 128);
		} else {
			SDL_SetWindowMaximumSize(window, get_width(), get_height());
			SDL_SetWindowMinimumSize(window, get_width(), get_height());
		}
	}
	if (options->is_maximized != new_options.is_maximized) {
		// Change maximized state
		options->is_maximized = new_options.is_maximized;

		if (options->is_maximized) {
			SDL_MaximizeWindow(window);
		} else {
			if (options->is_resizable) {
				SDL_RestoreWindow(window);
			} else {
				SDL_MinimizeWindow(window);
			}
		}
	}
	if (options->is_highdpi != new_options.is_highdpi) {
		// Change highdpi state
		options->is_highdpi = new_options.is_highdpi;

		// I currently have no way to test highdpi functionality
	}
	if (options->is_visible != new_options.is_visible) {
		// Change visible state
		options->is_visible = new_options.is_visible;

		if (options->is_visible) {
			SDL_ShowWindow(window);
		} else {
			SDL_HideWindow(window);
		}
	}
	if (options->is_minimized != new_options.is_minimized) {
		// Change minimization state
		options->is_minimized = new_options.is_minimized;

		if (options->is_minimized) {
			SDL_MinimizeWindow(window);
		} else {
			SDL_RestoreWindow(window);
		}
	}
	if (options->renderer_type != new_options.renderer_type) {
		// Change OpenGL state
		options->renderer_type = new_options.renderer_type;

		if (options->renderer_type != BEE_RENDERER_SDL) { // Enter OpenGL mode
			sdl_renderer_close();
			render_reset();
		} else { // Enter SDL rendering mode
			opengl_close();
			render_reset();
		}
	}
	if (options->is_vsync_enabled != new_options.is_vsync_enabled) {
		// Change vsync state
		options->is_vsync_enabled = new_options.is_vsync_enabled;

		render_reset();
	}
	if (options->is_network_enabled != new_options.is_network_enabled) {
		// Change networking state
		options->is_network_enabled = new_options.is_network_enabled;

		if (options->is_network_enabled) {
			if (!network_init()) {
				net->is_initialized = true;
			}
		} else {
			network_close();
			net->is_initialized = false;
		}
	}
	if (options->is_debug_enabled != new_options.is_debug_enabled) {
		// Change debugging state
		options->is_debug_enabled = new_options.is_debug_enabled;
	}

	return 0;
}

int BEE::opengl_init() {
	context = SDL_GL_CreateContext(window);
	if (context == nullptr) {
		throw std::string("Couldn't create OpenGL context: ") + SDL_GetError() + "\n";
	}

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK) {
		throw std::string("Couldn't initialize GLEW: ") + std::string((const char*)glewGetErrorString(glew_error)) + "\n";
	}

	if (options->is_vsync_enabled) {
		SDL_GL_SetSwapInterval(1);
	} else {
		SDL_GL_SetSwapInterval(0);
	}

	program = glCreateProgram();

	// Compile vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vs = file_get_contents("resources/vertex_shader.glsl");
	vs = opengl_prepend_version(vs);
	const GLchar* vertex_shader_source[] = {vs.c_str()};
	glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	GLint is_shader_compiled = GL_FALSE;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		std::cerr << get_shader_error(vertex_shader);
		glDeleteShader(vertex_shader);
		throw std::string("Couldn't compile OpenGL vertex shader: ") + bee_itos(vertex_shader) + "\n";
	}
	glAttachShader(program, vertex_shader);

	// Compile geometry shader
	GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	std::string gs = file_get_contents("resources/geometry_shader.glsl");
	gs = opengl_prepend_version(gs);
	const GLchar* geometry_shader_source[] = {gs.c_str()};
	glShaderSource(geometry_shader, 1, geometry_shader_source, nullptr);
	glCompileShader(geometry_shader);
	is_shader_compiled = GL_FALSE;
	glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		std::cerr << get_shader_error(geometry_shader);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		throw std::string("Couldn't compile OpenGL geometry shader: ") + bee_itos(geometry_shader) + "\n";
	}
	glAttachShader(program, geometry_shader);

	// Compile fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fs = file_get_contents("resources/fragment_shader.glsl");
	fs = opengl_prepend_version(fs);
	const GLchar* fragment_shader_source[] = {fs.c_str()};
	glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	is_shader_compiled = GL_FALSE;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		std::cerr << get_shader_error(fragment_shader);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't compile OpenGL fragment shader: ") + bee_itos(fragment_shader) + "\n";
	}
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	GLint is_program_linked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &is_program_linked);
	if (is_program_linked != GL_TRUE) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't link OpenGL program: ") + bee_itos(program) + "\n";
	}

	vertex_location = glGetAttribLocation(program, "LVertexPos2D");
	if (vertex_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'LVertexPos2D' in the vertex shader\n");
	}
	fragment_location = glGetAttribLocation(program, "LTexCoord");
	if (fragment_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'LTexCoord' in the vertex shader\n");
	}

	projection_location = glGetUniformLocation(program, "projection");
	if (projection_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'projection' in the vertex shader\n");
	}
	view_location = glGetUniformLocation(program, "view");
	if (view_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'view' in the vertex shader\n");
	}
	model_location = glGetUniformLocation(program, "model");
	if (model_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'model' in the vertex shader\n");
	}
	port_location = glGetUniformLocation(program, "port");
	if (port_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'port' in the vertex shader\n");
	}

	rotation_location = glGetUniformLocation(program, "rotation");
	if (rotation_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'rotation' in the fragment shader\n");
	}

	texture_location = glGetUniformLocation(program, "LTexture");
	if (texture_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'LTexture' in the fragment shader\n");
	}
	colorize_location = glGetUniformLocation(program, "colorize");
	if (colorize_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'colorize' in the fragment shader\n");
	}
	primitive_location = glGetUniformLocation(program, "is_primitive");
	if (primitive_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'is_primitive' in the fragment shader\n");
	}
	flip_location = glGetUniformLocation(program, "flip");
	if (flip_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'flip' in the fragment shader\n");
	}

	is_lightable_location = glGetUniformLocation(program, "is_lightable");
	if (is_lightable_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'is_lightable' in the fragment shader\n");
	}
	light_amount_location = glGetUniformLocation(program, "light_amount");
	if (light_amount_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'light_amount' in the fragment shader\n");
	}
	for (int i=0; i<BEE_MAX_LIGHTS; i++) {
		lighting_location[i].type = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].type").c_str());
		lighting_location[i].position = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].position").c_str());
		lighting_location[i].direction = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].direction").c_str());
		lighting_location[i].attenuation = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].attenuation").c_str());
		lighting_location[i].color = glGetUniformLocation(program, std::string("lighting[" + bee_itos(i) + "].color").c_str());
	}

	lightable_amount_location = glGetUniformLocation(program, "lightable_amount");
	if (lightable_amount_location == -1) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(geometry_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'lightable_amount' in the fragment shader\n");
	}
	for (int i=0; i<BEE_MAX_LIGHTABLES; i++) {
		lightable_location[i].position = glGetUniformLocation(program, std::string("lightables[" + bee_itos(i) + "].position").c_str());
		lightable_location[i].vertex_amount = glGetUniformLocation(program, std::string("lightables[" + bee_itos(i) + "].vertex_amount").c_str());
		for (int e=0; e<BEE_MAX_MASK_VERTICES; e++) {
			lightable_location[i].mask[e] = glGetUniformLocation(program, std::string("lightables[" + bee_itos(i) + "].mask[" + bee_itos(e) + "]").c_str());
		}
	}

	draw_set_color({255, 255, 255, 255});
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);

	if (options->renderer_type == BEE_RENDERER_OPENGL4) {
		std::cout << "Now rendering with OpenGL 4.1\n";
	} else if (options->renderer_type == BEE_RENDERER_OPENGL3) {
		std::cout << "Now rendering with OpenGL 3.3\n";
	}

	int va = 0, vi = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &va);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &vi);
	std::cerr << "GLversion: " << va << "." << vi << "\n";

	return 0;
}
int BEE::opengl_close() {
	if (program != 0) {
		glDeleteProgram(program);
		program = 0;
	}

	if (context != nullptr) {
		SDL_GL_DeleteContext(context);
		context = nullptr;
	}

	return 0;
}
std::string BEE::opengl_prepend_version(const std::string& shader) {
	switch (options->renderer_type) {
		case BEE_RENDERER_OPENGL4: {
			if (GL_VERSION_4_1) {
				options->renderer_type = BEE_RENDERER_OPENGL4;
				return "#version 410\n" + shader;
			}
		}
		case BEE_RENDERER_OPENGL3: {
			if (GL_VERSION_3_3) {
				options->renderer_type = BEE_RENDERER_OPENGL3;
				return "#version 330\n" + shader;
			}
		}
		case BEE_RENDERER_SDL:
		default: {
			options->renderer_type = BEE_RENDERER_SDL;
			return shader;
		}
	}
}
int BEE::sdl_renderer_init() {
	int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if (options->is_vsync_enabled) {
		renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (renderer == nullptr) {
		throw std::string("Couldn't create SDL renderer: ") + SDL_GetError() + "\n";
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	std::cout << "Now rendering with SDL2\n";

	return 0;
}
int BEE::sdl_renderer_close() {
	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	return 0;
}

int BEE::render_clear() {
	draw_set_color(*color);
	if (options->renderer_type != BEE_RENDERER_SDL) {
		if (target > 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, target);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);
	} else {
		SDL_RenderClear(renderer);
	}
	return 0;
}
int BEE::render() const {
	if (options->renderer_type != BEE_RENDERER_SDL) {
		glUseProgram(0);
		SDL_GL_SwapWindow(window);
	} else {
		SDL_RenderPresent(renderer);
	}
	return 0;
}
int BEE::render_reset() {
	if (options->renderer_type != BEE_RENDERER_SDL) {
		opengl_close();
		opengl_init();
	} else {
		sdl_renderer_close();
		sdl_renderer_init();
	}

	// Reload sprite and background textures
	Sprite* s;
	for (int i=0; i<resource_list->sprites.get_amount(); i++) {
		if (get_sprite(i) != nullptr) {
			s = get_sprite(i);
			if (s->get_is_loaded()) {
				s->free();
				s->load();
			}
		}
	}
	Background* b;
	for (int i=0; i<resource_list->backgrounds.get_amount(); i++) {
		if (get_background(i) != nullptr) {
			b = get_background(i);
			if (b->get_is_loaded()) {
				b->free();
				b->load();
			}
		}
	}

	return 0;
}

int BEE::restart_game() const {
	throw 2;
	return 0;
}
int BEE::end_game() const {
	throw 1;
	return 0;
}

#include "game/info.cpp"
#include "game/room.cpp"
#include "game/transition.cpp"
#include "game/display.cpp"
#include "game/window.cpp"
#include "game/input.cpp"
#include "game/draw.cpp"
#include "game/network.cpp"

#endif // _BEE_GAME
