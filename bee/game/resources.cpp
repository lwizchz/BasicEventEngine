/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_RESOURCES
#define _BEE_GAME_RESOURCES 1

#include "../game.hpp"

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

void BEE::sound_finished(int channel) {
	for (int i=0; i<resource_list->sounds.get_amount(); i++) {
		if (get_sound(i) != NULL) {
			if (!get_sound(i)->get_is_music()) {
				get_sound(i)->finished(channel);
			}
		}
	}
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

BEE::Sprite* BEE::add_sprite(std::string name, std::string path) {
	Sprite* new_sprite = new Sprite(name, path);
	new_sprite->game = this;
	new_sprite->load();
	return new_sprite;
}
BEE::Sound* BEE::add_sound(std::string name, std::string path, bool type) {
	Sound* new_sound = new Sound(name, path, type);
	new_sound->game = this;
	new_sound->load();
	return new_sound;
}
BEE::Background* BEE::add_background(std::string name, std::string path) {
	Background* new_background = new Background(name, path);
	new_background->game = this;
	new_background->load();
	return new_background;
}
BEE::Font* BEE::add_font(std::string name, std::string path, int size) {
	Font* new_font = new Font(name, path, size);
	new_font->game = this;
	new_font->load();
	return new_font;
}
BEE::Path* BEE::add_path(std::string name, std::string path) {
	Path* new_path = new Path(name, path);
	new_path->game = this;
	//new_path->load();
	return new_path;
}
BEE::Object* BEE::add_object(std::string name, std::string path) {
	Object* new_object = new Object(name, path);
	new_object->game = this;
	//new_object->load();
	return new_object;
}
/*BEE::Room* BEE::add_room(std::string name, std::string path) {
	Room* new_room = new Room(name, path);
	new_room->game = this;
	//new_room->load();
	return new_room;
}*/

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

BEE::Object* BEE::get_object_by_name(std::string name) {
	for (int i=0; i<resource_list->objects.get_amount(); i++) {
		if (get_object(i) != NULL) {
			if (get_object(i)->get_name() == name) {
				return get_object(i);
			}
		}
	}
	return NULL;
}

#endif // _BEE_GAME_RESOURCES
