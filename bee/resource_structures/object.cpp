/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_OBJECT
#define _BEE_OBJECT 1

#include "object.hpp"

BEE::Object::Object () {
	if (BEE::resource_list->objects.game != nullptr) {
		game = BEE::resource_list->objects.game;
	}

	reset();
}
BEE::Object::Object (std::string new_name, std::string path) {
	reset();

	std::vector<bee_event_t> events = {BEE_EVENT_UPDATE, BEE_EVENT_CREATE, BEE_EVENT_DESTROY, BEE_EVENT_ALARM, BEE_EVENT_STEP_BEGIN, BEE_EVENT_STEP_MID, BEE_EVENT_STEP_END, BEE_EVENT_KEYBOARD_PRESS, BEE_EVENT_MOUSE_PRESS, BEE_EVENT_KEYBOARD_INPUT, BEE_EVENT_MOUSE_INPUT, BEE_EVENT_KEYBOARD_RELEASE, BEE_EVENT_MOUSE_RELEASE, BEE_EVENT_CONTROLLER_AXIS, BEE_EVENT_CONTROLLER_PRESS, BEE_EVENT_CONTROLLER_RELEASE, BEE_EVENT_CONTROLLER_MODIFY, BEE_EVENT_PATH_END, BEE_EVENT_OUTSIDE_ROOM, BEE_EVENT_INTERSECT_BOUNDARY, BEE_EVENT_COLLISION, BEE_EVENT_CHECK_COLLISION_LIST, BEE_EVENT_DRAW, BEE_EVENT_ANIMATION_END, BEE_EVENT_ROOM_START, BEE_EVENT_ROOM_END, BEE_EVENT_GAME_START, BEE_EVENT_GAME_END, BEE_EVENT_WINDOW};
	for (auto& e : events) {
		implemented_events[e] = false;
	}

	add_to_resources("resources/objects/"+path);
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add object resource: " + path);
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
	}

	id = BEE::resource_list->objects.add_resource(this);
	BEE::resource_list->objects.set_resource(id, this);

	if (BEE::resource_list->objects.game != nullptr) {
		game = BEE::resource_list->objects.game;
	}

	return 0;
}
int BEE::Object::reset() {
	name = "";
	object_path = "";
	sprite = nullptr;
	is_solid = false;
	is_visible = true;
	is_persistent = false;
	depth = 0;
	parent = nullptr;
	mask = nullptr;
	xoffset = 0;
	yoffset = 0;

	instances.clear();

	return 0;
}
int BEE::Object::print() {
	std::string instance_string = get_instance_string();

	std::stringstream s;
	s <<
	"Object { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	object_path	" << object_path;
	if (sprite != nullptr) {
		s << "\n	sprite		" << sprite->get_id() << ", " << sprite->get_name();
	} else {
		s << "\n	sprite		nullptr";
	}
	s <<
	"\n	is_solid	" << is_solid <<
	"\n	is_visible	" << is_visible <<
	"\n	is_persistent	" << is_persistent <<
	"\n	depth		" << depth;
	if (parent != nullptr) {
		s << "\n	parent		" << parent->get_id() << ", " << parent->get_name();
	} else {
		s << "\n	parent		nullptr";
	}
	if (mask != nullptr) {
		s << "\n	mask		" << mask->get_id() << ", " << mask->get_name();
	} else {
		s << "\n	mask		nullptr";
	}
	s << "\n	instances\n" << debug_indent(instance_string, 2) <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

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
std::pair<int,int> BEE::Object::get_mask_offset() {
	return std::make_pair(xoffset, yoffset);
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
	if (mask == nullptr) {
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

	for (auto i : instances) {
		i.second->depth = depth;
	}

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
int BEE::Object::set_mask_offset(std::pair<int,int> new_offset) {
	std::tie(xoffset, yoffset) = new_offset;
	return 0;
}
int BEE::Object::set_mask_offset(int new_xoffset, int new_yoffset) {
	xoffset = new_xoffset;
	yoffset = new_yoffset;
	return 0;
}

int BEE::Object::add_instance(int index, InstanceData* new_instance) {
	if (new_instance->object != this) {
		return 1;
	}

	if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
		instances.erase(index);
	}
	instances.insert(std::pair<int,InstanceData*>(index, new_instance));
	return 0;
}
int BEE::Object::remove_instance(int index) {
	instances.erase(index);
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
	return nullptr;
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
