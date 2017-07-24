/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

#include <glm/gtc/type_ptr.hpp> // Include the required OpenGL headers

#include "room.hpp" // Include the class resource header

#include "../debug.hpp"
#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/string.hpp"
#include "../util/collision.hpp"
#include "../util/files.hpp"
#include "../util/platform.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/console.hpp"
#include "../core/enginestate.hpp"
#include "../core/resources.hpp"
#include "../core/room.hpp"

#include "../render/drawing.hpp"
#include "../render/renderer.hpp"
#include "../render/transition.hpp"
#include "../render/viewdata.hpp"
#include "../render/particle/system.hpp"

#include "../physics/body.hpp"
#include "../physics/world.hpp"

#include "sprite.hpp"
#include "background.hpp"
#include "timeline.hpp"
#include "light.hpp"
#include "object.hpp"

namespace bee {
	namespace internal {
		std::pair<Instance*,int> flip_instancemap_pair(const std::pair<int,Instance*>& p) {
			return std::pair<Instance*,int>(p.second, p.first);
		}
	}

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

	Room::Room () :
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
		is_views_enabled(false),
		views(),

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
	Room::Room (const std::string& new_name, const std::string& new_path) :
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
		views.clear();
		instances.clear();
		instances_sorted.clear();
		particle_systems.clear();
		destroyed_instances.clear();
		instances_sorted_events.clear();

