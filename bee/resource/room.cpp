/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ROOM
#define BEE_ROOM 1

#include "../defines.hpp"

#include <algorithm>
#include <set>

#include <glm/gtc/type_ptr.hpp> // Include the required OpenGL headers

#include "room.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/collision.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/console.hpp"
#include "../core/enginestate.hpp"
#include "../core/instance.hpp"
#include "../core/rooms.hpp"

#include "../render/background.hpp"
#include "../render/drawing.hpp"
#include "../render/render.hpp"
#include "../render/viewport.hpp"

#include "../physics/body.hpp"
#include "../physics/world.hpp"

#include "texture.hpp"
#include "path.hpp"
#include "timeline.hpp"
#include "object.hpp"

namespace bee {
	/**
	* @param lhs the left-hand side of the sort comparison
	* @param rhs the right-hand side of the sort comparison
	*
	* @returns the comparison of Instance values instead of pointers
	* @see Instance::operator<()
	*/
	bool InstanceSort::operator()(Instance* lhs, Instance* rhs) const {
		return (*lhs) < (*rhs);
	}

	std::map<size_t,Room*> Room::list;
	size_t Room::next_id = 0;

	/**
	* Default construct the Room.
	* @note This constructor should only be directly used for temporary Rooms, the other constructor should be used for all other cases.
	*/
	Room::Room() :
		Resource(),

		id(-1),
		name(),
		path(),

		width(DEFAULT_WINDOW_WIDTH),
		height(DEFAULT_WINDOW_HEIGHT),
		is_persistent(false),

		backgrounds(),
		viewports({{"default", ViewPort()}}),

		next_instance_id(0),
		instances(),
		created_instances(),
		destroyed_instances(),
		instances_events(),

		physics_world(nullptr),
		physics_instances(),

		view_current(nullptr),

