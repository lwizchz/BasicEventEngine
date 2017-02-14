/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INSTANCEDATA
#define _BEE_INSTANCEDATA 1

#include "instancedata.hpp"

int BEE::CollisionPolygon::add_vertex(double vx, double vy) {
	if (lines.empty()) {
		lines.push_back({vx, vy, vx, vy});
	} else {
		double x1 = lines.back().x2;
		double y1 = lines.back().y2;
		lines.push_back({x1, y1, vx, vy});
	}

	return 0;
}
int BEE::CollisionPolygon::finalize() {
	if (!lines.empty()) {
		lines.front().x1 = lines.back().x2;
		lines.front().y1 = lines.back().y2;

		line_amount = lines.size();
	}
	return 0;
}

BEE::InstanceData::InstanceData() {
	velocity.clear();
	for (size_t i=0; i<ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}
}
BEE::InstanceData::InstanceData(BEE* new_game, int new_id, Object* new_object, int new_x, int new_y) {
	game = new_game;
	init(new_id, new_object, new_x, new_y);
}
int BEE::InstanceData::init(int new_id, Object* new_object, int new_x, int new_y) {
	id = new_id;
	object = new_object;
	subimage_time = SDL_GetTicks();
	depth = object->get_depth();

	x = (double)new_x;
	y = (double)new_y;
	xprevious = x;
	yprevious = y;
	xstart = x;
	ystart = y;
	velocity.clear();
	old_velocity.clear();
	gravity = 0.0;

	for (size_t i=0; i<ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}

	if (object->get_mask() != nullptr) {
		mask = {};
		mask.x = x;
		mask.y = y;
		mask.w = (double)get_width();
		mask.h = (double)get_height();

		mask.add_vertex(0.0,                 0.0);
		mask.add_vertex((double)get_width(), 0.0);
		mask.add_vertex((double)get_width(), (double)get_height());
		mask.add_vertex(0.0,                 (double)get_height());
		mask.finalize();
	}

	return 0;
}
int BEE::InstanceData::print() {
	std::stringstream s;
	s <<
	"InstanceData { "
	"\n	id            " << id <<
	"\n	object        " << object <<
	"\n	subimage_time " << subimage_time <<
	"\n	x, y          " << x << ", " << y <<
	"\n	depth         " << depth <<
	//"\n	velocity      " << velocity <<
	"\n	gravity       " << gravity <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}

bool BEE::InstanceData::operator< (const InstanceData& other) const {
	if (depth == other.depth) {
		return (id < other.id);
	}
	return (depth > other.depth);
}

int BEE::InstanceData::remove() {
	object->destroy(this);
	game->get_current_room()->remove_instance(id);
	return 0;
}

int BEE::InstanceData::set_alarm(int alarm, Uint32 elapsed_ticks) {
	alarm_end[alarm] = elapsed_ticks + SDL_GetTicks();
	return 0;
}

int BEE::InstanceData::set_object(BEE::Object* new_object) {
	object->remove_instance(id);
	object = new_object;
	object->add_instance(id, this);
	return 0;
}

double BEE::InstanceData::get_xstart() {
	return xstart;
}
double BEE::InstanceData::get_ystart() {
	return ystart;
}
int BEE::InstanceData::get_width() {
	if (object->get_mask() == nullptr) {
		return 0;
	}
	return object->get_mask()->get_subimage_width();
}
int BEE::InstanceData::get_height() {
	if (object->get_mask() == nullptr) {
		return 0;
	}
	return object->get_mask()->get_height();
}
double BEE::InstanceData::get_center_x() {
	return x + (double)get_width()/2.0;
}
double BEE::InstanceData::get_center_y() {
	return y + (double)get_height()/2.0;
}

