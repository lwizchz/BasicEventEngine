/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_ROOM
#define _BEE_ROOM 1

#include "room.hpp"

BEE::Room::Room () {
	if (BEE::resource_list->rooms.game != nullptr) {
		game = BEE::resource_list->rooms.game;
	}

	reset();
}
BEE::Room::Room (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/rooms/"+path);
	if (id < 0) {
		std::cerr << "Failed to add room resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Room::~Room() {
	backgrounds.clear();
	views.clear();
	instances.clear();
	particles.clear();
	destroyed_instances.clear();
	collision_tree->reset(0, 0, get_width());

	if (view_texture != nullptr) {
		view_texture->free();
		delete view_texture;
		view_texture = nullptr;
	}

	free_media();
	BEE::resource_list->rooms.remove_resource(id);
}
int BEE::Room::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == room_path) {
			return 1;
		}
		BEE::resource_list->rooms.remove_resource(id);
		id = -1;
	} else {
		for (auto& r : BEE::resource_list->rooms.resources) {
			if ((r.second != nullptr)&&(r.second->get_path() == path)) {
				list_id = r.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list->rooms.add_resource(this);
	}
	BEE::resource_list->rooms.set_resource(id, this);

	if (BEE::resource_list->rooms.game != nullptr) {
		game = BEE::resource_list->rooms.game;
	}

	return 0;
}
int BEE::Room::reset() {
	name = "";
	room_path = "";
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
	particles.clear();
	destroyed_instances.clear();
	next_instance_id = 0;
	should_sort = false;

	tree_x = 0;
	tree_y = 0;
	tree_width = get_width();
	if (collision_tree != nullptr) {
		delete collision_tree;
		collision_tree = nullptr;
	}
	collision_tree = new CollisionTree(game, tree_x, tree_y, tree_width, 0);

	if (view_texture != nullptr) {
		view_texture->free();
		delete view_texture;
		view_texture = nullptr;
	}
	view_texture = new Sprite();

	return 0;
}
int BEE::Room::print() {
	std::string background_string = get_background_string();
	std::string view_string = get_view_string();
	std::string instance_string = get_instance_string();

	std::cout <<
	"Room { "
	"\n	id				" << id <<
	"\n	name				" << name <<
	"\n	room_path			" << room_path <<
	"\n	width				" << width <<
	"\n	height				" << height <<
	"\n	is_isometric			" << is_isometric <<
	"\n	is_persistent			" << is_persistent <<
	"\n	is_background_color_enabled	" << is_background_color_enabled;
	if (is_background_color_enabled) {
		std::cerr << "\n	background_color		" << (int)background_color.r << ", " << (int)background_color.g << ", " << (int)background_color.b;
	}
	std::cerr <<
	"\n	backgrounds			\n" << debug_indent(background_string, 2) <<
	"	is_views_enabled		" << is_views_enabled <<
	"\n	views				\n" << debug_indent(view_string, 2) <<
	"	instances			\n" << debug_indent(instance_string, 2) <<
	"	collistion tree			\n" << debug_indent(collision_tree->print(), 2) <<
	"}\n";

	return 0;
}
int BEE::Room::get_id() {
	return id;
}
std::string BEE::Room::get_name() {
	return name;
}
std::string BEE::Room::get_path() {
	return room_path;
}
int BEE::Room::get_width() {
	return width;
}
int BEE::Room::get_height() {
	return height;
}
bool BEE::Room::get_is_isometric() {
	return is_isometric;
}
bool BEE::Room::get_is_persistent() {
	return is_persistent;
}
BEE::RGBA BEE::Room::get_background_color() {
	return background_color;
}
bool BEE::Room::get_is_background_color_enabled() {
	return is_background_color_enabled;
}
std::map<int, BEE::BackgroundData*> BEE::Room::get_backgrounds() {
	return backgrounds;
}
std::string BEE::Room::get_background_string() {
	if (backgrounds.size() > 0) {
		std::ostringstream background_string;
		background_string << "(name	visible	fore	x	y	htile	vtile	hspeed	vspeed	stretch)\n";
		for (auto& b : backgrounds) {
			background_string <<
			b.second->background->get_name() << "\t" <<
			b.second->is_visible << "\t" <<
			b.second->is_foreground << "\t" <<
			b.second->x << "\t" <<
			b.second->y << "\t" <<
			b.second->is_horizontal_tile << "\t" <<
			b.second->is_vertical_tile << "\t" <<
			b.second->horizontal_speed << "\t" <<
			b.second->vertical_speed << "\t" <<
			b.second->is_stretched << "\n";
		}

		return background_string.str();
	}
	return "none\n";
}
bool BEE::Room::get_is_views_enabled() {
	return is_views_enabled;
}
std::map<int, BEE::ViewData*> BEE::Room::get_views() {
	return views;
}
std::string BEE::Room::get_view_string() {
	if (views.size() > 0) {
		std::ostringstream view_string;
		view_string << "(visible	vx, vy	vwidth	vheight	px, py	pwidth	pheight	object	hborder	vborder	hspeed	vspeed)\n";
		for (auto& v : views) {
			view_string <<
			v.second->is_visible << "\t" <<
			v.second->view_x << ", " << v.second->view_y <<"\t" <<
			v.second->view_width << "\t" <<
			v.second->view_height << "\t" <<
			v.second->port_x << ", " << v.second->port_y << "\t" <<
			v.second->port_width << "\t" <<
			v.second->port_height << "\t" <<
			v.second->following->object->get_name() << "\t" <<
			v.second->horizontal_border << "\t" <<
			v.second->vertical_border << "\t" <<
			v.second->horizontal_speed << "\t" <<
			v.second->vertical_speed << "\n";
		}

		return view_string.str();
	}
	return "none\n";
}
const std::map<int, BEE::InstanceData*>& BEE::Room::get_instances() {
	return instances;
}
std::string BEE::Room::get_instance_string() {
	if (instances.size() > 0) {
		std::ostringstream instance_string;
		instance_string << "(id	object		depth	x	y)\n";

		for (auto& i : instances_sorted) {
			instance_string <<
			i.first->id << "\t" <<
			i.first->object->get_name() << "\t" <<
			i.first->depth << "\t" <<
			i.first->x << "\t" <<
			i.first->y << "\n";
		}

		return instance_string.str();
	}
	return "none\n";
}
BEE::ViewData* BEE::Room::get_current_view() {
	return view_current;
}

int BEE::Room::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Room::set_path(std::string path) {
	add_to_resources("resources/rooms"+path);
	room_path = "resources/rooms/"+path;
	return 0;
}
int BEE::Room::set_width(int new_width) {
	width = new_width;
	return 0;
}
int BEE::Room::set_height(int new_height) {
	height = new_height;
	return 0;
}
int BEE::Room::set_is_isometric(bool new_is_isometric) {
	is_isometric = new_is_isometric;
	return 0;
}
int BEE::Room::set_is_persistent(bool new_is_persistent) {
	is_persistent = new_is_persistent;
	return 0;
}
int BEE::Room::set_background_color(RGBA new_background_color) {
	background_color = new_background_color;
	return 0;
}
int BEE::Room::set_background_color(Uint8 r, Uint8 g, Uint8 b) {
	background_color = {r, g, b, 255};
	return 0;
}
int BEE::Room::set_background_color(bee_rgba_t new_background_color) {
	background_color = game->get_enum_color(new_background_color);
	return 0;
}
int BEE::Room::set_is_background_color_enabled(bool new_is_background_color_enabled) {
	is_background_color_enabled = new_is_background_color_enabled;
	return 0;
}
int BEE::Room::set_background(int index, BackgroundData* new_background) {
	if (backgrounds.find(index) != backgrounds.end()) { // if the background exists, overwrite it
		backgrounds.erase(index);
	}
	backgrounds.insert(std::pair<int,BackgroundData*>(index,new_background));
	return 0;
}
int BEE::Room::add_background(int index, Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	BackgroundData* background = new BackgroundData(new_background, new_is_visible, new_is_foreground, new_x, new_y, new_is_horizontal_tile, new_is_vertical_tile, new_horizontal_speed, new_vertical_speed, new_is_stretched);
	if (index < 0) {
		index = backgrounds.size();
	}
	return set_background(index, background);
}
int BEE::Room::set_is_views_enabled(bool new_is_views_enabled) {
	is_views_enabled = new_is_views_enabled;
	return 0;
}
int BEE::Room::set_view(int index, ViewData* new_view) {
	if (views.find(index) != views.end()) { // if the view exists, overwrite it
		views.erase(index);
	}
	views.insert(std::pair<int,ViewData*>(index, new_view));
	return 0;
}
int BEE::Room::set_instance(int index, InstanceData* new_instance) {
	if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
		remove_instance(index);
	}
	instances.insert(std::pair<int,InstanceData*>(index, new_instance));
	return 0;
}
int BEE::Room::add_instance(int index, Object* object, int x, int y) {
	object->game = game;
	if (object->get_sprite() != nullptr) {
		if (!object->get_sprite()->get_is_loaded()) {
			std::cerr << "Automatically loading the sprite for object " << object->get_name() << "\n";
			object->get_sprite()->load();
			//std::cerr << "An instance of " << object->get_name() << " has been created but its sprite has not been loaded\n";
		}
	}

	InstanceData* new_instance = new InstanceData(game, index, object, x, y);
	if (index < 0) {
		index = next_instance_id++;
		new_instance->id = index;
	}
	set_instance(index, new_instance);
	sort_instances();
	object->add_instance(index, new_instance);

	int r = collision_tree->insert(new_instance);
	if (r == 1) {
		expand_collision_tree(x, y);
	} else if (r == 2) {
		collision_tree->set_capacity(collision_tree->max_capacity*2);
		collision_tree->combine();
	}
	if (r != 0) {
		if (collision_tree->insert(new_instance) != 0) {
			std::cerr << "Failed to add " << new_instance->object->get_name() << " with id " << new_instance->id << " to the collision tree: error " << r << "\n";
			//new_instance->print();
		}
	}

	if (game->get_is_ready()) {
		new_instance->object->create(new_instance);
	}

	return index;
}
int BEE::Room::add_instance_grid(int index, Object* object, double x, double y) {
	double xg = x, yg = y;

	if (object->get_sprite() != nullptr) {
		if (!object->get_sprite()->get_is_loaded()) {
			std::cerr << "Automatically loading the sprite for object " << object->get_name() << "\n";
			object->get_sprite()->load();
		}

		xg = 0.0; yg = 0.0;
		if (x >= 0) {
			xg = x*object->get_sprite()->get_subimage_width();
		} else {
			xg = width + x*object->get_sprite()->get_subimage_width();
		}
		if (y >= 0) {
			yg = y*object->get_sprite()->get_height();
		} else {
			yg = height + y*object->get_sprite()->get_height();
		}
	}

	return add_instance(index, object, (int)xg, (int)yg);
}
int BEE::Room::remove_instance(int index) {
	if (instances.find(index) != instances.end()) {
		InstanceData* inst = instances[index];
		collision_tree->remove(inst);
		inst->object->remove_instance(index);
		instances.erase(index);
		instances_sorted.erase(inst);
		delete inst;

		return 0;
	}
	return 1;
}
int BEE::Room::sort_instances() {
	std::transform(instances.begin(), instances.end(), std::inserter(instances_sorted, instances_sorted.begin()), flip_pair<int,InstanceData*>);
	return 0;
}
int BEE::Room::request_instance_sort() {
	should_sort = true;
	return 0;
}
int BEE::Room::add_particle_system(ParticleSystem* new_system) {
	new_system->load();
	new_system->id = particle_count;
	particles.insert(std::make_pair(particle_count++, new_system));
	return 0;
}
int BEE::Room::add_particle(ParticleSystem* sys, Particle* pd, int x, int y) {
	auto it = particles.find(sys->id);
	if (it != particles.end()) {
		it->second->add_particle(pd, x, y);

		return 0;
	}
	return 1;
}
int BEE::Room::clear_particles() {
	for (auto& p : particles) {
		delete p.second;
	}
	particles.clear();
	particle_count = 0;
	return 0;
}
int BEE::Room::add_light(LightData* lighting) {
	lights.push_back(lighting);
	return 0;
}
int BEE::Room::handle_lights() {
	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		int i = 0;
		for (auto& l : lights) {
			if (i >= BEE_MAX_LIGHTS) {
				break;
			}

			glm::vec4 c = glm::vec4((float)l->color.r/255.0f, (float)l->color.g/255.0f, (float)l->color.b/255.0f, (float)l->color.a/255.0f);

			glUniform1i(game->lighting_location[i].type, l->type);
			glUniform4fv(game->lighting_location[i].position, 1, glm::value_ptr(l->position));
			glUniform4fv(game->lighting_location[i].direction, 1, glm::value_ptr(l->direction));
			glUniform4fv(game->lighting_location[i].attenuation, 1, glm::value_ptr(l->attenuation));
			glUniform4fv(game->lighting_location[i].color, 1, glm::value_ptr(c));

			i++;
		}
		glUniform1i(game->light_amount_location, i);
	} else {
		int a = 0;
		int w = get_width(), h = get_height();
		game->draw_set_blend(SDL_BLENDMODE_MOD);
		for (auto& l : lights) {
			switch (l->type) {
				case BEE_LIGHT_AMBIENT: {
					a += l->color.a;
					game->draw_rectangle(0, 0, w, h, true, l->color, false);
					break;
				}
				case BEE_LIGHT_DIFFUSE: {
					break;
				}
				case BEE_LIGHT_POINT: {
					a += l->color.a/2;
					int r = 1000.0/l->attenuation.y;
					game->add_sprite("pt_sprite_sphere", "particles/07_sphere.png")->draw(l->position.x-r/2, l->position.y-r/2, 0, r, r, 0.0, l->color, SDL_FLIP_NONE, false);
					break;
				}
				case BEE_LIGHT_SPOT: {
					break;
				}
			}
		}
		if (a < 255) {
			game->draw_rectangle(0, 0, w, h, true, {0, 0, 0, (Uint8)(255-a)}, false);
		}
		game->draw_set_blend(SDL_BLENDMODE_BLEND);
	}
	lights.clear();

	return 0;
}
int BEE::Room::expand_collision_tree(int expand_x, int expand_y) {
	CollisionTree* ct = collision_tree;

	if ((expand_x >= 0)&&(expand_y >= 0)) {
		collision_tree = new CollisionTree(game, tree_x, tree_y, tree_width*2, 0);
		if (collision_tree->divide()) {
			return 1;
		}

		delete collision_tree->topleft;
		collision_tree->topleft = ct;
	} else {
		collision_tree = new CollisionTree(game, tree_x-tree_width, tree_y-tree_width, tree_width*2, 0);
		if (collision_tree->divide()) {
			return 1;
		}

		tree_x = -tree_width;
		tree_y = -tree_width;

		delete collision_tree->bottomright;
		collision_tree->bottomright = ct;
	}

	tree_width *= 2;

	return 0;
}

