/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_H
#define _BEE_GAME_H 1

#include <iostream> // Include the required library headers
#include <string>
#include <time.h>
#include <functional>
#include <getopt.h>
#include <memory>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <regex>

#include <SDL2/SDL.h> // Include the required SDL headers
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>

#define GLM_FORCE_RADIANS // Force all GLM functions to use radians instead of degrees

#define GLEW_STATIC // Statically link GLEW

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define DEFAULT_WINDOW_WIDTH 1280 // Define the default window dimensions
//#define DEFAULT_WINDOW_HEIGHT 720
#define DEFAULT_WINDOW_WIDTH 1920 // Define the default window dimensions
#define DEFAULT_WINDOW_HEIGHT 1080

#define DEFAULT_GAME_FPS 60 // Define the default game fps goal

#define BEE_MAX_LIGHTS 8 // Define the maximum amount of processed lights
#define BEE_MAX_LIGHTABLES 96
#define BEE_MAX_MASK_VERTICES 8

#define MACRO_TO_STR_(x) #x
#define MACRO_TO_STR(x) MACRO_TO_STR_(x)

#ifndef BEE_GAME_ID // BEE_GAME_ID should always be defined but just in case
#define BEE_GAME_ID 4294967295 // pow(2,32)-1, the maximum value
#endif // BEE_GAME_ID

#include "debug.hpp" // Include the required debug and utility functions
#include "util.hpp"

#ifndef _BEE_RESOURCEES_H
#include "resources.hpp" // Include the resource classes
#endif

#include "enum.hpp" // Include the required engine enumerations

class BEE { // The master engine class which effectively acts as a namespace
	public:
		class Sprite; class Sound; class Background; class Font; class Path; class Timeline; class Mesh; class Object; class Room; // The main resource types
		class Particle; class ParticleData; class ParticleEmitter; class ParticleAttractor; class ParticleDestroyer; class ParticleDeflector; class ParticleChanger; class ParticleSystem; // The particle system components
		class Light; class Camera; // The OpenGL-only resources (poor SDL implementations may exist)
		class ProgramFlags; class GameOptions; class RGBA; // The engine related data
		class InstanceData; class SoundGroup; // The additional resource data types
		class PhysicsWorld; class PhysicsDraw; class PhysicsBody; // The classes which interface with the external Physics library
		struct SpriteDrawData; class TextData; class LightData; class LightableData; // Additional resource-related data structs

		struct ViewData; struct BackgroundData; struct NetworkData; // The configurational structs
		struct MessageContents; struct MessageRecipient; // The messaging subsystem structs
		struct SIDP; // The utility structs
		struct Console; // The console subsystem structs
	private:
		// These contain data about the engine initialization
		int argc;
		char** argv; // The provided commandline flags
		std::list<ProgramFlags*> flags; // The available commandline flags
		GameOptions* options; // The engine options

		// These contain data about the event loop
		bool quit, is_ready, is_paused;
		Room *first_room, *current_room;

		// This defines the platform where 0=Linux, 1=Windows, 2=OSX, 3=other
		#ifdef __linux__
			const unsigned int platform = 0;
		#elif _WIN32
			const unsigned int platform = 1;
		#elif __APPLE__
			const unsigned int platform = 2;
		#else
			const unsigned int platform = 3;
		#endif

		unsigned int width, height;
		SDL_Cursor* cursor;

		// These are used for the SDL renderer
		SDL_Window* window;
		SDL_Renderer* renderer;

		// This is the OpenGL renderer context
		SDL_GLContext context;

		// The following GLint's are all uniforms in the OpenGL shaders
		GLuint program = 0; // This is the location of the OpenGL program (where the shaders are compiled)
		GLint vertex_location = -1;
		GLint normal_location = -1;
		GLint fragment_location = -1;
		GLuint target = 0;

		GLint projection_location = -1;
		GLint view_location = -1;
		GLint model_location = -1;
		GLint port_location = -1;

		GLint rotation_location = -1;

