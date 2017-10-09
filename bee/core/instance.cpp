/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_INSTANCE
#define BEE_CORE_INSTANCE 1

#include "instance.hpp"

#include "../engine.hpp"

#include "../util/collision.hpp"
#include "../util/real.hpp"
#include "../util/string.hpp"
#include "../util/debug.hpp"
#include "../util/template/string.hpp"

#include "../messenger/messenger.hpp"

#include "resources.hpp"
#include "sidp.hpp"
#include "rooms.hpp"
#include "serialdata.hpp"

#include "../render/drawing.hpp"
#include "../render/viewdata.hpp"

#include "../physics/body.hpp"
#include "../physics/world.hpp"

#include "../resource/sprite.hpp"
#include "../resource/path.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

namespace bee {
	Instance::Instance() :
		pos_start(),

		object(nullptr),
		sprite(nullptr),

		body(nullptr),
		is_solid(false),
		computation_type(E_COMPUTATION::STATIC),
		is_persistent(false),

		path(nullptr),
		path_speed(0.0),
		path_end_action(E_PATH_END::STOP),
		path_current_node(0),
		path_is_drawn(false),
		path_is_pausable(false),
		path_previous_mass(0.0),

		data(),

		id(-1),
		subimage_time(0),
		alarm_end(),
		depth(0),