int BEE::Room::load_media() {
	// Load room sprites
	for (auto& i : instances) {
		if (i.second->object->get_sprite() != nullptr) {
			i.second->object->get_sprite()->load();
		}
	}

	// Load room backgrounds
	for (auto& b : backgrounds) {
		b.second->background->load();
	}

	return 0;
}
int BEE::Room::free_media() {
	// Free room sprites
	for (auto& i : instances) {
		if (i.second->object->get_sprite() != nullptr) {
			i.second->object->get_sprite()->free();
		}
	}

	// Free room backgrounds
	for (auto& b : backgrounds) {
		b.second->background->free();
	}

	return 0;
}
int BEE::Room::reset_properties() {
	for (auto& i : instances) {
		i.second->object->update(i.second);
		i.second->object->destroy(i.second);
		remove_instance(i.second->id);
	}
	instances.clear();
	instances_sorted.clear();
	destroyed_instances.clear();
	next_instance_id = 0;
	should_sort = false;

	lights.clear();

	tree_x = 0;
	tree_y = 0;
	tree_width = get_width();
	collision_tree->game = game;
	collision_tree->reset(tree_x, tree_y, tree_width);

	// Reset background data
	for (auto& b : backgrounds) {
		b.second->background->set_time_update();
		delete b.second;
	}
	backgrounds.clear();

	clear_particles();

	return 0;
}

