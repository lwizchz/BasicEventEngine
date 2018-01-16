/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ROOM
#define BEE_ROOM 1

#include "../defines.hpp"

#include <sstream> // Include the required library headers
#include <fstream>
#include <algorithm>
#include <set>
#include <iterator>

#include <glm/gtc/type_ptr.hpp> // Include the required OpenGL headers

#include "room.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/string.hpp"
#include "../util/collision.hpp"
#include "../util/files.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/console.hpp"
#include "../core/enginestate.hpp"
#include "../core/rooms.hpp"

#include "../render/drawing.hpp"
#include "../render/render.hpp"
#include "../render/renderer.hpp"
#include "../render/shader.hpp"
#include "../render/transition.hpp"
#include "../render/viewport.hpp"
#include "../render/particle/system.hpp"

#include "../physics/body.hpp"
#include "../physics/world.hpp"

#include "texture.hpp"
#include "timeline.hpp"
#include "light.hpp"
#include "object.hpp"

namespace bee {
	namespace internal {
		std::pair<Instance*,int> flip_instancemap_pair(const std::pair<int,Instance*>& p) {
			return std::pair<Instance*,int>(p.second, p.first);
		}
	}

	/*
	* BackgroundData::BackgroundData() - Construct the data struct and initiliaze all values
	*/
	BackgroundData::BackgroundData() :
		texture(nullptr),
		is_visible(false),
		is_foreground(false),
		transform()
	{}
	/*
	* BackgroundData::BackgroundData() - Construct the data struct and initiliaze with all the given values
	*/
	BackgroundData::BackgroundData(Texture* _texture, bool _is_visible, bool _is_foreground, int _x, int _y, bool _is_horizontal_tile, bool _is_vertical_tile, int _horizontal_speed, int _vertical_speed, bool _is_stretched) :
		texture(_texture),
		is_visible(_is_visible),
		is_foreground(_is_foreground),
		transform(_x, _y, _is_horizontal_tile, _is_vertical_tile, _horizontal_speed, _vertical_speed, _is_stretched)
	{}

	const std::list<E_EVENT> Room::event_list = {
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
		E_EVENT::WINDOW,
		E_EVENT::NETWORK
	};

	std::map<int,Room*> Room::list;
	int Room::next_id = 0;

	Room::Room() :
		Resource(),

		id(-1),
		name(),
		path(),
		width(DEFAULT_WINDOW_WIDTH),
		height(DEFAULT_WINDOW_HEIGHT),
		is_isometric(false),
		is_persistent(false),

		background_color({255, 255, 255, 255}),
		is_background_color_enabled(true),
		backgrounds(),
		views({new ViewPort()}),

		next_instance_id(0),
		instances(),
		instances_sorted(),
		created_instances(),
		destroyed_instances(),
		should_sort(false),

		instances_sorted_events(),

		particle_systems(),

		lights(),
		lightables(),
		light_map(nullptr),

		physics_world(nullptr),
		physics_instances(),

		instance_map(),

		view_current(nullptr)
	{}
	Room::Room(const std::string& new_name, const std::string& new_path) :
		Room()
	{
		add_to_resources();
		if (id < 0) {
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add room resource: \"" + new_name + "\"" + new_path);
			throw(-1);
		}