		pos_previous(),
		path_pos_start()
	{
		for (size_t i=0; i<BEE_ALARM_COUNT; i++) {
			alarm_end[i] = 0xffffffff;
		}
	}
	Instance::Instance(int new_id, Object* new_object, double new_x, double new_y, double new_z) :
		Instance()
	{
		init(new_id, new_object, new_x, new_y, new_z);
	}
	Instance::Instance(const Instance& other) :
		Instance(other.id, other.object, other.get_x(), other.get_y(), other.get_z())
	{
		this->data = other.data;
	}
	Instance::~Instance() {
		delete body;
		data.clear();
	}
	int Instance::init(int new_id, Object* new_object, double new_x, double new_y, double new_z) {
		id = new_id;
		object = new_object;
		subimage_time = get_ticks();
		depth = object->get_depth();

		if (body == nullptr) {
			PhysicsWorld* w = get_current_room()->get_phys_world();
			body = new PhysicsBody(w, this, E_PHYS_SHAPE::NONE, 0.0, new_x, new_y, new_z, nullptr);
		} else {
			set_position(new_x, new_y, new_z);
		}

		pos_start = btVector3(btScalar(new_x), btScalar(new_y), btScalar(new_z));
		pos_previous = pos_start;
		path_pos_start = btVector3(0.0, 0.0, 0.0);
		path_previous_mass = 0.0;

		data.clear();
		data["object"] = object->get_name();

		is_solid = true;
		if (!object->get_is_solid()) {
			body->get_body()->setCollisionFlags(body->get_body()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
			is_solid = false;
		}

		set_computation_type(computation_type);
		is_persistent = object->get_is_persistent();

		for (size_t i=0; i<BEE_ALARM_COUNT; i++) {
			alarm_end[i] = 0xffffffff;
		}

		return 0;
	}
	int Instance::print() {
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
		messenger::send({"engine", "instance"}, E_MESSAGE::INFO, s.str());

		return 0;
	}

	bool Instance::operator<(const Instance& rhs) const {
		if (depth == rhs.depth) {
			return (id < rhs.id);
		}
		return (depth > rhs.depth);
	}
	Instance& Instance::operator=(const Instance& rhs) {
		if (this != &rhs) {
			btVector3 pos = rhs.get_position();
			this->init(rhs.id, rhs.object, pos.x(), pos.y(), pos.z());
			this->data = rhs.data;
		}
		return *this;
	}

	std::string Instance::serialize(bool should_pretty_print) const {
		std::map<std::string,SIDP> instance_info;
		instance_info["id"] = id;
		instance_info["object"] = object->get_name();
		instance_info["sprite"] = "";
		if (get_sprite() != nullptr) {
			instance_info["sprite"] = get_sprite()->get_name();
		}
		instance_info["subimage_time"] = static_cast<int>(subimage_time);

		std::string b = body->serialize(should_pretty_print);
		if (should_pretty_print) {
			b = debug_indent(b, 1);
		}
		instance_info["body"] = b;

		instance_info["is_solid"] = is_solid;
		instance_info["depth"] = depth;
		instance_info["pos_start"].vector(new std::vector<SIDP>({pos_start.x(), pos_start.y(), pos_start.z()}));
		instance_info["pos_previous"].vector(new std::vector<SIDP>({pos_previous.x(), pos_previous.y(), pos_previous.z()}));

		instance_info["path"] = "";
		if (path != nullptr) {
			instance_info["path"] = path->get_name();
		}
		instance_info["path_speed"] = path_speed;
		instance_info["path_end_action"] = static_cast<int>(path_end_action);
		instance_info["path_current_node"] = path_current_node;
		instance_info["path_is_drawn"] = path_is_drawn;
		instance_info["path_is_pausable"] = path_is_pausable;
		instance_info["path_previous_mass"] = path_previous_mass;
		instance_info["path_pos_start"].vector(new std::vector<SIDP>({path_pos_start.x(), path_pos_start.y(), path_pos_start.z()}));

		return map_serialize(instance_info, should_pretty_print);
	}
	std::string Instance::serialize() const {
		return serialize(false);
	}
	int Instance::deserialize(std::map<SIDP,SIDP>& m, Object* new_object) {
		id = SIDP_i(m["id"]);
		if (new_object != nullptr) {
			object = new_object;
		} else {
			object = get_object_by_name(SIDP_s(m["object"]));
		}
		sprite = get_sprite_by_name(SIDP_s(m["sprite"]));

		subimage_time = SIDP_i(m["subimage_time"]);
		body->deserialize(SIDP_m(m["body"]), this);
		is_solid = SIDP_i(m["is_solid"]);
		depth = SIDP_i(m["depth"]);
		pos_start = btVector3(
			btScalar(SIDP_cd(m["pos_start"], 0)),
			btScalar(SIDP_cd(m["pos_start"], 1)),
			btScalar(SIDP_cd(m["pos_start"], 2))
		);
		pos_previous = btVector3(
			btScalar(SIDP_cd(m["pos_previous"], 0)),
			btScalar(SIDP_cd(m["pos_previous"], 1)),
			btScalar(SIDP_cd(m["pos_previous"], 2))
		);

		path = get_path_by_name(SIDP_s(m["path"]));
		path_speed = SIDP_d(m["path_speed"]);
		path_end_action = static_cast<E_PATH_END>(SIDP_i(m["path_end_action"]));
		path_current_node = SIDP_i(m["path_current_node"]);
		path_is_drawn = SIDP_i(m["path_is_drawn"]);
		path_is_pausable = SIDP_i(m["path_is_pausable"]);
		path_previous_mass = SIDP_d(m["path_previous_mass"]);
		path_pos_start = btVector3(
			btScalar(SIDP_cd(m["path_pos_start"], 0)),
			btScalar(SIDP_cd(m["path_pos_start"], 1)),
			btScalar(SIDP_cd(m["path_pos_start"], 2))
		);

		return 0;
	}
	int Instance::deserialize(const std::string& instance_info, Object* new_object) {
		std::map<SIDP,SIDP> m;
		if (map_deserialize(instance_info, &m)) {
			messenger::send({"engine", "instance"}, E_MESSAGE::WARNING, "Failed to deserialize instance");
			return 1;
		}

		return deserialize(m, new_object);
	}
	int Instance::deserialize(const std::string& instance_info) {
		return deserialize(instance_info, nullptr);
	}

	std::vector<Uint8> Instance::serialize_net() {
		SerialData sd (256);

		std::string sprite_name = get_sprite()->get_name();
		sd.store_string(sprite_name);
		int s = subimage_time;
		sd.store_int(s);

		std::vector<Uint8> body_data = body->serialize_net();
		sd.store_serial_v(body_data);

		std::vector<double> ppos = {pos_previous.x(), pos_previous.y(), pos_previous.z()};
		sd.store_vector(ppos);

		return sd.get();
	}
	int Instance::deserialize_net(std::vector<Uint8> d) {
		if (d.empty()) {
			return 1;
		}

		SerialData sd (d);

		std::string sprite_name;
		sd.store_string(sprite_name);
		sprite = get_sprite_by_name(sprite_name);
		int s;
		sd.store_int(s);
		subimage_time = s;

		std::vector<Uint8> body_data;
		sd.store_serial_v(body_data);
		body->deserialize_net(body_data);

		std::vector<double> ppos;
		sd.store_vector(ppos);
		pos_previous = {
			static_cast<float>(ppos[0]),
			static_cast<float>(ppos[1]),
			static_cast<float>(ppos[2])
		};

		return 0;
	}

	int Instance::remove() {
		object->destroy(this);
		get_current_room()->remove_instance(id);
		return 0;
	}

	int Instance::set_alarm(size_t alarm, Uint32 elapsed_ticks) {
		alarm_end[alarm] = elapsed_ticks + get_ticks();
		return 0;
	}

	int Instance::set_object(Object* new_object) {
		object->remove_instance(id);

		object = new_object;
		object->add_instance(id, this);
		data["object"] = object->get_name();

		return 0;
	}
	int Instance::set_sprite(Sprite* new_sprite) {
		sprite = new_sprite;
		return 0;
	}
	int Instance::add_physbody() {
		get_current_room()->add_physbody(this, body);
		return 0;
	}
	int Instance::set_computation_type(E_COMPUTATION new_computation_type) {
		computation_type = new_computation_type;

		switch (computation_type) {
			case E_COMPUTATION::NOTHING:
			case E_COMPUTATION::STATIC:
			case E_COMPUTATION::SEMISTATIC: {
				if (get_physbody()->get_mass() != 0.0) { // If a body already has 0 mass, setting it to 0 will segfault
					get_physbody()->set_mass(0.0);
				}
				break;
			}
			case E_COMPUTATION::SEMIPLAYER:
			case E_COMPUTATION::PLAYER: {
				get_physbody()->get_body()->forceActivationState(DISABLE_DEACTIVATION);
				break;
			}
			case E_COMPUTATION::DYNAMIC:
			default: {}
		}

		return 0;
	}
	int Instance::set_is_persistent(bool new_is_persistent) {
		is_persistent = new_is_persistent;
		return 0;
	}

	/*
	* Instance::get_data() - Return a reference to the data map
	*/
	std::map<std::string,SIDP>& Instance::get_data() {
		return data;
	}
	/*
	* Instance::get_data() - Return the requested data field from the data map
	* @field: the name of the field to fetch
	* @default_value: the value to return if the field doesn't exist
	* @should_output: whether a warning should be output if the field doesn't exist
	*/
	const SIDP& Instance::get_data(const std::string& field, const SIDP& default_value, bool should_output) const {
		if (data.find(field) == data.end()) { // If the data field doesn't exist, output a warning and return the default value
			if (should_output) {
				messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to get the data field \"" + field + "\" from the instance of object \"" + get_object()->get_name() + "\", returning SIDP(0)");
			}
			return default_value;
		}

		return data.at(field); // Return the data field value on success
	}
	/*
	* Instance::get_data() - Return the requested data field from the data map
	* ! When the function is called without a default value, simply call it with a default value of 0 and with warning output enabled
	* @field: the name of the field to fetch
	*/
	const SIDP& Instance::get_data(const std::string& field) const {
		return get_data(field, 0, true);
	}
	/*
	* Instance::set_data() - Replace the data map
	* @new_data: the new data map to use
	*/
	int Instance::set_data(const std::map<std::string,SIDP>& new_data) {
		data = new_data;
		return 0;
	}
	/*
	* Instance::set_data() - Set the requested data field
	* @field: the name of the field to set
	* @value: the value to set the field to
	*/
	int Instance::set_data(const std::string& field, const SIDP& value) {
		data[field] = value;
		return 0;
	}