int BEE::Room::save_instance_map(std::string fname) {
	std::ofstream savefile (fname, std::ios::trunc);
        if (savefile.good()) {
                for (auto& i : instances) {
                        savefile << i.second->object->get_name() << "," << (int)(i.second->get_xstart()) << "," << (int)(i.second->get_ystart()) << "\n";
                }
                savefile.close();
                return 0;
        }
        return 1;
}
int BEE::Room::load_instance_map(std::string fname) {
	if (game->get_is_ready()) {
		instance_map = fname;
		game->restart_room();
		return 0;
	}

	std::vector<std::tuple<Object*,int,int>> data;
	std::string datastr = file_get_contents(fname);
        if (!datastr.empty()) {
                std::istringstream data_stream (datastr);

                while (!data_stream.eof()) {
                        std::string tmp;
                        getline(data_stream, tmp);

                        if (tmp.empty()) {
                                continue;
                        }

			std::string v;
                        std::stringstream vs (tmp);
                        getline(vs, v, ',');
                        v = trim(v);

                        std::string d = tmp.substr(tmp.find(",")+1);
                        d = trim(d);
			int x = bee_stoi(d.substr(0, d.find(",")));
			int y = bee_stoi(d.substr(d.find(",")+1));

			if (game->get_object_by_name(v) == nullptr) {
				std::cerr << "Error loading instance map: unknown object \"" << v << "\"\n";
			} else {
				data.push_back(std::make_tuple(game->get_object_by_name(v), x, y));
			}
                }

		for (auto& i : data) {
			add_instance(-1, std::get<0>(i), std::get<1>(i), std::get<2>(i));
		}
        } else {
		std::cerr << "No instances loaded.\n";
		return 1;
	}

	return 0;
}
int BEE::Room::load_instance_map() {
	if (!instance_map.empty()) {
		return load_instance_map(instance_map);
	}
	return 1;
}
std::string BEE::Room::get_instance_map() {
	return instance_map;
}
int BEE::Room::set_instance_map(std::string new_instance_map) {
	instance_map = new_instance_map;
	return 1;
}

