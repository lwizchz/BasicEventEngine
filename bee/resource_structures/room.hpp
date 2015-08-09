/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_ROOM_H
#define _BEE_ROOM_H 1

#include <iostream>
#include <map>
#include <tuple>

class ViewData {
	public:
		bool is_visible;
		int view_x, view_y, view_width, view_height;
		int port_x, port_y, port_width, port_height;
		BEE::InstanceData* following;
		int object_horizontal_border, object_vertical_border;
		int horizontal_speed, vertical_speed;
};

class BEE::Room: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string room_path;
		int width, height;
		bool is_isometric;
		int speed;
		bool is_persistent;
		RGBA background_color;
		bool is_background_color_enabled;
		std::map<int,BackgroundData*> backgrounds;
		bool is_views_enabled;
		std::map<int,ViewData*> views;
		std::map<int,InstanceData*> instances;
	public:
		Room();
		Room(std::string, std::string);
		~Room();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		bool get_is_isometric();
		int get_speed();
		bool get_is_persistent();
		RGBA get_background_color();
		bool get_is_background_color_enabled();
		std::map<int,BackgroundData*> get_backgrounds();
		std::string get_background_string();
		bool get_is_views_enabled();
		std::map<int,ViewData*> get_views();
		std::string get_view_string();
		std::map<int,InstanceData*> get_instances();
		std::string get_instance_string();

		int set_name(std::string);
		int set_path(std::string);
		int set_width(int);
		int set_height(int);
		int set_is_isometric(bool);
		int set_speed(int);
		int set_is_persistent(bool);
		int set_background_color(RGBA);
		int set_background_color(Uint8, Uint8, Uint8);
		int set_is_background_color_enabled(bool);
		int set_background(int, BackgroundData*);
		int add_background(int, Background*, bool, bool, int, int, bool, bool, int, int, bool);
		int set_is_views_enabled(bool);
		int set_view(int, ViewData*);
		int set_instance(int, InstanceData*);
		int add_instance(int, Object*, int, int);
		int remove_instance(int);

		int load_media();
		int free_media();
		int reset_properties();

		int create();
		int destroy();
		int check_alarms();
		int step_begin();
		int step_mid();
		int step_end();
		int keyboard(SDL_Event*);
		int mouse(SDL_Event*);
		int keyboard_press(SDL_Event*);
		int mouse_press(SDL_Event*);
		int keyboard_release(SDL_Event*);
		int mouse_release(SDL_Event*);
		int check_paths();
		int outside_room();
		int intersect_boundary();
		int collision();
		int draw();
		int animation_end(Sprite*);
		int room_start();
		int room_end();
		int game_start();
		int game_end();
		int window(SDL_Event*);

		virtual int init() =0;
};