	btVector3 Instance::get_position() const {
		return body->get_position();
	}
	double Instance::get_x() const {
		return get_position().x();
	}
	double Instance::get_y() const {
		return get_position().y();
	}
	double Instance::get_z() const {
		return get_position().z();
	}
	double Instance::get_corner_x() const {
		return get_x() - get_width()/2.0;
	}
	double Instance::get_corner_y() const {
		return get_y() - get_height()/2.0;
	}
	btVector3 Instance::get_start() const {
		return pos_start;
	}
	double Instance::get_xstart() const {
		return pos_start.x();
	}
	double Instance::get_ystart() const {
		return pos_start.y();
	}
	double Instance::get_zstart() const {
		return pos_start.z();
	}
	Object* Instance::get_object() const {
		return object;
	}
	Sprite* Instance::get_sprite() const {
		if (sprite == nullptr) {
			if (object == nullptr) {
				return nullptr;
			}
			return object->get_sprite();
		}
		return sprite;
	}
	PhysicsBody* Instance::get_physbody() const {
		return body;
	}
	bool Instance::get_is_solid() const {
		return is_solid;
	}
	double Instance::get_mass() const {
		return get_physbody()->get_mass();
	}
	E_COMPUTATION Instance::get_computation_type() const {
		return computation_type;
	}
	bool Instance::get_is_persistent() const {
		return is_persistent;
	}

