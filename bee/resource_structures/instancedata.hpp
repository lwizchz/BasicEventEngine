/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INSTANCEDATA_H
#define _BEE_INSTANCEDATA_H 1

#define ALARM_COUNT 8

class InstanceData {
		float xprevious = 0.0, yprevious = 0.0;
		float xstart = 0.0, ystart = 0.0;

		double friction;
		double gravity = 0.0, gravity_direction = 180.0;

		Path* path = NULL;
		double path_speed = 0.0;
		int path_end_action = 0;
		int path_xstart = 0, path_ystart = 0;
		unsigned int path_current_node = 0;
	public:
		BEE* game = NULL;

		int id = -1;
		Object* object = NULL;
		Uint32 subimage_time = 0;
		Uint32 alarm_end[ALARM_COUNT];

		float x = 0.0, y = 0.0;
		std::list<std::pair<double,double>> velocity;

		InstanceData();
		InstanceData(BEE*, int, Object*, int, int);
		int init(int, Object*, int, int);
		int print();

		int remove();

		int set_alarm(int, Uint32);

		int move(double, double);
		int move_to(double, double, double);
		int move_away(double, double, double);
		int set_friction(double);
		int set_gravity(double);
		int set_gravity_direction(double);

		std::pair<double,double> get_motion();
		double get_hspeed();
		double get_vspeed();
		double get_direction();
		double get_speed();
		double get_friction();
		double get_gravity();
		double get_gravity_direction();

		bool is_place_free(int, int);
		bool is_place_empty(int, int);
		bool is_place_meeting(int, int, Object*);
		bool is_place_meeting(int, int, int);
		bool is_snapped(int, int);

		int move_random(int, int);
		int move_snap(int, int);
		int move_wrap(bool, bool, int);

		double get_distance(int, int);
		double get_distance(InstanceData*);
		double get_distance(Object*);

		int path_start(Path*, double, int, bool);
		int path_end();
		int path_update_node();
		int handle_path_end();
		bool has_path();
		int get_path_speed();
		unsigned int get_path_node();
		std::vector<path_coord> get_path_coords();

		int draw(int, int, double, RGBA);
		int draw();
		int draw(int, int);
		int draw(double);
		int draw(RGBA);
};
InstanceData::InstanceData() {
	velocity.clear();
	for (int i=0; i<ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}
}
InstanceData::InstanceData(BEE* new_game, int new_id, Object* new_object, int new_x, int new_y) {
	game = new_game;
	init(new_id, new_object, new_x, new_y);
}
int InstanceData::init(int new_id, Object* new_object, int new_x, int new_y) {
	id = new_id;
	object = new_object;
	subimage_time = SDL_GetTicks();
	x = (float)new_x;
	y = (float)new_y;
	xprevious = x;
	yprevious = y;
	xstart = x;
	ystart = y;
	velocity.clear();
	gravity = 0.0;

	for (int i=0; i<ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}

	return 0;
}
int InstanceData::print() {
	std::cout <<
	"InstanceData { "
	"\n	id		" << id <<
	"\n	object		" << object <<
	"\n	subimage_time	" << subimage_time <<
	"\n	x, y		" << x << ", " << y <<
	//"\n	velocity	" << velocity <<
	"\n	gravity		" << gravity <<
	"\n}\n";

	return 0;
}

int InstanceData::remove() {
	object->destroy(this);
	game->get_current_room()->remove_instance(id);
	return 0;
}

int InstanceData::set_alarm(int alarm, Uint32 elapsed_ticks) {
	alarm_end[alarm] = elapsed_ticks + SDL_GetTicks();
	return 0;
}

int InstanceData::move(double new_magnitude, double new_direction) {
	if (new_direction < 0.0) {
		new_direction = 360.0 + new_direction;
	}
	velocity.push_back(std::make_pair(new_magnitude, fmod(new_direction, 360.0)));
	return 0;
}
int InstanceData::move_to(double new_magnitude, double other_x, double other_y) {
	if (distance(x, y, other_x, other_y) < new_magnitude) {
		return 1;
	}
	move(new_magnitude, direction_of(x, y, other_x, other_y));
	return 0;
}
int InstanceData::move_away(double new_magnitude, double other_x, double other_y) {
	move(new_magnitude, direction_of(x, y, other_x, other_y)+180.0);
	return 0;
}
int InstanceData::set_friction(double new_friction) {
	friction = new_friction;
	return 0;
}
int InstanceData::set_gravity(double new_gravity) {
	gravity = new_gravity;
	return 0;
}
int InstanceData::set_gravity_direction(double new_gravity_direction) {
	gravity_direction = new_gravity_direction;
	return 0;
}