int BEE::Room::create() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->create(i.first);
	}

	return 0;
}
int BEE::Room::destroy() {
	for (auto& i : destroyed_instances) {
		i->object->update(i);
		i->object->destroy(i);
		remove_instance(i->id);
	}
	destroyed_instances.clear();

	if (should_sort) {
		sort_instances();
		should_sort = false;
	}

	return 0;
}
int BEE::Room::destroy(InstanceData* inst) {
	destroyed_instances.push_back(inst);
	return 0;
}
int BEE::Room::destroy_all(Object* obj) {
	for (auto& i : obj->get_instances()) {
		destroyed_instances.push_back(i.second);
	}

	return 0;
}
int BEE::Room::check_alarms() {
	for (auto& i : instances_sorted) {
		for (int e=0; e<ALARM_COUNT; e++) {
			if (i.first->alarm_end[e] != 0xffffffff) {
				if (SDL_GetTicks() >= i.first->alarm_end[e]) {
					i.first->alarm_end[e] = 0xffffffff; // Reset alarm
					i.first->object->update(i.first);
					i.first->object->alarm(i.first, e);
				}
			}
		}
	}

	return 0;
}
int BEE::Room::step_begin() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->step_begin(i.first);
	}

	if (game->options->is_debug_enabled) {
		if (is_background_color_enabled) {
			game->draw_set_color(background_color);
		} else {
			game->draw_set_color(c_white);
		}

		game->render_clear();
	}

	return 0;
}
int BEE::Room::step_mid() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->step_mid(i.first);
	}

	// Move instances along their paths
	for (auto& i : instances_sorted) {
		if (i.first->has_path()) {
			if ((game->get_is_paused())&&(i.first->get_path_pausable())) {
				continue;
			}

			i.first->path_update_node();

			bee_path_coord c = std::make_tuple(0, 0, 0);
			if (i.first->get_path_speed() >= 0) {
				if (i.first->get_path_node()+1 < (int) i.first->get_path_coords().size()) {
					c = i.first->get_path_coords().at(i.first->get_path_node()+1);
				} else {
					break;
				}
			} else if (i.first->get_path_node() >= 0) {
				c = i.first->get_path_coords().at(i.first->get_path_node());
			}
			i.first->move(std::get<2>(c)*abs(i.first->get_path_speed()), direction_of(i.first->x, i.first->y, i.first->path_xstart+std::get<0>(c), i.first->path_ystart+std::get<1>(c)));
		}
	}

	// Run timelines
	for (auto& t : BEE::resource_list->timelines.resources) {
		Timeline* tt = dynamic_cast<BEE::Timeline*>(t.second);
		if (tt->get_is_running()) {
			int r = tt->step(game->get_frame());
			if (r == 2) {
				tt->end();
				if (tt->get_is_looping()) {
					tt->start();
				}
			}
		}
	}

	// Condense all instance motion based on velocity and gravity into a single step
	for (auto& i : instances_sorted) {
		i.first->xprevious = i.first->x;
		i.first->yprevious = i.first->y;

		std::tie (i.first->x, i.first->y) = i.first->get_motion();
		std::tie (i.first->x, i.first->y) = i.first->get_applied_gravity();

		i.first->old_velocity.clear();
		i.first->old_velocity.swap(i.first->velocity);
	}

	// Update collision tree for all moving instances
	for (auto& i : instances_sorted) {
		if ((i.first->xprevious != i.first->x)||(i.first->yprevious != i.first->y)) {
			collision_tree->remove(i.first);
			int r = collision_tree->insert(i.first);
			if (r == 1) {
				expand_collision_tree(i.first->x, i.first->y);
			} else if (r == 2) {
				collision_tree->set_capacity(collision_tree->max_capacity*2);
				collision_tree->combine();
			}
			if (r != 0) {
				if (collision_tree->insert(i.first) != 0) {
					std::cerr << "Failed to add " << i.first->object->get_name() << " with id " << i.first->id << " to the collision tree: error " << r << "\n";
				}
			}
		}
	}

	return 0;
}
int BEE::Room::step_end() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->step_end(i.first);
	}

	return 0;
}
int BEE::Room::keyboard_press(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->keyboard_press(i.first, e);
	}

	return 0;
}
int BEE::Room::mouse_press(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->mouse_press(i.first, e);
	}

	return 0;
}int BEE::Room::keyboard_input(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->keyboard_input(i.first, e);
	}

	return 0;
}
int BEE::Room::mouse_input(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->mouse_input(i.first, e);
	}

	return 0;
}
int BEE::Room::keyboard_release(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->keyboard_release(i.first, e);
	}

	return 0;
}
int BEE::Room::mouse_release(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->mouse_release(i.first, e);
	}

	return 0;
}
int BEE::Room::controller_axis(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->controller_axis(i.first, e);
	}

	return 0;
}
int BEE::Room::controller_press(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->controller_press(i.first, e);
	}

	return 0;
}
int BEE::Room::controller_release(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->controller_release(i.first, e);
	}

	return 0;
}
int BEE::Room::controller_modify(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->controller_modify(i.first, e);
	}

	return 0;
}
int BEE::Room::console_input(const std::string& input) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->console_input(i.first, input);
	}

	return 0;
}
int BEE::Room::check_paths() {
	for (auto& i : instances_sorted) {
		if (i.first->has_path()) {
			if (((i.first->get_path_speed() >= 0)&&(i.first->get_path_node() == (int) i.first->get_path_coords().size()-1))
				|| ((i.first->get_path_speed() < 0)&&(i.first->get_path_node() == -1))) {

				i.first->object->update(i.first);
				i.first->object->path_end(i.first);
				i.first->handle_path_end();
			}
		}
	}

	return 0;
}
int BEE::Room::outside_room() {
	for (auto& i : instances_sorted) {
		if (i.first->object->get_mask() != nullptr) {
			SDL_Rect a = {(int)i.first->x, (int)i.first->y, i.first->object->get_mask()->get_subimage_width(), i.first->object->get_mask()->get_height()};
			SDL_Rect b = {0, 0, get_width(), get_height()};
			if (!check_collision(a, b)) {
				i.first->object->update(i.first);
				i.first->object->outside_room(i.first);
			}
		}
	}

	return 0;
}
int BEE::Room::intersect_boundary() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->intersect_boundary(i.first);
	}

	return 0;
}
int BEE::Room::collision() {
	if (collision_tree == nullptr) {
		tree_x = 0;
		tree_y = 0;
		tree_width = get_width();
		collision_tree = new CollisionTree(game, tree_x, tree_y, tree_width, 0);
	}
	if (tree_width == 0) {
		tree_width = get_width();
	}

	collision_tree->check_collisions();

	return 0;
}
int BEE::Room::draw() {
	if (is_background_color_enabled) {
		game->draw_set_color(background_color);
	} else {
		game->draw_set_color(c_white);
	}

	if (is_views_enabled) { // Render different viewports
		if (view_texture->game == nullptr) {
			view_texture->game = game;
		}
		if (!game->options->is_debug_enabled) {
			game->render_clear();
		}

		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			handle_lights();
		}

		for (auto& v : views) {
			if (v.second->is_visible) {
				view_current = v.second;

				if (view_current->following != nullptr) {
					InstanceData* f = view_current->following;
					if (instances_sorted.find(f) != instances_sorted.end()) {
						SDL_Rect a = {(int)f->x, (int)f->y, f->get_width(), f->get_height()};
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

				game->set_viewport(view_current);
				draw_view();

				if (game->options->renderer_type == BEE_RENDERER_SDL) {
					handle_lights();
				}
			}
		}
		view_current = nullptr;
		game->set_viewport(nullptr);
	} else {
		if (!game->options->is_debug_enabled) {
			game->render_clear();
		}
		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			handle_lights();
		}

		game->set_viewport(nullptr);
		draw_view();

		if (game->options->renderer_type == BEE_RENDERER_SDL) {
			handle_lights();
		}
	}

	game->render();

	return 0;
}
int BEE::Room::draw_view() {
	// Draw backgrounds
	for (auto& b : backgrounds) {
		if (b.second->is_visible && !b.second->is_foreground) {
			b.second->background->draw(b.second->x, b.second->y, b.second);
		}
	}

	// Draw instances
	for (auto& i : instances_sorted) {
		if (i.first->object->get_is_visible()) {
			i.first->object->update(i.first);
			i.first->object->draw(i.first);
		}
	}

	// Draw particles
	for (auto& p : particles) {
		p.second->draw();
	}

	// Draw foregrounds
	for (auto& b : backgrounds) {
		if (b.second->is_visible && b.second->is_foreground) {
			b.second->background->draw(b.second->x, b.second->y, b.second);
		}
	}

	if (game->options->is_debug_enabled) {
		collision_tree->draw();

		// Draw instance paths and bounding boxes
		for (auto& i : instances_sorted) {
			if ((i.first->has_path())&&(i.first->get_path_drawn())) {
				i.first->draw_path();
			}

			i.first->draw_debug();
		}

		// Draw particle system bounding boxes
		for (auto& p : particles) {
			p.second->draw_debug();
		}
	}

	return 0;
}
int BEE::Room::animation_end() {
	for (auto& i : instances_sorted) {
		if (i.first->object->get_sprite() != nullptr) {
			if (!i.first->object->get_sprite()->get_is_animated()) {
				i.first->object->update(i.first);
				i.first->object->animation_end(i.first);
			}
		}
	}

	return 0;
}
int BEE::Room::room_start() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->room_start(i.first);
	}

	return 0;
}
int BEE::Room::room_end() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->room_end(i.first);
	}

	return 0;
}
int BEE::Room::game_start() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->game_start(i.first);
	}

	return 0;
}
int BEE::Room::game_end() {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->game_end(i.first);
	}

	return 0;
}
int BEE::Room::window(SDL_Event* e) {
	for (auto& i : instances_sorted) {
		i.first->object->update(i.first);
		i.first->object->window(i.first, e);
	}

	return 0;
}

#endif // _BEE_ROOM