int BEE::InstanceData::move(double new_magnitude, double new_direction) {
	if (new_magnitude < 0.0) {
		new_direction -= 180.0;
		new_magnitude = fabs(new_magnitude);
	}
	new_direction = absolute_angle(new_direction);
	velocity.push_back(std::make_pair(new_magnitude, new_direction));
	return 0;
}
int BEE::InstanceData::move_to(double new_magnitude, double other_x, double other_y) {
	if (distance(x, y, other_x, other_y) < new_magnitude) {
		return 1;
	}
	move(new_magnitude, direction_of(x, y, other_x, other_y));
	return 0;
}
int BEE::InstanceData::move_away(double new_magnitude, double other_x, double other_y) {
	move(new_magnitude, direction_of(x, y, other_x, other_y)+180.0);
	return 0;
}
int BEE::InstanceData::set_friction(double new_friction) {
	friction = new_friction;
	return 0;
}
int BEE::InstanceData::set_gravity(double new_gravity) {
	gravity = new_gravity;
	return 0;
}
int BEE::InstanceData::set_gravity_direction(double new_gravity_direction) {
	gravity_direction = new_gravity_direction;
	return 0;
}
int BEE::InstanceData::set_gravity_acceleration(double new_acceleration) {
	acceleration = new_acceleration;
	return 0;
}
int BEE::InstanceData::reset_gravity_acceleration() {
	acceleration_amount = 0.0;
	return 0;
}
bool BEE::InstanceData::check_collision_polygon(const CollisionPolygon& m1, const CollisionPolygon& m2) {
	SDL_Rect a = {(int)m1.x, (int)m1.y, (int)m1.w, (int)m1.h};
	SDL_Rect b = {(int)m2.x, (int)m2.y, (int)m2.w, (int)m2.h};

	if ((m1.line_amount == 0)||(m2.line_amount == 0)) {
		return check_collision(a, b);
	} else {
		bool r = false;
		if (check_collision(a, b)) {
			for (const auto& l1 : m1.lines) {
				for (const auto& l2 : m2.lines) {
					if (game->options->is_debug_enabled) {
						game->draw_line(l1.x1+m1.x, l1.y1+m1.y, l1.x2+m1.x, l1.y2+m1.y, {255, 0, 0, 255});
					}

					if (check_collision_line({l1.x1+m1.x, l1.y1+m1.y, l1.x2+m1.x, l1.y2+m1.y}, {l2.x1+m2.x, l2.y1+m2.y, l2.x2+m2.x, l2.y2+m2.y})) {
						if (!game->options->is_debug_enabled) {
							return true;
						}
						r = true;
					}
				}
			}
		}
		return r;
	}
}
bool BEE::InstanceData::check_collision_polygon(const CollisionPolygon& other) {
	return check_collision_polygon(mask, other);
}
int BEE::InstanceData::move_outside(const Line& l, const CollisionPolygon& m) {
	double dist = distance(l.x1, l.y1, l.x2, l.y2);
	double dir = direction_of(l.x1, l.y1, l.x2, l.y2);
	x = l.x1;
	y = l.y1;

	int max_attempts = 10;
	double delta = 1.0/((double)max_attempts);
	int attempts = 0;

	mask.x = x; mask.y = y;
	while ((check_collision_polygon(mask, m))&&(attempts++ < max_attempts)) {
		x += cos(degtorad(dir)) * delta*dist * game->get_delta();
		y += -sin(degtorad(dir)) * delta*dist * game->get_delta();
		mask.x = x;
		mask.y = y;
	}

	if (check_collision_polygon(mask, m)) {
		x -= cos(degtorad(dir)) * delta*dist * game->get_delta();
		y -= -sin(degtorad(dir)) * delta*dist * game->get_delta();
		mask.x = x;
		mask.y = y;
	}

	return 0;
}
int BEE::InstanceData::move_avoid(const CollisionPolygon& other) {
	mask.x = x;
	mask.y = y;

	for (auto v=old_velocity.begin(); v!=old_velocity.end(); ++v) {
		xprevious = x;
		yprevious = y;

		x += cos(degtorad((*v).second)) * (*v).first * game->get_delta();
		y += -sin(degtorad((*v).second)) * (*v).first * game->get_delta();
		double resultant_magnitude = (*v).first;
		if (!is_place_free(x, y)) {
			if ((x != xprevious)||(y != yprevious)) {
				move_outside({xprevious, yprevious, x, y}, other);
				resultant_magnitude -= distance(x, y, xprevious, yprevious);

				if (resultant_magnitude > 0.0) {
					bool check_right = true;
					if (is_angle_between((*v).second, 180.0, 360.0)) {
						check_right = false;
					}

					int d = 180;
					while ((d > 0)&&(d < 360)) {
						double px = cos(degtorad(d)) * resultant_magnitude * game->get_delta();
						double py = -sin(degtorad(d)) * resultant_magnitude * game->get_delta();
						mask.x = x+px;
						mask.y = y+py;

						if (!check_collision_polygon(mask, other)) {
							if (is_place_free(x+px, y+py)) {
								x += px;
								y += py;
								break;
							}
						}

						if (check_right) {
							d -= 5;
						} else {
							d += 5;
						}
					}
				}
				(*v).first = distance(x, y, xprevious, yprevious);
			}
		}
	}

	return 0;
}