		GLint texture_location = -1;
		GLint colorize_location = -1;
		GLint primitive_location = -1;
		GLint flip_location = -1;

		GLint is_lightable_location = -1;
		GLint light_amount_location = -1;
		struct {
			GLint type;
			GLint position;
			GLint direction;
			GLint attenuation;
			GLint color;
		} lighting_location[BEE_MAX_LIGHTS];

		GLint lightable_amount_location = -1;
		struct {
			GLint position;
			GLint vertex_amount;
			GLint mask[BEE_MAX_MASK_VERTICES];
		} lightable_location[BEE_MAX_LIGHTABLES];

		bool render_is_3d;
		Camera* render_camera;
		glm::mat4* projection_cache;

		// These should only be used internally by the functions in bee/game/draw.cpp
		GLuint triangle_vao;
		GLuint triangle_vbo;
		GLuint triangle_ibo;

		// This is the current drawing color
		RGBA* color;
		Font* font_default; // A default font for engine drawing

		// These contain data about the current window state
		bool has_mouse, has_focus;

		Uint32 tickstamp, new_tickstamp, fps_ticks, tick_delta;

		NetworkData* net;

		double volume;

		unsigned int fps_goal, fps_max, fps_unfocused;
		unsigned int fps_count;
		Uint32 frame_number;

		Sprite* texture_before;
		Sprite* texture_after;
		bee_transition_t transition_type;
		double transition_speed;
		std::function<void (BEE*, Sprite*, Sprite*)> transition_custom_func;

		const Uint8* keystate;
		std::map<std::string,SDL_Keycode> keystrings_keys;
		std::map<SDL_Keycode,std::string> keystrings_strings;

		std::vector<std::string> commandline_input;
		unsigned int commandline_current;

		std::unordered_map<std::string,std::unordered_set<std::shared_ptr<MessageRecipient>>> recipients;
		const std::unordered_set<std::string> protected_tags;
		std::vector<std::shared_ptr<MessageContents>> messages;
		bee_output_t messenger_output_level;

		Console* console;

		int update_delta();
	protected:
		// bee/game/info.cpp
		static std::list<BEE::ProgramFlags*>& get_standard_flags_internal();

		// bee/game/messenger.cpp
		int messenger_register_protected(std::shared_ptr<MessageRecipient>);
		std::shared_ptr<MessageRecipient> messenger_register_protected(std::string, const std::vector<std::string>&, bool, std::function<void (BEE*, std::shared_ptr<MessageContents>)>);
		int messenger_unregister_protected(std::shared_ptr<MessageRecipient>);
		int messenger_send_urgent(std::shared_ptr<MessageContents>);

		// bee/game/console.cpp
		int console_handle_input(SDL_Event*);
		int console_init_commands();
		int console_run_internal(const std::string&, bool, Uint32);
		int console_run(const std::string&, bool, Uint32);
		int console_complete(const std::string&);
		std::vector<SIDP> console_parse_parameters(const std::string&) const;
		int console_draw();
	public:
		unsigned int fps_stable;

		static MetaResourceList* resource_list;
		static bool is_initialized;

		BEE(int, char**, const std::list<ProgramFlags*>&, Room**, GameOptions*);
		~BEE();
		int handle_flags(const std::list<ProgramFlags*>&, bool);
		int loop();
		int close();

		// User defined
		static int init_resources();
		static int close_resources();

		// bee/game/resources.cpp
		int load_media() const;
		int free_media() const;

		static void sound_finished(int);

		double get_volume() const;
		int set_volume(double);
		int sound_stop_all() const;

		Sprite* add_sprite(const std::string&, const std::string&);
		Sound* add_sound(const std::string&, const std::string&, bool);
		Background* add_background(const std::string&, const std::string&);
		Font* add_font(const std::string&, const std::string&, int, bool);
		Path* add_path(const std::string&, const std::string&);
		Timeline* add_timeline(const std::string&, const std::string&);
		Mesh* add_mesh(const std::string&, const std::string&);
		Light* add_light(const std::string&, const std::string&);
		Object* add_object(const std::string&, const std::string&);
		Room* add_room(const std::string&, const std::string&);

