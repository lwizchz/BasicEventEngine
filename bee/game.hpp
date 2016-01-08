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
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#define DEFAULT_GAME_FPS 60

#define BEE_GAME_ID 11111111

#include "debug.hpp"
#include "util.hpp"

class BEE;
#ifndef _BEE_RESOURCE_STRUCTURES_H
#include "resource_structures.hpp"
#endif

enum rgba_t {c_cyan, c_aqua, c_black, c_blue, c_dkgray, c_magenta, c_fuchsia, c_gray, c_green, c_lime, c_silver, c_ltgray, c_maroon, c_navy, c_olive, c_orange, c_purple, c_red, c_teal, c_white, c_yellow};

class BEE {
	public:
		class Sprite; class Sound; class Background; class Font; class Path; class Timeline; class Object; class Room;
		class GameOptions; class InstanceData; class CollisionTree; class CollisionPolygon; class RGBA;
		class Particle; class ParticleData; class ParticleEmitter; class ParticleAttractor; class ParticleDestroyer; class ParticleDeflector; class ParticleChanger; class ParticleSystem;
		class SoundGroup;
		class NetworkData;
	private:
		int argc;
		char** argv;
		bool quit, is_ready, is_paused;
		Room *first_room = NULL, *current_room = NULL;

		// 0=Linux, 1=Windows
		#ifdef _WINDOWS
			const int platform = 1;
		#else // _WINDOWS
			const int platform = 0;
		#endif // _WINDOWS else

		int width, height;
		SDL_Cursor* cursor;

		RGBA* color;

		bool is_minimized, is_fullscreen;
		bool has_mouse, has_focus;

		SDL_Event event;
		Uint32 tickstamp, new_tickstamp, fps_ticks;

		NetworkData* net;

		double volume = 1.0;
	protected:
		GameOptions* options;
	public:
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;

		SDL_GLContext context = NULL;
		GLuint program = 0;
		GLint vertex_location = -1;
		GLint fragment_location = -1;
		GLint model_location = -1;
		GLint texture_location = -1;
		GLint mvp_location = -1;

		unsigned int fps_max, fps_goal, fps_unfocused;
		unsigned int fps_count, fps_stable;
		Uint32 frame_number = 0;

		const Uint8* keystate;

		Sprite* texture_before;
		Sprite* texture_after;
		unsigned int transition_type = 1;
		unsigned int transition_speed = 80;

		static MetaResourceList* resource_list;
		static bool is_initialized;

		BEE(int, char**, Room**, GameOptions*);
		~BEE();
		int loop();
		int close();

		// User defined
		static int init_resources();
		static int close_resources();

		// bee/game/resources.cpp
		int load_media();
		int free_media();

		static void sound_finished(int);

		double get_volume();
		int set_volume(double);
		int sound_stop_all();

		Sprite* add_sprite(std::string, std::string);
		Sound* add_sound(std::string, std::string, bool);
		Background* add_background(std::string, std::string);
		Font* add_font(std::string, std::string, int);
		Path* add_path(std::string, std::string);
		Timeline* add_timeline(std::string, std::string);
		Object* add_object(std::string, std::string);
		Room* add_room(std::string, std::string);

		static Sprite* get_sprite(int);
		static Sound* get_sound(int);
		static Background* get_background(int);
		static Font* get_font(int);
		static Path* get_path(int);
		static Timeline* get_timeline(int);
		static Object* get_object(int);
		static Room* get_room(int);

		Object* get_object_by_name(std::string);

		// bee/game.cpp
		Uint32 get_ticks();
		Uint32 get_seconds();
		Uint32 get_frame();

		GameOptions get_options();
		int set_options(GameOptions);

		int opengl_init();
		int opengl_close();
		int renderer_init(bool);
		int renderer_close();

		int render();
		int render_clear();
		int render_reset();

		int restart_game();
		int end_game();

		// bee/game/room.cpp
		int restart_room();
		int change_room(Room*, bool);
		int change_room(Room*);
		int room_goto(int);
		int room_goto_previous();
		int room_goto_next();

		Room* get_current_room();
		bool get_is_ready();
		int get_room_width();
		int get_room_height();
		bool is_on_screen(SDL_Rect*);

		bool set_is_paused(bool);
		bool get_is_paused();

		// bee/game/transition.cpp
		int reset_render_target();
		int set_render_target(Sprite*, int, int);
		int set_render_target(Sprite*);
		int set_render_target(Background*, int, int);
		int set_render_target(Background*);
		int draw_transition();

		// bee/game/display.cpp
		SDL_DisplayMode get_display();
		Uint32 get_display_format();
		int get_display_width();
		int get_display_height();
		int get_display_refresh_rate();
		int set_display(int, int, int);
		int set_display_size(int, int);
		int set_display_refresh_rate(int);