std::pair<double,double> BEE::InstanceData::get_motion() {
	double xsum = x;
	double ysum = y;

	for (auto& v : velocity) {
		xsum += cos(degtorad(v.second))*v.first * game->get_delta();
		ysum += -sin(degtorad(v.second))*v.first * game->get_delta();
	}

	double d = direction_of(x, y, xsum, ysum);
	xsum += cos(degtorad(d))*friction * game->get_delta();
	ysum += -sin(degtorad(d))*friction * game->get_delta();

	return std::make_pair(xsum, ysum);
}
std::pair<double,double> BEE::InstanceData::get_applied_gravity() {
	double xsum = x;
	double ysum = y;

	if (gravity != 0.0) {
		double g = gravity*pow(acceleration, acceleration_amount);
		xsum += cos(degtorad(gravity_direction))*g * game->get_delta();
		ysum += -sin(degtorad(gravity_direction))*g * game->get_delta();
		if (acceleration_amount < 10.0) {
			acceleration_amount += 0.01;
		}
	}

	return std::make_pair(xsum, ysum);
}
std::pair<double,double> BEE::InstanceData::get_position() {
	return std::make_pair(x, y);
}
double BEE::InstanceData::get_hspeed() {
	return get_motion().first - xprevious;
}
double BEE::InstanceData::get_vspeed() {
	double g = gravity*pow(acceleration, acceleration_amount), gd = gravity_direction;
	return get_motion().second - yprevious + cos(degtorad(gd))*g;
}
double BEE::InstanceData::get_direction() {
	double xsum=0.0, ysum=0.0;
	std::tie (xsum, ysum) = get_motion();
	if ((x == xsum)&&(y == ysum)) {
		return direction_of(xprevious, yprevious, x, y);
	}
	return direction_of(x, y, xsum, ysum);
}
double BEE::InstanceData::get_speed() {
	double xsum=0.0, ysum=0.0;
	std::tie (xsum, ysum) = get_motion();
	if ((x == xsum)&&(y == ysum)) {
		return distance(xprevious, yprevious, x, y);
	}
	return distance(x, y, xsum, ysum);
}
double BEE::InstanceData::get_friction() {
	return friction;
}
double BEE::InstanceData::get_gravity() {
	return gravity;
}
double BEE::InstanceData::get_gravity_direction() {
	return gravity_direction;
}
double BEE::InstanceData::get_gravity_acceleration() {
	return acceleration;
}
double BEE::InstanceData::get_gravity_acceleration_amount() {
	return acceleration_amount;
}

bool BEE::InstanceData::is_place_free(int new_x, int new_y) {
	mask.x = new_x;
	mask.y = new_y;

	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second == this) {
			continue;
		}

		i.second->mask.x = (int)i.second->x;
		i.second->mask.y = (int)i.second->y;

		if (i.second->object->get_is_solid()) {
			if (check_collision_polygon(i.second->mask)) {
				object->update(this);
				if (object->check_collision_list(i.second->object)) {
					i.second->object->update(i.second);
					if (i.second->object->check_collision_list(object)) {
						return false;
					}
				}
			}
		}
	}
	return true;
}
bool BEE::InstanceData::is_place_empty(int new_x, int new_y) {
	mask.x = new_x;
	mask.y = new_y;

	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second == this) {
			continue;
		}

		i.second->mask.x = (int)i.second->x;
		i.second->mask.y = (int)i.second->y;

		if (check_collision_polygon(i.second->mask)) {
			return false;
		}
	}
	return true;
}
bool BEE::InstanceData::is_place_meeting(int new_x, int new_y, Object* other) {
	mask.x = new_x;
	mask.y = new_y;

	for (auto& i : other->get_instances()) {
		if (i.second == this) {
			continue;
		}

		i.second->mask.x = (int)i.second->x;
		i.second->mask.y = (int)i.second->y;

		if (check_collision_polygon(i.second->mask)) {
			return true;
		}
	}
	return false;
}
bool BEE::InstanceData::is_place_meeting(int new_x, int new_y, int other_id) {
	return is_place_meeting(new_x, new_y, game->get_object(other_id));
}
bool BEE::InstanceData::is_move_free(double magnitude, double direction) {
	double dx = cos(degtorad(direction)) * magnitude;
	double dy = -sin(degtorad(direction)) * magnitude;
	return is_place_free(x+dx, y+dy);
}
bool BEE::InstanceData::is_snapped(int hsnap, int vsnap) {
	if (((int)x % hsnap == 0)&&((int)y % vsnap == 0)) {
		return true;
	}
	return false;
}

