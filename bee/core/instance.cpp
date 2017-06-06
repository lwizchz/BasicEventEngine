/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_INSTANCE
#define _BEE_CORE_INSTANCE 1

#include "instance.hpp"

BEE::Instance::Instance() {
	for (size_t i=0; i<BEE_ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}
}
BEE::Instance::Instance(BEE* new_game, int new_id, Object* new_object, double new_x, double new_y, double new_z) {
	game = new_game;
	init(new_id, new_object, new_x, new_y, new_z);
}
BEE::Instance::~Instance() {
	delete body;
}
int BEE::Instance::init(int new_id, Object* new_object, double new_x, double new_y, double new_z) {
	id = new_id;
	object = new_object;
	subimage_time = game->get_ticks();
	depth = object->get_depth();

	if (body == nullptr) {
		PhysicsWorld* w = game->get_current_room()->get_phys_world();

		double* p = new double[3] {(double)get_width(), (double)get_height(), w->get_scale()};
		if ((p[0] != 0.0)&&(p[1] != 0.0)) {
			body = new PhysicsBody(w, this, bee::E_PHYS_SHAPE::BOX, 0.0, new_x, new_y, new_z, p); // PhysicsBody assumes ownership of any shape parameters passed to it
		} else {
			body = new PhysicsBody(w, this, bee::E_PHYS_SHAPE::NONE, 0.0, new_x, new_y, new_z, nullptr);
			delete[] p;
		}
	} else {
		set_position(new_x, new_y, new_z);
	}

	pos_start = btVector3(new_x, new_y, new_z);
	pos_previous = pos_start;
	path_pos_start = btVector3(0.0, 0.0, 0.0);
	path_previous_mass = 0.0;

	is_solid = true;
	if (!object->get_is_solid()) {
		body->get_body()->setCollisionFlags(body->get_body()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		is_solid = false;
	}

	for (size_t i=0; i<BEE_ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}

	return 0;
}
int BEE::Instance::print() {
	std::stringstream s;
	s <<
	"Instance {"
	"\n	id                 " << id <<
	"\n	object             " << object <<
	"\n	sprite             " << sprite <<
	"\n	subimage_time      " << subimage_time <<
	"\n"
	"\n	body               " << body <<
	"\n	is_solid           " << is_solid <<
	"\n	depth              " << depth <<
	"\n	position           (" << get_x() << ", " << get_y() << ", " << get_z() << ")" <<
	"\n	pos_start          (" << pos_start.x() << ", " << pos_start.y() << ", " << pos_start.z() << ")" <<
	"\n	pos_previous       (" << pos_previous.x() << ", " << pos_previous.y() << ", " << pos_previous.z() << ")" <<
	"\n"
	"\n	path               " << path <<
	"\n	path_speed         " << path_speed <<
	"\n	path_end_action    " << static_cast<int>(path_end_action) <<
	"\n	path_current_node  " << path_current_node <<
	"\n	path_is_drawn      " << path_is_drawn <<
	"\n	path_is_pausable   " << path_is_pausable <<
	"\n	path_previous_mass " << path_previous_mass <<
	"\n	path_pos_start     (" << path_pos_start.x() << ", " << path_pos_start.y() << ", " << path_pos_start.z() << ")" <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::INFO, s.str());

	return 0;
}

