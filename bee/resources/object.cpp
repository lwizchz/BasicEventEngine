/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJECT
#define BEE_OBJECT 1

#include <sstream>

#include "object.hpp"

#include "sprite.hpp"

#include "../debug.hpp"
#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"

#include "../core/instance.hpp"
#include "../core/messenger/messenger.hpp"

namespace bee {
	Object::Object () {
		reset();
	}
	Object::Object (const std::string& new_name, const std::string& new_path) {
		reset();

		std::vector<E_EVENT> events = {
			E_EVENT::CREATE,
			E_EVENT::DESTROY,
			E_EVENT::ALARM,
			E_EVENT::STEP_BEGIN,
			E_EVENT::STEP_MID,
			E_EVENT::STEP_END,
			E_EVENT::KEYBOARD_PRESS,
			E_EVENT::MOUSE_PRESS,
			E_EVENT::KEYBOARD_INPUT,
			E_EVENT::MOUSE_INPUT,
			E_EVENT::KEYBOARD_RELEASE,
			E_EVENT::MOUSE_RELEASE,
			E_EVENT::CONTROLLER_AXIS,
			E_EVENT::CONTROLLER_PRESS,
			E_EVENT::CONTROLLER_RELEASE,
			E_EVENT::CONTROLLER_MODIFY,
			E_EVENT::COMMANDLINE_INPUT,
			E_EVENT::PATH_END,
			E_EVENT::OUTSIDE_ROOM,
			E_EVENT::INTERSECT_BOUNDARY,
			E_EVENT::COLLISION,
			E_EVENT::DRAW,
			E_EVENT::ANIMATION_END,
			E_EVENT::ROOM_START,
			E_EVENT::ROOM_END,
			E_EVENT::GAME_START,
			E_EVENT::GAME_END,
			E_EVENT::WINDOW
		};
		for (auto& e : events) {
			implemented_events[e] = false;
		}

		add_to_resources();
		if (id < 0) {
			messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add object resource: \"" + new_name + "\" from " + new_path);
			throw(-1);
		}

