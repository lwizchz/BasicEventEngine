/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_ROOM
#define _BEE_ROOM 1

#include "room.hpp"

BEE::Room::Room () {
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

	if (view_texture != NULL) {
		view_texture->free();
		delete view_texture;
		view_texture = NULL;
	}

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
			if ((r.second != NULL)&&(r.second->get_path() == path)) {
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

	return 0;
}
int BEE::Room::reset() {
	name = "";
	room_path = "";
	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;
	is_isometric = false;
	speed = DEFAULT_GAME_FPS;
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

	if (view_texture != NULL) {
		view_texture->free();
		delete view_texture;
		view_texture = NULL;
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
	"\n	speed				" << speed <<
	"\n	is_persistent			" << is_persistent <<
	"\n	background_color		" << background_color.r << ", " << background_color.g << ", " << background_color.b <<
	"\n	is_background_color_enabled	" << is_background_color_enabled <<
	"\n	backgrounds			\n" << debug_indent(background_string, 2) <<
	"	is_views_enabled		" << is_views_enabled <<
	"\n	views				\n" << debug_indent(view_string, 2) <<
	"	instances			\n" << debug_indent(instance_string, 2) <<
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
int BEE::Room::get_speed() {
	return speed;
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
std::map<int, BackgroundData*> BEE::Room::get_backgrounds() {
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
std::map<int, ViewData*> BEE::Room::get_views() {
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
			v.second->object_horizontal_border << "\t" <<
			v.second->object_vertical_border << "\t" <<
			v.second->horizontal_speed << "\t" <<
			v.second->vertical_speed << "\n";
		}

		return view_string.str();
	}
	return "none\n";
}
std::map<int, BEE::InstanceData*> BEE::Room::get_instances() {
	return instances;
}
std::string BEE::Room::get_instance_string() {
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
ViewData* BEE::Room::get_current_view() {
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
int BEE::Room::set_speed(int new_speed) {
	speed = new_speed;
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
int BEE::Room::set_background_color(rgba_t new_background_color) {
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
		instances.erase(index);
	}
	instances.insert(std::pair<int,InstanceData*>(index, new_instance));
	return 0;
}
int BEE::Room::add_instance(int index, Object* object, int x, int y) {
	InstanceData* new_instance = new InstanceData(game, index, object, x, y);
	if (index < 0) {
		index = instances.size();
		new_instance->id = index;
	}
	set_instance(index, new_instance);
	object->game = game;
	object->add_instance(index, new_instance);

	if (object->get_sprite() != NULL) {
		if (!object->get_sprite()->get_is_loaded()) {
			//object->get_sprite()->load();
			std::cerr << "An instance of " << object->get_name() << " has been created but its sprite has not been loaded\n";
		}
	}

	if (game->get_is_ready()) {
		new_instance->object->create(new_instance);
	}

	return index;
}
int BEE::Room::remove_instance(int index) {
	if (instances.find(index) != instances.end()) {
		instances[index]->object->remove_instance(index);
		instances.erase(index);
		for (unsigned int i=index; i<instances.size(); i++) {
			if (instances.find(i)++ != instances.end()) {
				instances[i] = instances[i+1];
			}
		}
		return 0;
	}
	return 1;
}
int BEE::Room::sort_instances() {
	std::transform(instances.begin(), instances.end(), std::inserter(instances_sorted, instances_sorted.begin()), flip_pair<int,InstanceData*>);
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

int BEE::Room::load_media() {
	// Load room sprites
	for (auto& i : instances) {
		if (i.second->object->get_sprite() != NULL) {
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
		if (i.second->object->get_sprite() != NULL) {
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
		delete i.second;
	}
	instances.clear();
	instances_sorted.clear();

	// Reset background data
	for (auto& b : backgrounds) {
		b.second->background->set_time_update();
		delete b.second;
	}
	backgrounds.clear();

	for (auto& p : particles) {
		p.second->clear();
	}
	particles.clear();

	return 0;
}

int BEE::Room::create() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->create(i.first);
	}

	return 0;
}
int BEE::Room::destroy() {
	for (auto& i : destroyed_instances) {
		i->object->destroy(i);
		remove_instance(i->id);
	}
	instances_sorted.clear();
	destroyed_instances.clear();

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
	sort_instances();
	for (auto& i : instances_sorted) {
		for (int e=0; e<ALARM_COUNT; e++) {
			if (SDL_GetTicks() >= i.first->alarm_end[e]) {
				i.first->alarm_end[e] = 0xffffffff; // Reset alarm
				i.first->object->alarm(i.first, e);
			}
		}
	}

	return 0;
}
int BEE::Room::step_begin() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->step_begin(i.first);
	}

	return 0;
}
int BEE::Room::step_mid() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->step_mid(i.first);
	}

	// Move instances along their paths
	for (auto& i : instances_sorted) {
		if (i.first->has_path()) {
			i.first->path_update_node();

			path_coord c = std::make_tuple(0, 0, 0);
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

	// Condense all instance motion based on velocity and gravity into a single step
	for (auto& i : instances_sorted) {
		i.first->xprevious = i.first->x;
		i.first->yprevious = i.first->y;

		double x=0.0, y=0.0;
		std::tie (x, y) = i.first->get_motion();
		i.first->velocity.clear();
		i.first->move_away(distance(i.first->x, i.first->y, x, y), x, y);
		i.first->x = x;
		i.first->y = y;
	}

	return 0;
}
int BEE::Room::step_end() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->step_end(i.first);
	}

	return 0;
}
int BEE::Room::keyboard(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->keyboard(i.first, e);
	}

	return 0;
}
int BEE::Room::mouse(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->mouse(i.first, e);
	}

	return 0;
}
int BEE::Room::keyboard_press(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->keyboard_press(i.first, e);
	}

	return 0;
}
int BEE::Room::mouse_press(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->mouse_press(i.first, e);
	}

	return 0;
}
int BEE::Room::keyboard_release(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->keyboard_release(i.first, e);
	}

	return 0;
}
int BEE::Room::mouse_release(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->mouse_release(i.first, e);
	}

	return 0;
}
int BEE::Room::check_paths() {
	sort_instances();
	for (auto& i : instances_sorted) {
		if (i.first->has_path()) {
			if (((i.first->get_path_speed() >= 0)&&(i.first->get_path_node() == (int) i.first->get_path_coords().size()-1))
				|| ((i.first->get_path_speed() < 0)&&(i.first->get_path_node() == -1))) {
				i.first->object->path_end(i.first);
				i.first->handle_path_end();
			}
		}
	}

	return 0;
}
int BEE::Room::outside_room() {
	sort_instances();
	for (auto& i : instances_sorted) {
		if (i.first->object->get_mask() != NULL) {
			SDL_Rect a = {(int)i.first->x, (int)i.first->y, i.first->object->get_mask()->get_subimage_width(), i.first->object->get_mask()->get_height()};
			SDL_Rect b = {0, 0, game->get_width(), game->get_height()};
			if (!check_collision(&a, &b)) {
				i.first->object->outside_room(i.first);
			}
		}
	}

	return 0;
}
int BEE::Room::intersect_boundary() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->intersect_boundary(i.first);
	}

	return 0;
}
int BEE::Room::collision() {
	sort_instances();

	std::map<InstanceData*,int> ilist = instances_sorted;

	for (auto& i1 : ilist) {
		if (i1.first->object->get_mask() != NULL) {
			double x, y;
			std::tie(x, y) = i1.first->get_position();
			SDL_Rect a = {(int)x, (int)y, i1.first->object->get_mask()->get_subimage_width(), i1.first->object->get_mask()->get_height()};
			for (auto& i2 : ilist) {
				if (i1.first == i2.first) {
					continue;
				}

				if (i2.first->object->get_mask() != NULL) {
					std::tie(x, y) = i2.first->get_position();
					SDL_Rect b = {(int)x, (int)y, i2.first->object->get_mask()->get_subimage_width(), i2.first->object->get_mask()->get_height()};
					if (check_collision(&a, &b)) {
						if (i1.first->object->get_is_solid()) {
							i1.first->x -= sin(degtorad(i1.first->velocity.front().first))*i1.first->velocity.front().first;
							i1.first->y -= -cos(degtorad(i1.first->velocity.front().first))*i1.first->velocity.front().first;
						}
						if (i2.first->object->get_is_solid()) {
							i2.first->x -= sin(degtorad(i2.first->velocity.front().first))*i2.first->velocity.front().first;
							i2.first->y -= -cos(degtorad(i2.first->velocity.front().first))*i2.first->velocity.front().first;
						}

						i1.first->object->collision(i1.first, i2.first);
						i2.first->object->collision(i2.first, i1.first);
					}
				}
			}
		}
		ilist.erase(i1.first);
	}

	for (auto& i : instances_sorted) {
		if (!i.first->velocity.empty()) {
			i.first->velocity.pop_front();
		}
	}

	return 0;
}
int BEE::Room::draw() {
	sort_instances();

	if (is_background_color_enabled) {
		game->draw_set_color(background_color);
	} else {
		game->draw_set_color(c_white);
	}

	if (is_views_enabled) { // Render different viewports
		if (view_texture->game == NULL) {
			view_texture->game = game;
		}
		/*game->set_render_target(view_texture, 5000, 5000);
		game->render_clear();
		draw_view();
		game->render();
		game->set_render_target(NULL);*/
		game->render_clear();

		SDL_Rect viewport, viewcoords;
		for (auto& v : views) {
			if (v.second->is_visible) {
				view_current = v.second;

				viewport.x = v.second->port_x;
				viewport.y = v.second->port_y;
				viewport.w = v.second->port_width;
				viewport.h = v.second->port_height;
				SDL_RenderSetViewport(game->renderer, &viewport);

				/*viewcoords.x = v.second->view_x;
				viewcoords.y = v.second->view_y;
				viewcoords.w = v.second->view_width;
				viewcoords.h = v.second->view_height;

				view_texture->draw_simple(&viewcoords, &viewport);*/
				draw_view();
			}
		}
		view_current = NULL;
		//view_texture->free();
		viewport = {0, 0, game->get_width(), game->get_height()};
		SDL_RenderSetViewport(game->renderer, &viewport);
	} else {
		game->render_clear();
		draw_view();
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

	// Draw paths
	for (auto& i : instances_sorted) {
		if ((i.first->has_path())&&(i.first->get_path_drawn())) {
			std::vector<path_coord> coords = i.first->get_path_coords();
			for (std::vector<path_coord>::iterator it = coords.begin(); it != coords.end(); ++it) {
				if (it != --coords.end()) {
					int xs = i.first->path_xstart;
					int ys = i.first->path_ystart;

					int x1 = std::get<0>(*it);
					int y1 = std::get<1>(*it);
					++it;
					int x2 = std::get<0>(*it);
					int y2 = std::get<1>(*it);
					--it;

					game->draw_line(x1+xs, y1+ys, x2+xs, y2+ys);
				}
			}
		}
	}

	// Draw instances
	for (auto& i : instances_sorted) {
		if (i.first->object->get_is_visible()) {
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

	return 0;
}
int BEE::Room::animation_end() {
	sort_instances();
	for (auto& i : instances_sorted) {
		if (i.first->object->get_sprite() != NULL) {
			if (!i.first->object->get_sprite()->get_is_animated()) {
				i.first->object->animation_end(i.first);
			}
		}
	}

	return 0;
}
int BEE::Room::room_start() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->room_start(i.first);
	}

	return 0;
}
int BEE::Room::room_end() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->room_end(i.first);
	}

	return 0;
}
int BEE::Room::game_start() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->game_start(i.first);
	}

	return 0;
}
int BEE::Room::game_end() {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->game_end(i.first);
	}

	return 0;
}
int BEE::Room::window(SDL_Event* e) {
	sort_instances();
	for (auto& i : instances_sorted) {
		i.first->object->window(i.first, e);
	}

	return 0;
}

#endif // _BEE_ROOM