#include "instancedata.hpp"

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
	BEE::resource_list.rooms.remove_resource(id);
}
int BEE::Room::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == room_path) {
			return 1;
		}
		BEE::resource_list.rooms.remove_resource(id);
		id = -1;
	} else {
		for (auto& r : BEE::resource_list.rooms.resources) {
			if ((r.second != NULL)&&(r.second->get_path() == path)) {
				list_id = r.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list.rooms.add_resource(this);
	}
	BEE::resource_list.rooms.set_resource(id, this);

	return 0;
}
int BEE::Room::reset() {
	name = "";
	room_path = "";
	width = 1280;
	height = 720;
	is_isometric = false;
	speed = 60;
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
	object->add_instance(index, new_instance);

	if (game->get_is_ready()) {
		new_instance->object->create(new_instance);
	}

	return 0;
}
int BEE::Room::remove_instance(int index) {
	instances[index]->object->remove_instance(index);
	instances.erase(index);
	for (unsigned int i=index; i<instances.size(); i++) {
		if (instances.find(i)++ != instances.end()) {
			instances[i] = instances[i+1];
		}
	}
	return 0;
}

int BEE::Room::load_media() {
	// Load room sprites
	for (auto& i : instances) {
		i.second->object->get_sprite()->load();
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
		i.second->object->get_sprite()->free();
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

	// Reset background data
	for (auto& b : backgrounds) {
		b.second->background->set_time_update();
		delete b.second;
	}
	backgrounds.clear();

	return 0;
}

int BEE::Room::create() {
	for (auto& i : instances) {
		i.second->object->create(i.second);
	}

	return 0;
}
int BEE::Room::destroy() {
	for (auto& i : instances) {
		i.second->object->destroy(i.second);
	}

	return 0;
}
int BEE::Room::check_alarms() {
	for (auto& i : instances) {
		for (int e=0; e<ALARM_COUNT; e++) {
			if (SDL_GetTicks() >= i.second->alarm_end[e]) {
				i.second->alarm_end[e] = 0xffffffff; // Reset alarm
				i.second->object->alarm(i.second, e);
			}
		}
	}

	return 0;
}
int BEE::Room::step_begin() {
	for (auto& i : instances) {
		i.second->object->step_begin(i.second);
	}

	return 0;
}
int BEE::Room::step_mid() {
	for (auto& i : instances) {
		i.second->object->step_mid(i.second);
	}

	// Move instances along their paths
	for (auto& i : instances) {
		if (i.second->has_path()) {
			i.second->path_update_node();

			path_coord c = std::make_tuple(0, 0, 0);
			if (i.second->get_path_speed() >= 0) {
				if (i.second->get_path_node()+1 < (int) i.second->get_path_coords().size()) {
					c = i.second->get_path_coords().at(i.second->get_path_node()+1);
				} else {
					break;
				}
			} else if (i.second->get_path_node() >= 0) {
				c = i.second->get_path_coords().at(i.second->get_path_node());
			}
			i.second->move(std::get<2>(c)*abs(i.second->get_path_speed()), direction_of(i.second->x, i.second->y, i.second->path_xstart+std::get<0>(c), i.second->path_ystart+std::get<1>(c)));
		}
	}

	// Condense all instance motion based on velocity and gravity into a single step
	for (auto& i : instances) {
		i.second->xprevious = i.second->x;
		i.second->yprevious = i.second->y;

		double x=0.0, y=0.0;
		std::tie (x, y) = i.second->get_motion();
		i.second->velocity.clear();
		i.second->move_away(distance(i.second->x, i.second->y, x, y), x, y);
		i.second->x = x;
		i.second->y = y;
	}

	return 0;
}
int BEE::Room::step_end() {
	for (auto& i : instances) {
		i.second->object->step_end(i.second);
	}

	return 0;
}
int BEE::Room::keyboard(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->keyboard(i.second, e);
	}

	return 0;
}
int BEE::Room::mouse(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->mouse(i.second, e);
	}

	return 0;
}
int BEE::Room::keyboard_press(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->keyboard_press(i.second, e);
	}

	return 0;
}
int BEE::Room::mouse_press(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->mouse_press(i.second, e);
	}

	return 0;
}
int BEE::Room::keyboard_release(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->keyboard_release(i.second, e);
	}

	return 0;
}
int BEE::Room::mouse_release(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->mouse_release(i.second, e);
	}

	return 0;
}
int BEE::Room::check_paths() {
	for (auto& i : instances) {
		if (i.second->has_path()) {
			if (((i.second->get_path_speed() >= 0)&&(i.second->get_path_node() == (int) i.second->get_path_coords().size()-1))
				|| ((i.second->get_path_speed() < 0)&&(i.second->get_path_node() == -1))) {
				i.second->object->path_end(i.second);
				i.second->handle_path_end();
			}
		}
	}

	return 0;
}
int BEE::Room::outside_room() {
	for (auto& i : instances) {
		SDL_Rect a = {(int)i.second->x, (int)i.second->y, i.second->object->get_mask()->get_subimage_width(), i.second->object->get_mask()->get_height()};
		SDL_Rect b = {0, 0, game->get_width(), game->get_height()};
		if (!check_collision(&a, &b)) {
			i.second->object->outside_room(i.second);
		}
	}

	return 0;
}
int BEE::Room::intersect_boundary() {
	for (auto& i : instances) {
		i.second->object->intersect_boundary(i.second);
	}

	return 0;
}
int BEE::Room::collision() {
	std::map<int,InstanceData*> ilist = instances;

	for (auto& i1 : ilist) {
		SDL_Rect a = {(int)i1.second->x, (int)i1.second->y, i1.second->object->get_mask()->get_subimage_width(), i1.second->object->get_mask()->get_height()};
		for (auto& i2 : ilist) {
			if (i1.first == i2.first) {
				continue;
			}

			SDL_Rect b = {(int)i2.second->x, (int)i2.second->y, i2.second->object->get_mask()->get_subimage_width(), i2.second->object->get_mask()->get_height()};
			if (check_collision(&a, &b)) {
				if (i1.second->object->get_is_solid()) {
					i1.second->x -= sin(degtorad(i1.second->velocity.front().second))*i1.second->velocity.front().first;
					i1.second->y -= -cos(degtorad(i1.second->velocity.front().second))*i1.second->velocity.front().first;
				}
				if (i2.second->object->get_is_solid()) {
					i2.second->x -= sin(degtorad(i2.second->velocity.front().second))*i2.second->velocity.front().first;
					i2.second->y -= -cos(degtorad(i2.second->velocity.front().second))*i2.second->velocity.front().first;
				}

				i1.second->object->collision(i1.second, i2.second);
				i2.second->object->collision(i2.second, i1.second);
			}
		}
		ilist.erase(i1.first);
	}

	for (auto& i : instances) {
		if (!i.second->velocity.empty()) {
			i.second->velocity.pop_front();
		}
	}

	return 0;
}
int BEE::Room::draw() {
	game->render_clear();

	// Draw backgrounds
	for (auto& b : backgrounds) {
		if (b.second->is_visible && !b.second->is_foreground) {
			b.second->background->draw(b.second->x, b.second->y, b.second);
		}
	}

	// Draw paths
	for (auto& i : instances) {
		if ((i.second->has_path())&&(i.second->get_path_drawn())) {
			std::vector<path_coord> coords = i.second->get_path_coords();
			for (std::vector<path_coord>::iterator it = coords.begin(); it != coords.end(); ++it) {
				if (it != --coords.end()) {
					int xs = i.second->path_xstart;
					int ys = i.second->path_ystart;

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
	for (auto& i : instances) {
		if (is_views_enabled) { // Render different viewports
			SDL_Rect viewport;
			for (auto& v : views) {
				if (v.second->is_visible) {
					viewport.x = v.second->port_x;
					viewport.y = v.second->port_y;
					viewport.w = v.second->port_width;
					viewport.h = v.second->port_height;
					SDL_RenderSetViewport(game->renderer, &viewport);

					i.second->object->draw(i.second);
				}
			}
			viewport = {0, 0, game->get_width(), game->get_height()};
			SDL_RenderSetViewport(game->renderer, &viewport);
		} else {
			if (i.second->object->get_is_visible()) {
				i.second->object->draw(i.second);
			}
		}
	}

	// Draw foregrounds
	for (auto& b : backgrounds) {
		if (b.second->is_visible && b.second->is_foreground) {
			b.second->background->draw(b.second->x, b.second->y, b.second);
		}
	}

	game->render();

	return 0;
}
int BEE::Room::animation_end(Sprite* finished_sprite) {
	for (auto& i : instances) {
		if (i.second->object->get_sprite()->get_id() == finished_sprite->get_id()) {
			i.second->object->animation_end(i.second);
		}
	}

	return 0;
}
int BEE::Room::room_start() {
	for (auto& i : instances) {
		i.second->object->room_start(i.second);
	}

	return 0;
}
int BEE::Room::room_end() {
	for (auto& i : instances) {
		i.second->object->room_end(i.second);
	}

	return 0;
}
int BEE::Room::game_start() {
	for (auto& i : instances) {
		i.second->object->game_start(i.second);
	}

	return 0;
}
int BEE::Room::game_end() {
	for (auto& i : instances) {
		i.second->object->game_end(i.second);
	}

	return 0;
}
int BEE::Room::window(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->window(i.second, e);
	}

	return 0;
}

#endif // _BEE_ROOM_H
