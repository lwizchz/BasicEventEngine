/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RESOURCE_STRUCTURES_H
#define _BEE_RESOURCE_STRUCTURES_H 1

#include <iostream>
#include <string>
#include <map>

#include "debug.hpp"

class Resource {
	public:
		BEE* game = NULL;

		virtual ~Resource() {};
		virtual int reset() =0;
		virtual int print() =0;
		virtual int get_id() =0;
		virtual std::string get_name() =0;
		virtual std::string get_path() =0;
		virtual int set_name(std::string) =0;
		virtual int set_path(std::string) =0;
};
class ResourceList {
	public:
		std::map<int,Resource*> resources;
		int next_index;

		ResourceList();
		int reset();
		int add_resource(Resource*);
		int set_resource(int, Resource*);
		Resource* get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites, sounds, backgrounds, fonts, paths, objects, rooms; //, all;
} resource_list;

ResourceList::ResourceList() {
	next_index = 0;
}
int ResourceList::reset() {
	resources.clear();
	next_index = 0;
	return 0;
}
int ResourceList::add_resource(Resource* new_resource) {
	int index = next_index;
	std::pair<std::map<int,Resource*>::iterator,bool> ret;
	ret = resources.insert(std::pair<int,Resource*>(index, new_resource));
	if (ret.second) {
		next_index++;
		return index;
	}
	return -1;
}
int ResourceList::set_resource(int id, Resource* resource) {
	resources.at(id) = resource;
	return 0;
}
Resource* ResourceList::get_resource(int id) {
	if (resources.find(id) != resources.end()) {
		return resources.at(id);
	}
	return NULL;
}
int ResourceList::get_amount() {
	return next_index; //resources.size();
}
int ResourceList::remove_resource(int id) {
	resources.erase(id);
	return 0;
}

Sprite* get_sprite(int id);
Sound* get_sound(int id);
Background* get_background(int id);
Font* get_font(int id);
Path* get_path(int id);
Object* get_object(int id);
Room* get_room(int id);

#include "resource_structures/sprite.hpp"
#include "resource_structures/sound.hpp"
#include "resource_structures/background.hpp"
#include "resource_structures/font.hpp"
#include "resource_structures/path.hpp"
#include "resource_structures/object.hpp"
#include "resource_structures/room.hpp"

Sprite* get_sprite(int id) {
	return dynamic_cast<Sprite*>(resource_list.sprites.get_resource(id));
}
Sound* get_sound(int id) {
	return dynamic_cast<Sound*>(resource_list.sounds.get_resource(id));
}
Background* get_background(int id) {
	return dynamic_cast<Background*>(resource_list.backgrounds.get_resource(id));
}
Font* get_font(int id) {
	return dynamic_cast<Font*>(resource_list.fonts.get_resource(id));
}
Path* get_path(int id) {
	return dynamic_cast<Path*>(resource_list.paths.get_resource(id));
}
Object* get_object(int id) {
	return dynamic_cast<Object*>(resource_list.objects.get_resource(id));
}
Room* get_room(int id) {
	return dynamic_cast<Room*>(resource_list.rooms.get_resource(id));
}

#endif // _BEE_RESOURCE_STRUCTURES_H
