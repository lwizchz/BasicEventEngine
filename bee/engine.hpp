/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENGINE_H
#define BEE_ENGINE_H 1

// Include the required library headers
#include <string>
#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <glm/glm.hpp> // Include the required OpenGL headers

#include "defines.hpp"
#include "enum.hpp" // Include the required engine enumerations

namespace bee { // The engine namespace
	struct EngineState; // The struct which holds the main game state

	class Resource;
	class ResourceList;
	class MetaResourceList;

	class Sprite; class Sound; class Background; class Font; class Path; class Timeline; class Mesh; class Object; class Room; // The main resource types
	class Particle; class ParticleData; class ParticleEmitter; class ParticleAttractor; class ParticleDestroyer; class ParticleDeflector; class ParticleChanger; class ParticleSystem; // The particle system components
	class Renderer; class Light; class Camera; // The OpenGL-only resources (poor SDL implementations may exist)
	class ProgramFlags; class GameOptions; class RGBA; // The engine related data
	class Instance; // The additional resource data types
	class PhysicsWorld; class PhysicsDraw; class PhysicsBody; // The classes which interface with the external Physics library
	struct SpriteDrawData; class TextData; class LightData; class LightableData; // Additional resource-related data structs

	struct ViewData; struct BackgroundData; // The configurational structs
	struct NetworkData; struct NetworkClient; // The networking subsystem structs
	struct MessageContents; struct MessageRecipient; // The messaging subsystem structs
	struct SIDP; // The utility structs
	struct Console; // The console subsystem structs

	extern EngineState* engine;
	extern MetaResourceList* resource_list;
	extern bool is_initialized;

	// User defined in resources/resources.hpp
	int init_resources();
	int close_resources();

	int update_delta();

	// bee/init/programflags.cpp
	std::list<ProgramFlags*>& get_standard_flags_internal();

	// bee/core/messenger/messenger.cpp
	int messenger_register_protected(std::shared_ptr<MessageRecipient>);
	std::shared_ptr<MessageRecipient> messenger_register_protected(std::string, const std::vector<std::string>&, bool, std::function<void (std::shared_ptr<MessageContents>)>);
	int messenger_unregister_protected(std::shared_ptr<MessageRecipient>);
	int messenger_send_urgent(std::shared_ptr<MessageContents>);

	// bee/core/console.cpp
	int console_handle_input(SDL_Event*);
	int console_init_commands();
	int console_run_internal(const std::string&, bool, Uint32);
	int console_run(const std::string&, bool, Uint32);
	int console_complete(const std::string&);
	std::vector<SIDP> console_parse_parameters(const std::string&);
	int console_draw();

	int init(int, char**, const std::list<ProgramFlags*>&, Room**, GameOptions*);
	int loop();
	int close();

	// bee/core/resources.cpp
	int load_media();
	int free_media();

	void sound_finished(int);

	double get_volume();
	int set_volume(double);
	int sound_stop_all();

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

	Sprite* get_sprite(int);
	Sound* get_sound(int);
	Background* get_background(int);
	Font* get_font(int);
	Path* get_path(int);
	Timeline* get_timeline(int);
	Mesh* get_mesh(int);
	Light* get_light(int);
	Object* get_object(int);
	Room* get_room(int);

	Sprite* get_sprite_by_name(const std::string&);
	Sound* get_sound_by_name(const std::string&);
	Background* get_background_by_name(const std::string&);
	Font* get_font_by_name(const std::string&);
	Path* get_path_by_name(const std::string&);
	Timeline* get_timeline_by_name(const std::string&);
	Mesh* get_mesh_by_name(const std::string&);
	Light* get_light_by_name(const std::string&);
	Object* get_object_by_name(const std::string&);
	Room* get_room_by_name(const std::string&);

	// bee/engine.cpp
	Uint32 get_ticks();
	Uint32 get_seconds();
	Uint32 get_frame();
	double get_delta();
	Uint32 get_tick_delta();
	unsigned int get_fps_goal();

	GameOptions get_options();
	int set_options(const GameOptions&);

	int restart_game();
	int end_game();

	// bee/init/info.cpp
	std::string get_usage_text();

	// bee/init/programflags.cpp
	int handle_flags(const std::list<ProgramFlags*>&, bool);
	std::list<ProgramFlags*> get_standard_flags();
	int free_standard_flags();

	// bee/core/room.cpp
	void restart_room();
	int change_room(Room*, bool);
	int change_room(Room*);
	int room_goto(int);
	int room_goto_previous();
	int room_goto_next();

	Room* get_current_room();
	bool get_is_ready();
	int get_room_width();
	int get_room_height();
	bool is_on_screen(const SDL_Rect&);

	int set_viewport(ViewData*);

	bool set_is_paused(bool);
	bool get_is_paused();

	// bee/core/transition.cpp
	int reset_render_target();
	int set_render_target(Sprite*, int, int);
	int set_render_target(Sprite*);
	int set_render_target(Background*, int, int);
	int set_render_target(Background*);

	E_TRANSITION get_transition_type();
	int set_transition_type(E_TRANSITION);
	int set_transition_custom(std::function<void (Sprite*, Sprite*)>);
	double get_transition_speed();
	int set_transition_speed(double);
	int draw_transition();
	bool compute_check_quit();