std::string BEE::Instance::serialize(bool should_pretty_print) const {
	std::map<std::string,SIDP> data;
	data["id"] = id;
	data["object"] = object->get_name();
	data["sprite"] = "";
	if (get_sprite() != nullptr) {
		data["sprite"] = get_sprite()->get_name();
	}
	data["subimage_time"] = static_cast<int>(subimage_time);

	std::string b = body->serialize(should_pretty_print);
	if (should_pretty_print) {
		b = debug_indent(b, 1);
		b = b.substr(1, b.length()-2);
	}
	data["body"] = "\"" + string_escape(b) + "\"";

	data["is_solid"] = is_solid;
	data["depth"] = depth;
	data["pos_start"].vector(new std::vector<SIDP>({pos_start.x(), pos_start.y(), pos_start.z()}));
	data["pos_previous"].vector(new std::vector<SIDP>({pos_previous.x(), pos_previous.y(), pos_previous.z()}));

	data["path"] = "";
	if (path != nullptr) {
		data["path"] = path->get_name();
	}
	data["path_speed"] = path_speed;
	data["path_end_action"] = static_cast<int>(path_end_action);
	data["path_current_node"] = path_current_node;
	data["path_is_drawn"] = path_is_drawn;
	data["path_is_pausable"] = path_is_pausable;
	data["path_previous_mass"] = path_previous_mass;
	data["path_pos_start"].vector(new std::vector<SIDP>({path_pos_start.x(), path_pos_start.y(), path_pos_start.z()}));

	return map_serialize(data, should_pretty_print);
}
std::string BEE::Instance::serialize() const {
	return serialize(false);
}
int BEE::Instance::deserialize(const std::string& data) {
	std::map<std::string,SIDP> m;
	map_deserialize(data, &m);

	id = SIDP_i(m["id"]);
	object = game->get_object_by_name(SIDP_s(m["object"]));
	sprite = game->get_sprite_by_name(SIDP_s(m["sprite"]));

	subimage_time = SIDP_i(m["subimage_time"]);
	std::string b = SIDP_s(m["body"]);
	b.substr(1, b.length()-2);
	body->deserialize(string_unescape(b));
	is_solid = SIDP_i(m["is_solid"]);
	depth = SIDP_i(m["depth"]);
	pos_start = btVector3(SIDP_cd(m["pos_start"], 0), SIDP_cd(m["pos_start"], 1), SIDP_cd(m["pos_start"], 2));
	pos_previous = btVector3(SIDP_cd(m["pos_previous"], 0), SIDP_cd(m["pos_previous"], 1), SIDP_cd(m["pos_previous"], 2));

	path = game->get_path_by_name(SIDP_s(m["path"]));
	path_speed = SIDP_d(m["path_speed"]);
	path_end_action = (bee::E_PATH_END)SIDP_i(m["path_end_action"]);
	path_current_node = SIDP_i(m["path_current_node"]);
	path_is_drawn = SIDP_i(m["path_is_drawn"]);
	path_is_pausable = SIDP_i(m["path_is_pausable"]);
	path_previous_mass = SIDP_d(m["path_previous_mass"]);
	path_pos_start = btVector3(SIDP_cd(m["path_pos_start"], 0), SIDP_cd(m["path_pos_start"], 1), SIDP_cd(m["path_pos_start"], 2));

	return 0;
}

bool BEE::Instance::operator< (const Instance& other) const {
	if (depth == other.depth) {
		return (id < other.id);
	}
	return (depth > other.depth);
}

int BEE::Instance::remove() {
	object->destroy(this);
	game->get_current_room()->remove_instance(id);
	return 0;
}

int BEE::Instance::set_alarm(size_t alarm, Uint32 elapsed_ticks) {
	alarm_end[alarm] = elapsed_ticks + game->get_ticks();
	return 0;
}

int BEE::Instance::set_object(BEE::Object* new_object) {
	object->remove_instance(id);
	object = new_object;
	object->add_instance(id, this);
	return 0;
}
int BEE::Instance::set_sprite(BEE::Sprite* new_sprite) {
	sprite = new_sprite;
	return 0;
}
int BEE::Instance::add_physbody() {
	game->get_current_room()->add_physbody(this, body);
	return 0;
}

BEE::SIDP BEE::Instance::get_data(const std::string& field) const {
	return object->get_data(id, field);
}
int BEE::Instance::set_data(const std::string& field, SIDP data) {
	return object->set_data(id, field, data);
}

