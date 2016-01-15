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

BEE::BEE(int new_argc, char** new_argv, Room** new_first_room, GameOptions* new_options) {
	argc = new_argc;
	argv = new_argv;
	is_ready = false;
	options = new_options;

	is_minimized = false;
	is_fullscreen = false;
	has_mouse = false;
	has_focus = false;

	fps_goal = DEFAULT_GAME_FPS;
	//fps_max = 300;
	fps_max = fps_goal;
	fps_unfocused = fps_max/20;
	fps_count = 0;
	fps_stable = 0;

	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;

	net = new NetworkData();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
		throw std::string("Couldn't init SDL: ") + SDL_GetError() + "\n";
	}

	// Use "modern" OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	int window_flags = SDL_WINDOW_OPENGL;
	if (options->is_fullscreen) {
		/*if (platform == 0) {
		       //if (options->is_resizable) {
				window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; // Changes the window dimensions
			//} else {
			//	window_flags |= SDL_WINDOW_FULLSCREEN; // Changes the video mode
			//}
		} else if (platform == 1) { // _WINDOWS
			window_flags |= SDL_WINDOW_BORDERLESS;
		}*/
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
	if (window == NULL) {
		throw std::string("Couldn't create SDL window: ") + SDL_GetError() + "\n";
	}

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	keystate = SDL_GetKeyboardState(NULL);

	color = new RGBA();
	if (options->is_opengl) {
		opengl_init();
	} else { // if not OpenGL, init an SDL renderer
		renderer_init();
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
	if (*new_first_room != NULL) {
		if (change_room(*new_first_room, false)) {
			throw std::string("Couldn't load first room\n");
		}
	}
}
BEE::~BEE() {
	close();
}
int BEE::loop() {
	if (current_room == NULL) {
		throw std::string("Failed to start event loop, i.e. current_room == NULL\n");
	}

	tickstamp = get_ticks();
	fps_ticks = get_ticks();
	while (!quit) {
		if (current_room == NULL) {
			throw std::string("Aborted event loop because current_room == NULL\n");
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
								//render_reset();
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
								is_minimized = true;
								has_mouse = false;
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_MAXIMIZED: {
								is_minimized = false;
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_RESTORED: {
								is_minimized = false;
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
								std::cerr << "other,";
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
					default:
						//std::cerr << "Unknown event type: " << event.type << "\n";
						break;
				}
			}
			current_room->keyboard();
			current_room->mouse();

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
			tickstamp = get_ticks();

			if (tickstamp - fps_ticks >= 1000) {
				fps_stable = fps_count / ((tickstamp-fps_ticks)/1000);
				fps_count = 0;
				fps_ticks = tickstamp;
			}
		} catch (int e) {
			switch (e) {
				case -1: { // Resource error
					throw std::string("Aborting due to resource error\n");
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
			}
		}
	}

	current_room->room_end();
	current_room->game_end();
	is_ready = false;
	current_room->reset_properties();

	return 0;
}
int BEE::close() {
	if (is_initialized) {
		free_media();
		close_resources();
	}

	if (texture_before != NULL) {
		texture_before->free();
		delete texture_before;
		texture_before = NULL;
	}
	if (texture_after != NULL) {
		texture_after->free();
		delete texture_after;
		texture_after = NULL;
	}

	delete color;

	if (context != NULL) {
		opengl_close();
	}
	if (renderer != NULL) {
		renderer_close();
	}
	if (window != NULL) {
		SDL_DestroyWindow(window);
		window = NULL;
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

	if (resource_list != NULL) {
		resource_list->reset(NULL);
		delete resource_list;
		resource_list = NULL;
	}

	return 0;
}

#ifndef _WINDOWS
#include "game/resources.cpp"
#endif // _WINDOWS

Uint32 BEE::get_ticks() const {
	return SDL_GetTicks();
}
Uint32 BEE::get_seconds() const {
	return SDL_GetTicks()/1000;
}
Uint32 BEE::get_frame() const {
	return frame_number;
}

BEE::GameOptions BEE::get_options() const {
	return *options;
}
int BEE::set_options(const GameOptions& new_options) {
	if ((options->is_fullscreen ^ new_options.is_fullscreen) == 1) {
		// Change fullscreen state
		options->is_fullscreen = new_options.is_fullscreen;

		bool b = 0;
		if (options->is_fullscreen) {
			b = SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		SDL_SetWindowFullscreen(window, b);
	}
	if ((options->is_opengl ^ new_options.is_opengl) == 1) {
		// Change OpenGL state
		options->is_opengl = new_options.is_opengl;

		if (options->is_opengl) { // Enter OpenGL mode
			renderer_close();
			render_reset();
		} else { // Enter SDL rendering mode
			opengl_close();
			render_reset();
		}
	}
	if ((options->is_borderless ^ new_options.is_borderless) == 1) {
		// Change borderless state
		options->is_borderless = new_options.is_borderless;

		SDL_bool b = SDL_TRUE;
		if (options->is_borderless) {
			b = SDL_FALSE;
		}
		SDL_SetWindowBordered(window, b);
	}
	if ((options->is_resizable ^ new_options.is_resizable) == 1) {
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
	if ((options->is_maximized ^ new_options.is_maximized) == 1) {
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
	if ((options->is_highdpi ^ new_options.is_highdpi) == 1) {
		// Change highdpi state
		options->is_highdpi = new_options.is_highdpi;

		// I currently have no way to test highdpi functionality
	}
	if ((options->is_visible ^ new_options.is_visible) == 1) {
		// Change visible state
		options->is_visible = new_options.is_visible;

		if (options->is_visible) {
			SDL_ShowWindow(window);
		} else {
			SDL_HideWindow(window);
		}
	}
	if ((options->is_vsync_enabled ^ new_options.is_vsync_enabled) == 1) {
		// Change vsync state
		options->is_vsync_enabled = new_options.is_vsync_enabled;

		render_reset();
	}
	if ((options->is_network_enabled ^ new_options.is_network_enabled) == 1) {
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
	if ((options->is_debug_enabled ^ new_options.is_debug_enabled) == 1) {
		// Change debugging state
		options->is_debug_enabled = new_options.is_debug_enabled;
	}

	return 0;
}

int BEE::opengl_init() {
	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
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
	}

	program = glCreateProgram();

	// Compile vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vs = file_get_contents("resources/vertex_shader.glsl");
	const GLchar* vertex_shader_source[] = {vs.c_str()};
	glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	GLint is_shader_compiled = GL_FALSE;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		std::cerr << get_shader_error(vertex_shader);
		glDeleteShader(vertex_shader);
		throw std::string("Couldn't compile OpenGL vertex shader: ") + std::to_string(vertex_shader) + "\n";
	}
	glAttachShader(program, vertex_shader);

	// Compile fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fs = file_get_contents("resources/fragment_shader.glsl");
	const GLchar* fragment_shader_source[] = {fs.c_str()};
	glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	is_shader_compiled = GL_FALSE;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_shader_compiled);
	if (is_shader_compiled != GL_TRUE) {
		std::cerr << get_shader_error(fragment_shader);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't compile OpenGL fragment shader: ") + std::to_string(fragment_shader) + "\n";
	}
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	GLint is_program_linked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &is_program_linked);
	if (is_program_linked != GL_TRUE) {
		std::cerr << get_program_error(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't link OpenGL program: ") + std::to_string(program) + "\n";
	}

	vertex_location = glGetAttribLocation(program, "LVertexPos2D");
	if (vertex_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'LVertexPos2D' in the vertex shader\n");
	}
	fragment_location = glGetAttribLocation(program, "LTexCoord");
	if (fragment_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'LTexCoord' in the fragment shader\n");
	}

	projection_location = glGetUniformLocation(program, "projection");
	if (projection_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'projection' in the vertex shader\n");
	}
	view_location = glGetUniformLocation(program, "view");
	if (view_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'view' in the vertex shader\n");
	}
	model_location = glGetUniformLocation(program, "model");
	if (model_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'model' in the vertex shader\n");
	}
	port_location = glGetUniformLocation(program, "port");
	if (port_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'port' in the vertex shader\n");
	}

	texture_location = glGetUniformLocation(program, "LTexture");
	if (texture_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'LTexture' in the fragment shader\n");
	}
	colorize_location = glGetUniformLocation(program, "colorize");
	if (colorize_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'colorize' in the fragment shader\n");
	}
	flip_location = glGetUniformLocation(program, "flip");
	if (flip_location == -1) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::string("Couldn't get location of 'flip' in the fragment shader\n");
	}

	draw_set_color({255, 255, 255, 255});
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return 0;
}
int BEE::opengl_close() {
	if (program != 0) {
		glDeleteProgram(program);
		program = 0;
	}

	if (context != NULL) {
		SDL_GL_DeleteContext(context);
		context = NULL;
	}

	return 0;
}
int BEE::renderer_init() {
	int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	if (options->is_vsync_enabled) {
		renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (renderer == NULL) {
		throw std::string("Couldn't create SDL renderer: ") + SDL_GetError() + "\n";
	}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return 0;
}
int BEE::renderer_close() {
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}

	return 0;
}

int BEE::render_clear() {
	draw_set_color(*color);
	if (options->is_opengl) {
		glm::mat4 projection = glm::ortho(0.0f, (float)get_width(), (float)get_height(), 0.0f, 0.0f, 10.0f);
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

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
	if (options->is_opengl) {
		glUseProgram(0);
		SDL_GL_SwapWindow(window);
	} else {
		SDL_RenderPresent(renderer);
	}
	return 0;
}
int BEE::render_reset() {
	if (options->is_opengl) {
		opengl_close();
		opengl_init();
	} else {
		renderer_close();
		renderer_init();
	}

	// Reload sprite and background textures
	for (int i=0; i<resource_list->sprites.get_amount(); i++) {
		if (get_sprite(i) != NULL) {
			Sprite* s = get_sprite(i);
			if (s->get_is_loaded()) {
				s->free();
				s->load();
			}
		}
	}
	for (int i=0; i<resource_list->backgrounds.get_amount(); i++) {
		if (get_background(i) != NULL) {
			Background* s = get_background(i);
			if (s->get_is_loaded()) {
				s->free();
				s->load();
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

#ifndef _WINDOWS
#include "game/room.cpp"
#include "game/transition.cpp"
#include "game/display.cpp"
#include "game/window.cpp"
#include "game/input.cpp"
#include "game/draw.cpp"
#include "game/network.cpp"
#endif // _WINDOWS

#endif // _BEE_GAME
