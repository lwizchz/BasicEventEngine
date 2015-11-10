/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_OBJECT
#define _BEE_OBJECT 1

#include "object.hpp"

BEE::Object::Object () {
	reset();
}
BEE::Object::Object (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/objects/"+path);
	if (id < 0) {
		std::cerr << "Failed to add object resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Object::~Object() {
	BEE::resource_list->objects.remove_resource(id);
}
int BEE::Object::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == object_path) {
			return 1;
		}
		BEE::resource_list->objects.remove_resource(id);
		id = -1;
	} else {
		for (auto i : BEE::resource_list->objects.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list->objects.add_resource(this);
	}
	BEE::resource_list->objects.set_resource(id, this);

	return 0;
}
int BEE::Object::reset() {
	name = "";
	object_path = "";
	sprite = NULL;
	is_solid = false;
	is_visible = true;
	is_persistent = false;
	depth = 0;
	parent = NULL;
	mask = NULL;

	instances.clear();

	return 0;
}
int BEE::Object::print() {
	std::string instance_string = get_instance_string();

	std::cout <<
	"Object { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	object_path	" << object_path;
	if (sprite != NULL) {
		std::cout << "\n	sprite		" << sprite->get_id() << ", " << sprite->get_name();
	} else {
		std::cout << "\n	sprite		NULL";
	}
	std::cout <<
	"\n	is_solid	" << is_solid <<
	"\n	is_visible	" << is_visible <<
	"\n	is_persistent	" << is_persistent <<
	"\n	depth		" << depth;
	if (parent != NULL) {
		std::cout << "\n	parent		" << parent->get_id() << ", " << parent->get_name();
	} else {
		std::cout << "\n	parent		NULL";
	}
	if (mask != NULL) {
		std::cout << "\n	mask		" << mask->get_id() << ", " << mask->get_name();
	} else {
		std::cout << "\n	mask		NULL";
	}
	std::cout << "\n	instances\n" << debug_indent(instance_string, 2) <<
	"\n}\n";

	return 0;
}

int BEE::Object::get_id() {
	return id;
}
std::string BEE::Object::get_name() {
	return name;
}
std::string BEE::Object::get_path() {
	return object_path;
}
BEE::Sprite* BEE::Object::get_sprite() {
	return sprite;
}
int BEE::Object::get_sprite_id() {
	return sprite->get_id();
}
bool BEE::Object::get_is_solid() {
	return is_solid;
}
bool BEE::Object::get_is_visible() {
	return is_visible;
}
bool BEE::Object::get_is_persistent() {
	return is_persistent;
}
int BEE::Object::get_depth() {
	return depth;
}
BEE::Object* BEE::Object::get_parent() {
	return parent;
}
int BEE::Object::get_parent_id() {
	return parent->get_id();
}
BEE::Sprite* BEE::Object::get_mask() {
	return mask;
}
int BEE::Object::get_mask_id() {
	return mask->get_id();
}
int BEE::Object::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Object::set_path(std::string path) {
	add_to_resources("resources/objects/"+path);
	object_path = "resources/objects/"+path;
	return 0;
}
int BEE::Object::set_sprite(Sprite* new_sprite) {
	sprite = new_sprite;
	if (mask == NULL) {
		mask = new_sprite;
	}
	return 0;
}
int BEE::Object::set_is_solid(bool new_is_solid) {
	is_solid = new_is_solid;
	return 0;
}
int BEE::Object::set_is_visible(bool new_is_visible) {
	is_visible = new_is_visible;
	return 0;
}
int BEE::Object::set_is_persistent(bool new_is_persistent) {
	is_persistent = new_is_persistent;
	return 0;
}
int BEE::Object::set_depth(int new_depth) {
	depth = new_depth;
	return 0;
}
int BEE::Object::set_parent(Object* new_parent) {
	parent = new_parent;
	return 0;
}
int BEE::Object::set_mask(Sprite* new_mask) {
	mask = new_mask;
	return 0;
}

int BEE::Object::add_instance(int index, InstanceData* new_instance) {
	if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
		instances.erase(index);
	}
	instances.insert(std::pair<int,InstanceData*>(index, new_instance));
	return 0;
}
int BEE::Object::remove_instance(int index) {
	instances.erase(index);
	for (unsigned int i=index; i<instances.size(); i++) {
		if (instances.find(i)++ != instances.end()) {
			instances[i] = instances[i+1];
		}
	}
	return 0;
}
int BEE::Object::clear_instances() {
	instances.clear();
	return 0;
}
std::map<int, BEE::InstanceData*> BEE::Object::get_instances() {
	return instances;
}
BEE::InstanceData* BEE::Object::get_instance(int inst_id) {
	int i = 0;
	for (auto& inst : instances) {
		if (i == inst_id) {
			return inst.second;
		}
		i++;
	}
	return NULL;
}
std::string BEE::Object::get_instance_string() {
	if (instances.size() > 0) {
		std::ostringstream instance_string;
		instance_string << "(id	object	x	y)\n";
		for (auto& i : instances) {
			instance_string <<
			i.second->id << "\t" <<
			i.second->object->get_name() << "\t" <<
			i.second->x << "\t" <<
			i.second->y << "\n";
		}

		return instance_string.str();
	}
	return "none\n";
}

#endif // _BEE_OBJECT