btVector3 BEE::Instance::get_position() const {
	return body->get_position();
}
double BEE::Instance::get_x() const {
	return get_position().x();
}
double BEE::Instance::get_y() const {
	return get_position().y();
}
double BEE::Instance::get_z() const {
	return get_position().z();
}
double BEE::Instance::get_corner_x() const {
	return get_x() - get_width()/2.0;
}
double BEE::Instance::get_corner_y() const {
	return get_y() - get_height()/2.0;
}
btVector3 BEE::Instance::get_start() const {
	return pos_start;
}
double BEE::Instance::get_xstart() const {
	return pos_start.x();
}
double BEE::Instance::get_ystart() const {
	return pos_start.y();
}
double BEE::Instance::get_zstart() const {
	return pos_start.z();
}
BEE::Object* BEE::Instance::get_object() const {
	return object;
}
BEE::Sprite* BEE::Instance::get_sprite() const {
	if (sprite == nullptr) {
		if (object == nullptr) {
			return nullptr;
		}
		return object->get_sprite();
	}
	return sprite;
}
BEE::PhysicsBody* BEE::Instance::get_physbody() const {
	return body;
}
bool BEE::Instance::get_is_solid() const {
	return is_solid;
}
double BEE::Instance::get_mass() const {
	return get_physbody()->get_mass();
}

int BEE::Instance::get_width() const {
	if (object->get_mask() == nullptr) {
		return 0;
	}
	return object->get_mask()->get_subimage_width();
}
int BEE::Instance::get_height() const {
	if (object->get_mask() == nullptr) {
		return 0;
	}
	return object->get_mask()->get_height();
}
SDL_Rect BEE::Instance::get_aabb() const {
	return {(int)get_x(), (int)get_y(), get_width(), get_height()};
}

