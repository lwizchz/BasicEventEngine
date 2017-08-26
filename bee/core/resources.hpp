/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_RESOURCES_H
#define BEE_CORE_RESOURCES_H 1

// Include the required library headers
#include <string>

namespace bee { // The engine namespace
	// Forward declarations
	class Sprite;
	class Sound;
	class Background;
	class Font;
	class Path;
	class Timeline;
	class Mesh;
	class Light;
	class Object;
	class Room;

	void sound_finished(int);

	double get_volume();
	int set_volume(double);
	int sound_stop_loops();
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
}

#endif // BEE_CORE_RESOURCES_H