std::pair<double,double> InstanceData::get_motion() {
	double xsum = x;
	double ysum = y;

	velocity.push_back(std::make_pair(gravity, gravity_direction));
	for (auto& v : velocity) {
		xsum += sin(degtorad(v.second))*v.first;
		ysum += -cos(degtorad(v.second))*v.first;
	}

	double d = direction_of(x, y, xsum, ysum);
	xsum += sin(degtorad(d))*friction;
	xsum += -cos(degtorad(d))*friction;

	return std::make_pair(xsum, ysum);
}
double InstanceData::get_hspeed() {
	return get_motion().first;
}
double InstanceData::get_vspeed() {
	return get_motion().second;
}
double InstanceData::get_direction() {
	double xsum=0.0, ysum=0.0;
	std::tie (xsum, ysum) = get_motion();
	return direction_of(x, y, xsum, ysum);
}
double InstanceData::get_speed() {
	double xsum=0.0, ysum=0.0;
	std::tie (xsum, ysum) = get_motion();
	return distance(x, y, xsum, ysum);
}
double InstanceData::get_friction() {
	return friction;
}
double InstanceData::get_gravity() {
	return gravity;
}
double InstanceData::get_gravity_direction() {
	return gravity_direction;
}

bool InstanceData::is_place_free(int new_x, int new_y) {
	bool is_collision = false;
	SDL_Rect a = {new_x, new_y, object->get_mask()->get_subimage_width(), object->get_mask()->get_height()};
	for (auto& i : game->get_current_room()->get_instances()) {
		SDL_Rect b = {(int)i.second->x, (int)i.second->y, i.second->object->get_mask()->get_subimage_width(), i.second->object->get_mask()->get_height()};
		if (check_collision(&a, &b)) {
			if (i.second->object->get_is_solid()) {
				is_collision = true;
				break;
			}
		}
	}
	return is_collision;
}
bool InstanceData::is_place_empty(int new_x, int new_y) {
	bool is_collision = false;
	SDL_Rect a = {new_x, new_y, object->get_mask()->get_subimage_width(), object->get_mask()->get_height()};
	for (auto& i : game->get_current_room()->get_instances()) {
		SDL_Rect b = {(int)i.second->x, (int)i.second->y, i.second->object->get_mask()->get_subimage_width(), i.second->object->get_mask()->get_height()};
		if (check_collision(&a, &b)) {
			is_collision = true;
			break;
		}
	}
	return is_collision;
}
bool InstanceData::is_place_meeting(int new_x, int new_y, Object* other) {
	bool is_collision = false;
	SDL_Rect a = {new_x, new_y, object->get_mask()->get_subimage_width(), object->get_mask()->get_height()};
	for (auto& i : other->get_instances()) {
		SDL_Rect b = {(int)i.second->x, (int)i.second->y, i.second->object->get_mask()->get_subimage_width(), i.second->object->get_mask()->get_height()};
		if (check_collision(&a, &b)) {
			is_collision = true;
			break;
		}
	}
	return is_collision;
}
bool InstanceData::is_place_meeting(int new_x, int new_y, int other_id) {
	bool is_collision = false;
	SDL_Rect a = {new_x, new_y, object->get_mask()->get_subimage_width(), object->get_mask()->get_height()};
	for (auto& i : get_object(other_id)->get_instances()) {
		SDL_Rect b = {(int)i.second->x, (int)i.second->y, i.second->object->get_mask()->get_subimage_width(), i.second->object->get_mask()->get_height()};
		if (check_collision(&a, &b)) {
			is_collision = true;
			break;
		}
	}
	return is_collision;
}
bool InstanceData::is_snapped(int hsnap, int vsnap) {
	if (((int)x % hsnap == 0)&&((int)y % vsnap == 0)) {
		return true;
	}
	return false;
}