		static Sprite* get_sprite(int);
		static Sound* get_sound(int);
		static Background* get_background(int);
		static Font* get_font(int);
		static Path* get_path(int);
		static Timeline* get_timeline(int);
		static Mesh* get_mesh(int);
		static Light* get_light(int);
		static Object* get_object(int);
		static Room* get_room(int);

		Sprite* get_sprite_by_name(const std::string&) const;
		Sound* get_sound_by_name(const std::string&) const;
		Background* get_background_by_name(const std::string&) const;
		Font* get_font_by_name(const std::string&) const;
		Path* get_path_by_name(const std::string&) const;
		Timeline* get_timeline_by_name(const std::string&) const;
		Mesh* get_mesh_by_name(const std::string&) const;
		Light* get_light_by_name(const std::string&) const;
		Object* get_object_by_name(const std::string&) const;
		Room* get_room_by_name(const std::string&) const;

		// bee/game.cpp
		Uint32 get_ticks() const;
		Uint32 get_seconds() const;
		Uint32 get_frame() const;
		double get_delta() const;
		Uint32 get_tick_delta() const;
		unsigned int get_fps_goal() const;

		GameOptions get_options() const;
		int set_options(const GameOptions&);

		int opengl_init();
		int opengl_close();
		std::string opengl_prepend_version(const std::string&);
		int sdl_renderer_init();
		int sdl_renderer_close();

		int render_clear();
		int render() const;
		int render_reset();

		int restart_game() const;
		int end_game() const;

		// bee/game/info.cpp
		static std::string get_usage_text();
		static std::list<BEE::ProgramFlags*> get_standard_flags();
		static int free_standard_flags();

		// bee/game/room.cpp
		void restart_room() const;
		int change_room(Room*, bool);
		int change_room(Room*);
		int room_goto(int);
		int room_goto_previous();
		int room_goto_next();

		Room* get_current_room() const;
		bool get_is_ready() const;
		int get_room_width() const;
		int get_room_height() const;
		bool is_on_screen(const SDL_Rect&) const;

		int set_viewport(ViewData*);

		bool set_is_paused(bool);
		bool get_is_paused() const;

		// bee/game/transition.cpp
		int reset_render_target();
		int set_render_target(Sprite*, int, int);
		int set_render_target(Sprite*);
		int set_render_target(Background*, int, int);
		int set_render_target(Background*);

		int get_transition_type() const;
		int set_transition_type(bee_transition_t);
		int set_transition_custom(std::function<void (BEE*, Sprite*, Sprite*)>);
		double get_transition_speed() const;
		int set_transition_speed(double);
		int draw_transition();
		bool compute_check_quit();

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
		std::string get_window_title() const;
		SDL_Cursor* get_cursor() const;
		int get_window_x() const;
		int get_window_y() const;
		int get_width() const;
		int get_height() const;

		int set_window_title(const std::string&) const;
		int set_cursor(SDL_SystemCursor);
		int set_show_cursor(bool) const;
		int set_window_position(int, int) const;
		int set_window_x(int) const;
		int set_window_y(int) const;
		int set_window_center() const;
		int set_window_size(int, int);
		int set_width(int);
		int set_height(int);

		// bee/game/input.cpp
		std::pair<int,int> get_mouse_global_position() const;
		int get_mouse_global_x() const;
		int get_mouse_global_y() const;
		std::pair<int,int> get_mouse_position() const;
		int get_mouse_x() const;
		int get_mouse_y() const;
		int set_mouse_global_position(int, int) const;
		int set_mouse_global_x(int) const;
		int set_mouse_global_y(int) const;
		bool is_mouse_inside(const InstanceData&) const;
		bool get_key_state(SDL_Scancode) const;
		bool get_key_state(SDL_Keycode) const;
		bool get_mod_state(Uint8) const;
		bool get_mouse_state(Uint8) const;
		char append_input(std::string*, SDL_KeyboardEvent*);
		int keystrings_populate();
		SDL_Keycode keystrings_get_key(const std::string&);
		std::string keystrings_get_string(SDL_Keycode);