		set_name(new_name);
		set_path(new_path);
	}
	Room::~Room() {
		backgrounds.clear();

		for (auto& v : views) {
			delete v;
		}
		views.clear();

		instances.clear();
		instances_sorted.clear();
		particle_systems.clear();
		destroyed_instances.clear();
		instances_sorted_events.clear();

		reset_lights();
		if (light_map != nullptr) {
			delete light_map;
			light_map = nullptr;
		}

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
	Room* Room::get(int id) {
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
		/*Room* new_room = new Room(name, path);
		new_room->load();
		return new_room;*/

		return nullptr; // Right now rooms cannot be added on the fly because they must be compiled
	}

	/*
	* Room::add_to_resources() - Add the room to the appropriate resource list
	*/
	int Room::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0;
	}
	int Room::reset() {
		name = "";
		path = "";
		width = DEFAULT_WINDOW_WIDTH;
		height = DEFAULT_WINDOW_HEIGHT;
		is_isometric = false;
		is_persistent = false;

		background_color = {255, 255, 255, 255};
		is_background_color_enabled = true;
		backgrounds.clear();

		for (auto& v : views) {
			delete v;
		}
		views.clear();
		views.push_back(new ViewPort());

		for (auto& i : instances) {
			delete i.second;
		}
		instances.clear();
		instances_sorted.clear();
		created_instances.clear();
		destroyed_instances.clear();
		instances_sorted_events.clear();
		should_sort = false;

		particle_systems.clear();
		next_instance_id = 0;

		reset_lights();
		if (light_map != nullptr) {
			delete light_map;
		}

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		return 0;
	}
	int Room::print() const {
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, get_print());
		return 0;
	}
	std::string Room::get_print() const {
		std::string background_string = get_background_string();
		std::string view_string = get_view_string();
		std::string instance_string = get_instance_string();

		background_string.pop_back();
		view_string.pop_back();
		instance_string.pop_back();

		std::stringstream s;
		s <<
		"Room { "
		"\n	id                          " << id <<
		"\n	name                        " << name <<
		"\n	path                        " << path <<
		"\n	width                       " << width <<
		"\n	height                      " << height <<
		"\n	is_isometric                " << is_isometric <<
		"\n	is_persistent               " << is_persistent <<
		"\n	is_background_color_enabled " << is_background_color_enabled;
		if (is_background_color_enabled) {
			s << "\n	background_color            " << static_cast<int>(background_color.r) << ", " << static_cast<int>(background_color.g) << ", " << static_cast<int>(background_color.b);
		}
		s <<
		"\n	backgrounds\n" << debug_indent(background_string, 2) <<
		"\n	views\n" << debug_indent(view_string, 2) <<
		"	instances\n" << debug_indent(instance_string, 2) <<
		"}\n";

		return s.str();
	}

	int Room::get_id() const {
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
	bool Room::get_is_isometric() const {
		return is_isometric;
	}
	bool Room::get_is_persistent() const {
		return is_persistent;
	}
	RGBA Room::get_background_color() const {
		return background_color;
	}
	bool Room::get_is_background_color_enabled() const {
		return is_background_color_enabled;
	}
	std::vector<BackgroundData*> Room::get_backgrounds() const {
		return backgrounds;
	}
	std::string Room::get_background_string() const {
		if (backgrounds.size() > 0) {
			std::vector<std::vector<std::string>> table;
			table.push_back({"(name", "visible", "fore", "x", "y", "htile", "vtile", "hspeed", "vspeed", "stretch)"});

			for (auto& b : backgrounds) {
				const TextureTransform& tr = b->transform;
				table.push_back({
					b->texture->get_name(), booltostring(b->is_visible), booltostring(b->is_foreground),
					bee_itos(tr.x), bee_itos(tr.y), booltostring(tr.is_horizontal_tile), booltostring(tr.is_vertical_tile),
					bee_itos(tr.horizontal_speed), bee_itos(tr.vertical_speed), booltostring(tr.is_stretched)
				});
			}

			return string_tabulate(table);
		}
		return "none\n";
	}
	std::vector<ViewPort*> Room::get_views() const {
		return views;
	}
	std::string Room::get_view_string() const {
		if (views.size() > 0) {
			std::vector<std::vector<std::string>> table;
			table.push_back({"(active", "vx,", "vy", "vwidth", "vheight", "px,", "py", "pwidth", "pheight)"});

			for (auto& v : views) {
				table.push_back({
					booltostring(v->is_active),
					bee_itos(v->view.x), bee_itos(v->view.y),
					bee_itos(v->view.w), bee_itos(v->view.h),
					bee_itos(v->port.x), bee_itos(v->port.y),
					bee_itos(v->port.w), bee_itos(v->port.h)
				});
			}

			return string_tabulate(table);
		}
		return "none\n";
	}
	const std::map<int,Instance*>& Room::get_instances() const {
		return instances;
	}
	std::string Room::get_instance_string() const {
		if (instances.size() > 0) {
			std::vector<std::vector<std::string>> table;
			table.push_back({"(id", "object", "x", "y", "z)"});

			for (auto& i : instances_sorted) {
				table.push_back({
					bee_itos(i.first->id),
					i.first->get_object()->get_name(),
					bee_itos(static_cast<int>(i.first->get_x())),
					bee_itos(static_cast<int>(i.first->get_y())),
					bee_itos(static_cast<int>(i.first->get_z()))
				});
			}

			return string_tabulate(table);
		}
		return "none\n";
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

	int Room::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Room::set_path(const std::string& new_path) {
		if (new_path.front() == '/') {
			path = new_path.substr(1);
		} else {
			path = "resources/rooms/"+new_path; // Append the path to the room directory if no root
		}
		return 0;
	}
	int Room::set_width(int new_width) {
		width = new_width;
		return 0;
	}
	int Room::set_height(int new_height) {
		height = new_height;
		return 0;
	}
	int Room::set_is_isometric(bool new_is_isometric) {
		is_isometric = new_is_isometric;
		return 0;
	}
	int Room::set_is_persistent(bool new_is_persistent) {
		is_persistent = new_is_persistent;
		return 0;
	}
	int Room::set_background_color(RGBA new_background_color) {
		background_color = new_background_color;
		return 0;
	}
	int Room::set_is_background_color_enabled(bool new_is_background_color_enabled) {
		is_background_color_enabled = new_is_background_color_enabled;
		return 0;
	}
	int Room::set_background(int desired_index, BackgroundData* new_background) {
		size_t new_index = desired_index;

		// If the index doesn't exist, append the backgrounds
		if (new_index >= backgrounds.size()) {
			new_index = backgrounds.size();
			backgrounds.push_back(new_background);
			return new_index;
		}

		// Otherwise, overwrite any previous background with the same index
		backgrounds[new_index] = new_background;
		return new_index;
	}
	int Room::add_background(Texture* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
		BackgroundData* background = new BackgroundData(new_background, new_is_visible, new_is_foreground, new_x, new_y, new_is_horizontal_tile, new_is_vertical_tile, new_horizontal_speed, new_vertical_speed, new_is_stretched);
		return set_background(-1, background);
	}
	int Room::set_view(int desired_index, ViewPort* new_view) {
		size_t new_index = desired_index;

		// If the index doesn't exist, append the view
		if (new_index >= views.size()) {
			new_index = views.size();
			views.push_back(new_view);
			return new_index;
		}

		// Otherwise, overwrite any previous view with the same index
		views[new_index] = new_view;
		return new_index;
	}
	int Room::set_instance(int index, Instance* new_instance) {
		if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
			remove_instance(index);
		}
		instances.insert(std::pair<int,Instance*>(index, new_instance));
		return 0;
	}
	Instance* Room::add_instance(int index, Object* object, double x, double y, double z) {
		if (object->get_sprite() != nullptr) {
			if (
				(!object->get_sprite()->get_is_loaded())
				&&(get_is_ready())
				&&(!get_options().is_headless)
			) {
				messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "An instance of " + object->get_name() + " has been created but its sprite has not been loaded");
			}
		}

		if (index < 0) {
			index = next_instance_id++;
		}
		while (instances.find(index) != instances.end()) {
			index = next_instance_id++;
		}

		Instance* new_instance = new Instance(index, object, x, y, z);
		set_instance(index, new_instance);
		sort_instances();
		object->add_instance(index, new_instance);

		for (E_EVENT e : object->implemented_events) {
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
	int Room::add_instance_grid(int index, Object* object, double x, double y, double z) {
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
				yg = y*object->get_sprite()->get_height();
			} else {
				yg = height + y*object->get_sprite()->get_height();
			}
		}

		return add_instance(index, object, xg, yg, z)->id;
	}
	int Room::remove_instance(int index) {
		if (instances.find(index) != instances.end()) {
			Instance* inst = instances[index];

			if (inst->get_physbody() == nullptr) {
				messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Null physbody for " + inst->get_object()->get_name() + ":" + bee_itos(index) + "\n");
			} else {
				remove_physbody(inst->get_physbody());
			}

			inst->get_object()->remove_instance(index);
			instances.erase(index);
			instances_sorted.erase(inst);

			for (E_EVENT e : inst->get_object()->implemented_events) {
				instances_sorted_events[e].erase(inst);
			}

			delete inst;

			return 0;
		}
		return 1;
	}
	int Room::sort_instances() {
		std::transform(instances.begin(), instances.end(), std::inserter(instances_sorted, instances_sorted.begin()), internal::flip_instancemap_pair);
		return 0;
	}
	int Room::request_instance_sort() {
		should_sort = true;
		return 0;
	}
	int Room::add_physbody(Instance* inst, PhysicsBody* body) {
		if (physics_instances.find(body->get_body()) == physics_instances.end()) {
			physics_instances.emplace(body->get_body(), inst);
		}
		return 0;
	}
	int Room::remove_physbody(PhysicsBody* body) {
		physics_instances.erase(body->get_body());
		return 0;
	}
	int Room::add_particle_system(ParticleSystem* new_system) {
		new_system->load();
		particle_systems.emplace_back(new_system);
		return 0;
	}
	int Room::add_lightable(LightableData* lightable) {
		lightables.push_back(lightable);
		return 0;
	}
	int Room::add_light(LightData lighting) {
		lighting.attenuation.x = 10000.f/lighting.attenuation.x;
		lighting.attenuation.y = 1000.f/lighting.attenuation.y;
		lighting.attenuation.z = 1000.f/lighting.attenuation.z;
		lights.push_back(lighting);
		return 0;
	}
	int Room::handle_lights() {
		int i = 0;
		for (auto& l : lightables) {
			if (i >= BEE_MAX_LIGHTABLES) {
				break;
			}

			glUniform4fv(render::get_program()->get_location("lightable[" + bee_itos(i) + "].position"), 1, glm::value_ptr(l->position));
			int j = 0;
			for (auto& v : l->mask) {
				if (j >= BEE_MAX_MASK_VERTICES) {
					break;
				}

				glUniform4fv(render::get_program()->get_location("lightable[" + bee_itos(i) + "].mask[" + bee_itos(j) + "]"), 1, glm::value_ptr(v));

				j++;
			}
			glUniform1i(render::get_program()->get_location("lightable[" + bee_itos(i) + "].vertex_amount"), j);

			i++;
		}
		glUniform1i(render::get_program()->get_location("lightable_amount"), i);

		i = 0;
		for (auto& l : lights) {
			if (i >= BEE_MAX_LIGHTS) {
				break;
			}

			glm::vec4 c (l.color.r, l.color.g, l.color.b, l.color.a);
			c /= 255.0f;

			glUniform1i(render::get_program()->get_location("lighting[" + bee_itos(i) + "].type"), static_cast<int>(l.type));
			glUniform4fv(render::get_program()->get_location("lighting[" + bee_itos(i) + "].position"), 1, glm::value_ptr(l.position));
			glUniform4fv(render::get_program()->get_location("lighting[" + bee_itos(i) + "].direction"), 1, glm::value_ptr(l.direction));
			glUniform4fv(render::get_program()->get_location("lighting[" + bee_itos(i) + "].attenuation"), 1, glm::value_ptr(l.attenuation));
			glUniform4fv(render::get_program()->get_location("lighting[" + bee_itos(i) + "].color"), 1, glm::value_ptr(c));

			i++;
		}
		glUniform1i(render::get_program()->get_location("light_amount"), i);

		reset_lights();

		return 0;
	}
	int Room::reset_lights() {
		for (auto& l : lightables) {
			delete l;
		}
		lightables.clear();
		lights.clear();
		return 0;
	}
	int Room::clear_lights() {
		glUniform1i(render::get_program()->get_location("lightable_amount"), 0);
		glUniform1i(render::get_program()->get_location("light_amount"), 0);
		return 0;
	}

	int Room::reset_properties() {
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

		lights.clear();
		if (light_map != nullptr) {
			delete light_map;
		}
		light_map = new Texture();

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_instances.clear();

		// Reset background data
		for (auto& b : backgrounds) {
			delete b;
		}
		backgrounds.clear();

		particle_systems.clear();

		return 0;
	}
	int Room::transfer_instances(const Room* old_room) {
		if (old_room == nullptr) {
			Object* obj_control = Object::get_by_name("obj_control");
			if (obj_control == nullptr) {
				return 2;
			}

			// If an obj_control exists, create it for the first room
			int index = next_instance_id++;
			Instance* inst_control = new Instance(index, obj_control, 0.0, 0.0, 0.0);
			set_instance(index, inst_control);
			sort_instances();
			obj_control->add_instance(index, inst_control);

			for (E_EVENT e : obj_control->implemented_events) {
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

			for (E_EVENT e : inst.second->get_object()->implemented_events) {
				instances_sorted_events[e].emplace(inst.second, inst.first);
			}
		}
		sort_instances();

		return 0;
	}

	int Room::save_instance_map(const std::string& fname) {
		std::ofstream savefile (fname, std::ios::trunc);
		if (!savefile.good()) {
			return 1;
		}

		for (auto& i : instances) {
			savefile << i.second->get_object()->get_name() << "\t" << i.second->get_xstart() << "\t" << i.second->get_ystart() << "\t" << i.second->get_zstart() << "\n";
		}
		savefile.close();
		return 0;
	}
	int Room::load_instance_map(const std::string& fname) {
		if (get_is_ready()) {
			instance_map = fname;
			restart_room();
			return 0;
		}

		std::string datastr = file_get_contents(fname);
		if (datastr.empty()) {
			messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "No instances loaded");
			return 1;
		}

		std::istringstream data_stream (datastr);

		while (!data_stream.eof()) {
			std::string tmp;
			getline(data_stream, tmp);

			if (tmp.empty()) {
				continue;
			}

			std::map<int,std::string> p = split(trim(tmp), '\t');
			std::map<int,std::string> params;
			for (auto& e : p) { // Remove empty values
				if (!e.second.empty()) {
					params.emplace(params.size(), e.second);
				}
			}
			p.clear();
			std::string v = params[0];

			if (params[0][0] == '#') {
				continue;
			} else if (params[0][0] == '!') {
				if (v == "!tilex") {
					unsigned int tile_amount = std::stoi(params[1]);
					double grid_x = std::stod(params[2]);

					std::string o = params[3];
					Object* object = Object::get_by_name(o);

					if (object == nullptr) {
						messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown object " + o);
						continue;
					}

					double x = std::stod(params[4]);
					double y = std::stod(params[5]);
					double z = std::stod(params[6]);

					for (size_t i=0; i<tile_amount; ++i) {
						add_instance(-1, object, x + i*grid_x, y, z);
					}

					continue;
				} else if (v == "!tiley") {
					unsigned int tile_amount = std::stoi(params[1]);
					double grid_y = std::stod(params[2]);

					std::string o = params[3];
					Object* object = Object::get_by_name(o);

					if (object == nullptr) {
						messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown object " + o);
						continue;
					}

					double x = std::stod(params[4]);
					double y = std::stod(params[5]);
					double z = std::stod(params[6]);

					for (size_t i=0; i<tile_amount; ++i) {
						add_instance(-1, object, x, y + i*grid_y, z);
					}

					continue;
				} else if (v == "!tilez") {
					unsigned int tile_amount = std::stoi(params[1]);
					double grid_z = std::stod(params[2]);

					std::string o = params[3];
					Object* object = Object::get_by_name(o);

					if (object == nullptr) {
						messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown object " + o);
						continue;
					}

					double x = std::stod(params[4]);
					double y = std::stod(params[5]);
					double z = std::stod(params[6]);

					for (size_t i=0; i<tile_amount; ++i) {
						add_instance(-1, object, x, y, z + i*grid_z);
					}

					continue;
				} else if (v == "!set") {
					std::string o = params[1];
					Object* object = Object::get_by_name(o);

					double x = std::stod(params[2]);
					double y = std::stod(params[3]);
					double z = std::stod(params[4]);

					Instance* inst = add_instance(-1, object, x, y, z);

					while (!data_stream.eof()) {
						std::string tmp_set;
						getline(data_stream, tmp_set);

						if (tmp_set.empty()) {
							continue;
						}

						std::map<int,std::string> set_params = split(trim(tmp_set), '\t');

						if (set_params[0][0] == '@') {
							if (set_params[0] == "@sprite") {
								inst->set_sprite(Texture::get_by_name(set_params[1]));
							} else if (set_params[0] == "@solid") {
								inst->set_is_solid(std::stoi(set_params[1]));
							} else if (set_params[0] == "@depth") {
								inst->depth = std::stoi(set_params[1]);
							} else {
								messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown setter \"" + v + "\"");
								continue;
							}
						} else if (set_params[0] == "!setend") {
							break;
						} else {
							inst->set_data(set_params[1], Variant(set_params[2], true));
						}
					}

					continue;
				} else if (v == "!setend") {
					messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: stray !setend");
					continue;
				} else {
					messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown command \"" + v + "\"");
					continue;
				}
			} else {
				double x = std::stod(params[1]);
				double y = std::stod(params[2]);
				double z = std::stod(params[3]);

				Object* object = Object::get_by_name(v);
				if (object == nullptr) {
					messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown object " + v);
					continue;
				} else {
					add_instance(-1, object, x, y, z);
				}
			}
		}

		return 0;
	}
	int Room::load_instance_map() {
		if (!instance_map.empty()) {
			return load_instance_map(instance_map);
		}
		return 1;
	}
	std::string Room::get_instance_map() const {
		return instance_map;
	}
	int Room::set_instance_map(const std::string& new_instance_map) {
		instance_map = new_instance_map;
		return 1;
	}

	int Room::create() {
		for (auto& i : created_instances) {
			i->get_object()->update(i);
			i->get_object()->create(i);
		}
		created_instances.clear();

		return 0;
	}
	int Room::destroy() {
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

		return 0;
	}
	int Room::destroy(Instance* inst) {
		destroyed_instances.push_back(inst);
		return 0;
	}
	int Room::destroy_all(Object* obj) {
		for (auto& i : obj->get_instances()) {
			destroyed_instances.push_back(i.second);
		}

		return 0;
	}
	int Room::check_alarms() {
		for (auto& i : instances_sorted_events[E_EVENT::ALARM]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			for (size_t e=0; e<BEE_ALARM_COUNT; ++e) {
				if (i.first->alarm_end[e] != 0xffffffff) {
					if (get_ticks() >= i.first->alarm_end[e]) {
						i.first->alarm_end[e] = 0xffffffff; // Reset alarm
						i.first->get_object()->update(i.first);
						i.first->get_object()->alarm(i.first, e);
					}
				}
			}
		}

		return 0;
	}
	int Room::step_begin() {
		for (auto& i : instances_sorted_events[E_EVENT::STEP_BEGIN]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_begin(i.first);
		}

		return 0;
	}
	int Room::step_mid() {
		for (auto& i : instances_sorted_events[E_EVENT::STEP_MID]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_mid(i.first);
		}

		// Move instances along their paths
		for (auto& i : instances_sorted) {
			if (i.first->has_path()) {
				if (
					(get_is_paused())
					&&(i.first->get_object()->get_is_pausable())
					&&(i.first->get_path_pausable())
				) {
					continue;
				}

				i.first->path_update_node();

				path_coord_t c (0.0, 0.0, 0.0, 0.0);
				if (i.first->get_path_speed() >= 0) {
					if (i.first->get_path_node()+1 < static_cast<int>(i.first->get_path_coords().size())) {
						c = i.first->get_path_coords().at(i.first->get_path_node()+1);
					} else {
						break;
					}
				} else if (i.first->get_path_node() >= 0) {
					c = i.first->get_path_coords().at(i.first->get_path_node());
				}

				i.first->set_position(
					i.first->get_position()
					+ btScalar(std::get<3>(c)*abs(i.first->get_path_speed()))
					* direction_of(
						i.first->get_x(), i.first->get_y(), i.first->get_z(),
						i.first->path_pos_start.x()+std::get<0>(c), i.first->path_pos_start.y()+std::get<1>(c), i.first->path_pos_start.z()+std::get<2>(c)
					) * btScalar(get_delta())
				);
			}
		}

		// Run timelines
		for (size_t i=0; i<Timeline::get_amount(); ++i) {
			Timeline* tt = Timeline::get(i);
			if (tt->get_is_running()) {
				int r = tt->step_to(get_frame());
				if (r == 2) {
					tt->end();
					if (tt->get_is_looping()) {
						tt->start();
					}
				}
			}
		}

		return 0;
	}
	int Room::step_end() {
		for (auto& i : instances_sorted_events[E_EVENT::STEP_END]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->step_end(i.first);
		}

		return 0;
	}
	int Room::keyboard_press(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::KEYBOARD_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_press(i.first, e);
		}

		return 0;
	}
	int Room::mouse_press(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::MOUSE_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_press(i.first, e);
		}

		return 0;
	}int Room::keyboard_input(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::KEYBOARD_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_input(i.first, e);
		}

		return 0;
	}
	int Room::mouse_input(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::MOUSE_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_input(i.first, e);
		}

		return 0;
	}
	int Room::keyboard_release(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::KEYBOARD_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->keyboard_release(i.first, e);
		}

		return 0;
	}
	int Room::mouse_release(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::MOUSE_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->mouse_release(i.first, e);
		}

		return 0;
	}
	int Room::controller_axis(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_AXIS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_axis(i.first, e);
		}

		return 0;
	}
	int Room::controller_press(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_PRESS]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_press(i.first, e);
		}

		return 0;
	}
	int Room::controller_release(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_RELEASE]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_release(i.first, e);
		}

		return 0;
	}
	int Room::controller_modify(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::CONTROLLER_MODIFY]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->controller_modify(i.first, e);
		}

		return 0;
	}
	int Room::commandline_input(const std::string& input) {
		for (auto& i : instances_sorted_events[E_EVENT::COMMANDLINE_INPUT]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->commandline_input(i.first, input);
		}

		return 0;
	}
	int Room::check_paths() {
		for (auto& i : instances_sorted) {
			if (i.first->has_path()) {
				if (
					(get_is_paused())
					&&(i.first->get_object()->get_is_pausable())
					&&(i.first->get_path_pausable())
				) {
					continue;
				}

				if (
					(
						(i.first->get_path_speed() >= 0)
						&&(i.first->get_path_node() == static_cast<int>(i.first->get_path_coords().size())-1)
					)
					||(
						(i.first->get_path_speed() < 0)
						&&(i.first->get_path_node() == -1)
					)
				) {
					if (instances_sorted_events[E_EVENT::PATH_END].find(i.first) != instances_sorted_events[E_EVENT::PATH_END].end()) {
						i.first->get_object()->update(i.first);
						i.first->get_object()->path_end(i.first);
					}
					i.first->handle_path_end();
				}
			}
		}

		return 0;
	}
	int Room::outside_room() {
		for (auto& i : instances_sorted_events[E_EVENT::OUTSIDE_ROOM]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			if (i.first->get_object()->get_mask() != nullptr) {
				SDL_Rect a = i.first->get_aabb();
				SDL_Rect b = {0, 0, get_width(), get_height()};
				if (!check_collision(a, b)) {
					i.first->get_object()->update(i.first);
					i.first->get_object()->outside_room(i.first);
				}
			}
		}

		return 0;
	}
	int Room::intersect_boundary() {
		for (auto& i : instances_sorted_events[E_EVENT::INTERSECT_BOUNDARY]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->intersect_boundary(i.first);
		}

		return 0;
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
	int Room::draw() {
		engine->renderer->program->apply();

		for (auto& v : views) {
			if (v->is_active) {
				view_current = v;
				view_current->update();
				draw_view(view_current);
			}
		}
		view_current = nullptr;

		if (is_background_color_enabled) {
			draw_set_color(background_color);
		} else {
			draw_set_color(RGBA(E_RGB::WHITE));
		}

		render::reset_target();
		render::set_viewport(nullptr);
		render::clear();

		clear_lights();

		for (auto& v : views) {
			if (v->is_active) {
				v->draw();
			}
		}

		engine->renderer->program->apply();


		return 0;
	}
	int Room::draw_view(ViewPort* viewport) {
		// Draw backgrounds
		if (is_background_color_enabled) {
			draw_set_color(background_color);
		} else {
			draw_set_color(RGBA(E_RGB::WHITE));
		}

		render::set_target(viewport->texture);
		render::set_viewport(viewport);

		render::clear();
		handle_lights();

		for (auto& b : backgrounds) {
			if ((b->is_visible)&&(!b->is_foreground)) {
				b->texture->draw_transform(b->transform);
			}
		}
		render::render_textures();

		// Draw instances
		for (auto& i : instances_sorted_events[E_EVENT::DRAW]) {
			if (i.first->get_object()->get_is_visible()) {
				i.first->get_object()->update(i.first);
				i.first->get_object()->draw(i.first);
			}
		}
		render::render_textures();

		// Draw particles
		for (auto& psys : particle_systems) {
			psys->draw();
		}
		render::render_textures();

		// Draw foregrounds
		for (auto& b : backgrounds) {
			if ((b->is_visible)&&(b->is_foreground)) {
				b->texture->draw_transform(b->transform);
			}
		}

		// Draw instance paths
		for (auto& i : instances_sorted) {
			if (i.first->has_path()) {
				if ((get_options().is_debug_enabled)||(i.first->get_path_drawn())) {
					i.first->draw_path();
				}
			}
		}

		if (get_options().is_debug_enabled) {
			// Draw room outline
			draw_rectangle(0, 0, get_width(), get_height(), 1, RGBA(E_RGB::RED));

			// Draw physics engine debug shapes
			physics_world->draw_debug();

			// Draw particle system bounding boxes
			for (auto& psys : particle_systems) {
				psys->draw_debug();
			}
		}

		if (console::get_is_open()) {
			console::internal::draw();
		}

		render::render_textures();
		render::render();

		return 0;
	}
	int Room::animation_end() {
		for (auto& i : instances_sorted_events[E_EVENT::ANIMATION_END]) {
			if (i.first->get_object()->get_sprite() != nullptr) {
				if (!i.first->get_object()->get_sprite()->get_is_animated()) {
					i.first->get_object()->update(i.first);
					i.first->get_object()->animation_end(i.first);
				}
			}
		}

		return 0;
	}
	int Room::room_start() {
		this->start();

		for (auto& i : instances_sorted_events[E_EVENT::ROOM_START]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->room_start(i.first);
		}

		return 0;
	}
	int Room::room_end() {
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

		return 0;
	}
	int Room::game_start() {
		for (auto& i : instances_sorted_events[E_EVENT::GAME_START]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->game_start(i.first);
		}

		return 0;
	}
	int Room::game_end() {
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

		return 0;
	}
	int Room::window(SDL_Event* e) {
		for (auto& i : instances_sorted_events[E_EVENT::WINDOW]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->window(i.first, e);
		}

		return 0;
	}
	int Room::network(const NetworkEvent& e) {
		for (auto& i :instances_sorted_events[E_EVENT::NETWORK]) {
			if ((get_is_paused())&&(i.first->get_object()->get_is_pausable())) {
				continue;
			}
			i.first->get_object()->update(i.first);
			i.first->get_object()->network(i.first, e);
		}

		return 0;
	}

	void Room::init() {
		physics_world = new PhysicsWorld();
	}
}

#endif // BEE_ROOM