int BEE::Instance::set_position(btVector3 p) {
	btTransform t;
	t.setIdentity();
	t.setOrigin(p/body->get_scale());

	body->get_body()->setCenterOfMassTransform(t);

	return 0;
}
int BEE::Instance::set_position(double new_x, double new_y, double new_z) {
	return set_position(btVector3(new_x, new_y, new_z));
}
int BEE::Instance::set_to_start() {
	return set_position(pos_start);
}
int BEE::Instance::set_mass(double new_mass) {
	btVector3 pos = get_position(); // Store the position since setting the mass to 0.0 resets it
	get_physbody()->set_mass(new_mass);
	set_position(pos);
	return 0;
}
int BEE::Instance::move(btVector3 new_impulse) {
	body->get_body()->activate();
	body->get_body()->applyCentralImpulse(new_impulse / body->get_scale());
	return 0;
}
int BEE::Instance::move(double new_magnitude, btVector3 new_direction) {
	if (new_magnitude < 0.0) {
		new_direction *= -1.0;
		new_magnitude = fabs(new_magnitude);
	}
	return move(new_magnitude*new_direction);
}
int BEE::Instance::move(double new_magnitude, double new_direction) {
	new_direction = absolute_angle(new_direction);
	return move(new_magnitude, btVector3(cos(degtorad(new_direction)), -sin(degtorad(new_direction)), 0.0));
}
int BEE::Instance::move_to(double new_magnitude, double other_x, double other_y, double other_z) {
	if (distance(get_x(), get_y(), get_z(), other_x, other_y, other_z) < new_magnitude) {
		return 1;
	}
	move(new_magnitude, direction_of(get_x(), get_y(), get_z(), other_x, other_y, other_z));
	return 0;
}
int BEE::Instance::move_to(double new_magnitude, double other_x, double other_y) {
	return move_to(new_magnitude, other_x, other_y, 0.0);
}
int BEE::Instance::move_away(double new_magnitude, double other_x, double other_y, double other_z) {
	move(new_magnitude, direction_of(other_x, other_y, other_z, get_x(), get_y(), get_z()));
	return 0;
}
int BEE::Instance::move_away(double new_magnitude, double other_x, double other_y) {
	return move_away(new_magnitude, other_x, other_y, 0.0);
}
int BEE::Instance::set_friction(double new_friction) {
	body->get_body()->setFriction(new_friction);
	return 0;
}
int BEE::Instance::set_gravity(btVector3 new_gravity) {
	body->get_body()->setGravity(new_gravity);
	return 0;
}
int BEE::Instance::set_gravity(double new_gx, double new_gy, double new_gz) {
	return set_gravity(btVector3(new_gx, new_gy, new_gz));
}
int BEE::Instance::move_outside(btVector3 dir) {
	/*double dist = distance(l.x1, l.y1, l.x2, l.y2);
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
	}*/

	return 0;
}
int BEE::Instance::set_is_solid(bool new_is_solid) {
	is_solid = new_is_solid;

	if (new_is_solid) {
		body->get_body()->setCollisionFlags(body->get_body()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	} else {
		body->get_body()->setCollisionFlags(body->get_body()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	return 0;
}
int BEE::Instance::set_velocity(btVector3 new_velocity) {
	body->get_body()->setLinearVelocity(new_velocity);
	return 0;
}
int BEE::Instance::set_velocity(double new_magnitude, double new_direction) {
	if (new_magnitude < 0.0) {
		new_direction -= 180.0;
		new_magnitude = fabs(new_magnitude);
	}
	new_direction = absolute_angle(new_direction);
	return set_velocity(btVector3(new_magnitude*cos(degtorad(new_direction)), new_magnitude*-sin(degtorad(new_direction)), 0.0) / body->get_scale());
}
int BEE::Instance::limit_velocity(double new_limit) {
	btVector3 v = get_velocity();
	double speed_sqr = dist_sqr(v.x(), v.y(), v.z(), 0.0, 0.0, 0.0);
	if (speed_sqr > sqr(new_limit)) {
		set_velocity(new_limit * v.normalize());
		return 1;
	}
	return 0;
}

double BEE::Instance::get_speed() const {
	btVector3 v = get_velocity();
	return distance(v.x(), v.y(), v.z());
}
btVector3 BEE::Instance::get_velocity() const {
	return body->get_body()->getLinearVelocity();
}
btVector3 BEE::Instance::get_velocity_ang() const {
	return body->get_body()->getAngularVelocity();
}
double BEE::Instance::get_friction() const {
	return body->get_body()->getFriction();
}
btVector3 BEE::Instance::get_gravity() const {
	return body->get_body()->getGravity();
}

bool BEE::Instance::is_place_free(int new_x, int new_y) const {
	SDL_Rect mask = get_aabb();
	mask.x = new_x;
	mask.y = new_y;

	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second == this) {
			continue;
		}

		SDL_Rect other = i.second->get_aabb();

		if (i.second->object->get_is_solid()) {
			if (check_collision(mask, other)) {
				if (object->check_collision_list(*this, *i.second)) {
					if (i.second->object->check_collision_list(*i.second, *this)) {
						return false;
					}
				}
			}
		}
	}

	return true;
}
bool BEE::Instance::is_place_empty(int new_x, int new_y) const {
	SDL_Rect mask = get_aabb();
	mask.x = new_x;
	mask.y = new_y;

	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second == this) {
			continue;
		}

		SDL_Rect other = i.second->get_aabb();

		if (check_collision(mask, other)) {
			return false;
		}
	}

	return true;
}
bool BEE::Instance::is_place_meeting(int new_x, int new_y, Object* other) const {
	SDL_Rect mask = get_aabb();
	mask.x = new_x;
	mask.y = new_y;

	for (auto& i : other->get_instances()) {
		if (i.second == this) {
			continue;
		}

		SDL_Rect other = i.second->get_aabb();

		if (check_collision(mask, other)) {
			return true;
		}
	}

	return false;
}
bool BEE::Instance::is_place_meeting(int new_x, int new_y, int other_id) const {
	return is_place_meeting(new_x, new_y, game->get_object(other_id));
}
bool BEE::Instance::is_place_meeting(int new_x, int new_y, Object* other, std::function<void(Instance*,Instance*)> func) {
	SDL_Rect mask = get_aabb();
	mask.x = new_x;
	mask.y = new_y;

	bool r = false;

	for (auto& i : other->get_instances()) {
		if (i.second == this) {
			continue;
		}

		SDL_Rect other = i.second->get_aabb();

		if (check_collision(mask, other)) {
			r = true;
			func(this, i.second);
		}
	}

	return r;
}
bool BEE::Instance::is_move_free(double magnitude, double direction) {
	double dx = cos(degtorad(direction)) * magnitude;
	double dy = -sin(degtorad(direction)) * magnitude;
	return is_place_free(get_x()+dx, get_y()+dy);
}
bool BEE::Instance::is_snapped(int hsnap, int vsnap) const {
	if (((int)get_x() % hsnap == 0)&&((int)get_y() % vsnap == 0)) {
		return true;
	}
	return false;
}

