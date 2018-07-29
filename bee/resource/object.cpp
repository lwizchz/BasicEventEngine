/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJECT
#define BEE_OBJECT 1

#include "object.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"

#include "../network/network.hpp"

#include "texture.hpp"

namespace bee {
	std::map<int,Object*> Object::list;
	int Object::next_id = 0;

	/**
	* Default construct the object.
	* @note This constructor should only be directly used for temporary objects, the other constructor should be used for all other cases.
	*/
	Object::Object() :
		Resource(),

		id(-1),
		name(),
		path(),
		sprite(nullptr),
		is_persistent(false),
		depth(0),
		parent(nullptr),
		draw_offset({0, 0}),
		is_pausable(true),

		instances(),
		current_instance(nullptr),
		s(nullptr),

		implemented_events({
			E_EVENT::UPDATE,
			E_EVENT::DESTROY,
			E_EVENT::STEP_BEGIN
		})
	{}
	/**
	* Construct the Object, add it to the Object resource list, and set the new name and path.
	* @param _name the name to use for the object
	* @param _path: the path of the object's header file
	*
	* @throws int(-1) Failed to initialize Resource
	*/
	Object::Object(const std::string& _name, const std::string& _path) :
		Object() // Default initialize all variables
	{
		if (add_to_resources() < 0) { // Attempt to add the Object to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add object resource: \"" + _name + "\" from " + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Remove the Object from the resource list.
	*/
	Object::~Object() {
		list.erase(id);
	}

	/**
	* @returns the number of Object resources
	*/
	size_t Object::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Object* Object::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired object
	*
	* @returns the Object resource with the given name or nullptr if not found
	*/
	Object* Object::get_by_name(const std::string& name) {
		for (auto& obj : list) { // Iterate over the Objects in order to find the first one with the given name
			Object* o = obj.second;
			if (o != nullptr) {
				if (o->get_name() == name) {
					return o; // Return the desired Object on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze and return a newly created Object resource.
	* @param name the name to initialize the object with
	* @param path the path to initialize the object with
	*
	* @returns the newly create Object
	*/
	Object* Object::add(const std::string& name, const std::string& path) {
		/*Object* new_object = new Object(name, path);
		return new_object;*/

		// It doesn't really make sense to add a generic Object on the fly
		return nullptr; // TODO: Python Objects
	}

	/**
	* Add the Object to the appropriate resource list.
	*
	* @returns the Object id
	*/
	int Object::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with its new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Object::reset() {
		// Reset all properties
		name = "";
		path = "";
		sprite = nullptr;
		is_persistent = false;
		depth = 0;
		parent = nullptr;
		draw_offset = {0, 0};
		is_pausable = true;

		// Clear instance data
		instances.clear();
		current_instance = nullptr;
		s = nullptr;

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Object
	*/
	std::map<Variant,Variant> Object::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["sprite"] = "";
		if (sprite != nullptr) {
			info["sprite"] = sprite->get_name();
		}
		info["is_persistent"] = is_persistent;
		info["depth"] = depth;
		info["parent"] = "";
		if (parent != nullptr) {
			info["parent"] = parent->get_name();
		}
		info["offsetx"] = draw_offset.first;
		info["offsety"] = draw_offset.second;
		info["is_pausable"] = is_pausable;

		std::vector<Variant> events;
		for (auto& e : implemented_events) {
			events.emplace_back(static_cast<int>(e));
		}
		info["implemented_events"] = events;

		return info;
	}
	/**
	* Restore the Object from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the Object
	*/
	int Object::deserialize(std::map<Variant,Variant>& m) {
		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		sprite = Texture::get_by_name(m["sprite"].s);
		is_persistent = m["is_persistent"].i;
		depth = m["depth"].i;
		parent = Object::get_by_name(m["parent"].s);
		draw_offset = std::make_pair(m["offsetx"].i, m["offsety"].i);
		is_pausable = m["is_pausable"].i;

		for (auto& e : m["implemented_events"].v) {
			implemented_events.emplace(static_cast<E_EVENT>(e.i));
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Object::print() const {
		Variant m (serialize());
		messenger::send({"engine", "object"}, E_MESSAGE::INFO, "Object " + m.to_str(true));
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
	Texture* Object::get_sprite() const {
		return sprite;
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
	std::pair<int,int> Object::get_mask_offset() const {
		return draw_offset;
	}
	bool Object::get_is_pausable() const {
		return is_pausable;
	}
	const std::set<E_EVENT>& Object::get_events() const {
		return implemented_events;
	}

	void Object::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Objects resource directory.
	*/
	void Object::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Object directory if not root
			path = "resources/objects/"+_path;
		}
	}
	void Object::set_sprite(Texture* _sprite) {
		sprite = _sprite;
	}
	void Object::set_is_persistent(bool _is_persistent) {
		is_persistent = _is_persistent;
	}
	/**
	* Set the sorting depth for all of the Object's Instances.
	* @param _depth the desired depth, lower values are processed first
	*/
	void Object::set_depth(int _depth) {
		depth = _depth;

		for (auto i : instances) { // Iterate over all current instances and adjust their depth
			i.second->depth = depth;
		}
	}
	void Object::set_parent(Object* _parent) {
		parent = _parent;
	}
	void Object::set_mask_offset(const std::pair<int,int>& _offset) {
		draw_offset = _offset;
	}
	void Object::set_is_pausable(bool _is_pausable) {
		is_pausable = _is_pausable;
	}

	/**
	* Add an Instance of this Object to its list.
	* @param index the Instance identifier
	* @param new_instance the Instance pointer
	*
	* @retval 0 success
	* @retval 1 failed since the Instance has an incorrect Object type
	*/
	int Object::add_instance(int index, Instance* new_instance) {
		if (new_instance->get_object() != this) { // Do not attempt to add the instance if it's the wrong type
			return 1;
		}

		// Overwrite any previous instance with the same id
		instances.erase(index);
		instances.emplace(index, new_instance); // Emplace the instance in the list

		return 0;
	}
	void Object::remove_instance(int index) {
		instances.erase(index);
	}
	void Object::clear_instances() {
		instances.clear();
	}
	const std::map<int,Instance*>& Object::get_instances() const {
		return instances;
	}
	size_t Object::get_instance_amount() const {
		return instances.size();
	}
	/**
	* @param inst_number the position of the Instance in the Object's Instance list, NOT the Instance id
	*
	* @returns a pointer to the nth Instance of this Object or nullptr if not found
	*/
	Instance* Object::get_instance_at(int inst_number) const {
		if (static_cast<size_t>(inst_number) >= instances.size()) { // If the desired Instance position is greater than the list size
			return nullptr;
		}

		int i = 0;
		for (auto& inst : instances) { // Iterate over the Instance list, counting how many there are
			if (i == inst_number) {
				return inst.second;
			}
			i++;
		}

		return nullptr;
	}

	/**
	* Update the Instance pointer and data map pointer to the given Instance, usually before a call to one of the Instance's events.
	* @param inst the Instance to update the pointers for
	*/
	void Object::update(Instance* inst) {
		current_instance = inst;
		s = &inst->get_data();
	}
	/**
	* Remove the data for the given Instance.
	* @param inst the Instance to remove data for
	*/
	void Object::destroy(Instance* inst) {
		net::internal::destroy_instance(inst);
	}
	/**
	* Update the Instance every step.
	* @param inst the Instance to update
	*/
	void Object::step_begin(Instance* inst) {
		inst->pos_previous = inst->get_pos();
	}
	/**
	* Compare Instance collision filters.
	* @see Room::check_collision_filter(btBroadphaseProxy*, btBroadphaseProxy*)
	* @param self the first Instance
	* @param other the second Instance
	*
	* @retval true the collision masks and computation types overlap
	* @retval false the collision masks and computation types do not overlap
	*/
	bool Object::check_collision_filter(const Instance* self, const Instance* other) const {
		int self_mask = self->get_data("__collision_mask").i;
		int self_comp = static_cast<int>(self->get_computation_type());
		int other_mask = other->get_data("__collision_mask").i;
		int other_comp = static_cast<int>(other->get_computation_type());

		if ((self_mask & other_comp) != 0) {
			if ((other_mask & self_comp) != 0) {
				return true;
			}
		}
		return false;
	}
}

#endif // BEE_OBJECT