int BEE::InstanceData::move_random(int hsnap, int vsnap) {
	x = random(game->get_current_room()->get_width());
	y = random(game->get_current_room()->get_height());
	move_snap(hsnap, vsnap);
	return 0;
}
std::pair<int,int> BEE::InstanceData::get_snapped(int hsnap, int vsnap) {
	if (hsnap < 1) {
		hsnap = 1;
	}
	if (vsnap < 1) {
		vsnap = 1;
	}

	int xsnap = (int)x;
	int ysnap = (int)y;

	int dx = xsnap % hsnap;
	int dy = ysnap % vsnap;

	if (x < 0) {
		xsnap -= hsnap;
	}
	if (y < 0) {
		ysnap -= hsnap;
	}
	xsnap -= dx;
	ysnap -= dy;

	return std::make_pair(xsnap, ysnap);
}
std::pair<int,int> BEE::InstanceData::get_snapped() {
	if (object->get_sprite() == nullptr) {
		return std::make_pair((int)x, (int)y);
	}
	return get_snapped(object->get_sprite()->get_width(), object->get_sprite()->get_height());
}
int BEE::InstanceData::move_snap(int hsnap, int vsnap) {
	std::tie(x, y) = get_snapped(hsnap, vsnap);
	return 0;
}
int BEE::InstanceData::move_snap() {
	if (object->get_sprite() == nullptr) {
		return 0;
	}
	return move_snap(object->get_sprite()->get_width(), object->get_sprite()->get_height());
}
int BEE::InstanceData::move_wrap(bool is_horizontal, bool is_vertical, int margin) {
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

double BEE::InstanceData::get_distance(int dx, int dy) {
	return distance(x, y, dx, dy);
}
double BEE::InstanceData::get_distance(InstanceData* other) {
	return distance(x, y, other->x, other->y);
}
double BEE::InstanceData::get_distance(Object* other) {
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
double BEE::InstanceData::get_direction_of(int dx, int dy) {
	return direction_of(x, y, dx, dy);
}
double BEE::InstanceData::get_direction_of(InstanceData* other) {
	return direction_of(x, y, other->x, other->y);
}
double BEE::InstanceData::get_direction_of(Object* other) {
	double shortest_distance = 0.0, current_distance = 0.0;
	InstanceData* closest_instance = nullptr;
	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second->object->get_id() == other->get_id()) {
			current_distance = distance(x, y, i.second->x, i.second->y);
			if (current_distance < shortest_distance) {
				shortest_distance = current_distance;
				closest_instance = i.second;
			}
		}
	}

	if (closest_instance != nullptr) {
		return direction_of(x, y, closest_instance->x, closest_instance->y);
	}
	return 0.0;
}
int BEE::InstanceData::get_relation(InstanceData* other) {
	if ((other->get_center_y() < get_center_y())&&(abs(other->get_center_x() - get_center_x()) < other->get_width()/2 + get_width()/2)) { // Top block
		return 1;
	} else if ((other->get_center_x() > get_center_x())&&(abs(other->get_center_y() - get_center_y()) < other->get_height()/2 + get_height()/2)) { // Right block
		return 2;
	} else if ((other->get_center_y() > get_center_y())&&(abs(other->get_center_x() - get_center_x()) < other->get_width()/2 + get_width()/2)) { // Bottom block
		return 3;
	} else if ((other->get_center_x() < get_center_x())&&(abs(other->get_center_y() - get_center_y()) < other->get_height()/2 + get_height()/2)) { // Left block
		return 4;
	}
	return 0;
}