std::pair<int,int> BEE::Instance::get_snapped(int hsnap, int vsnap) const {
	if (hsnap < 1) {
		hsnap = 1;
	}
	if (vsnap < 1) {
		vsnap = 1;
	}

	int xsnap = (int)get_x();
	int ysnap = (int)get_y();

	int dx = xsnap % hsnap;
	int dy = ysnap % vsnap;

	if (get_x() < 0) {
		xsnap -= hsnap;
	}
	if (get_y() < 0) {
		ysnap -= hsnap;
	}
	xsnap -= dx;
	ysnap -= dy;

	return std::make_pair(xsnap, ysnap);
}
std::pair<int,int> BEE::Instance::get_snapped() const {
	if (get_sprite() == nullptr) {
		return std::make_pair((int)get_x(), (int)get_y());
	}
	return get_snapped(get_sprite()->get_width(), get_sprite()->get_height());
}
int BEE::Instance::move_random(int hsnap, int vsnap) {
	double rx = random(game->get_current_room()->get_width());
	double ry = random(game->get_current_room()->get_height());

	set_position(rx, ry, get_z());

	move_snap(hsnap, vsnap);
	return 0;
}
int BEE::Instance::move_snap(int hsnap, int vsnap) {
	double sx, sy;
	std::tie(sx, sy) = get_snapped(hsnap, vsnap);

	set_position(sx, sy, get_z());

	return 0;
}
int BEE::Instance::move_snap() {
	if (get_sprite() == nullptr) {
		return 0;
	}
	return move_snap(get_sprite()->get_width(), get_sprite()->get_height());
}
int BEE::Instance::move_wrap(bool is_horizontal, bool is_vertical, int margin) {
	int w = game->get_current_room()->get_width();
	int h = game->get_current_room()->get_height();

	double nx = get_x();
	double ny = get_y();
	if (is_horizontal) {
		if (nx < -margin){
			nx = w+margin-nx;
		} else if (nx > w+margin) {
			nx -= w+margin;
		}
	}
	if (is_vertical) {
		if (ny < -margin) {
			ny = h+margin-ny;
		} else if (ny > h+margin) {
			ny -= h+margin;
		}
	}

	set_position(nx, ny, get_z());

	return 0;
}

