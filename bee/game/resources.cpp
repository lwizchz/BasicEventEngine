/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_RESOURCES
#define _BEE_GAME_RESOURCES 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::load_media() - Load the media (i.e. sprites and backgrounds) for the current room
*/
int BEE::load_media() const {
	if (current_room != nullptr) { // If we are currently in a room, load its media
		current_room->load_media();
	}

	return 0; // Return 0 on success
}
/*
* BEE::free_media() - Free the media (i.e. sprites and backgrounds) for the current room
*/
int BEE::free_media() const {
	if (current_room != nullptr) { // If we are currently in a room, free its media
		current_room->free_media();
	}

	return 0; // Return 0 on success
}

/*
* BEE::sound_finished() - Called by Mix_ChannelFinished() whenever a channel finishes playback
* ! See https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_37.html for details
* @channel: the channel which has finished playback
*/
void BEE::sound_finished(int channel) {
	for (size_t i=0; i<resource_list->sounds.get_amount(); ++i) { // Iterate over the sounds in order to remove finished channels from each sound's list
		if (get_sound(i) != nullptr) {
			if (!get_sound(i)->get_is_music()) { // Music cannot be played on multiple channels
				get_sound(i)->finished(channel); // Remove the finished channel from the list
			}
		}
	}
}

/*
* BEE::get_volume() - Return the global sound volume from [0.0, 1.0]
*/
double BEE::get_volume() const {
	return volume;
}
/*
* BEE::set_volume() - Set a new global sound volume and update it for all currently playing sounds
* @new_volume: the new volume to use
*/
int BEE::set_volume(double new_volume) {
	volume = new_volume; // Set the volume

	for (size_t i=0; i<resource_list->sounds.get_amount(); ++i) { // Iterate over the sounds and update them to the new volume
		if (get_sound(i) != nullptr) {
			get_sound(i)->update_volume();
		}
	}

	return 0; // Return 0 on success
}
/*
* BEE::sound_stop_all() - Immediately stop all sound output
*/
int BEE::sound_stop_all() const {
	for (size_t i=0; i<resource_list->sounds.get_amount(); ++i) { // Iterate over the sounds and stop them individually
		if (get_sound(i) != nullptr) {
			get_sound(i)->stop();
		}
	}

	return 0; // Return 0 on success
}