		automatic_paths(),
		automatic_timelines()
	{}
	/**
	* Construct the Room, add it to the Room resource list, and set the new name and path
	* @param _name the name to use for the Room
	* @param _path the path of the Room's header file
	*/
	Room::Room(const std::string& _name, const std::string& _path) :
		Room()
	{
		add_to_resources();
		if (id == static_cast<size_t>(-1)) {
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add room resource: \"" + _name + "\"" + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Free the Room data and remove it from the resource list.
	*/
	Room::~Room() {
		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}

		list.erase(id);
	}

	/**
	* @returns the number of Room resources
	*/
	size_t Room::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Room* Room::get(size_t id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Room
	*
	* @returns the Room resource with the given name or nullptr if not found
	*/
	Room* Room::get_by_name(const std::string& name) {
		for (auto& room : list) { // Iterate over the Rooms in order to find the first one with the given name
			Room* r = room.second;
			if (r != nullptr) {
				if (r->get_name() == name) {
					return r; // Return the desired Room on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze and return a newly created Room resource.
	* @param name the name to initialize the Room with
	* @param path the path to initialize the Room with
	*
	* @returns the newly created Room
	*/
	Room* Room::add(const std::string& name, const std::string& path) {
		/*Room* _room = new Room(name, path);
		_room->load();
		return _room;*/

		// It doesn't really make sense to add a generic Room on the fly
		return nullptr; // TODO: Python Rooms
	}

	/**
	* Add the Room to the appropriate resource list.
	*
	* @returns the Room id
	*/
	size_t Room::add_to_resources() {
		if (id == static_cast<size_t>(-1)) { // If the resource needs to be added to the resource list
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
	int Room::reset() {
		// Reset all properties
		name = "";
		path = "";

		width = DEFAULT_WINDOW_WIDTH;
		height = DEFAULT_WINDOW_HEIGHT;
		is_persistent = false;

		backgrounds.clear();
		viewports.clear();
		viewports.emplace("default", ViewPort());

		// Clear Instance data
		next_instance_id = 0;
		for (auto& i : instances) {
			delete i.second;
		}
		instances.clear();
		created_instances.clear();
		destroyed_instances.clear();
		instances_events.clear();

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Room
	*/
	std::map<Variant,Variant> Room::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = static_cast<int>(id);
		info["name"] = name;
		info["path"] = path;

		info["width"] = width;
		info["height"] = height;
		info["is_persistent"] = is_persistent;

		std::map<Variant,Variant> bgs;
		for (auto& _bg : backgrounds) {
			const Background& bg = _bg.second;
			const TextureTransform& tr = bg.transform;

			bgs.emplace(Variant(_bg.first), Variant(std::map<Variant,Variant>{
				{"texture", Variant(bg.texture->get_name())},
				{"is_visible", Variant(bg.is_visible)},
				{"is_foreground", Variant(bg.is_foreground)},
				{"transform", Variant(std::map<Variant,Variant>{
					{"x", Variant(tr.x)},
					{"y", Variant(tr.y)},
					{"is_horizontal_tile", Variant(tr.is_horizontal_tile)},
					{"is_vertical_tile", Variant(tr.is_vertical_tile)},
					{"horizontal_speed", Variant(tr.horizontal_speed)},
					{"vertical_speed", Variant(tr.vertical_speed)},
					{"is_stretched", Variant(tr.is_stretched)}
				})}
			}));
		}
		info["backgrounds"] = bgs;

		std::map<Variant,Variant> vws;
		for (auto& _vp : viewports) {
			const ViewPort& vp = _vp.second;
			const SDL_Rect& v = vp.view;
			const SDL_Rect& p = vp.port;

			vws.emplace(Variant(_vp.first), Variant(std::map<Variant,Variant>{
				{"is_active", Variant(vp.is_active)},
				{"view", Variant(std::map<Variant,Variant>{
					{"x", Variant(v.x)},
					{"y", Variant(v.y)},
					{"w", Variant(v.w)},
					{"h", Variant(v.h)}
				})},
				{"port", Variant(std::map<Variant,Variant>{
					{"x", Variant(p.x)},
					{"y", Variant(p.y)},
					{"w", Variant(p.w)},
					{"h", Variant(p.h)}
				})}
			}));
		}
		info["viewports"] = vws;

		std::vector<Variant> insts;
		for (auto& inst : instances) {
			insts.push_back(Variant(inst.second->serialize()));
		}
		info["instances"] = insts;

		std::vector<Variant> paths;
		for (auto& p : automatic_paths) {
			const PathFollower& pf = p.second;

			paths.push_back(Variant(std::map<Variant,Variant>{
				{"instance", Variant(static_cast<int>(p.first->id))},
				{"follower", Variant(std::map<Variant,Variant>{
					{"path", Variant(pf.path->get_name())},
					{"offset", Variant(std::vector<Variant>{
						Variant(pf.offset.x()),
						Variant(pf.offset.y()),
						Variant(pf.offset.z())
					})},

					{"node", Variant(static_cast<int>(pf.node))},
					{"progress", Variant(static_cast<int>(pf.progress))},
					{"speed", Variant(static_cast<int>(pf.speed))},

					{"direction", Variant(pf.direction)},
					{"is_curved", Variant(pf.is_curved)},
					{"is_closed", Variant(pf.is_closed)},

					{"is_pausable", Variant(pf.is_pausable)}
				})}
			}));
		}
		info["automatic_paths"] = paths;

		std::vector<Variant> timelines;
		for (auto& tlit : automatic_timelines) {
			timelines.push_back(Variant(std::map<Variant,Variant>{
				{"timeline", Variant(tlit.tl->get_name())},

				{"start_frame", Variant(static_cast<int>(tlit.start_frame))},
				{"position_frame", Variant(static_cast<int>(tlit.position_frame))},
				{"start_offset", Variant(static_cast<int>(tlit.start_offset))},
				{"pause_offset", Variant(static_cast<int>(tlit.pause_offset))},

				{"is_looping", Variant(static_cast<int>(tlit.is_looping))},
				{"is_pausable", Variant(static_cast<int>(tlit.is_pausable))}
			}));
		}
		info["automatic_timelines"] = timelines;

		return info;
	}
	/**
	* Restore the Room from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the Room
	*/
	int Room::deserialize(std::map<Variant,Variant>& m) {
		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		width = m["width"].i;
		height = m["height"].i;
		is_persistent = m["is_persistent"].i;

		backgrounds.clear();
		for (auto& bg : m["backgrounds"].m) {
			std::map<Variant,Variant>& t (bg.second.m["transform"].m);
			TextureTransform tr (
				t["x"].i,
				t["y"].i,
				t["is_horizontal_tile"].i,
				t["is_vertical_tile"].i,
				t["horizontal_speed"].i,
				t["vertical_speed"].i,
				t["is_stretched"].i
			);

			Background b (
				Texture::get_by_name(bg.second.m["texture"].s),
				bg.second.m["is_visible"].i,
				bg.second.m["is_foreground"].i,
				tr
			);

			add_background(bg.first.s, b);
		}

		viewports.clear();
		for (auto& vw : m["viewports"].m) {
			std::map<Variant,Variant>& vm (vw.second.m["view"].m);
			SDL_Rect v = {
				vm["x"].i,
				vm["y"].i,
				vm["w"].i,
				vm["h"].i
			};
			std::map<Variant,Variant>& pm (vw.second.m["port"].m);
			SDL_Rect p = {
				pm["x"].i,
				pm["y"].i,
				pm["w"].i,
				pm["h"].i
			};

			ViewPort vp (
				vw.second.m["is_active"].i,
				v,
				p
			);

			add_viewport(vw.first.s, vp);
		}

		bool was_ready = engine->is_ready;
		engine->is_ready = false;

		instances.clear();
		created_instances.clear();
		destroyed_instances.clear();
		instances_events.clear();
		next_instance_id = 0;

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		for (auto& _inst : m["instances"].v) {
			Instance* inst = add_instance(
				Object::get_by_name(_inst.m["object"].s),
				btVector3(
					_inst.m["pos_start"].v[0].f,
					_inst.m["pos_start"].v[1].f,
					_inst.m["pos_start"].v[2].f
				)
			);

			inst->deserialize(_inst.m);
		}

		created_instances.clear();
		engine->is_ready = was_ready;

		automatic_paths.clear();
		for (auto& p : m["automatic_paths"].v) {
			std::map<size_t,Instance*>::iterator inst = instances.find(p.m["instance"].i);
			if (inst == instances.end()) {
				continue;
			}

			PathFollower pf (
				Path::get_by_name(p.m["path"].s),
				btVector3(
					p.m["offset"].v[0].f,
					p.m["offset"].v[1].f,
					p.m["offset"].v[2].f
				),
				p.m["speed"].i
			);
			pf.node = p.m["node"].i;
			pf.progress = p.m["progress"].i;

			pf.direction = p.m["direction"].i;
			pf.is_curved = p.m["is_curved"].i;
			pf.is_closed = p.m["is_closed"].i;

			pf.is_pausable = p.m["is_pausable"].i;

			automate_path(inst->second, pf);
		}

		automatic_timelines.clear();
		for (auto& tl : m["automatic_timelines"].v) {
			TimelineIterator tlit (
				Timeline::get_by_name(tl.m["timeline"].s),
				tl.m["start_offset"].i,
				tl.m["is_looping"].i,
				tl.m["is_pausable"].i
			);

			tlit.start_frame = tl.m["start_frame"].i;
			tlit.position_frame = tl.m["position_frame"].i;
			tlit.pause_offset = tl.m["pause_offset"].i;

			automate_timeline(tlit);
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Room::print() const {
		Variant m (serialize());
		messenger::send({"engine", "room"}, E_MESSAGE::INFO, "Room " + m.to_str(true));
	}

	size_t Room::get_id() const {
		return id;
	}
	std::string Room::get_name() const {
		return name;
	}
	std::string Room::get_path() const {
		return path;
	}
	int Room::get_width() const {
		return width;
	}
	int Room::get_height() const {
		return height;
	}
	bool Room::get_is_persistent() const {
		return is_persistent;
	}
	const std::map<std::string,Background>& Room::get_backgrounds() const {
		return backgrounds;
	}
	const std::map<std::string,ViewPort>& Room::get_viewports() const {
		return viewports;
	}
	const std::map<size_t,Instance*>& Room::get_instances() const {
		return instances;
	}
	ViewPort* Room::get_current_view() const {
		return view_current;
	}
	PhysicsWorld* Room::get_phys_world() const {
		return physics_world;
	}
	const std::map<const btRigidBody*,Instance*>& Room::get_phys_instances() const {
		return physics_instances;
	}
	const std::map<Instance*,PathFollower>& Room::get_paths() const {
		return automatic_paths;
	}
	const std::vector<TimelineIterator>& Room::get_timelines() const {
		return automatic_timelines;
	}

	void Room::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Rooms resource directory.
	*/
	void Room::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Room directory if not root
			path = "resources/rooms/"+_path;
		}
	}
	void Room::set_width(int _width) {
		width = _width;
	}
	void Room::set_height(int _height) {
		height = _height;
	}
	void Room::set_is_persistent(bool _is_persistent) {
		is_persistent = _is_persistent;
	}

	/**
	* Add the Background to the Room using the given name.
	* @param bg_name the name to use for the Background
	* @param bg the Background to add
	*
	* @retval 0 success
	* @retval 1 failed to add the Background since one with the same name already exists
	*/
	int Room::add_background(const std::string& bg_name, Background bg) {
		bool r;
		std::tie(std::ignore, r) = backgrounds.emplace(bg_name, bg);
		return (r) ? 0 : 1;
	}
	/**
	* Remove the Background with the given name from the Room.
	* @param bg_name the name of the Background to remove
	*/
	void Room::remove_background(const std::string& bg_name) {
		backgrounds.erase(bg_name);
	}
	/**
	* Add the ViewPort to the Room using the given name.
	* @param vp_name the name to use for the ViewPort
	* @param vp the ViewPort to add
	*
	* @retval 0 success
	* @retval 1 failed to add the ViewPort since one with the same name already exists
	*/
	int Room::add_viewport(const std::string& vp_name, ViewPort vp) {
		bool r;
		std::tie(std::ignore, r) = viewports.emplace(vp_name, vp);
		return (r) ? 0 : 1;
	}
	/**
	* Remove the ViewPort with the given name from the Room.
	* @param vp_name the name of the ViewPort to remove
	*/
	void Room::remove_viewport(const std::string& vp_name) {
		viewports.erase(vp_name);
	}
	/**
	* Insert an Instance at the given index.
	* @param index the index to use for the Instance
	* @param instance the Instance to insert
	*/
	void Room::set_instance(size_t index, Instance* instance) {
		// If the instance exists, overwrite it
		if (instances.find(index) != instances.end()) {
			remove_instance(index);
		}

		instances.emplace(index, instance);
	}
	/**
	* Add an Instance of the given Object at the given position.
	* @param object the Object to instantiate
	* @param pos the initial position of the new Instance
	*
	* @returns a pointer to the newly created Instance
	*/
	Instance* Room::add_instance(Object* object, btVector3 pos) {
		// Output a warning if the Object's Sprite is not loaded
		if (object->get_sprite() != nullptr) {
			if (
				(!object->get_sprite()->get_is_loaded())
				&&(get_is_ready())
				&&(!get_option("is_headless").i)
			) {
				messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "An instance of " + object->get_name() + " has been created but its sprite has not been loaded");
			}
		}

		// Get the new identifier for the Instance
		size_t new_id = next_instance_id++;
		while (instances.find(new_id) != instances.end()) {
			new_id = next_instance_id++;
		}

		// Create and add the Instance to the internal lists
		Instance* new_instance = new Instance(new_id, object, pos);
		set_instance(new_id, new_instance);
		object->add_instance(new_id, new_instance);

		// Register the Instance's events
		for (E_EVENT e : object->get_events()) {
			instances_events[e].emplace(new_instance, new_instance->id);
		}

		// Add the Instance's PhysicsBody to the PhysicsWorld if it exists
		if (new_instance->get_physbody() != nullptr) {
			add_physbody(new_instance, new_instance->get_physbody());
			physics_world->add_body(new_instance->get_physbody());
		}

		// Schedule the create event
		if (get_is_ready()) { // Run it immediately if the event loop is already processing
        	object->update(new_instance);
			object->create(new_instance);
		} else {
			created_instances.push_back(new_instance);
		}

		return new_instance;
	}
	/**
	* Remove the Instance at the given index.
	* @param index the Instance index to remove
	*
	* @retval 0 success
	* @retval 1 failed to remove since an Instance at the given index doesn't exist
	*/
	int Room::remove_instance(size_t index) {
		std::map<size_t,Instance*>::iterator _inst = instances.find(index);
		if (_inst == instances.end()) {
			return 1;
		}

		Instance* inst = _inst->second;

		if (inst->get_physbody() != nullptr) {
			remove_physbody(inst->get_physbody());
		}

		inst->get_object()->remove_instance(index);
		instances.erase(index);

		for (E_EVENT e : inst->get_object()->get_events()) {
			instances_events[e].erase(inst);
		}

		delete inst;

		return 0;
	}
	/**
	* Add the given PhysicsBody and an associated Instance.
	* @note This mapping is used by collision_internal() and check_collision_filter()
	* @param inst the Instance to add
	* @param body the PhysicsBody to add
	*/
	void Room::add_physbody(Instance* inst, PhysicsBody* body) {
		physics_instances.emplace(body->get_body(), inst);
	}
	/**
	* Remove the given PhysicsBody from the internal map.
	* @param body the PhysicsBody to remove
	*/
	void Room::remove_physbody(PhysicsBody* body) {
		physics_instances.erase(body->get_body());
	}
	/**
	* Automatically advance the given Instance with the given PathFollower.
	* @param inst the Instance to automate
	* @param pf the PathFollower to use
	*/
	void Room::automate_path(Instance* inst, PathFollower pf) {
		automatic_paths[inst] = pf;

		// Store and restore the Instance's mass
		if (pf.path == nullptr) {
			inst->set_mass(inst->get_data("__path_previous_mass", Variant(0.0), false).d);
		} else {
			inst->set_data("__path_previous_mass", inst->get_mass());
			inst->set_mass(0.0);
		}
	}
	/**
	* Automatically advance the given TimelineIterator.
	* @param tlit the TimelineIterator to automate
	*/
	void Room::automate_timeline(TimelineIterator tlit) {
		automatic_timelines.emplace_back(tlit);
	}

	/**
	* Partially reset the Room when changing to it.
	*/
	void Room::reset_properties() {
		created_instances.clear();
		next_instance_id = 0;

		backgrounds.clear();
		viewports.clear();
		viewports.emplace("default", ViewPort());

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		automatic_paths.clear();
		automatic_timelines.clear();
	}
	/**
	* Transfer persistent Instances from the previous Room.
	* @param old_room the previous Room
	*
	* @retval 0 success
	* @retval 1 failed since this is the first Room
	*/
	int Room::transfer_instances(const Room* old_room) {
		if (old_room == nullptr) {
			return 1;
		}

		for (auto& inst : instances) {
			inst.second->get_object()->remove_instance(inst.first);
		}

		const std::map<size_t,Instance*> old_instances = old_room->get_instances();

		instances.clear();
		instances_events.clear();

		for (auto& inst : old_instances) {
			set_instance(inst.first, inst.second);
			inst.second->get_object()->add_instance(inst.first, inst.second);

			if (inst.second->get_physbody() != nullptr) {
				PhysicsBody* b = inst.second->get_physbody();
				add_physbody(inst.second, b);
				b->attach(get_phys_world());
			}

			for (E_EVENT e : inst.second->get_object()->get_events()) {
				instances_events[e].emplace(inst.second, inst.first);
			}
		}

		return 0;
	}

	/**
	* Run the Instance create events.
	*/
	void Room::create() {
		for (auto& i : created_instances) {
			i->get_object()->update(i);
			i->get_object()->create(i);
		}
		created_instances.clear();
	}
	/**
	* Run the Instance destroy events.
	*/
	void Room::destroy() {
		std::set<Instance*> destroyed;

		// Continually destroy Instances
		while (!destroyed_instances.empty()) {
			Instance* inst = destroyed_instances.back();
			destroyed_instances.pop_back();

			// Skip the Instance if it's already been destroyed
			if (destroyed.find(inst) != destroyed.end()) {
				continue;
			}

			destroyed.insert(inst);

			// Destroy the Instance if it's not persistent
			if (!inst->get_is_persistent()) {
				inst->get_object()->update(inst);
				inst->get_object()->destroy(inst);
				remove_instance(inst->id);
			}
		}
	}
	/**
	* Destroy the given Instance at the end of the frame.
	* @param inst the Instance to schedule for destruction
	*/
	void Room::destroy(Instance* inst) {
		destroyed_instances.push_back(inst);
	}
	/**
	* Destroy all Instances of a given Object.
	* @param obj the Object type to destroy
	*/
	void Room::destroy_all(Object* obj) {
		for (auto& i : obj->get_instances()) {
			destroy(i.second);
		}
	}
	/**
	* Run the Instance alarm events if needed.
	*/
	void Room::check_alarms() {
		for (auto& i : instances_events[E_EVENT::ALARM]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}

			// Iterate over the Instance's alarms
			Variant alarms = i.first->get_data("__alarms");
			for (auto& a : alarms.m) {
				if (get_ticks() >= static_cast<unsigned int>(a.second.i)) {
					i.first->set_alarm(a.first.s, -1);
					i.first->get_object()->update(i.first);
					i.first->get_object()->alarm(i.first, a.first.s);
				}
			}
		}
	}
	/**
	* Run the Instance step_begin events.
	*/
	void Room::step_begin() {
		for (auto& i : instances_events[E_EVENT::STEP_BEGIN]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_begin(i.first);
		}
	}
	/**
	* Run the Instance step_mid events, automatic Paths, and automatic Timelines.
	*/
	void Room::step_mid() {
		// Run the step_mid events
		for (auto& i : instances_events[E_EVENT::STEP_MID]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_mid(i.first);
		}

		// Move Instances along their Paths
		for (auto& ipf : automatic_paths) {
			if (
				(get_is_paused())
				&&(ipf.first->get_object()->get_is_pausable())
				&&(ipf.second.is_pausable)
			) {
				continue;
			}

			ipf.second.path->advance(ipf.first, ipf.second);
		}

		// Step Timelines
		automatic_timelines.erase(std::remove_if(automatic_timelines.begin(), automatic_timelines.end(), [] (TimelineIterator& tlit) {
			return tlit.step_to(get_frame());
		}), automatic_timelines.end());
	}
	/**
	* Run the Instance step_end events.
	*/
	void Room::step_end() {
		for (auto& i : instances_events[E_EVENT::STEP_END]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_end(i.first);
		}
	}
	/**
	* Run the Instance keyboard_press events.
	*/
	void Room::keyboard_press(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::KEYBOARD_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_press(i.first, e);
		}
	}
	/**
	* Run the Instance mouse_press events.
	*/
	void Room::mouse_press(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::MOUSE_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_press(i.first, e);
		}
	}
	/**
	* Run the Instance keyboard_input events.
	*/
	void Room::keyboard_input(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::KEYBOARD_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_input(i.first, e);
		}
	}
	/**
	* Run the Instance mouse_input events.
	*/
	void Room::mouse_input(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::MOUSE_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_input(i.first, e);
		}
	}
	/**
	* Run the Instance keyboard_release events.
	*/
	void Room::keyboard_release(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::KEYBOARD_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_release(i.first, e);
		}
	}
	/**
	* Run the Instance mouse_release events.
	*/
	void Room::mouse_release(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::MOUSE_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_release(i.first, e);
		}
	}
	/**
	* Run the Instance controller_axis events.
	*/
	void Room::controller_axis(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::CONTROLLER_AXIS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_axis(i.first, e);
		}
	}
	/**
	* Run the Instance controller_press events.
	*/
	void Room::controller_press(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::CONTROLLER_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_press(i.first, e);
		}
	}
	/**
	* Run the Instance controller_release events.
	*/
	void Room::controller_release(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::CONTROLLER_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_release(i.first, e);
		}
	}
	/**
	* Run the Instance controller_modify events.
	*/
	void Room::controller_modify(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::CONTROLLER_MODIFY]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_modify(i.first, e);
		}
	}
	/**
	* Run the Instance commandline_input events.
	*/
	void Room::commandline_input(const std::string& input) {
		for (auto& i : instances_events[E_EVENT::COMMANDLINE_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->commandline_input(i.first, input);
		}
	}
	/**
	* Run the Instance path_end events if needed.
	*/
	void Room::check_paths() {
		// Check the automatic Paths
		for (auto& ipf : automatic_paths) {
			if (
				(ipf.second.path == nullptr)
				||(
					(get_is_paused())
					&&(ipf.first->get_object()->get_is_pausable())
					&&(ipf.second.is_pausable)
				)
			) {
				continue;
			}

			if (ipf.second.path->at_end(ipf.second)) {
				if (instances_events[E_EVENT::PATH_END].find(ipf.first) != instances_events[E_EVENT::PATH_END].end()) {
					ipf.first->get_object()->update(ipf.first);
					ipf.first->get_object()->path_end(ipf.first, ipf.second);
				}
			}
		}

		// Remove ended Paths
		for (auto it=automatic_paths.begin(); it!=automatic_paths.end(); ) {
			if (it->second.path == nullptr) {
				it = automatic_paths.erase(it);
			} else {
				++it;
			}
		}
	}
	/**
	* Run the Instance outside_room events if needed.
	*/
	void Room::outside_room() {
		for (auto& i : instances_events[E_EVENT::OUTSIDE_ROOM]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}

			SDL_Rect a = i.first->get_aabb();
			SDL_Rect b = {0, 0, get_width(), get_height()};
			if (!util::check_collision(a, b)) {
				i.first->get_object()->update(i.first);
				i.first->get_object()->outside_room(i.first);
			}
		}
	}
	/**
	* Run the Instance intersect_boundary events if needed.
	*/
	void Room::intersect_boundary() {
		for (auto& i : instances_events[E_EVENT::INTERSECT_BOUNDARY]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}

			SDL_Rect a = i.first->get_aabb();
			SDL_Rect b1 = {0, 0, get_width(), get_height()};
			SDL_Rect b2 = {0+a.w, 0+a.h, get_width()-2*a.w, get_height()-2*a.h};
			if ((util::check_collision(a, b1))&&(!util::check_collision(a, b2))) {
				i.first->get_object()->update(i.first);
				i.first->get_object()->intersect_boundary(i.first);
			}
		}
	}
	/**
	* Step the PhysicsWorld collisions.
	*/
	int Room::collision() {
		if (get_is_paused()) {
			return 1;
		}

		physics_world->step(get_delta());

		return 0;
	}
	/**
	* Run the Instance collision events if necessary.
	* @param w the world that was stepped
	* @param timestep the amount of time that was stepped
	*/
	void Room::collision_internal(btDynamicsWorld* w, btScalar timestep) {
		PhysicsWorld* world = static_cast<PhysicsWorld*>(w->getWorldUserInfo());
		const std::map<const btRigidBody*,Instance*>& physics_instances = get_current_room()->get_phys_instances();

		//w->clearForces();

		size_t manifold_amount = world->get_dispatcher()->getNumManifolds();
		for (size_t i=0; i<manifold_amount; ++i) {
			btPersistentManifold* manifold = world->get_dispatcher()->getManifoldByIndexInternal(i);
			const btRigidBody* body1 = btRigidBody::upcast(manifold->getBody0());
			const btRigidBody* body2 = btRigidBody::upcast(manifold->getBody1());

			std::map<const btRigidBody*,Instance*>::const_iterator inst1 = physics_instances.find(body1);
			std::map<const btRigidBody*,Instance*>::const_iterator inst2 = physics_instances.find(body2);
			if ((inst1 != physics_instances.end())&&(inst2 != physics_instances.end())) {
				Instance* i1 = inst1->second;
				Instance* i2 = inst2->second;
				if (
					(i1 != nullptr)
					&&(i2 != nullptr)
					&&(i1->get_object() != nullptr)
					&&(i2->get_object() != nullptr)
				) {
					i1->get_object()->update(i1);
					i1->get_object()->collision(i1, i2);
					i2->get_object()->update(i2);
					i2->get_object()->collision(i2, i1);
				}
			}
		}
	}
	/**
	* Check the Instance collision filters if found.
	* @param proxy0 the proxy containing the first body
	* @param proxy1 the proxy containing the second body
	*/
	bool Room::check_collision_filter(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) {
		bool should_collide = false;
		const std::map<const btRigidBody*,Instance*>& physics_instances = get_current_room()->get_phys_instances();

		btRigidBody* body1 = static_cast<btRigidBody*>(proxy0->m_clientObject);
		btRigidBody* body2 = static_cast<btRigidBody*>(proxy1->m_clientObject);

		std::map<const btRigidBody*,Instance*>::const_iterator inst1 = physics_instances.find(body1);
		std::map<const btRigidBody*,Instance*>::const_iterator inst2 = physics_instances.find(body2);
		if ((inst1 != physics_instances.end())&&(inst2 != physics_instances.end())) {
			Instance* i1 = inst1->second;
			Instance* i2 = inst2->second;

			if (
				(i1 != nullptr)
				&&(i2 != nullptr)
				&&(i1->get_object() != nullptr)
				&&(i2->get_object() != nullptr)
			) {
				i1->get_object()->update(i1);
				should_collide = i1->get_object()->check_collision_filter(i1, i2);
				i2->get_object()->update(i2);
				should_collide = should_collide && i2->get_object()->check_collision_filter(i2, i1);
			}
		}

		return should_collide;
	}
	/**
	* Render the ViewPorts.
	*/
	void Room::draw() {
		Texture* prev_target = render::get_target();

		for (auto& v : viewports) {
			ViewPort& vp = v.second;
			if (vp.is_active) {
				view_current = &vp;
				view_current->update();
				draw_view(view_current);
			}
		}
		view_current = nullptr;

		draw_set_color(RGBA(E_RGB::WHITE));

		render::set_target(prev_target);
		render::set_viewport(nullptr);
		render::clear();

		render::clear_lights();

		for (auto& v : viewports) {
			ViewPort& vp = v.second;
			if (vp.is_active) {
				vp.draw();
			}
		}

		if (console::get_is_open()) {
			console::internal::draw();
		}

		render::render();
	}
	/**
	* Render the Lights, Backgrounds, and call Instance draw events for the given ViewPort.
	* @param viewport the ViewPort to draw inside of
	*/
	void Room::draw_view(ViewPort* viewport) {
		// Draw backgrounds
		draw_set_color(RGBA(E_RGB::WHITE));

		render::set_target(viewport->texture.get());
		render::set_viewport(viewport);

		render::clear();
		render::render_lights();

		for (auto& b : backgrounds) {
			Background& bg = b.second;
			if ((bg.is_visible)&&(!bg.is_foreground)) {
				bg.texture->draw_transform(bg.transform);
			}
		}
		render::render_textures();

		// Draw instances
		for (auto& i : instances_events[E_EVENT::DRAW]) {
			i.first->get_object()->update(i.first);
			i.first->get_object()->draw(i.first);
		}
		render::render_textures();

		// Draw foregrounds
		for (auto& b : backgrounds) {
			Background& bg = b.second;
			if ((bg.is_visible)&&(bg.is_foreground)) {
				bg.texture->draw_transform(bg.transform);
			}
		}

		if (get_option("is_debug_enabled").i) {
			// Draw room outline
			draw_rectangle(0, 0, get_width(), get_height(), 1, RGBA(E_RGB::RED));

			// Draw physics engine debug shapes
			physics_world->draw_debug();
		}

		render::render_textures();
	}
	/**
	* Run the Instance animation_end events if needed.
	*/
	void Room::animation_end() {
		for (auto& i : instances_events[E_EVENT::ANIMATION_END]) {
			if (i.first->get_object()->get_sprite() != nullptr) {
				if (!i.first->get_object()->get_sprite()->get_is_animated()) {
					i.first->get_object()->update(i.first);
					i.first->get_object()->animation_end(i.first);
				}
			}
		}
	}
	/**
	* Run the Room's start function and Instance room_start events.
	*/
	void Room::room_start() {
		this->start();

		for (auto& i : instances_events[E_EVENT::ROOM_START]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->room_start(i.first);
		}
	}
	/**
	* Run the Instance room_end events, destroy the Room's Instances, and run the Room's end function.
	*/
	void Room::room_end() {
		for (auto& i : instances_events[E_EVENT::ROOM_END]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->room_end(i.first);
		}

		for (auto& i : instances) {
			destroy(i.second);
		}
		destroy();

		this->end();
	}
	/**
	* Run the Instance game_start events.
	*/
	void Room::game_start() {
		for (auto& i : instances_events[E_EVENT::GAME_START]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->game_start(i.first);
		}
	}
	/**
	* Run the Instance game_end events and destroy the Room's leftover persistent Instances.
	*/
	void Room::game_end() {
		for (auto& i : instances_events[E_EVENT::GAME_END]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->game_end(i.first);
		}

		for (auto& i : instances) {
			i.second->set_is_persistent(false);
			destroy(i.second);
		}
		destroy();
	}
	/**
	* Run the Instance window events.
	*/
	void Room::window(SDL_Event* e) {
		for (auto& i : instances_events[E_EVENT::WINDOW]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->window(i.first, e);
		}
	}
	/**
	* Run the Instance network events.
	*/
	void Room::network(const NetworkEvent& e) {
		for (auto& i :instances_events[E_EVENT::NETWORK]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->network(i.first, e);
		}
	}

	/**
	* Initialize the Room and add an Instance of obj_control if it exists.
	* @note This is called during the Room change in change_room()
	*/
	void Room::init() {
		physics_world = new PhysicsWorld();
	}
	/**
	* @note This is called before the Instance room_start events.
	*/
	void Room::start() {}
	/**
	* @note This is called after the Instance room_end events.
	*/
	void Room::end() {}
}

#endif // BEE_ROOM