		// bee/game/draw.cpp
		RGBA get_enum_color(bee_rgba_t, Uint8) const;
		RGBA get_enum_color(bee_rgba_t) const;

		int draw_triangle(glm::vec3, glm::vec3, glm::vec3, const RGBA&, bool);
		int draw_line(glm::vec3, glm::vec3, const RGBA&);
		int draw_line(int, int, int, int, const RGBA&);
		int draw_line(int, int, int, int);
		int draw_line(int, int, int, int, bee_rgba_t);
		int draw_line(const Line&, const RGBA&);
		int draw_quad(glm::vec3, glm::vec3, bool, const RGBA&);
		int draw_rectangle(int, int, int, int, bool, const RGBA&);
		int draw_rectangle(int, int, int, int, bool);
		int draw_rectangle(int, int, int, int, bool, bee_rgba_t);
		int draw_rectangle(const SDL_Rect&, bool, const RGBA&);

		int draw_set_color(const RGBA&);
		int draw_set_color(bee_rgba_t);
		RGBA draw_get_color() const;
		int draw_set_blend(SDL_BlendMode);
		SDL_BlendMode draw_get_blend();

		RGBA get_pixel_color(int, int) const;
		int save_screenshot(const std::string&);

		int set_is_lightable(bool);

		int render_set_3d(bool);
		int render_set_camera(Camera*);
		bool render_get_3d() const;
		glm::mat4 render_get_projection();
		glm::mat4 render_calc_projection();
		Camera render_get_camera() const;

		// bee/game/messenger.cpp
		int messenger_register(std::shared_ptr<MessageRecipient>);
		std::shared_ptr<MessageRecipient> messenger_register(std::string, const std::vector<std::string>&, bool, std::function<void (BEE*, std::shared_ptr<MessageContents>)>);
		std::shared_ptr<MessageRecipient> messenger_register(const std::vector<std::string>&, std::function<void (BEE*, std::shared_ptr<MessageContents>)>);
		int messenger_unregister(std::shared_ptr<MessageRecipient>);
		int messenger_unregister_name(const std::string&);
		int messenger_unregister_all();

		int messenger_send(std::shared_ptr<MessageContents>);
		int messenger_send(const std::vector<std::string>&, bee_message_t, const std::string&, std::shared_ptr<void>);
		int messenger_send(const std::vector<std::string>&, bee_message_t, const std::string&);

		int messenger_set_level(bee_output_t);
		bee_output_t messenger_get_level();

		int handle_messages();
		std::string messenger_get_type_string(bee_message_t) const;

		// bee/game/network.cpp
		int net_init();
		bool net_get_is_initialized() const;
		int net_close();
		int net_handle_events();

		int net_session_start(const std::string&, int, const std::string&);
		std::map<std::string,std::string> net_session_find();
		int net_session_join(const std::string&, const std::string&);
		bool net_get_is_connected();
		int net_session_end();

		// bee/game/console.cpp
		int console_open();
		int console_close();
		int console_toggle();
		bool console_get_is_open() const ;

		int console_add_command(const std::string&, const std::string&, std::function<void (BEE*, std::shared_ptr<MessageContents>)>);
		int console_add_command(const std::string&, std::function<void (BEE*, std::shared_ptr<MessageContents>)>);
		std::string console_bind(SDL_Keycode, const std::string&);
		std::string console_bind(SDL_Keycode);
		int console_unbind(SDL_Keycode);
		int console_unbind_all();
		int console_alias(const std::string&, const std::string&);
		const std::unordered_map<std::string,std::string>& console_get_aliases() const;
		int console_set_var(const std::string&, SIDP);
		SIDP console_get_var(const std::string&) const;

		int console_run(const std::string&);
		std::string console_get_help(const std::string&) const;
};

typedef std::tuple<double, double, double, double> bee_path_coord; // {x, y, z, speed}
typedef std::multimap<Uint32, std::pair<std::string,std::function<void()>>> bee_timeline_list;