	int Instance::get_width() const {
		if (object->get_mask() == nullptr) {
			return 0;
		}
		return object->get_mask()->get_subimage_width();
	}
	int Instance::get_height() const {
		if (object->get_mask() == nullptr) {
			return 0;
		}
		return object->get_mask()->get_height();
	}
	SDL_Rect Instance::get_aabb() const {
		return {static_cast<int>(get_corner_x()), static_cast<int>(get_corner_y()), get_width(), get_height()};
	}

	int Instance::set_position(btVector3 p) {
		btTransform t;
		t.setIdentity();
		t.setOrigin(p / btScalar(body->get_scale()));

		body->get_body()->setCenterOfMassTransform(t);

		return 0;
	}
	int Instance::set_position(double new_x, double new_y, double new_z) {
		return set_position(btVector3(
			btScalar(new_x),
			btScalar(new_y),
			btScalar(new_z)
		));
	}
	int Instance::set_to_start() {
		return set_position(pos_start);
	}
	int Instance::set_corner_x(double new_x) {
		return set_position(new_x + get_width()/2.0, get_y(), get_z());
	}
	int Instance::set_corner_y(double new_y) {
		return set_position(get_x(), new_y + get_height()/2.0, get_z());
	}
	int Instance::set_mass(double new_mass) {
		btVector3 pos = get_position(); // Store the position since setting the mass to 0.0 resets it
		get_physbody()->set_mass(new_mass);
		set_position(pos);
		return 0;
	}
	int Instance::move(btVector3 new_impulse) {
		body->get_body()->activate();
		body->get_body()->applyCentralImpulse(new_impulse / btScalar(body->get_scale()));
		return 0;
	}
	int Instance::move(double new_magnitude, btVector3 new_direction) {
		if (new_magnitude < 0.0) {
			new_direction *= -1.0;
			new_magnitude = fabs(new_magnitude);
		}
		return move(btScalar(new_magnitude)*new_direction);
	}
	int Instance::move(double new_magnitude, double new_direction) {
		new_direction = absolute_angle(new_direction);
		return move(new_magnitude, btVector3(
			btScalar(cos(degtorad(new_direction))),
			btScalar(-sin(degtorad(new_direction))),
			btScalar(0.0)
		));
	}
	int Instance::move_to(double new_magnitude, double other_x, double other_y, double other_z) {
		if (distance(get_x(), get_y(), get_z(), other_x, other_y, other_z) < new_magnitude) {
			return 1;
		}
		move(new_magnitude, direction_of(get_x(), get_y(), get_z(), other_x, other_y, other_z));
		return 0;
	}
	int Instance::move_to(double new_magnitude, double other_x, double other_y) {
		return move_to(new_magnitude, other_x, other_y, 0.0);
	}
	int Instance::move_away(double new_magnitude, double other_x, double other_y, double other_z) {
		move(new_magnitude, direction_of(other_x, other_y, other_z, get_x(), get_y(), get_z()));
		return 0;
	}
	int Instance::move_away(double new_magnitude, double other_x, double other_y) {
		return move_away(new_magnitude, other_x, other_y, 0.0);
	}
	int Instance::set_friction(double new_friction) {
		body->get_body()->setFriction(btScalar(new_friction));
		return 0;
	}
	int Instance::set_gravity(btVector3 new_gravity) {
		body->get_body()->setGravity(new_gravity);
		return 0;
	}
	int Instance::set_gravity(double new_gx, double new_gy, double new_gz) {
		return set_gravity(btVector3(
			btScalar(new_gx),
			btScalar(new_gy),
			btScalar(new_gz)
		));
	}
	int Instance::move_outside(btVector3 dir) {
		/*double dist = distance(l.x1, l.y1, l.x2, l.y2);
		double dir = direction_of(l.x1, l.y1, l.x2, l.y2);
		x = l.x1;
		y = l.y1;

		int max_attempts = 10;
		double delta = 1.0/((double)max_attempts);
		int attempts = 0;

		mask.x = x; mask.y = y;
		while ((check_collision_polygon(mask, m))&&(attempts++ < max_attempts)) {
			x += cos(degtorad(dir)) * delta*dist * get_delta();
			y += -sin(degtorad(dir)) * delta*dist * get_delta();
			mask.x = x;
			mask.y = y;
		}

		if (check_collision_polygon(mask, m)) {
			x -= cos(degtorad(dir)) * delta*dist * get_delta();
			y -= -sin(degtorad(dir)) * delta*dist * get_delta();
			mask.x = x;
			mask.y = y;
		}*/

		return 1;
	}
	int Instance::set_is_solid(bool new_is_solid) {
		is_solid = new_is_solid;

		if (new_is_solid) {
			body->get_body()->setCollisionFlags(body->get_body()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		} else {
			body->get_body()->setCollisionFlags(body->get_body()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		return 0;
	}
	int Instance::set_velocity(btVector3 new_velocity) {
		body->get_body()->setLinearVelocity(new_velocity);
		return 0;
	}
	int Instance::set_velocity(double new_magnitude, double new_direction) {
		if (new_magnitude < 0.0) {
			new_direction -= 180.0;
			new_magnitude = fabs(new_magnitude);
		}
		new_direction = absolute_angle(new_direction);
		return set_velocity(btVector3(
			btScalar(new_magnitude*cos(degtorad(new_direction))),
			btScalar(new_magnitude*-sin(degtorad(new_direction))),
			btScalar(0.0)
		) / btScalar(body->get_scale()));
	}
	int Instance::add_velocity(btVector3 new_velocity) {
		return set_velocity(get_velocity() + new_velocity);
	}
	int Instance::add_velocity(double new_magnitude, double new_direction) {
		if (new_magnitude < 0.0) {
			new_direction -= 180.0;
			new_magnitude = fabs(new_magnitude);
		}
		new_direction = absolute_angle(new_direction);
		return set_velocity(get_velocity() + btVector3(
			btScalar(new_magnitude*cos(degtorad(new_direction))),
			btScalar(new_magnitude*-sin(degtorad(new_direction))),
			btScalar(0.0)
		) / btScalar(body->get_scale()));
	}
	int Instance::limit_velocity(double new_limit) {
		btVector3 v = get_velocity();
		double speed_sqr = v.length2();
		if (speed_sqr > sqr(new_limit)) {
			set_velocity(btScalar(new_limit) * v.normalize());
			return 1;
		}
		return 0;
	}
	int Instance::limit_velocity_x(double x_limit) {
		btVector3 v = get_velocity();
		if (abs(v.x()) > x_limit) {
			v.setX(btScalar(x_limit) * sign(v.x()));
			set_velocity(v);
			return 1;
		}
		return 0;
	}
	int Instance::limit_velocity_y(double y_limit) {
		btVector3 v = get_velocity();
		if (abs(v.y()) > y_limit) {
			v.setY(btScalar(y_limit) * sign(v.y()));
			set_velocity(v);
			return 1;
		}
		return 0;
	}
	int Instance::limit_velocity_z(double z_limit) {
		btVector3 v = get_velocity();
		if (abs(v.z()) > z_limit) {
			v.setZ(btScalar(z_limit) * sign(v.z()));
			set_velocity(v);
			return 1;
		}
		return 0;
	}

	double Instance::get_speed() const {
		return get_velocity().length();
	}
	btVector3 Instance::get_velocity() const {
		return body->get_body()->getLinearVelocity();
	}
	btVector3 Instance::get_velocity_ang() const {
		return body->get_body()->getAngularVelocity();
	}
	double Instance::get_friction() const {
		return body->get_body()->getFriction();
	}
	btVector3 Instance::get_gravity() const {
		return body->get_body()->getGravity();
	}

	bool Instance::is_place_free(int new_x, int new_y) const {
		SDL_Rect mask = get_aabb();
		mask.x = new_x;
		mask.y = new_y;

		for (auto& i : get_current_room()->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other = i.second->get_aabb();

			if (i.second->object->get_is_solid()) {
				if (check_collision(mask, other)) {
					if (object->check_collision_filter(this, i.second)) {
						if (i.second->object->check_collision_filter(i.second, this)) {
							return false;
						}
					}
				}
			}
		}

		return true;
	}
	bool Instance::is_place_empty(int new_x, int new_y) const {
		SDL_Rect mask = get_aabb();
		mask.x = new_x;
		mask.y = new_y;

		for (auto& i : get_current_room()->get_instances()) {
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
	bool Instance::is_place_meeting(int new_x, int new_y, Object* other) const {
		SDL_Rect mask = get_aabb();
		mask.x = new_x;
		mask.y = new_y;

		for (auto& i : other->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other_rect = i.second->get_aabb();

			if (check_collision(mask, other_rect)) {
				return true;
			}
		}

		return false;
	}
	bool Instance::is_place_meeting(int new_x, int new_y, int other_id) const {
		return is_place_meeting(new_x, new_y, Object::get(other_id));
	}
	bool Instance::is_place_meeting(int new_x, int new_y, Object* other, std::function<void(Instance*,Instance*)> func) {
		SDL_Rect mask = get_aabb();
		mask.x = new_x;
		mask.y = new_y;

		bool r = false;

		for (auto& i : other->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other_rect = i.second->get_aabb();

			if (check_collision(mask, other_rect)) {
				r = true;
				func(this, i.second);
			}
		}

		return r;
	}
	bool Instance::is_move_free(double magnitude, double direction) {
		double dx = cos(degtorad(direction)) * magnitude;
		double dy = -sin(degtorad(direction)) * magnitude;
		return is_place_free(
			static_cast<int>(get_x()+dx),
			static_cast<int>(get_y()+dy)
		);
	}
	bool Instance::is_snapped(int hsnap, int vsnap) const {
		if ((static_cast<int>(get_x()) % hsnap == 0)&&(static_cast<int>(get_y()) % vsnap == 0)) {
			return true;
		}
		return false;
	}

	std::pair<int,int> Instance::get_snapped(int hsnap, int vsnap) const {
		if (hsnap < 1) {
			hsnap = 1;
		}
		if (vsnap < 1) {
			vsnap = 1;
		}

		int xsnap = static_cast<int>(get_x());
		int ysnap = static_cast<int>(get_y());

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
	std::pair<int,int> Instance::get_snapped() const {
		if (get_sprite() == nullptr) {
			return std::make_pair(static_cast<int>(get_x()), static_cast<int>(get_y()));
		}
		return get_snapped(get_sprite()->get_width(), get_sprite()->get_height());
	}
	int Instance::move_random(int hsnap, int vsnap) {
		double rx = random(get_current_room()->get_width());
		double ry = random(get_current_room()->get_height());

		set_position(rx, ry, get_z());

		move_snap(hsnap, vsnap);
		return 0;
	}
	int Instance::move_snap(int hsnap, int vsnap) {
		double sx, sy;
		std::tie(sx, sy) = get_snapped(hsnap, vsnap);

		set_position(sx, sy, get_z());

		return 0;
	}
	int Instance::move_snap() {
		if (get_sprite() == nullptr) {
			return 0;
		}
		return move_snap(get_sprite()->get_width(), get_sprite()->get_height());
	}
	int Instance::move_wrap(bool is_horizontal, bool is_vertical, int margin) {
		int w = get_current_room()->get_width();
		int h = get_current_room()->get_height();

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

	double Instance::get_distance(int dx, int dy, int dz) const {
		return distance(get_x(), get_y(), get_z(), dx, dy, dz);
	}
	double Instance::get_distance(Instance* other) const {
		return get_position().distance(other->get_position());
	}
	double Instance::get_distance(Object* other) const {
		double shortest_distance = 0.0, current_distance = 0.0;
		for (auto& i : get_current_room()->get_instances()) {
			if (i.second->object->get_id() == other->get_id()) {
				current_distance = get_position().distance(i.second->get_position());;
				if (current_distance < shortest_distance) {
					shortest_distance = current_distance;
				}
			}
		}
		return shortest_distance;
	}
	double Instance::get_direction_of(int dx, int dy) const {
		return direction_of(get_x(), get_y(), dx, dy);
	}
	double Instance::get_direction_of(Instance* other) const {
		return direction_of(get_x(), get_y(), other->get_x(), other->get_y());
	}
	double Instance::get_direction_of(Object* other) const {
		double shortest_distance = 0.0, current_distance = 0.0;
		Instance* closest_instance = nullptr;
		for (auto& i : get_current_room()->get_instances()) {
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
	int Instance::get_relation(Instance* other) const {
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

	int Instance::path_start(Path* new_path, double new_path_speed, E_PATH_END new_end_action, bool absolute) {
		path = new_path;
		path_speed = new_path_speed;
		path_end_action = new_end_action;
		path_current_node = -1;

		path_previous_mass = get_physbody()->get_mass();
		set_mass(0.0);

		if (absolute) {
			path_pos_start = btVector3(
				btScalar(std::get<0>(path->get_coordinate_list().front())),
				btScalar(std::get<1>(path->get_coordinate_list().front())),
				btScalar(std::get<2>(path->get_coordinate_list().front()))
			);
		} else {
			path_pos_start = btVector3(
				btScalar(get_x()),
				btScalar(get_y()),
				btScalar(get_z())
			);
		}

		return 0;
	}
	int Instance::path_end() {
		path = nullptr;
		path_speed = 0.0;
		path_end_action = E_PATH_END::STOP;
		path_pos_start = btVector3(0.0, 0.0, 0.0);
		path_current_node = 0;

		set_mass(path_previous_mass);
		path_previous_mass = 0.0;

		return 0;
	}
	int Instance::path_reset() {
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
	int Instance::path_update_node() {
		if (has_path()) {
			if (path_speed >= 0) {
				if (path_current_node+1 < static_cast<int>(path->get_coordinate_list().size())) {
					path_coord_t c = path->get_coordinate_list().at(path_current_node+1);
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
				path_coord_t c = path->get_coordinate_list().at(path_current_node);
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
	int Instance::set_path_drawn(bool new_path_is_drawn) {
		path_is_drawn = new_path_is_drawn;
		return 0;
	}
	int Instance::set_path_pausable(bool new_path_is_pausable) {
		path_is_pausable = new_path_is_pausable;
		return 0;
	}
	int Instance::handle_path_end() {
		if (has_path()) {
			switch (path_end_action) {
				case E_PATH_END::STOP: { // Stop path
					path_end();
					break;
				}
				case E_PATH_END::RESTART: { // Continue from start
					path_current_node = -1;
					set_position(path_pos_start);
					pos_previous = path_pos_start;
					break;
				}
				case E_PATH_END::CONTINUE: { // Continue from current position
					path_current_node = -1;
					path_pos_start = btVector3(
						btScalar(get_x()),
						btScalar(get_y()),
						btScalar(get_z())
					);
					break;
				}
				case E_PATH_END::REVERSE: { // Reverse direction
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
	bool Instance::has_path() {
		if (path == nullptr) {
			return false;
		}
		return true;
	}
	bool Instance::get_path_drawn() {
		return path_is_drawn;
	}
	double Instance::get_path_speed() {
		return path_speed;
	}
	int Instance::get_path_node() {
		return path_current_node;
	}
	std::vector<path_coord_t> Instance::get_path_coords() {
		if (has_path()) {
			return path->get_coordinate_list();
		}
		return std::vector<path_coord_t>();
	}
	bool Instance::get_path_pausable() {
		return path_is_pausable;
	}

	int Instance::draw(int w, int h, double angle, RGBA color, SDL_RendererFlip flip) {
		if (get_sprite() == nullptr) {
			return 1;
		}
		int xo=0, yo=0;
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(static_cast<int>(get_corner_x())-xo, static_cast<int>(get_corner_y())-yo, subimage_time, w, h, angle, color, flip);
	}
	int Instance::draw(int w, int h, double angle, E_RGB color, SDL_RendererFlip flip) {
		if (get_sprite() == nullptr) {
			return 1;
		}
		return draw(w, h, angle, get_enum_color(color), flip);
	}
	int Instance::draw() {
		if (get_sprite() == nullptr) {
			return 1;
		}
		int xo=0, yo=0;
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(static_cast<int>(get_corner_x())-xo, static_cast<int>(get_corner_y())-yo, subimage_time);
	}
	int Instance::draw(int w, int h) {
		if (get_sprite() == nullptr) {
			return 1;
		}
		int xo=0, yo=0;
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(static_cast<int>(get_corner_x())-xo, static_cast<int>(get_corner_y())-yo, subimage_time, w, h);
	}
	int Instance::draw(double angle) {
		if (get_sprite() == nullptr) {
			return 1;
		}
		int xo=0, yo=0;
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(static_cast<int>(get_corner_x())-xo, static_cast<int>(get_corner_y())-yo, subimage_time, angle);
	}
	int Instance::draw(RGBA color) {
		if (get_sprite() == nullptr) {
			return 1;
		}
		int xo=0, yo=0;
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(static_cast<int>(get_corner_x())-xo, static_cast<int>(get_corner_y())-yo, subimage_time, color);
	}
	int Instance::draw(E_RGB color) {
		return draw(get_enum_color(color));
	}
	int Instance::draw(SDL_RendererFlip flip) {
		if (get_sprite() == nullptr) {
			return 1;
		}
		int xo=0, yo=0;
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(static_cast<int>(get_corner_x())-xo, static_cast<int>(get_corner_y())-yo, subimage_time, flip);
	}

	int Instance::draw_path() {
		if (path != nullptr) {
			return path->draw(path_pos_start.x(), path_pos_start.y(), path_pos_start.z());
		}
		return 0;
	}
}

#endif // BEE_CORE_INSTANCE