int BEE::InstanceData::path_start(Path* new_path, double new_path_speed, int new_end_action, bool absolute) {
	path = new_path;
	path_speed = new_path_speed;
	path_end_action = new_end_action;
	path_current_node = 0;

	if (absolute) {
		path_xstart = std::get<0>(path->get_coordinate_list().front());
		path_ystart = std::get<1>(path->get_coordinate_list().front());
	} else {
		path_xstart = x;
		path_ystart = y;
	}

	return 0;
}
int BEE::InstanceData::path_end() {
	path = nullptr;
	path_speed = 0.0;
	path_end_action = 0;
	path_xstart = 0;
	path_ystart = 0;
	path_current_node = 0;
	return 0;
}
int BEE::InstanceData::path_reset() {
	bool a = false;
	if (path_xstart == std::get<0>(path->get_coordinate_list().front())) {
		if (path_ystart == std::get<1>(path->get_coordinate_list().front())) {
			a = true;
		}
	}

	return path_start(path, path_speed, path_end_action, a);
}
int BEE::InstanceData::path_update_node() {
	if (has_path()) {
		if (path_speed >= 0) {
			if (path_current_node+1 < (int) path->get_coordinate_list().size()) {
				bee_path_coord c = path->get_coordinate_list().at(path_current_node+1);
				if (distance(x, y, path_xstart+std::get<0>(c), path_ystart+std::get<1>(c)) < get_speed()) {
					path_current_node++;
				}
			}
		} else {
			bee_path_coord c = path->get_coordinate_list().at(path_current_node);
			if (distance(x, y, path_xstart+std::get<0>(c), path_ystart+std::get<1>(c)) < get_speed()) {
				path_current_node--;
			}
		}

		return 0;
	}
	return 1;
}
int BEE::InstanceData::set_path_drawn(bool new_path_is_drawn) {
	path_is_drawn = new_path_is_drawn;
	return 0;
}
int BEE::InstanceData::set_path_pausable(bool new_path_is_pausable) {
	path_is_pausable = new_path_is_pausable;
	return 0;
}
int BEE::InstanceData::handle_path_end() {
	if (has_path()) {
		switch (path_end_action) {
			case 0: { // Stop path
				path_end();
				break;
			}
			case 1: { // Continue from start
				path_current_node = 0;
				x = path_xstart;
				y = path_ystart;
				xprevious = x;
				yprevious = y;
				break;
			}
			case 2: { // Continue from current position
				path_current_node = 0;
				path_xstart = x;
				path_ystart = y;
				break;
			}
			case 3: { // Reverse direction
				path_speed *= -1;
				if (path_speed >= 0) {
					path_current_node = 0;
				} else {
					path_current_node = path->get_coordinate_list().size()-2;
				}
				break;
			}
		}
		return 0;
	}
	return 1;
}
bool BEE::InstanceData::has_path() {
	return (path != nullptr) ? true : false;
}
bool BEE::InstanceData::get_path_drawn() {
	return path_is_drawn;
}
int BEE::InstanceData::get_path_speed() {
	return path_speed;
}
int BEE::InstanceData::get_path_node() {
	return path_current_node;
}
std::vector<bee_path_coord> BEE::InstanceData::get_path_coords() {
	std::vector<bee_path_coord> no_path;
	return (has_path()) ? path->get_coordinate_list() : no_path;
}
bool BEE::InstanceData::get_path_pausable() {
	return path_is_pausable;
}

int BEE::InstanceData::draw(int w, int h, double angle, RGBA color, SDL_RendererFlip flip) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (object->get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return object->get_sprite()->draw(x-xo, y-yo, subimage_time, w, h, angle, color, flip);
}
int BEE::InstanceData::draw(int w, int h, double angle, bee_rgba_t color, SDL_RendererFlip flip) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	return draw(w, h, angle, game->get_enum_color(color), flip);
}
int BEE::InstanceData::draw() {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (object->get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return object->get_sprite()->draw(x-xo, y-yo, subimage_time);
}
int BEE::InstanceData::draw(int w, int h) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (object->get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return object->get_sprite()->draw(x-xo, y-yo, subimage_time, w, h);
}
int BEE::InstanceData::draw(double angle) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (object->get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return object->get_sprite()->draw(x-xo, y-yo, subimage_time, angle);
}
int BEE::InstanceData::draw(RGBA color) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (object->get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return object->get_sprite()->draw(x-xo, y-yo, subimage_time, color);
}
int BEE::InstanceData::draw(bee_rgba_t color) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	return draw(game->get_enum_color(color));
}
int BEE::InstanceData::draw(SDL_RendererFlip flip) {
	if (object->get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (object->get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return object->get_sprite()->draw(x-xo, y-yo, subimage_time, flip);
}

int BEE::InstanceData::draw_path() {
	if (path != nullptr) {
		return path->draw(path_xstart, path_ystart);
	}
	return 0;
}

int BEE::InstanceData::draw_debug() {
	if (mask.lines.size() > 0) {
		int xs = (int)x;
		int ys = (int)y;
		for (auto& l : mask.lines) {
			game->draw_line(l.x1+xs, l.y1+ys, l.x2+xs, l.y2+ys, c_aqua);
		}
		return 0;
	} else if (object->get_mask() != nullptr) {
		int w = object->get_mask()->get_subimage_width();
		int h = object->get_mask()->get_height();
		return game->draw_rectangle(x, y, w, h, false, c_aqua);
	}
	return 1;
}

#endif // _BEE_INSTANCEDATA
