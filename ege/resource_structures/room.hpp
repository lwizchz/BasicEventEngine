/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _EGE_ROOM_H
#define _EGE_ROOM_H 1

#include <iostream>
#include <map>
#include <tuple>

typedef std::tuple<int,int,int> rgb;

class BackgroundData {
	public:
		Background* background;
		bool is_visible;
		bool is_foreground;
		int x, y;
		bool is_horizontal_tile, is_vertical_tile;
		int horizontal_speed, vertical_speed;
		bool is_stretched;
		BackgroundData() {background=NULL;is_visible=false;is_foreground=false;x=0;y=0;is_horizontal_tile=false;is_vertical_tile=false;horizontal_speed=0;vertical_speed=0;is_stretched=false;};
		int init(Background*, bool, bool, int, int, bool, bool, int, int, bool);
};
int BackgroundData::init(Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	background = new_background;
	is_visible = new_is_visible;
	is_foreground = new_is_foreground;
	x = new_x;
	y = new_y;
	is_horizontal_tile = new_is_horizontal_tile;
	is_vertical_tile = new_is_vertical_tile;
	horizontal_speed = new_horizontal_speed;
	vertical_speed = new_vertical_speed;
	is_stretched = new_is_stretched;
	return 0;
}
class ViewData {
	public:
		bool is_visible;
		int view_x, view_y, view_width, view_height;
		int port_x, port_y, port_width, port_height;
		Object* object;
		int object_horizontal_border, object_vertical_border;
		int horizontal_speed, vertical_speed;
};
class InstanceData {
	public:
		int id;
		Object* object;
		int x, y, vx, vy;
		InstanceData() {id=-1;object=NULL;x=0;y=0;vx=0;vy=0;};
		int init(int, Object*, int, int);
};
int InstanceData::init(int new_id, Object* new_object, int new_x, int new_y) {
	id = new_id;
	object = new_object;
	x = new_x;
	y = new_y;
	vx = x;
	vy = y;
	return 0;
}