struct BEE::Camera {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 orientation;

	float width, height;

	float fov;
	float z_near, z_far;

	Camera() :
		position(),
		direction(),
		orientation(),

		width(0.0),
		height(0.0),

		fov(90.0),
		z_near(1.0),
		z_far(10000.0)
	{}
	Camera(float w, float h) :
		position(),
		direction(),
		orientation(),

		width(w),
		height(h),

		fov(90.0),
		z_near(1.0),
		z_far(10000.0)
	{}
	Camera(glm::vec3 p, glm::vec3 d, glm::vec3 o) :
		position(p),
		direction(d),
		orientation(o),

		width(0.0),
		height(0.0),

		fov(90.0),
		z_near(1.0),
		z_far(10000.0)
	{}
};

struct BEE::ProgramFlags {
	std::string longopt;
	char shortopt;
	bool pre_init;
	int has_arg;
	std::function<void (BEE*, char*)> func;

	ProgramFlags() :
		longopt(),
		shortopt('\0'),
		pre_init(true),
		has_arg(no_argument),
		func(nullptr)
	{}
	ProgramFlags(std::string l, char s, bool p, int a, std::function<void (BEE*, char*)> f) :
		longopt(l),
		shortopt(s),
		pre_init(p),
		has_arg(a),
		func(f)
	{}
};
struct BEE::GameOptions {
	// Window options
	bool is_fullscreen, is_borderless;
	bool is_resizable, is_maximized;
	bool is_highdpi, is_visible;
	bool is_minimized;

	// Renderer options
	bee_renderer_t renderer_type;
	bool is_vsync_enabled;
	bool is_basic_shaders_enabled;

	// Miscellaneous options
	bool is_network_enabled;
	bool is_debug_enabled;

	// Commandline flags
	bool should_assert;
	bool single_run;

	GameOptions() :
		is_fullscreen(true),
		is_borderless(true),
		is_resizable(true),
		is_maximized(true),
		is_highdpi(false),
		is_visible(true),
		is_minimized(false),

		renderer_type(BEE_RENDERER_OPENGL3),
		is_vsync_enabled(false),
		is_basic_shaders_enabled(false),

		is_network_enabled(true),
		is_debug_enabled(false),

		should_assert(true),
		single_run(false)
	{}
	GameOptions(bool f, bool b, bool r, bool m, bool h, bool v, bee_renderer_t rend, bool vsync, bool bs, bool n, bool d) :
		is_fullscreen(f),
		is_borderless(b),
		is_resizable(r),
		is_maximized(m),
		is_highdpi(h),
		is_visible(v),
		is_minimized(false),

		renderer_type(rend),
		is_vsync_enabled(vsync),
		is_basic_shaders_enabled(bs),

		is_network_enabled(n),
		is_debug_enabled(d),

		should_assert(true),
		single_run(false)
	{}
};

struct BEE::RGBA {
	Uint8 r, g, b, a;

	RGBA() :
		r(0),
		g(0),
		b(0),
		a(0)
	{}
	RGBA(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na) :
		r(nr),
		g(ng),
		b(nb),
		a(na)
	{}
};

struct BEE::ViewData {
	bool is_visible;
	int view_x, view_y, view_width, view_height;
	int port_x, port_y, port_width, port_height;
	BEE::InstanceData* following;
	int horizontal_border, vertical_border;
	int horizontal_speed, vertical_speed;
};

struct BEE::SIDP { // This class can hold a string, integer, double, or pointer and is meant to allow multiple types in the same container
	int type; // Possible types: 0=string, 1=int, 2=double, 3=pointer
	std::string str;
	int integer;
	double floating;
	void* pointer;

	// Initialize the variable
	SIDP();
	SIDP(const std::string&);
	SIDP(const std::string&, bool);
	SIDP(int);
	SIDP(double);
	SIDP(void*);
	int reset();

	int interpret(const std::string&);
	std::string to_str();