		set_name(new_name);
		set_path(new_path);
	}
	Object::~Object() {
		resource_list->objects.remove_resource(id);
	}
	int Object::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = resource_list->objects.add_resource(this); // Add the resource and get the new id
		}

		return 0;
	}
	int Object::reset() {
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
		current_instance = nullptr;

		return 0;
	}
	int Object::print() const {
		std::string instance_string = get_instance_string();

		std::stringstream ss;
		ss <<
		"Object { "
		"\n	id            " << id <<
		"\n	name          " << name <<
		"\n	path          " << path;
		if (sprite != nullptr) {
			ss << "\n	sprite        " << sprite->get_id() << ", " << sprite->get_name();
		} else {
			ss << "\n	sprite        nullptr";
		}
		ss <<
		"\n	is_solid      " << is_solid <<
		"\n	is_visible    " << is_visible <<
		"\n	is_persistent " << is_persistent <<
		"\n	depth         " << depth;
		if (parent != nullptr) {
			ss << "\n	parent        " << parent->get_id() << ", " << parent->get_name();
		} else {
			ss << "\n	parent        nullptr";
		}
		if (mask != nullptr) {
			ss << "\n	mask          " << mask->get_id() << ", " << mask->get_name();
		} else {
			ss << "\n	mask          nullptr";
		}
		ss <<
		"\n	instances\n" << debug_indent(instance_string, 2) <<
		"\n}\n";
		messenger_send({"engine", "resource"}, E_MESSAGE::INFO, ss.str());

		return 0;
	}

	int Object::get_id() const {
		return id;
	}
	std::string Object::get_name() const {
		return name;
	}
	std::string Object::get_path() const {
		return path;
	}
	Sprite* Object::get_sprite() const {
		return sprite;
	}
	int Object::get_sprite_id() const {
		return sprite->get_id();
	}
	bool Object::get_is_solid() const {
		return is_solid;
	}
	bool Object::get_is_visible() const {
		return is_visible;
	}
	bool Object::get_is_persistent() const {
		return is_persistent;
	}
	int Object::get_depth() const {
		return depth;
	}
	Object* Object::get_parent() const {
		return parent;
	}
	int Object::get_parent_id() const {
		return parent->get_id();
	}
	Sprite* Object::get_mask() const {
		return mask;
	}
	int Object::get_mask_id() const {
		return mask->get_id();
	}
	std::pair<int,int> Object::get_mask_offset() const {
		return std::make_pair(xoffset, yoffset);
	}

	int Object::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Object::set_path(const std::string& new_path) {
		path = "resources/objects/"+new_path;
		return 0;
	}
	int Object::set_sprite(Sprite* new_sprite) {
		sprite = new_sprite;
		if (mask == nullptr) {
			mask = new_sprite;
		}
		return 0;
	}
	int Object::set_is_solid(bool new_is_solid) {
		is_solid = new_is_solid;
		return 0;
	}
	int Object::set_is_visible(bool new_is_visible) {
		is_visible = new_is_visible;
		return 0;
	}
	int Object::set_is_persistent(bool new_is_persistent) {
		is_persistent = new_is_persistent;
		return 0;
	}
	int Object::set_depth(int new_depth) {
		depth = new_depth;

		for (auto i : instances) {
			i.second->depth = depth;
		}

		return 0;
	}
	int Object::set_parent(Object* new_parent) {
		parent = new_parent;
		return 0;
	}
	int Object::set_mask(Sprite* new_mask) {
		mask = new_mask;
		return 0;
	}
	int Object::set_mask_offset(std::pair<int,int> new_offset) {
		std::tie(xoffset, yoffset) = new_offset;
		return 0;
	}
	int Object::set_mask_offset(int new_xoffset, int new_yoffset) {
		xoffset = new_xoffset;
		yoffset = new_yoffset;
		return 0;
	}

	int Object::add_instance(int index, Instance* new_instance) {
		if (new_instance->get_object() != this) {
			return 1;
		}

		// Overwrite any previous instance with the same id
		instances.erase(index);
		instances.emplace(index, new_instance);

		return 0;
	}
	int Object::remove_instance(int index) {
		instances.erase(index);
		return 0;
	}
	int Object::clear_instances() {
		instances.clear();
		return 0;
	}
	std::map<int, Instance*> Object::get_instances() const {
		return instances;
	}
	size_t Object::get_instance_amount() const {
		return instances.size();
	}
	Instance* Object::get_instance(int inst_id) const {
		int i = 0;
		for (auto& inst : instances) {
			if (i == inst_id) {
				return inst.second;
			}
			i++;
		}
		return nullptr;
	}
	std::string Object::get_instance_string() const {
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

	SIDP Object::get_data(int inst_id, const std::string& field, const SIDP& default_value, bool should_output) const {
		int error_type = 0;

		if (instance_data.find(inst_id) != instance_data.end()) {
			if (instance_data.at(inst_id).find(field) != instance_data.at(inst_id).end()) {
				return instance_data.at(inst_id).at(field);
			} else {
				error_type = 1;
			}
		} else {
			error_type = 2;
		}

		if (should_output) {
			switch (error_type) {
				case 1: {
					messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to get the data field \"" + field + "\" from the instance of object \"" + name + "\", returning SIDP(0)");
					break;
				}
				case 2: {
					messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to get data for the instance with id " + bee_itos(inst_id) + " of object \"" + name + "\"");
					break;
				}
				default: {}
			}
		}

		return default_value;
	}
	SIDP Object::get_data(int inst_id, const std::string& field) const {
		return get_data(inst_id, field, 0, true);
	}
	int Object::set_data(int inst_id, const std::string& field, SIDP data) {
		if (instance_data.find(inst_id) == instance_data.end()) {
			return 1;
		}
		instance_data.at(inst_id)[field] = data;
		return 0;
	}

	void Object::update(Instance* self) {
		current_instance = self;
		s = &instance_data[self->id];
		(*s)["object"] = name;
	}
	void Object::destroy(Instance* self) {
		instance_data.erase(self->id);
	}
}

#endif // BEE_OBJECT