int InstanceData::move_random(int hsnap, int vsnap) {
	x = random(game->get_current_room()->get_width());
	y = random(game->get_current_room()->get_height());
	move_snap(hsnap, vsnap);
	return 0;
}
int InstanceData::move_snap(int hsnap, int vsnap) {
	int dx = (int)x % hsnap;
	int dy = (int)y % vsnap;

	if (dx >= hsnap/2) {
		x += hsnap;
	}
	x -= dx;
	if (dy >= vsnap/2) {
		y += vsnap;
	}
	y -= dy;

	return 0;
}
int InstanceData::move_wrap(bool is_horizontal, bool is_vertical, int margin) {
	int w = game->get_current_room()->get_width();
	int h = game->get_current_room()->get_height();

	if (is_horizontal) {
		if (x < -margin){
			x = w+margin-x;
		} else if (x > w+margin) {
			x -= w+margin;
		}
	}
	if (is_vertical) {
		if (y < -margin) {
			y = h+margin-y;
		} else if (x > h+margin) {
			y -= h+margin;
		}
	}

	return 0;
}

double InstanceData::get_distance(int dx, int dy) {
	return distance(x, y, dx, dy);
}
double InstanceData::get_distance(InstanceData* other) {
	return distance(x, y, other->x, other->y);
}
double InstanceData::get_distance(Object* other) {
	double shortest_distance = 0.0, current_distance = 0.0;
	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second->object->get_id() == other->get_id()) {
			current_distance = distance(x, y, i.second->x, i.second->y);
			if (current_distance < shortest_distance) {
				shortest_distance = current_distance;
			}
		}
	}
	return shortest_distance;
}

int InstanceData::path_start(Path* new_path, double new_path_speed, int new_end_action, bool absolute) {
	path = new_path;
	path_speed = new_path_speed;
	path_end_action = new_end_action;
	path_current_node = 0;

	if (absolute) {
		path_xstart = std::get<0>(path->get_coordinate_list().front());
		path_xstart = std::get<1>(path->get_coordinate_list().front());
	} else {
		path_xstart = x;
		path_ystart = y;
	}

	return 0;
}
int InstanceData::path_end() {
	path = NULL;
	path_speed = 0.0;
	path_end_action = 0;
	path_xstart = 0;
	path_ystart = 0;
	path_current_node = 0;
	return 0;
}
int InstanceData::path_update_node() {
	if (path_speed >= 0) {
		path_coord c = path->get_coordinate_list().at(path_current_node+1);
		if ((x == std::get<0>(c))&&(y == std::get<1>(c))) {
			path_current_node++;
		}
	} else {
		path_coord c = path->get_coordinate_list().at(path_current_node);
		if ((x == std::get<0>(c))&&(y == std::get<1>(c))) {
			path_current_node--;
		}
	}
	return 0;
}
int InstanceData::handle_path_end() {
	switch (path_end_action) {
		case 0: { // Stop path
			path_end();
			break;
		}
		case 1: {
			path_current_node = 0;
			break;
		}
		case 2: {
			path_current_node = 0;
			path_xstart = x;
			path_ystart = y;
			break;
		}
		case 3: {
			path_speed *= -1;
			break;
		}
	}
	return 0;
}
bool InstanceData::has_path() {
	return (path != NULL) ? true : false;
}
int InstanceData::get_path_speed() {
	return path_speed;
}
unsigned int InstanceData::get_path_node() {
	return path_current_node;
}
std::vector<path_coord> InstanceData::get_path_coords() {
	std::vector<path_coord> no_path;
	return (has_path()) ? path->get_coordinate_list() : no_path;
}

int InstanceData::draw(int w, int h, double angle, RGBA color) {
	return object->get_sprite()->draw(x, y, subimage_time, w, h, angle, color);
}
int InstanceData::draw() {
	return object->get_sprite()->draw(x, y, subimage_time);
}
int InstanceData::draw(int w, int h) {
	return object->get_sprite()->draw(x, y, subimage_time, w, h);
}
int InstanceData::draw(double angle) {
	return object->get_sprite()->draw(x, y, subimage_time, angle);
}
int InstanceData::draw(RGBA color) {
	return object->get_sprite()->draw(x, y, subimage_time, color);
}

#endif // _BEE_INSTANCEDATA_H
