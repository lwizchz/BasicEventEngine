/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
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

	fps_max = 300;
	fps_goal = 60;
	fps_count = 0;
	fps_stable = 0;

	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
		throw std::string("Couldn't init SDL: ") + SDL_GetError() + "\n";
	}

	int window_flags = SDL_WINDOW_SHOWN;
	if (options->is_fullscreen) {
		if (options->is_resizable) {
			window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; // Changes the window dimensions
		} else {
			window_flags |= SDL_WINDOW_FULLSCREEN; // Changes the video mode
		}
	}
	if (options->is_opengl) {
		window_flags |= SDL_WINDOW_OPENGL;
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
	window = SDL_CreateWindow("Easy Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
	if (window == NULL) {
		throw std::string("Couldn't create SDL window: ") + SDL_GetError() + "\n";
	}

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	if (options->is_opengl) {
		// Do OpenGL stuff
	} else {
		int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if (options->is_vsync_enabled) {
			renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
		}
		renderer = SDL_CreateRenderer(window, -1, renderer_flags);
		if (renderer == NULL) {
			throw std::string("Couldn't create SDL renderer: ") + SDL_GetError() + "\n";
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
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

	if (!is_initialized) {
		resource_list = new MetaResourceList();
		if (init_resources()) {
			throw std::string("Couldn't init resources\n");
		}
	}

	if (set_engine_pointer()) {
		throw std::string("Couldn't set engine pointer\n");
	}

	texture_before = new Sprite();
	texture_before->game = this;
	texture_after = new Sprite();
	texture_after->game = this;

	quit = false;
	if (*new_first_room != NULL) {
		if (change_room(*new_first_room)) {
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

	tickstamp = SDL_GetTicks();
	fps_ticks = SDL_GetTicks();
	while (!quit) {
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
							case SDL_WINDOWEVENT_RESIZED:
							case SDL_WINDOWEVENT_SIZE_CHANGED: {
								width = event.window.data1;
								height = event.window.data2;
								render();
								break;
							}
							case SDL_WINDOWEVENT_EXPOSED: {
								render();
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
							case SDL_WINDOWEVENT_MINIMIZED: {
								is_minimized = true;
								has_mouse = false;
								break;
							}case SDL_WINDOWEVENT_MAXIMIZED: {
								is_minimized = false;
								break;
							}case SDL_WINDOWEVENT_RESTORED: {
								is_minimized = false;
								break;
							}
						}
						current_room->window(&event);
						break;
					}

					case SDL_KEYDOWN: {
						if (event.key.repeat != 0) {
							current_room->keyboard(&event);
						} else {
							current_room->keyboard_press(&event);
						}
						break;
					}
					case SDL_KEYUP: {
						current_room->keyboard_release(&event);
						break;
					}
					case SDL_MOUSEMOTION:
					case SDL_MOUSEWHEEL: {
						current_room->mouse(&event);
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

			current_room->step_mid();
			current_room->check_paths();
			current_room->outside_room();
			current_room->intersect_boundary();
			current_room->collision();

			current_room->step_end();
			current_room->draw();

			fps_count++;
			new_tickstamp = SDL_GetTicks();
			if ((new_tickstamp - tickstamp < 1000/fps_max)&&(!options->is_vsync_enabled)) {
				SDL_Delay((1000/fps_max) - (new_tickstamp - tickstamp));
			}
			tickstamp = SDL_GetTicks();

			if (tickstamp - fps_ticks >= 1000) {
				fps_stable = fps_count;
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
					change_room(first_room);
					break;
				}
				case 3: { // Restart room
					change_room(current_room);
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

	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window != NULL) {
		SDL_DestroyWindow(window);
		window = NULL;
	}

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	if (resource_list != NULL) {
		resource_list->reset();
		delete resource_list;
		resource_list = NULL;
	}

	return 0;
}
int BEE::set_engine_pointer() {
	for (int i=0; i<resource_list->sprites.get_amount(); i++) {
		if (get_sprite(i) != NULL) {
			get_sprite(i)->game = this;
		}
	}
	for (int i=0; i<resource_list->sounds.get_amount(); i++) {
		if (get_sound(i) != NULL) {
			get_sound(i)->game = this;
		}
	}
	for (int i=0; i<resource_list->backgrounds.get_amount(); i++) {
		if (get_background(i) != NULL) {
			get_background(i)->game = this;
		}
	}
	for (int i=0; i<resource_list->fonts.get_amount(); i++) {
		if (get_font(i) != NULL) {
			get_font(i)->game = this;
		}
	}
	for (int i=0; i<resource_list->paths.get_amount(); i++) {
		if (get_path(i) != NULL) {
			get_path(i)->game = this;
		}
	}
	for (int i=0; i<resource_list->objects.get_amount(); i++) {
		if (get_object(i) != NULL) {
			get_object(i)->game = this;
		}
	}
	for (int i=0; i<resource_list->rooms.get_amount(); i++) {
		if  (get_room(i) != NULL) {
			get_room(i)->game = this;
		}
	}

	return 0;
}
int BEE::load_media() {
	if (current_room != NULL) {
		current_room->load_media();
	}

	return 0;
}
int BEE::free_media() {
	if (current_room != NULL) {
		current_room->free_media();
	}

	return 0;
}

int BEE::render() {
	if (options->is_opengl) {
		// OpenGL rendering
	} else {
		SDL_RenderPresent(renderer);
	}
	return 0;
}
int BEE::render_clear() {
	if (options->is_opengl) {
		// OpenGL render clearing
	} else {
		SDL_RenderClear(renderer);
	}
	return 0;
}

int BEE::restart_game() {
	throw 2;
	return 0;
}
int BEE::end_game() {
	throw 1;
	return 0;
}

int BEE::restart_room() {
	throw 3;
	return 0;
}
int BEE::change_room(Room* new_room) {
	if (quit) {
		return 1;
	}

	bool is_game_start = false;
	if (current_room != NULL) {
		if (transition_type != 0) {
			set_render_target(texture_before);
			render_clear();
			current_room->draw();
			render();
		}
		current_room->room_end();
		current_room->reset_properties();
	} else {
		if (transition_type != 0) {
			set_render_target(texture_before);
			render_clear();
			render();
		}
		is_game_start = true;
		first_room = new_room;
	}

	sound_stop_all();
	free_media();

	current_room = new_room;
	is_ready = false;
	current_room->reset_properties();
	current_room->init();

	if (load_media()) {
		std::cerr << "Couldn't load room media for " << current_room->get_name() << "\n";
		return 1;
	}

	SDL_SetWindowTitle(window, current_room->get_name().c_str());
	std::cout << current_room->get_instance_string();

	if (transition_type != 0) {
		set_render_target(texture_after);
		render_clear();
	}

	is_ready = true;
	current_room->create();
	if (is_game_start) {
		current_room->game_start();
	}
	current_room->room_start();
	current_room->draw();

	if (transition_type != 0) {
		render();
		set_render_target(NULL);
		draw_transition();
	}

	return 0;
}
int BEE::room_goto(int index) {
	return change_room(get_room(index));
}
int BEE::room_goto_previous() {
	return room_goto(get_current_room()->get_id()-1);
}
int BEE::room_goto_next() {
	return room_goto(get_current_room()->get_id()+1);
}

int BEE::animation_end(Sprite* finished_sprite) {
	return current_room->animation_end(finished_sprite);
}
void BEE::sound_finished(int channel) {
	for (int i=0; i<resource_list->sounds.get_amount(); i++) {
		if (get_sound(i) != NULL) {
			if (!get_sound(i)->get_is_music()) {
				get_sound(i)->finished(channel);
			}
		}
	}
}

int BEE::set_render_target(Sprite* sprite_target) {
	if (sprite_target == NULL) {
		SDL_SetRenderTarget(renderer, NULL);
	} else {
		sprite_target->set_as_target();
	}

	return 0;
}
int BEE::draw_transition() {
	switch (transition_type) {
		case 1: { // Create from left
			break;
		}
		case 2: { // Create from right
			break;
		}
		case 3: { // Create from top
			break;
		}
		case 4: { // Create from bottom
			break;
		}
		case 5: { // Create from center
			break;
		}
		case 6: { // Shift from left
			for (int i=-width; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 7: { // Shift from right
			for (int i=width; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 8: { // Shift from top
			for (int i=-height; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(0, i, 0);
				render();
			}
			break;
		}
		case 9: { // Shift from bottom
			for (int i=height; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(0, i, 0);
				render();
			}
			break;
		}
		case 10: { // Interlaced from left
			break;
		}
		case 11: { // Interlaced from right
			break;
		}
		case 12: { // Interlaced from top
			break;
		}
		case 13: { // Interlaced from bottom
			break;
		}
		case 14: { // Push from left
			for (int i=-width; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(i+width, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 15: { // Push from right
			for (int i=width; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(i-width, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 16: { // Push from top
			for (int i=-height; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(i+height, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 17: { // Push from bottom
			for (int i=height; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(i-height, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 18: { // Rotate to left
			break;
		}
		case 19: { // Rotate to right
			break;
		}
		case 20: { // Blend
			break;
		}
		case 21: { // Fade out and in
			break;
		}

	}
	transition_type++;
	for (auto& b : current_room->get_backgrounds()) {
		b.second->background->set_time_update();
	}

	return 0;
}

BEE::Room* BEE::get_current_room() {
	return current_room;
}
bool BEE::get_is_ready() {
	return is_ready;
}
int BEE::get_room_width() {
	if (current_room != NULL) {
		return current_room->get_width();
	}
	return -1;
}
int BEE::get_room_height() {
	if (current_room != NULL) {
		return current_room->get_height();
	}
	return -1;
}


SDL_DisplayMode BEE::get_display() {
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
		std::cerr << "Failed to get display mode: " << SDL_GetError() << "\n";
	}
	return dm;
}
Uint32 BEE::get_display_format() {
	return get_display().format;
}
int BEE::get_display_width() {
	return get_display().w;
}
int BEE::get_display_height() {
	return get_display().h;
}
int BEE::get_display_refresh_rate() {
	return get_display().refresh_rate;
}

int BEE::set_display(int w, int h, int hz) {
	if (options->is_fullscreen) {
		SDL_DisplayMode dm = {get_display_format(), w, h, hz, 0};
		if (SDL_SetWindowDisplayMode(window, &dm) != 0) {
			std::cerr << "Failed to set display size: " << SDL_GetError() << "\n";
			return 1;
		}
		return 0;
	} else {
		std::cerr << "Failed to set display size because the window is not fullscreen.\n";
		return 1;
	}
}
int BEE::set_display_size(int w, int h) {
	return set_display(w, h, get_display_refresh_rate());
}
int BEE::set_display_refresh_rate(int hz) {
	return set_display(get_display_width(), get_display_height(), hz);
}

bool BEE::get_is_visible() {
	return options->is_visible;
}
bool BEE::get_is_fullscreen() {
	return options->is_fullscreen;
}
bool BEE::get_is_borderless() {
	return options->is_borderless;
}
bool BEE::get_is_resizable() {
	return options->is_resizable;
}
std::string BEE::get_window_title() {
	return SDL_GetWindowTitle(window);
}
SDL_Cursor* BEE::get_cursor() {
	return cursor;
}
int BEE::get_window_x() {
	int wx;
	SDL_GetWindowPosition(window, &wx, NULL);
	return wx;
}
int BEE::get_window_y() {
	int wy;
	SDL_GetWindowPosition(window, NULL, &wy);
	return wy;
}
int BEE::get_width() {
	return width;
}
int BEE::get_height() {
	return height;
}
int BEE::set_is_visible(bool new_is_visible) {
	options->is_visible = new_is_visible;
	if (options->is_visible) {
		SDL_ShowWindow(window);
	} else {
		SDL_HideWindow(window);
	}
	return 0;
}
int BEE::set_is_fullscreen(bool new_is_fullscreen) {
	options->is_fullscreen = new_is_fullscreen;
	if (options->is_fullscreen) {
		if (options->is_resizable) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		} else {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		}
	} else {
		SDL_SetWindowFullscreen(window, 0);
	}
	return 0;
}
int BEE::set_window_title(std::string new_title) {
	SDL_SetWindowTitle(window, new_title.c_str());
	return 0;
}
int BEE::set_cursor(SDL_SystemCursor cid) {
	SDL_FreeCursor(cursor);
	cursor = SDL_CreateSystemCursor(cid);
	SDL_SetCursor(cursor);
	return 0;
}
int BEE::set_show_cursor(bool new_show_cursor) {
	SDL_ShowCursor((new_show_cursor) ? SDL_ENABLE : SDL_DISABLE);
	return 0;
}
int BEE::set_window_position(int new_x, int new_y) {
	SDL_SetWindowPosition(window, new_x, new_y);
	return 0;
}
int BEE::set_window_x(int new_x) {
	return set_window_position(new_x, get_window_y());
}
int BEE::set_window_y(int new_y) {
	return set_window_position(get_window_x(), new_y);
}
int BEE::set_window_center() {
	return set_window_position(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
int BEE::set_window_size(int new_width, int new_height) {
	width = new_width;
	height = new_height;
	SDL_SetWindowSize(window, width, height);
	return 0;
}
int BEE::set_width(int new_width) {
	return set_window_size(new_width, height);
}
int BEE::set_height(int new_height) {
	return set_window_size(width, new_height);
}

int BEE::get_mousex() {
	int mx;
	SDL_GetMouseState(&mx, NULL);
	return mx;
}
int BEE::get_mousey() {
	int my;
	SDL_GetMouseState(NULL, &my);
	return my;
}
int BEE::set_mouse_position(int new_mx, int new_my) {
	SDL_WarpMouseInWindow(window, new_mx, new_my);
	return 0;
}
int BEE::set_mousex(int new_mx) {
	return set_mouse_position(new_mx, get_mousey());
}
int BEE::set_mousey(int new_my) {
	return set_mouse_position(get_mousex(), new_my);
}

int BEE::draw_point(int x, int y) {
	return SDL_RenderDrawPoint(renderer, x, y);
}
int BEE::draw_line(int x1, int y1, int x2, int y2) {
	return SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}
int BEE::draw_rectangle(int x1, int y1, int x2, int y2, bool is_filled) {
	SDL_Rect rect = {x1, y1, x2, y2};
	if (is_filled) {
		return SDL_RenderFillRect(renderer, &rect);
	} else {
		return SDL_RenderDrawRect(renderer, &rect);
	}
}
int BEE::draw_set_color(RGBA new_color) {
	return SDL_SetRenderDrawColor(renderer, new_color.r, new_color.g, new_color.b, new_color.a);
}
BEE::RGBA BEE::draw_get_color() {
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	RGBA c = {r, g, b, a};
	return c;
}
BEE::RGBA BEE::get_pixel_color(int x, int y) {
	SDL_Surface *screenshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

	RGBA color;
	SDL_GetRGBA(((Uint32*)screenshot->pixels)[x+y*height], screenshot->format, &color.r, &color.g, &color.b, &color.a);

	SDL_FreeSurface(screenshot);

	return color;
}
int BEE::save_screenshot(std::string filename) { // Slow, use sparingly
	if (options->is_opengl) {
		/*unsigned char* pixels = new unsigned char[width*height*4]; // 4 bytes for RGBA
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		SDL_Surface* screenshot  = SDL_CreateRGBSurfaceFrom(pixels, width, height, 8*4, width*4, 0,0,0,0);
		SDL_SaveBMP(screenshot), filename.c_str());

		SDL_FreeSurface(screenshot);
		delete [] pixels;*/
	} else {
		SDL_Surface *screenshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

		SDL_SaveBMP(screenshot, filename.c_str());

		SDL_FreeSurface(screenshot);
	}

	return 0;
}

double BEE::get_volume() {
	return volume;
}
int BEE::set_volume(double new_volume) {
	volume = new_volume;

	for (int i=0; i<resource_list->sounds.get_amount(); i++) {
		if (get_sound(i) != NULL) {
			get_sound(i)->update_volume();
		}
	}

	return 0;
}
int BEE::sound_stop_all() {
	for (int i=0; i<resource_list->sounds.get_amount(); i++) {
		if (get_sound(i) != NULL) {
			get_sound(i)->stop();
		}
	}
	return 0;
}

BEE::Sprite* BEE::get_sprite(int id) {
	return dynamic_cast<Sprite*>(resource_list->sprites.get_resource(id));
}
BEE::Sound* BEE::get_sound(int id) {
	return dynamic_cast<Sound*>(resource_list->sounds.get_resource(id));
}
BEE::Background* BEE::get_background(int id) {
	return dynamic_cast<Background*>(resource_list->backgrounds.get_resource(id));
}
BEE::Font* BEE::get_font(int id) {
	return dynamic_cast<Font*>(resource_list->fonts.get_resource(id));
}
BEE::Path* BEE::get_path(int id) {
	return dynamic_cast<Path*>(resource_list->paths.get_resource(id));
}
BEE::Object* BEE::get_object(int id) {
	return dynamic_cast<Object*>(resource_list->objects.get_resource(id));
}
BEE::Room* BEE::get_room(int id) {
	return dynamic_cast<Room*>(resource_list->rooms.get_resource(id));
}

#endif // _BEE_GAME
