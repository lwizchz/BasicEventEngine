/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJECT
#define BEE_OBJECT 1

#include <sstream> // Include the required library headers

#include "object.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"

#include "../network/network.hpp"

#include "sprite.hpp"

namespace bee {
	std::map<int,Object*> Object::list;
	int Object::next_id = 0;

	/*
	* Object::Object() - Default construct the object
	* ! This constructor should only be directly used for temporary objects, the other constructor should be used for all other cases
	*/
	Object::Object() :
		Resource(),

		id(-1),
		name(),
		path(),
		sprite(nullptr),
		is_solid(false),
		is_visible(true),
		is_persistent(false),
		depth(0),
		parent(nullptr),
		mask(nullptr),
		xoffset(0),
		yoffset(0),
		is_pausable(true),

		instances(),
		s(nullptr),
		current_instance(nullptr),

		implemented_events({
			E_EVENT::UPDATE,
			E_EVENT::DESTROY,
			E_EVENT::STEP_BEGIN
		})
	{}
	/*
	* Object::Object() - Construct the object, add it to the object resource list, and set the new name and path
	* @new_name: the name to use for the object
	* @new_path: the path of the object's header file
	*/
	Object::Object(const std::string& new_name, const std::string& new_path) :
		Object() // Default initialize all variables
	{
		add_to_resources(); // Add the object to the appropriate resource list
		if (id < 0) { // If it could not be added, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add object resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name);
		set_path(new_path);
	}
	/*
	* Object::~Object() - Remove the object from the resource list
	*/
	Object::~Object() {
		if (list.find(id) != list.end()) { // Remove the object from the resource list
			list.erase(id);
		}
	}


	/*
	* Object::add_to_resources() - Add the object to the appropriate resource list
	*/
	int Object::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Object::get_amount() - Return the amount of object resources
	*/
	size_t Object::get_amount() {
		return list.size();
	}
	/*
	* Object::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Object* Object::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Object::reset() - Reset all resource variables for reinitialization
	*/
	int Object::reset() {
		// Reset all properties
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
		is_pausable = true;

		// Clear instance data
		instances.clear();
		s = nullptr;
		current_instance = nullptr;

		return 0; // Return 0 on success
	}
	/*
	* Object::print() - Print all relevant information about the resource
	*/
	int Object::print() const {
		std::string instance_string = get_instance_string(); // Get the list of instances in string form

		std::stringstream ss; // Declare the output stream
		ss << // Append all info to the output
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
		"\n	is_pausable   " << is_pausable <<
		"\n	instances\n" << debug_indent(instance_string, 2) <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, ss.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Object::get_*() - Return the requested resource information
	*/
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
	bool Object::get_is_pausable() const {
		return is_pausable;
	}

	/*
	* Object::set_*() - Set the requested resource data
	*/
	int Object::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Object::set_path(const std::string& new_path) {
		path = "resources/objects/"+new_path; // Append the path to the object directory
		return 0;
	}
	int Object::set_sprite(Sprite* new_sprite) {
		sprite = new_sprite;
		if (mask == nullptr) { // If there is no mask, set it to the new sprite
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

		for (auto i : instances) { // Iterate over all current instances and adjust their depth
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
	int Object::set_is_pausable(bool new_is_pausable) {
		is_pausable = new_is_pausable;
		return 0;
	}

	/*
	* Object::add_instance() - Add an instance of this object to its list
	*/
	int Object::add_instance(int index, Instance* new_instance) {
		if (new_instance->get_object() != this) { // Do not attempt to add the instance if it's the wrong type
			return 1; // Return 1 on wrong object type
		}

		// Overwrite any previous instance with the same id
		instances.erase(index);
		instances.emplace(index, new_instance); // Emplace the instance in the list

		return 0; // Return 0 on success
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
	/*
	* Object::get_instance() - Return a pointer to the nth instance of this object
	* @inst_number: the position of the instance in the object's instance list, NOT the instance id
	*/
	Instance* Object::get_instance(int inst_number) const {
		if ((size_t)inst_number >= instances.size()) { // If the desired instance position is greater than the list size, return nullptr
			return nullptr;
		}

		int i = 0;
		for (auto& inst : instances) { // Iterate over the instance list, counting how many there are
			if (i == inst_number) { // If the instance position is the desired number, return the instance pointer
				return inst.second;
			}
			i++;
		}

		return nullptr; // Otherwise return nullptr (this statement should never be reached)
	}
	std::string Object::get_instance_string() const {
		if (instances.empty()) { // If there are no instances in the list, return a none-string
			return "none\n";
		}

		std::vector<std::vector<std::string>> table; // Declare a table to hold the instances
		table.push_back({"(id", "object", "x", "y", "z)"}); // Append the table header

		for (auto& i : instances) { // Iterate over the instances and add each of them to the table
			table.push_back({
				bee_itos(i.second->id),
				i.second->get_object()->get_name(),
				bee_itos(static_cast<int>(i.second->get_position()[0])),
				bee_itos(static_cast<int>(i.second->get_position()[1])),
				bee_itos(static_cast<int>(i.second->get_position()[2]))
			});
		}

		return string_tabulate(table); // Return the table as a properly spaced string
	}

	/*
	* Object::update() - Update the instance and data map pointers to the given instance, usually before a call to one of the instance's events
	* @inst: the instance to update the pointers for
	*/
	void Object::update(Instance* inst) {
		current_instance = inst; // Set the current instance, i.e. the next instance to be operated on
		s = &inst->get_data(); // Update the pointer to the instance data map
	}
	/*
	* Object::destroy() - Remove the instance data for the given instance
	* @inst: the instance to remove data for
	*/
	void Object::destroy(Instance* inst) {
		net::internal::destroy_instance(inst);
	}
	/*
	* Object::step_begin() - Update the instance every step
	* @inst: the instance to update
	*/
	void Object::step_begin(Instance* inst) {
		inst->pos_previous = inst->get_position();
	}
}

#endif // BEE_OBJECT