	// bee/core/display.cpp
	SDL_DisplayMode get_display();
	Uint32 get_display_format();
	int get_display_width();
	int get_display_height();
	int get_display_refresh_rate();
	int set_display(int, int, int);
	int set_display_size(int, int);
	int set_display_refresh_rate(int);

	// bee/core/window.cpp
	std::string get_window_title();
	SDL_Cursor* get_cursor();
	int get_window_x();
	int get_window_y();
	int get_width();
	int get_height();

	int set_window_title(const std::string&);
	int set_cursor(SDL_SystemCursor);
	int set_show_cursor(bool);
	int set_window_position(int, int);
	int set_window_x(int);
	int set_window_y(int);
	int set_window_center();
	int set_window_size(int, int);
	int set_width(int);
	int set_height(int);

	// bee/core/input.cpp
	std::pair<int,int> get_mouse_global_position();
	int get_mouse_global_x();
	int get_mouse_global_y();
	std::pair<int,int> get_mouse_position();
	int get_mouse_x();
	int get_mouse_y();
	int set_mouse_global_position(int, int);
	int set_mouse_global_x(int);
	int set_mouse_global_y(int);
	bool is_mouse_inside(const Instance&);
	bool get_key_state(SDL_Scancode);
	bool get_key_state(SDL_Keycode);
	bool get_mod_state(Uint8);
	bool get_mouse_state(Uint8);
	char append_input(std::string*, SDL_KeyboardEvent*);
	int keystrings_populate();
	SDL_Keycode keystrings_get_key(const std::string&);
	std::string keystrings_get_string(SDL_Keycode);

	// bee/render/draw.cpp
	RGBA get_enum_color(E_RGB, Uint8);
	RGBA get_enum_color(E_RGB);

	int draw_triangle(glm::vec3, glm::vec3, glm::vec3, const RGBA&, bool);
	int draw_line(glm::vec3, glm::vec3, const RGBA&);
	int draw_line(int, int, int, int, const RGBA&);
	int draw_line(int, int, int, int);
	int draw_line(int, int, int, int, E_RGB);
	int draw_quad(glm::vec3, glm::vec3, bool, const RGBA&);
	int draw_rectangle(int, int, int, int, bool, const RGBA&);
	int draw_rectangle(int, int, int, int, bool);
	int draw_rectangle(int, int, int, int, bool, E_RGB);
	int draw_rectangle(const SDL_Rect&, bool, const RGBA&);

	int draw_set_color(const RGBA&);
	int draw_set_color(E_RGB);
	RGBA draw_get_color();
	int draw_set_blend(SDL_BlendMode);
	SDL_BlendMode draw_get_blend();

	RGBA get_pixel_color(int, int);
	int save_screenshot(const std::string&);

	int set_is_lightable(bool);

	int render_set_3d(bool);
	int render_set_camera(Camera*);
	bool render_get_3d();
	glm::mat4 render_get_projection();
	glm::mat4 render_calc_projection();
	Camera render_get_camera();

	// bee/core/messenger/messenger.cpp
	int messenger_register(std::shared_ptr<MessageRecipient>);
	std::shared_ptr<MessageRecipient> messenger_register(std::string, const std::vector<std::string>&, bool, std::function<void (std::shared_ptr<MessageContents>)>);
	std::shared_ptr<MessageRecipient> messenger_register(const std::vector<std::string>&, std::function<void (std::shared_ptr<MessageContents>)>);
	int messenger_unregister(std::shared_ptr<MessageRecipient>);
	int messenger_unregister_name(const std::string&);
	int messenger_unregister_all();

	int messenger_send(std::shared_ptr<MessageContents>);
	int messenger_send(const std::vector<std::string>&, E_MESSAGE, const std::string&, std::shared_ptr<void>);
	int messenger_send(const std::vector<std::string>&, E_MESSAGE, const std::string&);

	int messenger_set_level(E_OUTPUT);
	E_OUTPUT messenger_get_level();

	int handle_messages();
	std::string messenger_get_type_string(E_MESSAGE);

	// bee/core/network/network.cpp
	int net_init();
	bool net_get_is_initialized();
	int net_close();
	int net_handle_events();
	const NetworkData& net_get_data();

	int net_session_start(const std::string&, int, const std::string&);
	std::map<std::string,std::string> net_session_find();
	int net_session_join(const std::string&, const std::string&);
	bool net_get_is_connected();
	int net_session_end();

	int net_session_sync_data(const std::string&, const std::string&);
	int net_session_sync_instance(Instance*);

	// bee/core/console.cpp
	int console_open();
	int console_close();
	int console_toggle();
	bool console_get_is_open();

	int console_add_command(const std::string&, const std::string&, std::function<void (std::shared_ptr<MessageContents>)>);
	int console_add_command(const std::string&, std::function<void (std::shared_ptr<MessageContents>)>);
	std::string console_bind(SDL_Keycode, const std::string&);
	std::string console_bind(SDL_Keycode);
	int console_unbind(SDL_Keycode);
	int console_unbind_all();
	int console_alias(const std::string&, const std::string&);
	const std::unordered_map<std::string,std::string>& console_get_aliases();
	int console_set_var(const std::string&, SIDP);
	SIDP console_get_var(const std::string&);

	int console_run(const std::string&);
	std::string console_get_help(const std::string&);
}

#endif // BEE_ENGINE_H
