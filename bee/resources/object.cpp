/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
BEE::Object::Object (const std::string& new_name, const std::string& new_path) {
	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->objects.game != nullptr) {
		game = BEE::resource_list->objects.game;
	}

	reset();

	std::vector<bee_event_t> events = {BEE_EVENT_UPDATE, BEE_EVENT_CREATE, BEE_EVENT_DESTROY, BEE_EVENT_ALARM, BEE_EVENT_STEP_BEGIN, BEE_EVENT_STEP_MID, BEE_EVENT_STEP_END, BEE_EVENT_KEYBOARD_PRESS, BEE_EVENT_MOUSE_PRESS, BEE_EVENT_KEYBOARD_INPUT, BEE_EVENT_MOUSE_INPUT, BEE_EVENT_KEYBOARD_RELEASE, BEE_EVENT_MOUSE_RELEASE, BEE_EVENT_CONTROLLER_AXIS, BEE_EVENT_CONTROLLER_PRESS, BEE_EVENT_CONTROLLER_RELEASE, BEE_EVENT_CONTROLLER_MODIFY, BEE_EVENT_PATH_END, BEE_EVENT_OUTSIDE_ROOM, BEE_EVENT_INTERSECT_BOUNDARY, BEE_EVENT_COLLISION, BEE_EVENT_CHECK_COLLISION_LIST, BEE_EVENT_DRAW, BEE_EVENT_ANIMATION_END, BEE_EVENT_ROOM_START, BEE_EVENT_ROOM_END, BEE_EVENT_GAME_START, BEE_EVENT_GAME_END, BEE_EVENT_WINDOW};
	for (auto& e : events) {
		implemented_events[e] = false;
	}

	add_to_resources();
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add object resource: \"" + new_name + "\" from " + new_path);
		throw(-1);
	}

	set_name(new_name);
	set_path(new_path);
}
BEE::Object::~Object() {
	BEE::resource_list->objects.remove_resource(id);
}
int BEE::Object::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->objects.add_resource(this); // Add the resource and get the new id
	}

	return 0;
}
int BEE::Object::reset() {
	name = "";
	path = "";
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
int BEE::Object::print() const {
	std::string instance_string = get_instance_string();

	std::stringstream s;
	s <<
	"Object { "
	"\n	id            " << id <<
	"\n	name          " << name <<
	"\n	path          " << path;
	if (sprite != nullptr) {
		s << "\n	sprite        " << sprite->get_id() << ", " << sprite->get_name();
	} else {
		s << "\n	sprite        nullptr";
	}
	s <<
	"\n	is_solid      " << is_solid <<
	"\n	is_visible    " << is_visible <<
	"\n	is_persistent " << is_persistent <<
	"\n	depth		" << depth;
	if (parent != nullptr) {
		s << "\n	parent        " << parent->get_id() << ", " << parent->get_name();
	} else {
		s << "\n	parent        nullptr";
	}
	if (mask != nullptr) {
		s << "\n	mask          " << mask->get_id() << ", " << mask->get_name();
	} else {
		s << "\n	mask          nullptr";
	}
	s << "\n	instances\n" << debug_indent(instance_string, 2) <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}

int BEE::Object::get_id() const {
	return id;
}
std::string BEE::Object::get_name() const {
	return name;
}
std::string BEE::Object::get_path() const {
	return path;
}
BEE::Sprite* BEE::Object::get_sprite() const {
	return sprite;
}
int BEE::Object::get_sprite_id() const {
	return sprite->get_id();
}
bool BEE::Object::get_is_solid() const {
	return is_solid;
}
bool BEE::Object::get_is_visible() const {
	return is_visible;
}
bool BEE::Object::get_is_persistent() const {
	return is_persistent;
}
int BEE::Object::get_depth() const {
	return depth;
}
BEE::Object* BEE::Object::get_parent() const {
	return parent;
}
int BEE::Object::get_parent_id() const {
	return parent->get_id();
}
BEE::Sprite* BEE::Object::get_mask() const {
	return mask;
}
int BEE::Object::get_mask_id() const {
	return mask->get_id();
}
std::pair<int,int> BEE::Object::get_mask_offset() const {
	return std::make_pair(xoffset, yoffset);
}

int BEE::Object::set_name(const std::string& new_name) {
	name = new_name;
	return 0;
}
int BEE::Object::set_path(const std::string& new_path) {
	path = "resources/objects/"+new_path;
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

int BEE::Object::add_instance(int index, Instance* new_instance) {
	if (new_instance->get_object() != this) {
		return 1;
	}

	// Overwrite any previous instance with the same id
	instances.erase(index);
	instances.emplace(index, new_instance);

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
std::map<int, BEE::Instance*> BEE::Object::get_instances() const {
	return instances;
}
size_t BEE::Object::get_instance_amount() const {
	return instances.size();
}
BEE::Instance* BEE::Object::get_instance(int inst_id) const {
	int i = 0;
	for (auto& inst : instances) {
		if (i == inst_id) {
			return inst.second;
		}
		i++;
	}
	return nullptr;
}
std::string BEE::Object::get_instance_string() const {
	if (instances.size() > 0) {
		std::vector<std::vector<std::string>> table;
		table.push_back({"(id", "object", "x", "y", "z)"});

		for (auto& i : instances) {
			table.push_back({bee_itos(i.second->id), i.second->get_object()->get_name(), bee_itos(i.second->get_position()[0]), bee_itos(i.second->get_position()[1]), bee_itos(i.second->get_position()[2])});
		}

		return string_tabulate(table);
	}
	return "none\n";
}

BEE::SIDP BEE::Object::get_data(int inst_id, const std::string& field) const {
	if (instance_data.find(inst_id) != instance_data.end()) {
		if (instance_data.at(inst_id).find(field) != instance_data.at(inst_id).end()) {
			return instance_data.at(inst_id).at(field);
		}
	}
	return 1;
}
int BEE::Object::set_data(int inst_id, const std::string& field, SIDP data) {
	if (instance_data.find(inst_id) == instance_data.end()) {
		return 1;
	}
	instance_data.at(inst_id)[field] = data;
	return 0;
}

/*void BEE::Object::update(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->update(self);
}
void BEE::Object::create(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->create(self);
}
void BEE::Object::destroy(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->destroy(self);
}
void BEE::Object::alarm(Instance* self, int a) {
	reinterpret_cast<const ObjType*>(this)->alarm(a);
}
void BEE::Object::step_begin(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->step_begin(self);
}
void BEE::Object::step_mid(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->step_mid(self);
}
void BEE::Object::step_end(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->step_end(self);
}
void BEE::Object::keyboard_press(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->keyboard_press(self, e);
}
void BEE::Object::mouse_press(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->mouse_press(self, e);
}
void BEE::Object::keyboard_input(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->keyboard_input(self, e);
}
void BEE::Object::mouse_input(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->mouse_input(self, e);
}
void BEE::Object::keyboard_release(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->keyboard_release(self, e);
}
void BEE::Object::mouse_release(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->mouse_release(self, e);
}
void BEE::Object::controller_axis(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_axis(self, e);
}
void BEE::Object::controller_press(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_press(self, e);
}
void BEE::Object::controller_release(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_release(self, e);
}
void BEE::Object::controller_modify(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->controller_modify(self, e);
}
void BEE::Object::commandline_input(Instance* self, const std::string& str) {
	reinterpret_cast<const ObjType*>(this)->commandline_input(self, str);
}
void BEE::Object::path_end(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->path_end(self);
}
void BEE::Object::outside_room(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->outside_room(self);
}
void BEE::Object::intersect_boundary(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->intersect_boundary(self);
}
void BEE::Object::collision(Instance* self, Instance* other) {
	reinterpret_cast<const ObjType*>(this)->collision(self, other);
}
bool BEE::Object::check_collision_list(const Instance& self, const Instance& other) const {
	reinterpret_cast<const ObjType*>(this)->check_collision_list(self, other);
}
void BEE::Object::draw(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->draw(self);
}
void BEE::Object::animation_end(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->animation_end(self);
}
void BEE::Object::room_start(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->room_start(self);
}
void BEE::Object::room_end(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->room_end(self);
}
void BEE::Object::game_start(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->game_start(self);
}
void BEE::Object::game_end(Instance* self) {
	reinterpret_cast<const ObjType*>(this)->game_end(self);
}
void BEE::Object::window(Instance* self, SDL_Event* e) {
	reinterpret_cast<const ObjType*>(this)->window(self, e);
}*/

#endif // _BEE_OBJECT