		// bee/game/window.cpp
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

		// bee/game/input.cpp
		std::pair<int,int> get_mouse_global_position();
		int get_mouse_global_x();
		int get_mouse_global_y();
		std::pair<int,int> get_mouse_position();
		int get_mouse_x();
		int get_mouse_y();
		int set_mouse_global_position(int, int);
		int set_mouse_global_x(int);
		int set_mouse_global_y(int);
		bool is_mouse_inside(InstanceData*);
		bool get_key_state(SDL_Scancode);
		bool get_key_state(SDL_Keycode);
		bool get_mod_state(Uint8);
		bool get_mouse_state(Uint8);
		char append_input(std::string&, SDL_KeyboardEvent*);

		// bee/game/draw.cpp
		RGBA get_enum_color(rgba_t, Uint8);
		RGBA get_enum_color(rgba_t);
		int convert_view_coords(int&, int&);
		int convert_window_coords(int&, int&);

		int draw_point(int, int, bool);
		int draw_line(int, int, int, int, bool);
		int draw_line(int, int, int, int, RGBA, bool);
		int draw_line(int, int, int, int, rgba_t, bool);
		int draw_line(Line, RGBA, bool);
		int draw_rectangle(int, int, int, int, bool, bool);
		int draw_rectangle(int, int, int, int, bool, RGBA, bool);
		int draw_rectangle(int, int, int, int, bool, rgba_t, bool);
		int draw_rectangle(SDL_Rect, bool, RGBA, bool);

		int draw_point(int, int);
		int draw_line(int, int, int, int);
		int draw_line(int, int, int, int, RGBA);
		int draw_line(int, int, int, int, rgba_t);
		int draw_rectangle(int, int, int, int, bool);
		int draw_rectangle(int, int, int, int, bool, RGBA);
		int draw_rectangle(int, int, int, int, bool, rgba_t);

		int draw_set_color(RGBA);
		int draw_set_color(rgba_t);
		RGBA draw_get_color();
		RGBA get_pixel_color(int, int);
		int save_screenshot(std::string);

		// bee/game/network.cpp
		int net_init();
		bool net_get_is_initialized();
		int net_end();
		int net_handle_events();
		bool net_session_start(std::string, int, std::string);
		std::map<std::string,std::string> net_session_find();
		bool net_session_join(std::string, std::string);
		bool net_get_is_connected();
		int net_session_end();
};

typedef std::tuple<int, int, double> path_coord;
typedef std::multimap<Uint32, std::pair<std::string,std::function<void()>>> timeline_list;

class BEE::GameOptions {
	public:
		// Window flags
		bool is_fullscreen, is_opengl, is_borderless;
		bool is_resizable, is_maximized, is_highdpi;
		bool is_visible;

		// Renderer flags
		bool is_vsync_enabled;

		// Miscellaneous flags
		bool is_network_enabled;
		bool is_debug_enabled;
};

class BEE::CollisionPolygon {
	public:
		double x, y, w, h;
		std::vector<Line> lines;

		int add_vertex(double, double);
		int finalize();
};

class BEE::RGBA {
	public:
		Uint8 r, g, b, a;
};

class ViewData {
	public:
		bool is_visible;
		int view_x, view_y, view_width, view_height;
		int port_x, port_y, port_width, port_height;
		BEE::InstanceData* following;
		int horizontal_border, vertical_border;
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

class BEE::NetworkData {
	public:
		bool is_initialized = false;
		UDPsocket udp_send = NULL;
		UDPsocket udp_recv = NULL;
		UDPpacket* udp_data = NULL;

		int id = BEE_GAME_ID;
		bool is_connected = false;
		bool is_host = false;
		int channel = -1;
		Uint32 timeout = 1000;
		std::map<std::string,std::string> servers;

		std::string name = "";
		int max_players = 0;
		int self_id = 0;
		std::map<int,UDPsocket> players;
		std::map<std::string,std::string> data;
};

#include "resource_structures/sprite.hpp"
#include "resource_structures/sound.hpp"
#include "resource_structures/background.hpp"
#include "resource_structures/font.hpp"
#include "resource_structures/path.hpp"
#include "resource_structures/timeline.hpp"
#include "resource_structures/object.hpp"
#include "resource_structures/ext/instancedata.hpp"
#include "resource_structures/ext/collisiontree.hpp"
#include "resource_structures/ext/particle.hpp"
#include "resource_structures/ext/soundgroup.hpp"
#include "resource_structures/room.hpp"

#endif // _BEE_GAME_H