/*
* BEE::add_sprite() - Initiliaze, load, and return a newly created sprite resource
* @name: the name to initialize the sprite with
* @path: the path to initialize the sprite with
*/
BEE::Sprite* BEE::add_sprite(const std::string& name, const std::string& path) {
	Sprite* new_sprite = new Sprite(name, path);
	new_sprite->load();
	return new_sprite;
}
/*
* BEE::add_sound() - Initiliaze, load, and return a newly created sound resource
* @name: the name to initialize the sound with
* @path: the path to initialize the sound with
* @is_music: whether the sound should be considered music or not
*/
BEE::Sound* BEE::add_sound(const std::string& name, const std::string& path, bool is_music) {
	Sound* new_sound = new Sound(name, path, is_music);
	new_sound->load();
	return new_sound;
}
/*
* BEE::add_background() - Initiliaze, load, and return a newly created background resource
* @name: the name to initialize the background with
* @path: the path to initialize the background with
*/
BEE::Background* BEE::add_background(const std::string& name, const std::string& path) {
	Background* new_background = new Background(name, path);
	new_background->load();
	return new_background;
}
/*
* BEE::add_font() - Initiliaze, load, and return a newly created font resource
* @name: the name to initialize the font with
* @path: the path to initialize the font with
* @size: the font size to initialize the font with
* @is_sprite: whether the font is a bitmap or not (i.e. ttf)
*/
BEE::Font* BEE::add_font(const std::string& name, const std::string& path, int size, bool is_sprite) {
	Font* new_font = new Font(name, path, size, is_sprite);
	new_font->load();
	return new_font;
}
/*
* BEE::add_path() - Initiliaze and return a newly created path resource
* @name: the name to initialize the path with
* @path: the path to initialize the path with
*/
BEE::Path* BEE::add_path(const std::string& name, const std::string& path) {
	Path* new_path = new Path(name, path);
	return new_path;
}
/*
* BEE::add_timeline() - Initiliaze and return a newly created timeline resource
* @name: the name to initialize the timeline with
* @path: the path to initialize the timeline with
*/
BEE::Timeline* BEE::add_timeline(const std::string& name, const std::string& path) {
	Timeline* new_timeline = new Timeline(name, path);
	return new_timeline;
}
/*
* BEE::add_mesh() - Initiliaze and return a newly created mesh resource
* @name: the name to initialize the mesh with
* @path: the path to initialize the mesh with
*/
BEE::Mesh* BEE::add_mesh(const std::string& name, const std::string& path) {
	Mesh* new_mesh = new Mesh(name, path);
	return new_mesh;
}
/*
* BEE::add_light() - Initiliaze and return a newly created light resource
* @name: the name to initialize the light with
* @path: the path to initialize the light with
*/
BEE::Light* BEE::add_light(const std::string& name, const std::string& path) {
	Light* new_light = new Light(name, path);
	return new_light;
}
/*
* BEE::add_object() - Initiliaze and return a newly created object resource
* @name: the name to initialize the object with
* @path: the path to initialize the object with
*/
BEE::Object* BEE::add_object(const std::string& name, const std::string& path) {
	/*Object* new_object = new Object(name, path);
	return new_object;*/

	return nullptr; // It doesn't really make sense to add a generic object on the fly
}
/*
* BEE::add_room() - Initiliaze, load, and return a newly created room resource
* @name: the name to initialize the room with
* @path: the path to initialize the room with
*/
BEE::Room* BEE::add_room(const std::string& name, const std::string& path) {
	/*Room* new_room = new Room(name, path);
	new_room->load();
	return new_room;*/

	return nullptr; // Right now rooms cannot be added on the fly because they must be compiled
}

/*
* BEE::get_sprite() - Return the sprite resource with the given id
* @id: the id of the desired sprite
*/
BEE::Sprite* BEE::get_sprite(int id) {
	return dynamic_cast<Sprite*>(resource_list->sprites.get_resource(id));
}
/*
* BEE::get_sound() - Return the sound resource with the given id
* @id: the id of the desired sound
*/
BEE::Sound* BEE::get_sound(int id) {
	return dynamic_cast<Sound*>(resource_list->sounds.get_resource(id));
}
/*
* BEE::get_background() - Return the background resource with the given id
* @id: the id of the desired background
*/
BEE::Background* BEE::get_background(int id) {
	return dynamic_cast<Background*>(resource_list->backgrounds.get_resource(id));
}
/*
* BEE::get_font() - Return the font resource with the given id
* @id: the id of the desired font
*/
BEE::Font* BEE::get_font(int id) {
	return dynamic_cast<Font*>(resource_list->fonts.get_resource(id));
}
/*
* BEE::get_path() - Return the path resource with the given id
* @id: the id of the desired path
*/
BEE::Path* BEE::get_path(int id) {
	return dynamic_cast<Path*>(resource_list->paths.get_resource(id));
}
/*
* BEE::get_timeline() - Return the timeline resource with the given id
* @id: the id of the desired timeline
*/
BEE::Timeline* BEE::get_timeline(int id) {
	return dynamic_cast<Timeline*>(resource_list->timelines.get_resource(id));
}
/*
* BEE::get_mesh() - Return the mesh resource with the given id
* @id: the id of the desired mesh
*/
BEE::Mesh* BEE::get_mesh(int id) {
	return dynamic_cast<Mesh*>(resource_list->meshes.get_resource(id));
}
/*
* BEE::get_light() - Return the light resource with the given id
* @id: the id of the desired light
*/
BEE::Light* BEE::get_light(int id) {
	return dynamic_cast<Light*>(resource_list->lights.get_resource(id));
}
/*
* BEE::get_object() - Return the object resource with the given id
* @id: the id of the desired object
*/
BEE::Object* BEE::get_object(int id) {
	return dynamic_cast<Object*>(resource_list->objects.get_resource(id));
}
/*
* BEE::get_room() - Return the room resource with the given id
* @id: the id of the desired room
*/
BEE::Room* BEE::get_room(int id) {
	return dynamic_cast<Room*>(resource_list->rooms.get_resource(id));
}