double BEE::Instance::get_distance(int dx, int dy, int dz) const {
	return distance(get_x(), get_y(), get_z(), dx, dy, dz);
}
double BEE::Instance::get_distance(Instance* other) const {
	return distance(get_x(), get_y(), get_z(), other->get_x(), other->get_y(), other->get_z());
}
double BEE::Instance::get_distance(Object* other) const {
	double shortest_distance = 0.0, current_distance = 0.0;
	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second->object->get_id() == other->get_id()) {
			current_distance = distance(get_x(), get_y(), i.second->get_x(), i.second->get_y());
			if (current_distance < shortest_distance) {
				shortest_distance = current_distance;
			}
		}
	}
	return shortest_distance;
}
double BEE::Instance::get_direction_of(int dx, int dy) const {
	return direction_of(get_x(), get_y(), dx, dy);
}
double BEE::Instance::get_direction_of(Instance* other) const {
	return direction_of(get_x(), get_y(), other->get_x(), other->get_y());
}
double BEE::Instance::get_direction_of(Object* other) const {
	double shortest_distance = 0.0, current_distance = 0.0;
	Instance* closest_instance = nullptr;
	for (auto& i : game->get_current_room()->get_instances()) {
		if (i.second->object->get_id() == other->get_id()) {
			current_distance = distance(get_x(), get_y(), i.second->get_x(), i.second->get_y());
			if (current_distance < shortest_distance) {
				shortest_distance = current_distance;
				closest_instance = i.second;
			}
		}
	}

	if (closest_instance != nullptr) {
		return direction_of(get_x(), get_y(), closest_instance->get_x(), closest_instance->get_y());
	}
	return 0.0;
}
int BEE::Instance::get_relation(Instance* other) const {
	if ((other->get_y() < get_y())&&(abs(other->get_x() - get_x()) < other->get_width()/2 + get_width()/2)) { // Top block
		return 1;
	} else if ((other->get_x() > get_x())&&(abs(other->get_y() - get_y()) < other->get_height()/2 + get_height()/2)) { // Right block
		return 2;
	} else if ((other->get_y() > get_y())&&(abs(other->get_x() - get_x()) < other->get_width()/2 + get_width()/2)) { // Bottom block
		return 3;
	} else if ((other->get_x() < get_x())&&(abs(other->get_y() - get_y()) < other->get_height()/2 + get_height()/2)) { // Left block
		return 4;
	}
	return 0;
}