	// Return the requested type
	std::string s(std::string, int);
	int i(std::string, int);
	double d(std::string, int);
	void* p(std::string, int);
	std::string s();
	int i();
	double d();
	void* p();

	SIDP& operator=(const SIDP&);
	SIDP& operator=(const std::string&);
	SIDP& operator=(int);
	SIDP& operator=(double);
	SIDP& operator=(void*);

	SIDP& operator+=(const SIDP&);
	SIDP& operator+=(int);

	SIDP& operator-=(const SIDP&);
	SIDP& operator-=(int);
};

struct BEE::NetworkData {
	bool is_initialized;
	UDPsocket udp_send;
	UDPsocket udp_recv;
	UDPpacket* udp_data;

	int id;
	bool is_connected;
	bool is_host;
	int channel;
	Uint32 timeout;
	std::map<std::string,std::string> servers;

	std::string name;
	unsigned int max_players;
	int self_id;
	std::map<int,UDPsocket> players;
	std::map<std::string,std::string> data;

	NetworkData() :
		is_initialized(false),
		udp_send(nullptr),
		udp_recv(nullptr),
		udp_data(nullptr),

		id(3054),
		is_connected(false),
		is_host(false),
		channel(-1),
		timeout(1000),
		servers(),

		name(),
		max_players(0),
		self_id(-1),
		players(),
		data()
	{}
};

struct BEE::MessageContents {
	bool has_processed;
	Uint32 tickstamp;
	std::vector<std::string> tags;
	bee_message_t type;
	std::string descr;
	std::shared_ptr<void> data;

	MessageContents() :
		has_processed(false),
		tickstamp(0),
		tags(),
		type(BEE_MESSAGE_GENERAL),
		descr(),
		data(nullptr)
	{}
	MessageContents(Uint32 tm, std::vector<std::string> tg, bee_message_t tp, std::string de, std::shared_ptr<void> da) :
		has_processed(false),
		tickstamp(tm),
		tags(tg),
		type(tp),
		descr(de),
		data(da)
	{}
};
struct BEE::MessageRecipient {
	std::string name;
	std::vector<std::string> tags;
	bool is_strict;
	std::function<void (BEE*, std::shared_ptr<MessageContents>)> func = nullptr;

	MessageRecipient() :
		name(),
		tags(),
		is_strict(false),
		func(nullptr)
	{}
	MessageRecipient(std::string n, std::vector<std::string> t, bool s, std::function<void (BEE*, std::shared_ptr<MessageContents>)> f) :
		name(n),
		tags(t),
		is_strict(s),
		func(f)
	{}
};

struct BEE::Console {
	bool is_open;

	std::unordered_map<std::string,std::pair<std::string,std::function<void (BEE*, std::shared_ptr<MessageContents>)>>> commands;
	std::unordered_map<std::string,std::string> aliases;
	std::unordered_map<std::string,SIDP> variables;

	std::map<SDL_Keycode,std::string> bindings;

	std::string input;

	std::vector<std::string> history;
	int history_index;

	std::stringstream log;
	size_t page_index;

	std::vector<std::string> completion_commands;
	int completion_index;
	std::string input_tmp;

	// Set the drawing sizes
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
	unsigned int line_height;

	TextData* td_log;

	Console();
	~Console();
};

#define SIDP_s(x) x.s(__FILE__, __LINE__)
#define SIDP_i(x) x.i(__FILE__, __LINE__)
#define SIDP_d(x) x.d(__FILE__, __LINE__)
#define SIDP_p(x) x.p(__FILE__, __LINE__)

#include "resources/sprite.hpp"
#include "resources/sound.hpp"
#include "resources/background.hpp"
#include "resources/font.hpp"
#include "resources/path.hpp"
#include "resources/timeline.hpp"
#include "resources/mesh.hpp"
#include "resources/object.hpp"
#include "resources/light.hpp"
#include "resources/ext/instancedata.hpp"
#include "resources/ext/physics.hpp"
#include "resources/ext/particle.hpp"
#include "resources/ext/soundgroup.hpp"
#include "resources/room.hpp"

#endif // _BEE_GAME_H