/*
* BEE::get_sprite_by_name() - Return the sprite resource with the given name
* @name: the name of the desired sprite
*/
BEE::Sprite* BEE::get_sprite_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->sprites.get_amount(); ++i) { // Iterate over the sprites in order to find the first one with the given name
		Sprite* s = get_sprite(i);
		if (s != nullptr) {
			if (s->get_name() == name) {
				return s; // Return the desired sprite on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_sound_by_name() - Return the sound resource with the given name
* @name: the name of the desired sound
*/
BEE::Sound* BEE::get_sound_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->sounds.get_amount(); ++i) { // Iterate over the sounds in order to find the first one with the given name
		Sound* s = get_sound(i);
		if (s != nullptr) {
			if (s->get_name() == name) {
				return s; // Return the desired sound on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_background_by_name() - Return the background resource with the given name
* @name: the name of the desired background
*/
BEE::Background* BEE::get_background_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->backgrounds.get_amount(); ++i) { // Iterate over the backgrounds in order to find the first one with the given name
		Background* b = get_background(i);
		if (b != nullptr) {
			if (b->get_name() == name) {
				return b; // Return the desired background on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_font_by_name() - Return the font resource with the given name
* @name: the name of the desired font
*/
BEE::Font* BEE::get_font_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->fonts.get_amount(); ++i) { // Iterate over the fonts in order to find the first one with the given name
		Font* f = get_font(i);
		if (f != nullptr) {
			if (f->get_name() == name) {
				return f; // Return the desired font on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_path_by_name() - Return the path resource with the given name
* @name: the name of the desired path
*/
BEE::Path* BEE::get_path_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->paths.get_amount(); ++i) { // Iterate over the paths in order to find the first one with the given name
		Path* f = get_path(i);
		if (f != nullptr) {
			if (f->get_name() == name) {
				return f; // Return the desired path on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_timeline_by_name() - Return the timeline resource with the given name
* @name: the name of the desired timeline
*/
BEE::Timeline* BEE::get_timeline_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->timelines.get_amount(); ++i) { // Iterate over the timelines in order to find the first one with the given name
		Timeline* f = get_timeline(i);
		if (f != nullptr) {
			if (f->get_name() == name) {
				return f; // Return the desired timeline on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_mesh_by_name() - Return the mesh resource with the given name
* @name: the name of the desired mesh
*/
BEE::Mesh* BEE::get_mesh_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->meshes.get_amount(); ++i) { // Iterate over the meshes in order to find the first one with the given name
		Mesh* f = get_mesh(i);
		if (f != nullptr) {
			if (f->get_name() == name) {
				return f; // Return the desired mesh on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_light_by_name() - Return the light resource with the given name
* @name: the name of the desired light
*/
BEE::Light* BEE::get_light_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->lights.get_amount(); ++i) { // Iterate over the lights in order to find the first one with the given name
		Light* f = get_light(i);
		if (f != nullptr) {
			if (f->get_name() == name) {
				return f; // Return the desired light on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_object_by_name() - Return the object resource with the given name
* @name: the name of the desired object
*/
BEE::Object* BEE::get_object_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->objects.get_amount(); ++i) { // Iterate over the objects in order to find the first one with the given name
		Object* o = get_object(i);
		if (o != nullptr) {
			if (o->get_name() == name) {
				return o; // Return the desired object on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}
/*
* BEE::get_room_by_name() - Return the room resource with the given name
* @name: the name of the desired room
*/
BEE::Room* BEE::get_room_by_name(const std::string& name) const {
	for (size_t i=0; i<resource_list->rooms.get_amount(); ++i) { // Iterate over the rooms in order to find the first one with the given name
		Room* f = get_room(i);
		if (f != nullptr) {
			if (f->get_name() == name) {
				return f; // Return the desired room on success
			}
		}
	}
	return nullptr; // Return nullptr on failure
}

#endif // _BEE_GAME_RESOURCES