		reset_lights();
		if (light_map != nullptr) {
			delete light_map;
		}

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}

		free_media();
		if (list.find(id) != list.end()) { // Remove the room from the resource list
			list.erase(id);
		}
	}

	int Room::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0;
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
		is_views_enabled = false;
		views.clear();

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
		physics_world = new PhysicsWorld();
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
		"	is_views_enabled            " << is_views_enabled <<
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
				table.push_back({
					b->background->get_name(), booltostring(b->is_visible), booltostring(b->is_foreground),
					bee_itos(b->x), bee_itos(b->y), booltostring(b->is_horizontal_tile), booltostring(b->is_vertical_tile),
					bee_itos(b->horizontal_speed), bee_itos(b->vertical_speed), booltostring(b->is_stretched)
				});
			}

			return string_tabulate(table);
		}
		return "none\n";
	}
	bool Room::get_is_views_enabled() const {
		return is_views_enabled;
	}
	std::vector<ViewData*> Room::get_views() const {
		return views;
	}
	std::string Room::get_view_string() const {
		if (views.size() > 0) {
			std::vector<std::vector<std::string>> table;
			table.push_back({"(visible", "vx,", "vy", "vwidth", "vheight", "px,", "py", "pwidth", "pheight", "object", "hborder", "vborder", "hspeed", "vspeed)"});

			for (auto& v : views) {
				std::string follow_name = "none";
				if (v->following != nullptr) {
					follow_name = v->following->get_object()->get_name();
				}

				table.push_back({
					booltostring(v->is_visible),
					bee_itos(v->view_x), bee_itos(v->view_y),
					bee_itos(v->view_width), bee_itos(v->view_height),
					bee_itos(v->port_x), bee_itos(v->port_y),
					bee_itos(v->port_width), bee_itos(v->port_height),
					follow_name,
					bee_itos(v->horizontal_border), bee_itos(v->vertical_border),
					bee_itos(v->horizontal_speed), bee_itos(v->vertical_speed)
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
				table.push_back({bee_itos(i.first->id), i.first->get_object()->get_name(), bee_itos(i.first->get_x()), bee_itos(i.first->get_y()), bee_itos(i.first->get_z())});
			}

			return string_tabulate(table);
		}
		return "none\n";
	}
	ViewData* Room::get_current_view() const {
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
		path = "resources/rooms/"+new_path;
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
	int Room::set_background_color(Uint8 r, Uint8 g, Uint8 b) {
		background_color = {r, g, b, 255};
		return 0;
	}
	int Room::set_background_color(E_RGB new_background_color) {
		background_color = get_enum_color(new_background_color);
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
	int Room::add_background(Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
		BackgroundData* background = new BackgroundData(new_background, new_is_visible, new_is_foreground, new_x, new_y, new_is_horizontal_tile, new_is_vertical_tile, new_horizontal_speed, new_vertical_speed, new_is_stretched);
		return set_background(-1, background);
	}
	int Room::set_is_views_enabled(bool new_is_views_enabled) {
		is_views_enabled = new_is_views_enabled;
		return 0;
	}
	int Room::set_view(int desired_index, ViewData* new_view) {
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
		lighting.attenuation.x = 10000.0/lighting.attenuation.x;
		lighting.attenuation.y = 1000.0/lighting.attenuation.y;
		lighting.attenuation.z = 1000.0/lighting.attenuation.z;
		lights.push_back(lighting);
		return 0;
	}
	int Room::handle_lights() {
		if (get_options().renderer_type != E_RENDERER::SDL) {
			int i = 0;
			for (auto& l : lightables) {
				if (i >= BEE_MAX_LIGHTABLES) {
					break;
				}

				glUniform4fv(engine->renderer->lightable_location[i].position, 1, glm::value_ptr(l->position));
				int e = 0;
				for (auto& v : l->mask) {
					if (e >= BEE_MAX_MASK_VERTICES) {
						break;
					}

					glUniform4fv(engine->renderer->lightable_location[i].mask[e], 1, glm::value_ptr(v));

					e++;
				}
				glUniform1i(engine->renderer->lightable_location[i].vertex_amount, e);

				i++;
			}
			glUniform1i(engine->renderer->lightable_amount_location, i);

			i = 0;
			for (auto& l : lights) {
				if (i >= BEE_MAX_LIGHTS) {
					break;
				}

				glm::vec4 c (l.color.r, l.color.g, l.color.b, l.color.a);
				c /= 255.0f;

				glUniform1i(engine->renderer->lighting_location[i].type, static_cast<int>(l.type));
				glUniform4fv(engine->renderer->lighting_location[i].position, 1, glm::value_ptr(l.position));
				glUniform4fv(engine->renderer->lighting_location[i].direction, 1, glm::value_ptr(l.direction));
				glUniform4fv(engine->renderer->lighting_location[i].attenuation, 1, glm::value_ptr(l.attenuation));
				glUniform4fv(engine->renderer->lighting_location[i].color, 1, glm::value_ptr(c));

				i++;
			}
			glUniform1i(engine->renderer->light_amount_location, i);
		} else {
			if (!lights.empty()) {
				int w = get_width(), h = get_height();
				set_render_target(light_map);
				draw_set_color({0, 0, 0, 255});
				engine->renderer->render_clear();

				Sprite* s = new Sprite("pt_sprite_sphere", "particles/07_sphere.png");
				s->load();

				for (auto& l : lights) {
					switch (l.type) {
						case E_LIGHT::AMBIENT: {
							draw_rectangle(0, 0, w, h, true, l.color);
							break;
						}
						case E_LIGHT::DIFFUSE: {
							break;
						}
						case E_LIGHT::POINT: {
							int r = 10000.0/l.attenuation.y;
							s->draw(l.position.x-r/2, l.position.y-r/2, 0, r, r, 0.0, l.color, SDL_FLIP_NONE);
							break;
						}
						case E_LIGHT::SPOT: {
							break;
						}
					}
				}
				delete s;
				reset_render_target();
				draw_set_blend(SDL_BLENDMODE_MOD);
				light_map->draw(0, 0, 0);
				draw_set_blend(SDL_BLENDMODE_BLEND);
			}
		}

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

	int Room::load_media() {
		// Load room sprites
		for (auto& i : instances) {
			if (i.second->get_object()->get_sprite() != nullptr) {
				if (!i.second->get_object()->get_sprite()->get_is_loaded()) {
					i.second->get_object()->get_sprite()->load();
				}
			}
		}

		// Load room backgrounds
		for (auto& b : backgrounds) {
			if (!b->background->get_is_loaded()) {
				b->background->load();
			}
		}

		return 0;
	}
	int Room::free_media() {
		// Free room sprites
		for (auto& i : instances) {
			if (i.second->get_object()->get_sprite() != nullptr) {
				i.second->get_object()->get_sprite()->free();
			}
		}

		// Free room backgrounds
		for (auto& b : backgrounds) {
			b->background->free();
		}

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
		light_map = new Sprite();

		if (physics_world != nullptr) {
			delete physics_world;
			physics_world = nullptr;
		}
		physics_world = new PhysicsWorld();
		physics_instances.clear();

		// Reset background data
		for (auto& b : backgrounds) {
			b->background->set_time_update();
			delete b;
		}
		backgrounds.clear();

		particle_systems.clear();

		return 0;
	}

	int Room::save_instance_map(std::string fname) {
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
	int Room::load_instance_map(std::string fname) {
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
					Object* object = get_object_by_name(o);

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
					Object* object = get_object_by_name(o);

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
					Object* object = get_object_by_name(o);

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
					Object* object = get_object_by_name(o);

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
								inst->set_sprite(get_sprite_by_name(set_params[1]));
							} else if (set_params[0] == "@solid") {
								inst->set_is_solid(SIDP(set_params[1], true).i());
							} else if (set_params[0] == "@depth") {
								inst->depth = SIDP(set_params[1], true).i();
							} else {
								messenger::send({"engine", "room"}, E_MESSAGE::WARNING, "Error while loading instance map: unknown setter \"" + v + "\"");
								continue;
							}
						} else if (set_params[0] == "!setend") {
							break;
						} else {
							inst->set_data(set_params[1], SIDP(set_params[2], true));
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

				Object* object = get_object_by_name(v);
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
	int Room::set_instance_map(std::string new_instance_map) {
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
			for (int e=0; e<BEE_ALARM_COUNT; e++) {
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

		if ((get_options().is_debug_enabled)&&(!get_options().is_headless)) {
			if (is_background_color_enabled) {
				draw_set_color(background_color);
			} else {
				draw_set_color(E_RGB::WHITE);
			}

			engine->renderer->render_clear();
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
					+ std::get<3>(c)*abs(i.first->get_path_speed())
					* direction_of(
						i.first->get_x(), i.first->get_y(), i.first->get_z(),
						i.first->path_pos_start.x()+std::get<0>(c), i.first->path_pos_start.y()+std::get<1>(c), i.first->path_pos_start.z()+std::get<2>(c)
					) * get_delta()
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
					((i.first->get_path_speed() >= 0)&&(i.first->get_path_node() == static_cast<int>(i.first->get_path_coords().size())-1))
					||((i.first->get_path_speed() < 0)&&(i.first->get_path_node() == -1))
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
		std::map<const btRigidBody*,Instance*> physics_instances = get_current_room()->get_phys_instances();

		//w->clearForces();

		size_t manifold_amount = world->get_dispatcher()->getNumManifolds();
		for (size_t i=0; i<manifold_amount; ++i) {
			btPersistentManifold* manifold = world->get_dispatcher()->getManifoldByIndexInternal(i);
			const btRigidBody* body1 = btRigidBody::upcast(manifold->getBody0());
			const btRigidBody* body2 = btRigidBody::upcast(manifold->getBody1());

			if ((physics_instances.find(body1) != physics_instances.end())&&(physics_instances.find(body2) != physics_instances.end())) {
				Instance* i1 = physics_instances[body1];
				Instance* i2 = physics_instances[body2];

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
					if ((i1 == nullptr)||(i1->get_object() == nullptr)) {
						physics_instances.erase(body1);
					}
					if ((i2 == nullptr)||(i2->get_object() == nullptr)) {
						physics_instances.erase(body2);
					}
				}
			}
		}
	}
	bool Room::check_collision_filter(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) {
		bool should_collide = false;
		std::map<const btRigidBody*,Instance*> physics_instances = get_current_room()->get_phys_instances();

		btRigidBody* body1 = static_cast<btRigidBody*>(proxy0->m_clientObject);
		btRigidBody* body2 = static_cast<btRigidBody*>(proxy1->m_clientObject);

		if ((physics_instances.find(body1) != physics_instances.end())&&(physics_instances.find(body2) != physics_instances.end())) {
			Instance* i1 = physics_instances[body1];
			Instance* i2 = physics_instances[body2];

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
				if ((i1 == nullptr)||(i1->get_object() == nullptr)) {
					physics_instances.erase(body1);
				}
				if ((i2 == nullptr)||(i2->get_object() == nullptr)) {
					physics_instances.erase(body2);
				}
			}
		}

		return should_collide;
	}
	int Room::draw() {
		if (is_background_color_enabled) {
			draw_set_color(background_color);
		} else {
			draw_set_color(E_RGB::WHITE);
		}

		if (is_views_enabled) { // Render different viewports
			if (!get_options().is_debug_enabled) {
				engine->renderer->render_clear();
			}

			for (auto& v : views) {
				if (v->is_visible) {
					view_current = v;

					if (view_current->following != nullptr) {
						Instance* f = view_current->following;
						if (instances_sorted.find(f) != instances_sorted.end()) {
							SDL_Rect a = f->get_aabb();
							SDL_Rect b = {
								view_current->view_x,
								view_current->view_y,
								view_current->port_width,
								view_current->port_height
							};
							if (a.x < -b.x+view_current->horizontal_border) {
								view_current->view_x = -(a.x - view_current->horizontal_border);
							} else if (a.x+a.w > -b.x+b.w-view_current->horizontal_border) {
								view_current->view_x = b.w - (a.x + a.w + view_current->horizontal_border);
							}
							if (a.y < -b.y+view_current->vertical_border) {
								view_current->view_y = -(a.y - view_current->vertical_border);
							} else if (a.y+a.h > -b.y+b.h-view_current->vertical_border) {
								view_current->view_y = b.h - (a.y + a.h + view_current->vertical_border);
							}
						} else {
							view_current->following = nullptr;
						}
					}
					if (view_current->horizontal_speed != 0) {
						view_current->view_x -= view_current->horizontal_speed;
					}
					if (view_current->vertical_speed != 0) {
						view_current->view_y -= view_current->vertical_speed;
					}

					set_viewport(view_current);
					draw_view();

					handle_lights();
				}
			}
			view_current = nullptr;
			set_viewport(nullptr);
		} else {
			if (!get_options().is_debug_enabled) {
				engine->renderer->render_clear();
			}

			set_viewport(nullptr);
			draw_view();

			handle_lights();
		}

		engine->renderer->render();
		reset_lights();

		return 0;
	}
	int Room::draw_view() {
		// Draw backgrounds
		for (auto& b : backgrounds) {
			if (b->is_visible && !b->is_foreground) {
				b->background->draw(b->x, b->y, b);
			}
		}

		// Draw instances
		for (auto& i : instances_sorted_events[E_EVENT::DRAW]) {
			if (i.first->get_object()->get_is_visible()) {
				i.first->get_object()->update(i.first);
				i.first->get_object()->draw(i.first);
			}
		}

		// Draw particles
		for (auto& psys : particle_systems) {
			psys->draw();
		}

		// Draw foregrounds
		for (auto& b : backgrounds) {
			if (b->is_visible && b->is_foreground) {
				b->background->draw(b->x, b->y, b);
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
			draw_rectangle(0, 0, get_width(), get_height(), false, E_RGB::RED);

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
}

#endif // BEE_ROOM
