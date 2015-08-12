/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_H
#define _BEE_GAME_H 1

#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#include "debug.hpp"
#include "util.hpp"

class BEE;
#ifndef _BEE_RESOURCE_STRUCTURES_H
#include "resource_structures.hpp"
#endif

class BEE {
	public:
		class Sprite; class Sound; class Background; class Font; class Path; class Object; class Room;
		class GameOptions; class InstanceData; class RGBA;
	private:
		int argc;
		char** argv;
		bool quit, is_ready;
		Room *first_room = NULL, *current_room = NULL;
		GameOptions* options;

		int width, height;
		SDL_Cursor* cursor;

		bool is_minimized, is_fullscreen;
		bool has_mouse, has_focus;

		SDL_Event event;
		Uint32 tickstamp, new_tickstamp, fps_ticks;

		double volume = 1.0;
	public:
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;
		unsigned int fps_max, fps_goal, fps_count, fps_stable;

		Sprite* texture_before;
		Sprite* texture_after;
		unsigned int transition_type = 6;
		unsigned int transition_speed = 80;

		static MetaResourceList* resource_list;
		static bool is_initialized;

		BEE(int, char**, Room**, GameOptions*);
		~BEE();
		int loop();
		int close();
		int set_engine_pointer();
		int load_media();
		int free_media();

		int render();
		int render_clear();

		int restart_game();
		int end_game();

		int restart_room();
		int change_room(Room*);
		int room_goto(int);
		int room_goto_previous();
		int room_goto_next();

		int animation_end(Sprite*);
		static void sound_finished(int);

		int set_render_target(Sprite*);
		int draw_transition();

		Room* get_current_room();
		bool get_is_ready();
		int get_room_width();
		int get_room_height();

		SDL_DisplayMode get_display();
		Uint32 get_display_format();
		int get_display_width();
		int get_display_height();
		int get_display_refresh_rate();
		int set_display(int, int, int);
		int set_display_size(int, int);
		int set_display_refresh_rate(int);

		bool get_is_visible();
		bool get_is_fullscreen();
		bool get_is_borderless();
		bool get_is_resizable();
		std::string get_window_title();
		SDL_Cursor* get_cursor();
		int get_window_x();
		int get_window_y();
		int get_width();
		int get_height();
		int set_is_visible(bool);
		int set_is_fullscreen(bool);
		int set_window_title(std::string);
		int set_cursor(SDL_SystemCursor);
		int set_show_cursor(bool);
		int set_window_position(int, int);
		int set_window_x(int);
		int set_window_y(int);
		int set_window_center();
		int set_window_size(int, int);
		int set_width(int);
		int set_height(int);

		int get_mousex();
		int get_mousey();
		int set_mouse_position(int, int);
		int set_mousex(int);
		int set_mousey(int);

		int draw_point(int, int);
		int draw_line(int, int, int, int);
		int draw_rectangle(int, int, int, int, bool);
		int draw_set_color(RGBA);
		RGBA draw_get_color();
		RGBA get_pixel_color(int, int);
		int save_screenshot(std::string);

		double get_volume();
		int set_volume(double);
		int sound_stop_all();

		static int init_resources();
		static int close_resources();

		static Sprite* get_sprite(int);
		static Sound* get_sound(int);
		static Background* get_background(int);
		static Font* get_font(int);
		static Path* get_path(int);
		static Object* get_object(int);
		static Room* get_room(int);
};

class BEE::GameOptions {
	public:
		// Window flags
		bool is_fullscreen, is_opengl, is_borderless;
		bool is_resizable, is_maximized, is_highdpi;
		bool is_visible;

		// Renderer flags
		bool is_vsync_enabled;
};

class BEE::RGBA {
	public:
		Uint8 r, g, b, a;
};

/*BEE::RGBA c_aqua	= {0, 255, 255, 255};
BEE::RGBA c_black	= {0, 0, 0, 255};
BEE::RGBA c_blue	= {0, 0, 255, 255};
BEE::RGBA c_dkgray	= {64, 64, 64, 255};
BEE::RGBA c_fuchsia	= {255, 0, 255, 255};
BEE::RGBA c_gray	= {128, 128, 128, 255};
BEE::RGBA c_green	= {0, 255, 0, 255};
//BEE::RGBA c_lime	= {255, 255, 255, 255};
BEE::RGBA c_ltgray	= {192, 192, 192, 255};
//BEE::RGBA c_maroon	= {255, 255, 255, 255};
//BEE::RGBA c_navy	= {255, 255, 255, 255};
//BEE::RGBA c_olive	= {255, 255, 255, 255};
BEE::RGBA c_orange	= {255, 128, 0, 255};
BEE::RGBA c_purple	= {128, 0, 255, 255};
BEE::RGBA c_red		= {255, 0, 0, 255};
//BEE::RGBA c_silver	= {255, 255, 255, 255};
//BEE::RGBA c_teal	= {255, 255, 255, 255};
BEE::RGBA c_white	= {255, 255, 255, 255};
BEE::RGBA c_yellow	= {255, 255, 0, 255};*/

typedef std::tuple<int, int, double> path_coord;

class ViewData {
	public:
		bool is_visible;
		int view_x, view_y, view_width, view_height;
		int port_x, port_y, port_width, port_height;
		BEE::InstanceData* following;
		int object_horizontal_border, object_vertical_border;
		int horizontal_speed, vertical_speed;
};

class BackgroundData { // Used to pass data to the Room class in bee/resource_structures/room.hpp
	public:
		BEE::Background* background;
		bool is_visible;
		bool is_foreground;
		int x, y;
		bool is_horizontal_tile, is_vertical_tile;
		int horizontal_speed, vertical_speed;
		bool is_stretched;
		BackgroundData() {background=NULL;is_visible=false;is_foreground=false;x=0;y=0;is_horizontal_tile=false;is_vertical_tile=false;horizontal_speed=0;vertical_speed=0;is_stretched=false;};
		BackgroundData(BEE::Background*, bool, bool, int, int, bool, bool, int, int, bool);
		int init(BEE::Background*, bool, bool, int, int, bool, bool, int, int, bool);
};

#include "resource_structures/sprite.hpp"
#include "resource_structures/sound.hpp"
#include "resource_structures/background.hpp"
#include "resource_structures/font.hpp"
#include "resource_structures/path.hpp"
#include "resource_structures/object.hpp"
#include "resource_structures/instancedata.hpp"
#include "resource_structures/room.hpp"

#endif // _BEE_GAME_H