int BEE::Instance::path_start(Path* new_path, double new_path_speed, bee::E_PATH_END new_end_action, bool absolute) {
	path = new_path;
	path_speed = new_path_speed;
	path_end_action = new_end_action;
	path_current_node = -1;

	path_previous_mass = get_physbody()->get_mass();
	set_mass(0.0);

	if (absolute) {
		path_pos_start = btVector3(
			std::get<0>(path->get_coordinate_list().front()),
			std::get<1>(path->get_coordinate_list().front()),
			std::get<2>(path->get_coordinate_list().front())
		);
	} else {
		path_pos_start = btVector3(
			get_x(),
			get_y(),
			get_z()
		);
	}

	return 0;
}
int BEE::Instance::path_end() {
	path = nullptr;
	path_speed = 0.0;
	path_end_action = bee::E_PATH_END::STOP;
	path_pos_start = btVector3(0.0, 0.0, 0.0);
	path_current_node = 0;

	set_mass(path_previous_mass);
	path_previous_mass = 0.0;

	return 0;
}
int BEE::Instance::path_reset() {
	bool a = false;
	if (
		(path_pos_start.x() == std::get<0>(path->get_coordinate_list().front()))
		&&(path_pos_start.y() == std::get<1>(path->get_coordinate_list().front()))
		&&(path_pos_start.z() == std::get<2>(path->get_coordinate_list().front()))
	) {
			a = true;
	}

	return path_start(path, path_speed, path_end_action, a);
}
int BEE::Instance::path_update_node() {
	if (has_path()) {
		if (path_speed >= 0) {
			if (path_current_node+1 < (int) path->get_coordinate_list().size()) {
				bee_path_coord c = path->get_coordinate_list().at(path_current_node+1);
				if (
					distance(
						get_x(), get_y(), get_z(),
						path_pos_start.x()+std::get<0>(c), path_pos_start.y()+std::get<1>(c), path_pos_start.z()+std::get<2>(c)
					) < std::get<3>(c)*path_speed
				) {
					path_current_node++;
				}
			}
		} else {
			bee_path_coord c = path->get_coordinate_list().at(path_current_node);
			if (
				distance(
					get_x(), get_y(), get_z(),
					path_pos_start.x()+std::get<0>(c), path_pos_start.y()+std::get<1>(c), path_pos_start.z()+std::get<2>(c)
				) < std::get<3>(c)*-path_speed
			) {
				path_current_node--;
			}
		}

		return 0;
	}
	return 1;
}
int BEE::Instance::set_path_drawn(bool new_path_is_drawn) {
	path_is_drawn = new_path_is_drawn;
	return 0;
}
int BEE::Instance::set_path_pausable(bool new_path_is_pausable) {
	path_is_pausable = new_path_is_pausable;
	return 0;
}
int BEE::Instance::handle_path_end() {
	if (has_path()) {
		switch (path_end_action) {
			case bee::E_PATH_END::STOP: { // Stop path
				path_end();
				break;
			}
			case bee::E_PATH_END::RESTART: { // Continue from start
				path_current_node = -1;
				set_position(path_pos_start);
				pos_previous = path_pos_start;
				break;
			}
			case bee::E_PATH_END::CONTINUE: { // Continue from current position
				path_current_node = -1;
				path_pos_start = btVector3(
					get_x(),
					get_y(),
					get_z()
				);
				break;
			}
			case bee::E_PATH_END::REVERSE: { // Reverse direction
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
bool BEE::Instance::has_path() {
	if (path == nullptr) {
		return false;
	}
	return true;
}
bool BEE::Instance::get_path_drawn() {
	return path_is_drawn;
}
int BEE::Instance::get_path_speed() {
	return path_speed;
}
int BEE::Instance::get_path_node() {
	return path_current_node;
}
std::vector<bee_path_coord> BEE::Instance::get_path_coords() {
	if (has_path()) {
		return path->get_coordinate_list();
	}
	return std::vector<bee_path_coord>();
}
bool BEE::Instance::get_path_pausable() {
	return path_is_pausable;
}

int BEE::Instance::draw(int w, int h, double angle, RGBA color, SDL_RendererFlip flip) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return get_sprite()->draw(get_corner_x()-xo, get_corner_y()-yo, subimage_time, w, h, angle, color, flip);
}
int BEE::Instance::draw(int w, int h, double angle, bee::E_RGB color, SDL_RendererFlip flip) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	return draw(w, h, angle, game->get_enum_color(color), flip);
}
int BEE::Instance::draw() {
	if (get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return get_sprite()->draw(get_corner_x()-xo, get_corner_y()-yo, subimage_time);
}
int BEE::Instance::draw(int w, int h) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return get_sprite()->draw(get_corner_x()-xo, get_corner_y()-yo, subimage_time, w, h);
}
int BEE::Instance::draw(double angle) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return get_sprite()->draw(get_corner_x()-xo, get_corner_y()-yo, subimage_time, angle);
}
int BEE::Instance::draw(RGBA color) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return get_sprite()->draw(get_corner_x()-xo, get_corner_y()-yo, subimage_time, color);
}
int BEE::Instance::draw(bee::E_RGB color) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	return draw(game->get_enum_color(color));
}
int BEE::Instance::draw(SDL_RendererFlip flip) {
	if (get_sprite() == nullptr) {
		return 1;
	}
	int xo=0, yo=0;
	if (get_sprite() != nullptr) {
		std::tie(xo, yo) = object->get_mask_offset();
	}
	return get_sprite()->draw(get_corner_x()-xo, get_corner_y()-yo, subimage_time, flip);
}

int BEE::Instance::draw_path() {
	if (path != nullptr) {
		return path->draw(path_pos_start.x(), path_pos_start.y(), path_pos_start.z());
	}
	return 0;
}

#endif // _BEE_CORE_INSTANCE
