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
	namespace internal {
		std::pair<Instance*,int> flip_instancemap_pair(const std::pair<int,Instance*>& p) {
			return std::pair<Instance*,int>(p.second, p.first);
		}
	}

	bool InstanceSort::operator()(Instance* lhs, Instance* rhs) const {
		return (*lhs) < (*rhs); // Compare the values instead of the pointers
	}

	std::map<size_t,Room*> Room::list;
	size_t Room::next_id = 0;

	Room::Room() :
		Resource(),

		id(-1),
		name(),
		path(),

		width(DEFAULT_WINDOW_WIDTH),
		height(DEFAULT_WINDOW_HEIGHT),
		is_persistent(false),

		backgrounds(),
		views({ViewPort()}),

		next_instance_id(0),
		instances(),
		instances_sorted(),
		created_instances(),
		destroyed_instances(),
		should_sort(false),

		instances_sorted_events(),

		physics_world(nullptr),
		physics_instances(),

		view_current(nullptr),

		automatic_paths(),
		automatic_timelines()
	{}
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
	Room::~Room() {
		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}

		list.erase(id); // Remove the room from the resource list
	}

	/*
	* Room::get_amount() - Return the amount of room resources
	*/
	size_t Room::get_amount() {
		return list.size();
	}
	/*
	* Room::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Room* Room::get(size_t id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Room::get_by_name() - Return the room resource with the given name
	* @name: the name of the desired room
	*/
	Room* Room::get_by_name(const std::string& name) {
		for (auto& room : list) { // Iterate over the rooms in order to find the first one with the given name
			Room* r = room.second;
			if (r != nullptr) {
				if (r->get_name() == name) {
					return r; // Return the desired room on success
				}
			}
		}
		return nullptr; // Return nullptr on failure
	}
	/*
	* Room::add() - Initiliaze, load, and return a newly created room resource
	* @name: the name to initialize the room with
	* @path: the path to initialize the room with
	*/
	Room* Room::add(const std::string& name, const std::string& path) {
		/*Room* _room = new Room(name, path);
		_room->load();
		return _room;*/

		return nullptr; // Right now rooms cannot be added on the fly because they must be compiled
	}

	/*
	* Room::add_to_resources() - Add the room to the appropriate resource list
	*/
	int Room::add_to_resources() {
		if (id == static_cast<size_t>(-1)) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with its new id
		}

		return 0;
	}
	int Room::reset() {
		name = "";
		path = "";

		width = DEFAULT_WINDOW_WIDTH;
		height = DEFAULT_WINDOW_HEIGHT;
		is_persistent = false;

		backgrounds.clear();
		views.clear();
		views.push_back(ViewPort());

		next_instance_id = 0;
		for (auto& i : instances) {
			delete i.second;
		}
		instances.clear();
		instances_sorted.clear();
		created_instances.clear();
		destroyed_instances.clear();
		instances_sorted_events.clear();
		should_sort = false;

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

		std::vector<Variant> bgs;
		for (auto& bg : backgrounds) {
			const TextureTransform& tr = bg.transform;

			bgs.push_back(Variant(std::map<Variant,Variant>{
				{"index", Variant(static_cast<int>(bgs.size()))},
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

		std::vector<Variant> vws;
		for (auto& vp : views) {
			const SDL_Rect& v = vp.view;
			const SDL_Rect& p = vp.port;

			vws.push_back(Variant(std::map<Variant,Variant>{
				{"index", Variant(static_cast<int>(vws.size()))},
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
		info["views"] = vws;

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
		for (auto& bg : m["backgrounds"].v) {
			std::map<Variant,Variant>& t (bg.m["transform"].m);
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
				Texture::get_by_name(bg.m["texture"].s),
				bg.m["is_visible"].i,
				bg.m["is_foreground"].i,
				tr
			);

			set_background(bg.m["index"].i, b);
		}

		views.clear();
		for (auto& vw : m["views"].v) {
			std::map<Variant,Variant>& vm (vw.m["view"].m);
			SDL_Rect v = {
				vm["x"].i,
				vm["y"].i,
				vm["w"].i,
				vm["h"].i
			};
			std::map<Variant,Variant>& pm (vw.m["port"].m);
			SDL_Rect p = {
				pm["x"].i,
				pm["y"].i,
				pm["w"].i,
				pm["h"].i
			};

			ViewPort vp (
				vw.m["is_active"].i,
				v,
				p
			);

			set_view(vw.m["index"].i, vp);
		}

		bool was_ready = engine->is_ready;
		engine->is_ready = false;

		instances.clear();
		instances_sorted.clear();
		created_instances.clear();
		destroyed_instances.clear();
		instances_sorted_events.clear();
		next_instance_id = 0;

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		for (auto& _inst : m["instances"].v) {
			Instance* inst = add_instance(
				_inst.m["id"].i,
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
		sort_instances();

		automatic_paths.clear();
		for (auto& p : m["automatic_paths"].v) {
			std::map<int,Instance*>::iterator inst = instances.find(p.m["instance"].i);
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
	const std::vector<Background>& Room::get_backgrounds() const {
		return backgrounds;
	}
	const std::vector<ViewPort>& Room::get_views() const {
		return views;
	}
	const std::map<int,Instance*>& Room::get_instances() const {
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

	size_t Room::set_background(size_t desired_index, Background _background) {
		size_t new_index = desired_index;

		// If the index doesn't exist, append the backgrounds
		if (new_index >= backgrounds.size()) {
			new_index = backgrounds.size();
			backgrounds.push_back(_background);
			return new_index;
		}

		// Otherwise, overwrite any previous background with the same index
		backgrounds[new_index] = _background;
		return new_index;
	}
	size_t Room::set_view(size_t desired_index, ViewPort _view) {
		size_t new_index = desired_index;

		// If the index doesn't exist, append the view
		if (new_index >= views.size()) {
			new_index = views.size();
			views.push_back(_view);
			return new_index;
		}

		// Otherwise, overwrite any previous view with the same index
		views[new_index] = _view;
		return new_index;
	}
	size_t Room::set_instance(size_t index, Instance* _instance) {
		if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
			remove_instance(index);
		}

		instances.insert(std::pair<int,Instance*>(index, _instance));

		return 0;
	}
	Instance* Room::add_instance(size_t index, Object* object, btVector3 pos) {
		if (object->get_sprite() != nullptr) {
			if (
				(!object->get_sprite()->get_is_loaded())
				&&(get_is_ready())
				&&(!get_option("is_headless").i)
			) {
				messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "An instance of " + object->get_name() + " has been created but its sprite has not been loaded");
			}
		}

		if (index == static_cast<size_t>(-1)) {
			index = next_instance_id++;
		}
		while (instances.find(index) != instances.end()) {
			index = next_instance_id++;
		}

		Instance* new_instance = new Instance(index, object, pos);
		set_instance(index, new_instance);
		request_instance_sort();
		object->add_instance(index, new_instance);

		for (E_EVENT e : object->get_events()) {
			instances_sorted_events[e].emplace(new_instance, new_instance->id);
		}

		if (new_instance->get_physbody() != nullptr) {
			add_physbody(new_instance, new_instance->get_physbody());
			physics_world->add_body(new_instance->get_physbody());
		}

		if (get_is_ready()) {
        	object->update(new_instance);
			object->create(new_instance);
		} else {
			created_instances.push_back(new_instance);
		}

		return new_instance;
	}
	size_t Room::add_instance_grid(size_t index, Object* object, btVector3 pos) {
		double x = pos.x(), y = pos.y();
		double xg = x, yg = y;

		if (object->get_sprite() != nullptr) {
			if (!object->get_sprite()->get_is_loaded()) {
				messenger::send({"engine", "room"}, E_MESSAGE::INFO, "Automatically loading the sprite for object " + object->get_name());
				object->get_sprite()->load();
			}

			xg = 0.0; yg = 0.0;
			if (x >= 0.0) {
				xg = x*object->get_sprite()->get_subimage_width();
			} else {
				xg = width + x*object->get_sprite()->get_subimage_width();
			}
			if (y >= 0.0) {
				yg = y*object->get_sprite()->get_size().second;
			} else {
				yg = height + y*object->get_sprite()->get_size().second;
			}
		}

		return add_instance(index, object, btVector3(xg, yg, pos.z()))->id;
	}
	int Room::remove_instance(size_t index) {
		if (instances.find(index) == instances.end()) {
			return 1;
		}

		Instance* inst = instances[index];

		if (inst->get_physbody() == nullptr) {
			messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Null physbody for instance " + std::to_string(index) + " of " + inst->get_object()->get_name() + "\n");
		} else {
			remove_physbody(inst->get_physbody());
		}

		inst->get_object()->remove_instance(index);
		instances.erase(index);
		instances_sorted.erase(inst);

		for (E_EVENT e : inst->get_object()->get_events()) {
			instances_sorted_events[e].erase(inst);
		}

		delete inst;

		return 0;
	}
	void Room::sort_instances() {
		std::transform(instances.begin(), instances.end(), std::inserter(instances_sorted, instances_sorted.begin()), internal::flip_instancemap_pair);
	}
	void Room::request_instance_sort() {
		should_sort = true;
	}
	void Room::add_physbody(Instance* inst, PhysicsBody* body) {
		physics_instances.emplace(body->get_body(), inst);
	}
	void Room::remove_physbody(PhysicsBody* body) {
		physics_instances.erase(body->get_body());
	}
	void Room::automate_path(Instance* inst, PathFollower pf) {
		automatic_paths[inst] = pf;

		// Store and restore the Instance mass
		if (pf.path == nullptr) {
			inst->set_mass(inst->get_data("__path_previous_mass", Variant(0.0), false).d);
		} else {
			inst->set_data("__path_previous_mass", inst->get_mass());
			inst->set_mass(0.0);
		}
	}
	void Room::automate_timeline(TimelineIterator tlit) {
		automatic_timelines.emplace_back(tlit);
	}

	void Room::reset_properties() {
		should_sort = false;
		for (auto& i : instances) {
			destroyed_instances.push_back(i.second);
		}
		destroy();

		// Remove all instances except persistent instances
		for (auto it=instances.begin(); it!=instances.end(); ) {
			if (!it->second->get_is_persistent()) {
				it = instances.erase(it);
			} else {
				++it;
			}
		}
		for (auto it=instances_sorted.begin(); it!=instances_sorted.end(); ) {
			if (!it->first->get_is_persistent()) {
				it = instances_sorted.erase(it);
			} else {
				++it;
			}
		}
		created_instances.clear();
		next_instance_id = 0;
		for (auto& event_map : instances_sorted_events) {
			for (auto it=event_map.second.begin(); it!=event_map.second.end(); ) {
				if (!it->first->get_is_persistent()) {
					it = event_map.second.erase(it);
				} else {
					++it;
				}
			}
		}

		backgrounds.clear();
		views.clear();
		views.push_back(ViewPort());

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		automatic_paths.clear();
		automatic_timelines.clear();
	}
	int Room::transfer_instances(const Room* old_room) {
		if (old_room == nullptr) {
			Object* obj_control = Object::get_by_name("obj_control");
			if (obj_control == nullptr) {
				return 2;
			}

			// If an obj_control exists, create it for the first room
			int index = next_instance_id++;
			Instance* inst_control = new Instance(index, obj_control, btVector3(0.0, 0.0, 0.0));
			set_instance(index, inst_control);
			sort_instances();
			obj_control->add_instance(index, inst_control);

			for (E_EVENT e : obj_control->get_events()) {
				instances_sorted_events[e].emplace(inst_control, inst_control->id);
			}

			created_instances.push_back(inst_control);

			return 1;
		}

		for (auto& inst : instances) {
			inst.second->get_object()->remove_instance(inst.first);
		}

		const std::map<int,Instance*> old_instances = old_room->get_instances();

		instances.clear();
		instances_sorted.clear();
		instances_sorted_events.clear();

		for (auto& inst : old_instances) {
			set_instance(inst.first, inst.second);
			inst.second->get_object()->add_instance(inst.first, inst.second);

			if (inst.second->get_physbody() != nullptr) {
				PhysicsBody* b = inst.second->get_physbody();
				add_physbody(inst.second, b);
				b->attach(get_phys_world());
			}

			for (E_EVENT e : inst.second->get_object()->get_events()) {
				instances_sorted_events[e].emplace(inst.second, inst.first);
			}
		}
		sort_instances();

		return 0;
	}

	void Room::create() {
		for (auto& i : created_instances) {
			i->get_object()->update(i);
			i->get_object()->create(i);
		}
		created_instances.clear();

		if (should_sort) {
			sort_instances();
			should_sort = false;
		}
	}
	void Room::destroy() {
		std::set<Instance*> destroyed;

		while (!destroyed_instances.empty()) {
			Instance* inst = destroyed_instances.back();
			destroyed_instances.pop_back();

			if (destroyed.find(inst) == destroyed.end()) {
				destroyed.insert(inst);

				if (!inst->get_is_persistent()) {
					inst->get_object()->update(inst);
					inst->get_object()->destroy(inst);
					remove_instance(inst->id);
				}
			}
		}

		if (should_sort) {
			sort_instances();
			should_sort = false;
		}
	}
	void Room::destroy(Instance* inst) {
		destroyed_instances.push_back(inst);
	}
	void Room::destroy_all(Object* obj) {
		for (auto& i : obj->get_instances()) {
			destroyed_instances.push_back(i.second);
		}
	}
	void Room::check_alarms() {
		for (auto& i : instances_sorted_events[E_EVENT::ALARM]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}

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
	void Room::step_begin() {
		for (auto& i : instances_sorted_events[E_EVENT::STEP_BEGIN]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_begin(i.first);
		}
	}
	void Room::step_mid() {
		for (auto& i : instances_sorted_events[E_EVENT::STEP_MID]) {
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
	void Room::step_end() {
		for (auto& i : instances_sorted_events[E_EVENT::STEP_END]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_end(i.first);
		}
	}
	void Room::keyboard_press(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::KEYBOARD_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_press(i.first, e);
		}
	}
	void Room::mouse_press(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::MOUSE_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_press(i.first, e);
		}
	}
	void Room::keyboard_input(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::KEYBOARD_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_input(i.first, e);
		}
	}
	void Room::mouse_input(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::MOUSE_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_input(i.first, e);
		}
	}
	void Room::keyboard_release(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::KEYBOARD_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_release(i.first, e);
		}
	}
	void Room::mouse_release(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::MOUSE_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_release(i.first, e);
		}
	}
	void Room::controller_axis(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_AXIS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_axis(i.first, e);
		}
	}
	void Room::controller_press(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_press(i.first, e);
		}
	}
	void Room::controller_release(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_release(i.first, e);
		}
	}
	void Room::controller_modify(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_MODIFY]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_modify(i.first, e);
		}
	}
	void Room::commandline_input(const std::string& input) {
		for (auto& i : instances_sorted_events[E_EVENT::COMMANDLINE_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->commandline_input(i.first, input);
		}
	}
	void Room::check_paths() {
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
				if (instances_sorted_events[E_EVENT::PATH_END].find(ipf.first) != instances_sorted_events[E_EVENT::PATH_END].end()) {
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
	void Room::outside_room() {
		for (auto& i : instances_sorted_events[E_EVENT::OUTSIDE_ROOM]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			if (i.first->get_object()->get_sprite() != nullptr) {
				SDL_Rect a = i.first->get_aabb();
				SDL_Rect b = {0, 0, get_width(), get_height()};
				if (!util::check_collision(a, b)) {
					i.first->get_object()->update(i.first);
					i.first->get_object()->outside_room(i.first);
				}
			}
		}
	}
	void Room::intersect_boundary() {
		for (auto& i : instances_sorted_events[E_EVENT::INTERSECT_BOUNDARY]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->intersect_boundary(i.first);
		}
	}
	int Room::collision() {
		if (get_is_paused()) {
			return 1;
		}

		physics_world->step(get_delta());

		return 0;
	}
	void Room::collision_internal(btDynamicsWorld* w, btScalar timestep) {
		PhysicsWorld* world = static_cast<PhysicsWorld*>(w->getWorldUserInfo());
		const std::map<const btRigidBody*,Instance*>& physics_instances = get_current_room()->get_phys_instances();

		//w->clearForces();

		size_t manifold_amount = world->get_dispatcher()->getNumManifolds();
		for (size_t i=0; i<manifold_amount; ++i) {
			btPersistentManifold* manifold = world->get_dispatcher()->getManifoldByIndexInternal(i);
			const btRigidBody* body1 = btRigidBody::upcast(manifold->getBody0());
			const btRigidBody* body2 = btRigidBody::upcast(manifold->getBody1());

			if ((physics_instances.find(body1) != physics_instances.end())&&(physics_instances.find(body2) != physics_instances.end())) {
				Instance* i1 = physics_instances.at(body1);
				Instance* i2 = physics_instances.at(body2);

				if (
					(i1 != nullptr)
					&&(i2 != nullptr)
					&&(i1->get_object() != nullptr)
					&&(i2->get_object() != nullptr)
				) {
					/*if (get_is_paused()) {
						if ((i1->get_object()->get_is_pausable())||(i2->get_object()->get_is_pausable())) {
							continue;
						}
					}*/

					i1->get_object()->update(i1);
					i1->get_object()->collision(i1, i2);
					i2->get_object()->update(i2);
					i2->get_object()->collision(i2, i1);
				} else {
					/*if ((i1 == nullptr)||(i1->get_object() == nullptr)) {
						physics_instances.erase(body1);
					}
					if ((i2 == nullptr)||(i2->get_object() == nullptr)) {
						physics_instances.erase(body2);
					}*/
				}
			}
		}
	}
	bool Room::check_collision_filter(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) {
		bool should_collide = false;
		const std::map<const btRigidBody*,Instance*>& physics_instances = get_current_room()->get_phys_instances();

		btRigidBody* body1 = static_cast<btRigidBody*>(proxy0->m_clientObject);
		btRigidBody* body2 = static_cast<btRigidBody*>(proxy1->m_clientObject);

		if ((physics_instances.find(body1) != physics_instances.end())&&(physics_instances.find(body2) != physics_instances.end())) {
			Instance* i1 = physics_instances.at(body1);
			Instance* i2 = physics_instances.at(body2);

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
			} else {
				/*if ((i1 == nullptr)||(i1->get_object() == nullptr)) {
					physics_instances.erase(body1);
				}
				if ((i2 == nullptr)||(i2->get_object() == nullptr)) {
					physics_instances.erase(body2);
				}*/
			}
		}

		return should_collide;
	}
	void Room::draw() {
		Texture* prev_target = render::get_target();

		for (auto& v : views) {
			if (v.is_active) {
				view_current = &v;
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

		for (auto& v : views) {
			if (v.is_active) {
				v.draw();
			}
		}

		if (console::get_is_open()) {
			console::internal::draw();
		}

		render::render();
	}
	void Room::draw_view(ViewPort* viewport) {
		// Draw backgrounds
		draw_set_color(RGBA(E_RGB::WHITE));

		render::set_target(viewport->texture.get());
		render::set_viewport(viewport);

		render::clear();
		render::render_lights();

		for (auto& b : backgrounds) {
			if ((b.is_visible)&&(!b.is_foreground)) {
				b.texture->draw_transform(b.transform);
			}
		}
		render::render_textures();

		// Draw instances
		for (auto& i : instances_sorted_events[E_EVENT::DRAW]) {
			i.first->get_object()->update(i.first);
			i.first->get_object()->draw(i.first);
		}
		render::render_textures();

		// Draw foregrounds
		for (auto& b : backgrounds) {
			if ((b.is_visible)&&(b.is_foreground)) {
				b.texture->draw_transform(b.transform);
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
	void Room::animation_end() {
		for (auto& i : instances_sorted_events[E_EVENT::ANIMATION_END]) {
			if (i.first->get_object()->get_sprite() != nullptr) {
				if (!i.first->get_object()->get_sprite()->get_is_animated()) {
					i.first->get_object()->update(i.first);
					i.first->get_object()->animation_end(i.first);
				}
			}
		}
	}
	void Room::room_start() {
		this->start();

		for (auto& i : instances_sorted_events[E_EVENT::ROOM_START]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->room_start(i.first);
		}
	}
	void Room::room_end() {
		for (auto& i : instances_sorted_events[E_EVENT::ROOM_END]) {
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
	void Room::game_start() {
		for (auto& i : instances_sorted_events[E_EVENT::GAME_START]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->game_start(i.first);
		}
	}
	void Room::game_end() {
		for (auto& i : instances_sorted_events[E_EVENT::GAME_END]) {
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
	void Room::window(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::WINDOW]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->window(i.first, e);
		}
	}
	void Room::network(const NetworkEvent& e) {
		for (auto& i :instances_sorted_events[E_EVENT::NETWORK]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->network(i.first, e);
		}
	}

	void Room::init() {
		physics_world = new PhysicsWorld();
	}
}

#endif // BEE_ROOM