class Room: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string room_path;
		int width, height;
		bool is_isometric;
		int speed;
		bool is_persistent;
		rgb background_color;
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
		rgb get_background_color();
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
		int set_background_color(rgb);
		int set_background_color(int, int, int);
		int set_is_background_color_enabled(bool);
		int set_background(int, BackgroundData*);
		int set_is_views_enabled(bool);
		int set_view(int, ViewData*);
		int set_instance(int, InstanceData*);
		int add_instance(InstanceData*);
		int remove_instance(int);

		int load_media();
		int free_media();
		int reset_properties();

		int create();
		int destroy();
		int alarm(int);
		int step_begin();
		int step_mid();
		int step_end();
		int keyboard(SDL_Event*);
		int mouse(SDL_Event*);
		int keyboard_press(SDL_Event*);
		int mouse_press(SDL_Event*);
		int keyboard_release(SDL_Event*);
		int mouse_release(SDL_Event*);
		int path_end();
		int outside_room();
		int intersect_boundary();
		int collision();
		int draw();
		int animation_end(Sprite*);
		int room_start();
		int room_end();
		int game_start();
		int game_end();

		virtual int init() =0;
};
Room::Room () {
	id = -1;
	reset();
}
Room::Room (std::string new_name, std::string path) {
	id = -1;
	reset();

	add_to_resources("resources/rooms/"+path);
	if (id < 0) {
		std::cerr << "Failed to add room resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
Room::~Room() {
	resource_list.rooms.remove_resource(id);
}
int Room::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == room_path) {
			return 1;
		}
		resource_list.rooms.remove_resource(id);
		id = -1;
	} else {
		for (auto& r : resource_list.rooms.resources) {
			if ((r.second != NULL)&&(r.second->get_path() == path)) {
				list_id = r.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.rooms.add_resource(this);
	}
	resource_list.rooms.set_resource(id, this);

	return 0;
}
int Room::reset() {
	name = "";
	room_path = "";
	width = 1280;
	height = 720;
	is_isometric = false;
	speed = 60;
	is_persistent = false;
	background_color = std::make_tuple(192, 192, 192);
	is_background_color_enabled = true;
	backgrounds.clear();
	is_views_enabled = false;
	views.clear();
	instances.clear();

	return 0;
}
int Room::print() {
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
	"\n	background_color		" << std::get<0>(background_color) << ", " << std::get<1>(background_color) << ", " << std::get<0>(background_color) <<
	"\n	is_background_color_enabled	" << is_background_color_enabled <<
	"\n	backgrounds			\n" << debug_indent(background_string, 2) <<
	"	is_views_enabled		" << is_views_enabled <<
	"\n	views				\n" << debug_indent(view_string, 2) <<
	"	instances			\n" << debug_indent(instance_string, 2) <<
	"}\n";

	return 0;
}
int Room::get_id() {
	return id;
}
std::string Room::get_name() {
	return name;
}
std::string Room::get_path() {
	return room_path;
}
int Room::get_width() {
	return width;
}
int Room::get_height() {
	return height;
}
bool Room::get_is_isometric() {
	return is_isometric;
}
int Room::get_speed() {
	return speed;
}
bool Room::get_is_persistent() {
	return is_persistent;
}
rgb Room::get_background_color() {
	return background_color;
}
bool Room::get_is_background_color_enabled() {
	return is_background_color_enabled;
}
std::map<int, BackgroundData*> Room::get_backgrounds() {
	return backgrounds;
}
std::string Room::get_background_string() {
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
bool Room::get_is_views_enabled() {
	return is_views_enabled;
}
std::map<int, ViewData*> Room::get_views() {
	return views;
}
std::string Room::get_view_string() {
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
			v.second->object->get_name() << "\t" <<
			v.second->object_horizontal_border << "\t" <<
			v.second->object_vertical_border << "\t" <<
			v.second->horizontal_speed << "\t" <<
			v.second->vertical_speed << "\n";
		}

		return view_string.str();
	}
	return "none\n";
}
std::map<int, InstanceData*> Room::get_instances() {
	return instances;
}
std::string Room::get_instance_string() {
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
int Room::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Room::set_path(std::string path) {
	add_to_resources("resources/rooms"+path);
	room_path = "resources/rooms/"+path;
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
int Room::set_speed(int new_speed) {
	speed = new_speed;
	return 0;
}
int Room::set_is_persistent(bool new_is_persistent) {
	is_persistent = new_is_persistent;
	return 0;
}
int Room::set_background_color(rgb new_background_color) {
	background_color = new_background_color;
	return 0;
}
int Room::set_background_color(int r, int g, int b) {
	background_color = std::make_tuple(r, g, b);
	return 0;
}
int Room::set_is_background_color_enabled(bool new_is_background_color_enabled) {
	is_background_color_enabled = new_is_background_color_enabled;
	return 0;
}
int Room::set_background(int index, BackgroundData* new_background) {
	if (backgrounds.find(index) != backgrounds.end()) { // if the background exists, overwrite it
		backgrounds.erase(index);
	}
	backgrounds.insert(std::pair<int,BackgroundData*>(index,new_background));
	return 0;
}
int Room::set_is_views_enabled(bool new_is_views_enabled) {
	is_views_enabled = new_is_views_enabled;
	return 0;
}
int Room::set_view(int index, ViewData* new_view) {
	if (views.find(index) != views.end()) { // if the view exists, overwrite it
		views.erase(index);
	}
	views.insert(std::pair<int,ViewData*>(index, new_view));
	return 0;
}
int Room::set_instance(int index, InstanceData* new_instance) {
	if (instances.find(index) != instances.end()) { //  if the instance exists, overwrite it
		instances.erase(index);
	}
	instances.insert(std::pair<int,InstanceData*>(index, new_instance));
	return 0;
}
int Room::add_instance(InstanceData* new_instance) {
	int index = new_instance->id;
	if (index < 0) {
		index = instances.size();
		new_instance->id = index;
	}
	return set_instance(index, new_instance);
}
int Room::remove_instance(int index) {
	instances.erase(index);
	for (unsigned int i=index; i<instances.size(); i++) {
		if (instances.find(i)++ != instances.end()) {
			instances[i] = instances[i+1];
		}
	}
	return 0;
}

int Room::load_media() {
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
int Room::free_media() {
	for (auto& i : instances) {
		i.second->object->get_sprite()->free();
	}

	return 0;
}
int Room::reset_properties() {
	instances.clear();

	// Reset background data
	for (auto& i : backgrounds) {
		i.second->background->set_time_update();
	}

	return 0;
}

int Room::create() {
	for (auto& i : instances) {
		i.second->object->create(i.second);
	}

	return 0;
}
int Room::destroy() {
	for (auto& i : instances) {
		i.second->object->destroy(i.second);
	}

	return 0;
}
int Room::alarm(int alarm) {
	for (auto& i : instances) {
		i.second->object->alarm(i.second, alarm);
	}

	return 0;
}
int Room::step_begin() {
	for (auto& i : instances) {
		i.second->object->step_begin(i.second);
	}

	return 0;
}
int Room::step_mid() {
	for (auto& i : instances) {
		i.second->object->step_mid(i.second);
	}

	return 0;
}
int Room::step_end() {
	for (auto& i : instances) {
		i.second->object->step_end(i.second);
	}

	return 0;
}
int Room::keyboard(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->keyboard(i.second, e);
	}

	return 0;
}
int Room::mouse(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->mouse(i.second, e);
	}

	return 0;
}
int Room::keyboard_press(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->keyboard_press(i.second, e);
	}

	return 0;
}
int Room::mouse_press(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->mouse_press(i.second, e);
	}

	return 0;
}
int Room::keyboard_release(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->keyboard_release(i.second, e);
	}

	return 0;
}
int Room::mouse_release(SDL_Event* e) {
	for (auto& i : instances) {
		i.second->object->mouse_release(i.second, e);
	}

	return 0;
}
int Room::path_end() {
	for (auto& i : instances) {
		i.second->object->path_end(i.second);
	}

	return 0;
}
int Room::outside_room() {
	for (auto& i : instances) {
		i.second->object->outside_room(i.second);
	}

	return 0;
}
int Room::intersect_boundary() {
	for (auto& i : instances) {
		i.second->object->intersect_boundary(i.second);
	}

	return 0;
}
int Room::collision() {
	int otherid = -1;

	for (auto& i : instances) {
		i.second->object->collision(i.second, otherid);
	}

	return 0;
}
int Room::draw() {
	SDL_RenderClear(game->renderer);

	BackgroundData b;
	for (unsigned int i=0; i<backgrounds.size(); i++) {
		b = *backgrounds[i];
		if (b.is_visible && !b.is_foreground) {
			b.background->draw(b.x, b.y, b.is_horizontal_tile, b.is_vertical_tile, b.horizontal_speed, b.vertical_speed, b.is_stretched);
		}
	}

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

					i.second->vx = i.second->x; // This needs to be fixed for when viewports are not the default
					i.second->vy = i.second->y;
					i.second->object->draw(i.second);
				}
			}
		} else {
			i.second->vx = i.second->x;
			i.second->vy = i.second->y;
			i.second->object->draw(i.second);
		}
	}

	for (unsigned int i=0; i<backgrounds.size(); i++) {
		b = *backgrounds[i];
		if (b.is_visible && b.is_foreground) {
			b.background->draw(b.x, b.y, b.is_horizontal_tile, b.is_vertical_tile, b.horizontal_speed, b.vertical_speed, b.is_stretched);
		}
	}

	SDL_RenderPresent(game->renderer);

	return 0;
}
int Room::animation_end(Sprite* finished_sprite) {
	for (auto& i : instances) {
		if (i.second->object->get_sprite()->get_id() == finished_sprite->get_id()) {
			i.second->object->animation_end(i.second);
		}
	}

	return 0;
}
int Room::room_start() {
	for (auto& i : instances) {
		i.second->object->room_start(i.second);
	}

	return 0;
}
int Room::room_end() {
	for (auto& i : instances) {
		i.second->object->room_end(i.second);
	}

	return 0;
}
int Room::game_start() {
	for (auto& i : instances) {
		i.second->object->game_start(i.second);
	}

	return 0;
}
int Room::game_end() {
	for (auto& i : instances) {
		i.second->object->game_end(i.second);
	}

	return 0;
}

#endif // _EGE_ROOM_H
