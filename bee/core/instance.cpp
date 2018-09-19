/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
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

#include "rooms.hpp"

#include "../data/serialdata.hpp"

#include "../render/drawing.hpp"

#include "../physics/body.hpp"
#include "../physics/world.hpp"

#include "../resource/texture.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

namespace bee {
	/**
	* Default construct the Instance.
	*/
	Instance::Instance() :
		pos_start(),

		object(nullptr),
		sprite(nullptr),

		body(nullptr),
		computation_type(E_COMPUTATION::SEMISTATIC),
		is_persistent(false),

		data({
			{"__object", Variant(std::string())},
			{"__alarms", Variant(std::map<Variant,Variant>())}
		}),

		id(-1),
		subimage_time(0),
		depth(0),

		pos_previous()
	{}
	/**
	* Construct the Instance with the given object and position.
	* @param _id the Instance ID
	* @param _object the type of Object of the Instance
	* @param x the x-coordinate of the Instance
	* @param y the y-coordinate of the Instance
	* @param z the z-coordinate of the Instance
	*/
	Instance::Instance(size_t _id, Object* _object, btVector3 pos) :
		Instance()
	{
		init(_id, _object, pos);
	}
	/**
	* Copy an Instance along with its internal data.
	* @param other the Instance to copy
	*/
	Instance::Instance(const Instance& other) :
		Instance(other.id, other.object, other.get_pos())
	{
		this->data = other.data;
	}
	Instance::~Instance() {
		body.reset();
		data.clear();
	}
	/**
	* Initialize the Instance with the given object and position.
	* @param _id the Instance ID
	* @param _object the type of Object of the Instance
	* @param x the x-coordinate of the Instance
	* @param y the y-coordinate of the Instance
	* @param z the z-coordinate of the Instance
	*/
	void Instance::init(size_t _id, Object* _object, btVector3 pos) {
		id = _id;
		object = _object;
		subimage_time = get_ticks();
		depth = object->get_depth();

		if (body == nullptr) {
			std::shared_ptr<PhysicsWorld> w = get_current_room()->get_phys_world();
			body.reset(new PhysicsBody(w, this, E_PHYS_SHAPE::NONE, 0.0, pos, nullptr));
		} else {
			set_pos(pos);
		}

		pos_start = pos;
		pos_previous = pos_start;

		data.clear();
		set_data("__object", object->get_name());

		set_computation_type(computation_type);
		is_persistent = object->get_is_persistent();

		set_data("__alarms", std::map<Variant,Variant>());
	}

	/**
	* Compare Instances first by depths, then by IDs.
	* @param rhs the right-hand side of the operator expression
	*
	* @retval true this is less than rhs
	* @retval false rhs is less than this
	*/
	bool Instance::operator<(const Instance& rhs) const {
		if (depth == rhs.depth) {
			return (id < rhs.id);
		}
		return (depth > rhs.depth);
	}
	/**
	* Copy the position and data from one Instance to another.
	* @param rhs the Instance to copy the data from
	*
	* @returns a reference to the modified Instance
	*/
	Instance& Instance::operator=(const Instance& rhs) {
		if (this != &rhs) {
			this->init(rhs.id, rhs.object, rhs.get_pos());
			this->data = rhs.data;
		}
		return *this;
	}

	/**
	* @returns a map of all the information required to restore an Instance
	*/
	std::map<Variant,Variant> Instance::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = static_cast<long>(id);
		info["object"] = object->get_name();
		info["sprite"] = "";
		if (get_sprite() != nullptr) {
			info["sprite"] = get_sprite()->get_name();
		}
		info["subimage_time"] = static_cast<long>(subimage_time);

		info["body"] = body->serialize();

		info["depth"] = depth;
		info["pos_start"] = {Variant(pos_start.x()), Variant(pos_start.y()), Variant(pos_start.z())};
		info["pos_previous"] = {Variant(pos_previous.x()), Variant(pos_previous.y()), Variant(pos_previous.z())};

		info["computation_type"] = static_cast<long>(computation_type);
		info["is_persistent"] = is_persistent;

		std::map<Variant,Variant> _data;
		for (auto& d : data) {
			_data.emplace(Variant(d.first), d.second);
		}
		info["data"] = _data;

		return info;
	}
	/**
	* Restore an Instance from its serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to deserialize the PhysicsBody
	*/
	int Instance::deserialize(std::map<Variant,Variant>& m) {
		id = m["id"].i;
		object = Object::get_by_name(m["object"].s);
		sprite = Texture::get_by_name(m["sprite"].s);

		subimage_time = m["subimage_time"].i;
		if (body->deserialize(m["body"].m, this) != 0) {
			return 1;
		}
		depth = m["depth"].i;
		pos_start = btVector3(
			btScalar(m["pos_start"].v[0].f),
			btScalar(m["pos_start"].v[1].f),
			btScalar(m["pos_start"].v[2].f)
		);
		pos_previous = btVector3(
			btScalar(m["pos_previous"].v[0].f),
			btScalar(m["pos_previous"].v[1].f),
			btScalar(m["pos_previous"].v[2].f)
		);

		computation_type = static_cast<E_COMPUTATION>(m["computation_type"].i);
		is_persistent = m["is_persistent"].i;

		data.clear();
		for (auto& d : m["data"].m) {
			data.emplace(d.first.s, d.second);
		}

		return 0;
	}
	/**
	* Print all relevant information about the Instance.
	*/
	int Instance::print() {
		Variant m (serialize());
		messenger::send({"engine", "instance"}, E_MESSAGE::INFO, "Instance " + m.to_str(true));
		return 0;
	}

	/**
	* @returns a map of the minimal data for synchronizing Instance positions over the network
	*/
	std::vector<Uint8> Instance::serialize_net() {
		SerialData sd (256);

		std::string sprite_name = get_sprite()->get_name();
		sd.store_string(sprite_name);
		long s = subimage_time;
		sd.store_long(s);

		std::vector<Uint8> body_data = body->serialize_net();
		sd.store_serial_v(body_data);

		std::vector<double> ppos = {pos_previous.x(), pos_previous.y(), pos_previous.z()};
		sd.store_vector(ppos);

		return sd.get();
	}
	/**
	* Restore the sprite and position from the serialized data.
	* @param d the serialized data
	*
	* @retval 0 success
	* @retval 1 failed to deserialize physics body
	*/
	int Instance::deserialize_net(std::vector<Uint8> d) {
		if (d.empty()) {
			return 1;
		}

		SerialData sd (d);

		std::string sprite_name;
		sd.store_string(sprite_name);
		sprite = Texture::get_by_name(sprite_name);
		long s;
		sd.store_long(s);
		subimage_time = s;

		std::vector<Uint8> body_data;
		sd.store_serial_v(body_data);
		if (body->deserialize_net(body_data) != 0) {
			return 1;
		}

		std::vector<double> ppos;
		sd.store_vector(ppos);
		pos_previous = {
			static_cast<float>(ppos[0]),
			static_cast<float>(ppos[1]),
			static_cast<float>(ppos[2])
		};

		return 0;
	}

	/**
	* Set the alarm with the given name.
	* @param name the alarm name
	* @param elapsed_ticks how far in the future to set the alarm
	*/
	void Instance::set_alarm(const std::string& name, int elapsed_ticks) {
		std::map<Variant,Variant>& alarms = data["__alarms"].m;
		if (elapsed_ticks >= 0) {
			alarms[name.c_str()] = static_cast<int>(elapsed_ticks + get_ticks());
		} else {
			alarms.erase(name.c_str());
		}
	}

	/**
	* Change the object type.
	* @param _object the new object type
	*/
	void Instance::set_object(Object* _object) {
		object->remove_instance(id);

		object = _object;
		object->add_instance(id, this);
		set_data("__object", object->get_name());
	}
	/**
	* Change the sprite.
	* @param _sprite the new sprite
	*/
	void Instance::set_sprite(Texture* _sprite) {
		sprite = _sprite;
	}
	/**
	* Change the computation type and collision mask.
	* @param _computation_type the new computation type
	*/
	void Instance::set_computation_type(E_COMPUTATION _computation_type) {
		computation_type = _computation_type;

		switch (computation_type) {
			case E_COMPUTATION::STATIC:
			case E_COMPUTATION::SEMISTATIC: {
				if (get_physbody()->get_mass() != 0.0) { // If a body already has 0 mass, setting it to 0 will segfault
					get_physbody()->set_mass(0.0);
				}
				set_data("__collision_mask", util::bitmask<E_COMPUTATION>({E_COMPUTATION::SEMIPLAYER, E_COMPUTATION::PLAYER, E_COMPUTATION::DYNAMIC}));
				break;
			}
			case E_COMPUTATION::SEMIPLAYER:
			case E_COMPUTATION::PLAYER: {
				get_physbody()->get_body()->forceActivationState(DISABLE_DEACTIVATION);
				set_data("__collision_mask", util::bitmask<E_COMPUTATION>({E_COMPUTATION::STATIC, E_COMPUTATION::SEMISTATIC, E_COMPUTATION::SEMIPLAYER, E_COMPUTATION::PLAYER}));
				break;
			}
			case E_COMPUTATION::DYNAMIC: {
				set_data("__collision_mask", util::bitmask<E_COMPUTATION>({E_COMPUTATION::STATIC, E_COMPUTATION::SEMISTATIC}));
				break;
			}
			case E_COMPUTATION::NOTHING:
			default: {
				if (get_physbody()->get_mass() != 0.0) {
					get_physbody()->set_mass(0.0);
				}
				set_data("__collision_mask", 0);
				break;
			}
		}
	}
	/**
	* Change whether the Instance will persist between rooms.
	* @param _is_persistent whether the Instance will remain after a room change
	*/
	void Instance::set_is_persistent(bool _is_persistent) {
		is_persistent = _is_persistent;
	}

	/**
	* @returns a reference to the data map
	*/
	std::map<std::string,Variant>& Instance::get_data() {
		return data;
	}
	/**
	* @param field the name of the field to fetch
	* @param default_value the value to return if the field doesn't exist
	* @param should_output whether a warning should be output if the field doesn't exist
	*
	* @returns the requested data field from the data map
	*/
	const Variant& Instance::get_data(const std::string& field, const Variant& default_value, bool should_output) const {
		std::map<std::string,Variant>::const_iterator d (data.find(field));
		if (d == data.end()) { // If the data field doesn't exist, output a warning and return the default value
			if (should_output) {
				messenger::send({"engine", "instance"}, E_MESSAGE::WARNING, "Failed to get the data field \"" + field + "\" from the instance of object \"" + get_object()->get_name() + "\", returning the provided default: " + default_value.to_str());
			}
			return default_value;
		}

		return d->second;
	}
	/**
	* @note If the function is called without a default value, then let it be an empty Variant.
	* @note This function cannot return a const reference because the default value would fall out of scope.
	* @param field the name of the field to fetch
	*
	* @returns the requested data field from the data map
	*/
	Variant Instance::get_data(const std::string& field) const {
		return get_data(field, Variant(), true);
	}
	/**
	* Replace the data map.
	* @param _data the new data map to use
	*/
	void Instance::set_data(const std::map<std::string,Variant>& _data) {
		data = _data;
	}
	/**
	* Set the requested data field.
	* @param field the name of the field to set
	* @param value the value to set the field to
	*/
	void Instance::set_data(const std::string& field, Variant value) {
		data[field] = value;
	}

	/**
	* @returns the 3D position of the Instance's PhysicsBody
	*/
	btVector3 Instance::get_pos() const {
		return body->get_pos();
	}
	double Instance::get_x() const {
		return get_pos().x();
	}
	double Instance::get_y() const {
		return get_pos().y();
	}
	double Instance::get_z() const {
		return get_pos().z();
	}
	/**
	* @returns the AABB for the sprite mask
	*/
	SDL_Rect Instance::get_aabb() const {
		std::pair<int,int> s (0, 0);
		if (object->get_sprite() != nullptr) {
			s = object->get_sprite()->get_size();
		}

		return {static_cast<int>(get_x() - s.first/2), static_cast<int>(get_y() - s.second/2), s.first, s.second};
	}
	/**
	* @returns the top-left corner of the 2D AABB
	*/
	std::pair<double,double> Instance::get_corner() const {
		SDL_Rect aabb (get_aabb());
		return std::make_pair(aabb.x, aabb.y);
	}
	/**
	* @returns the starting position
	*/
	btVector3 Instance::get_start() const {
		return pos_start;
	}

	Object* Instance::get_object() const {
		return object;
	}
	Texture* Instance::get_sprite() const {
		if (sprite == nullptr) {
			if (object == nullptr) {
				return nullptr;
			}
			return object->get_sprite();
		}
		return sprite;
	}
	std::shared_ptr<PhysicsBody> Instance::get_physbody() const {
		return body;
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

	/**
	* Set the center position of the attached PhysicsBody.
	* @param p the new position
	*/
	void Instance::set_pos(btVector3 p) {
		if (computation_type == E_COMPUTATION::STATIC) {
			//messenger::send({"engine", "instance"}, E_MESSAGE::WARNING, "Failed to set the position of a static instance.");
			return;
		}

		btTransform t;
		t.setIdentity();
		t.setOrigin(p / btScalar(body->get_scale()));

		body->get_body()->setCenterOfMassTransform(t);
	}
	void Instance::set_pos(double x, double y, double z) {
		set_pos(btVector3(
			btScalar(x),
			btScalar(y),
			btScalar(z)
		));
	}
	void Instance::set_to_start() {
		set_pos(pos_start);
	}
	void Instance::set_corner(double x, double y) {
		set_pos(x + get_aabb().w/2.0, y + get_aabb().h/2.0, get_z());
	}
	void Instance::set_mass(double mass) {
		btVector3 pos = get_pos(); // Store the position since setting the mass to 0.0 resets it
		get_physbody()->set_mass(mass);
		set_pos(pos);
	}
	/**
	* Move with the given impulse.
	* @param impulse the impulse to apply to physics body
	*/
	void Instance::move(btVector3 impulse) {
		body->get_body()->activate();
		body->get_body()->applyCentralImpulse(impulse / btScalar(body->get_scale()));
	}
	/**
	* Move in the given direction with the given magnitude.
	* @param magnitude the magnitude of the impulse
	* @param direction the direction of the impulse
	*/
	void Instance::move(double magnitude, btVector3 direction) {
		move(btScalar(magnitude)*direction);
	}
	/**
	* Move in the 2D given direction with the given magnitude.
	* @param magnitude the magnitude of the impulse
	* @param direction the direction in the xy-plane
	*/
	void Instance::move(double magnitude, double direction) {
		direction = util::absolute_angle(direction);
		move(magnitude, btVector3(
			btScalar(cos(util::degtorad(direction))),
			btScalar(-sin(util::degtorad(direction))),
			btScalar(0.0)
		));
	}
	/**
	* Move towards the given point by the given magnitude.
	* @param magnitude the maximum distance to move
	* @param destination the point to move towards
	*/
	void Instance::move_to(double magnitude, btVector3 destination) {
		btScalar dist = get_distance(destination);
		if (dist < magnitude) {
			magnitude = dist;
		}
		move(magnitude, get_direction_of(destination));
	}
	/**
	* Move away from the given point by the given magnitude.
	* @param magnitude the distance to move
	* @param source the point to move away from
	*/
	void Instance::move_away(double magnitude, btVector3 source) {
		move(magnitude, get_direction_of(get_pos()));
	}
	/**
	* Set the friction of the attached PhysicsBody.
	* @param friction the friction to use
	*/
	void Instance::set_friction(double friction) {
		body->get_body()->setFriction(btScalar(friction));
	}
	/**
	* Set the gravity vector of the attached PhysicsBody.
	* @param gravity the gravity to use
	*/
	void Instance::set_gravity(btVector3 gravity) {
		body->get_body()->setGravity(gravity);
	}
	/**
	* Set the velocity of the attached PhysicsBody.
	* @param velocity the velocity to use
	*/
	void Instance::set_velocity(btVector3 velocity) {
		body->get_body()->setLinearVelocity(velocity);
	}
	/**
	* Set the velocity to the given magnitude and direction.
	* @param magnitude the magnitude to use
	* @param direction the 2D direction to use
	*/
	void Instance::set_velocity(double magnitude, double direction) {
		set_velocity(btVector3(
			btScalar(magnitude*cos(util::degtorad(direction))),
			btScalar(magnitude*-sin(util::degtorad(direction))),
			btScalar(0.0)
		) / btScalar(body->get_scale()));
	}
	/**
	* Add the given velocity to the velocity of the attached PhysicsBody.
	* @param velocity the velocity to add
	*/
	void Instance::add_velocity(btVector3 velocity) {
		set_velocity(get_velocity() + velocity);
	}
	/**
	* Add the velocity with the given magnitude and direction to the current velocity.
	* @param magnitude the magnitude to add
	* @param direction the 2D direction to add the magnitude in
	*/
	void Instance::add_velocity(double magnitude, double direction) {
		set_velocity(get_velocity() + btVector3(
			btScalar(magnitude*cos(util::degtorad(direction))),
			btScalar(magnitude*-sin(util::degtorad(direction))),
			btScalar(0.0)
		) / btScalar(body->get_scale()));
	}
	/**
	* Limit the velocity of the attached PhysicsBody along the axes.
	* @param limit the vector whose components specify the limit for each axis
	*
	* @returns a vector with each axis set to 1.0 if the limit was applied, and 0 otherwise
	*/
	btVector3 Instance::limit_velocity(btVector3 limit) {
		btVector3 l (0.0, 0.0, 0.0);
		btVector3 v (get_velocity());

		if ((limit.x() >= 0.0)&&(abs(v.x()) > limit.x())) {
			v.setX(limit.x() * util::sign(v.x()));
			l.setX(1.0);
		}
		if ((limit.y() >= 0.0)&&(abs(v.y()) > limit.y())) {
			v.setY(limit.y() * util::sign(v.y()));
			l.setY(1.0);
		}
		if ((limit.z() >= 0.0)&&(abs(v.z()) > limit.z())) {
			v.setZ(limit.z() * util::sign(v.z()));
			l.setZ(1.0);
		}

		set_velocity(v);

		return l;
	}
	/**
	* Limit the speed of the attached PhysicsBody.
	* @param limit the speed limit
	*
	* @retval 0 the limit was not applied
	* @retval 1 the limit was applied
	*/
	int Instance::limit_velocity(double limit) {
		btVector3 v = get_velocity();
		double speed_sqr = v.length2();
		if (speed_sqr > util::sqr(limit)) {
			set_velocity(btScalar(limit) * v.normalize());
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

	/**
	* Check whether a move to the given coordinates would be collision-free.
	* @note See is_place_empty() for a version which doesn't check collision filters.
	* @param (x, y) the 2D coordinates to check
	*
	* @retval true a collision will not occur
	* @retval false a collision will occur
	*/
	bool Instance::is_place_free(int x, int y) const {
		SDL_Rect mask = get_aabb();
		mask.x = x;
		mask.y = y;

		for (auto& i : get_current_room()->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other = i.second->get_aabb();

			if (util::check_collision(mask, other)) {
				if (object->check_collision_filter(this, i.second)) {
					if (i.second->object->check_collision_filter(i.second, this)) {
						return false;
					}
				}
			}
		}

		return true;
	}
	/**
	* Check whether a move to the given coordinates would overlap with another Instance.
	* @note See is_place_free() for a version which checks collision filters.
	* @param (x, y) the 2D coordinates to check
	*
	* @retval true an overlap will not occur
	* @retval false an overlap will occur
	*/
	bool Instance::is_place_empty(int x, int y) const {
		SDL_Rect mask = get_aabb();
		mask.x = x;
		mask.y = y;

		for (auto& i : get_current_room()->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other = i.second->get_aabb();

			if (util::check_collision(mask, other)) {
				return false;
			}
		}

		return true;
	}
	/**
	* Check whether a move to the given coordinates would overlap with a certain Object.
	* @param (x, y) the 2D coordinates to check
	* @param other_obj the object to check for
	*
	* @retval true an overlap will not occur
	* @retval false an overlap will occur
	*/
	bool Instance::is_place_meeting(int x, int y, Object* other_obj) const {
		SDL_Rect mask = get_aabb();
		mask.x = x;
		mask.y = y;

		for (auto& i : other_obj->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other_rect = i.second->get_aabb();

			if (util::check_collision(mask, other_rect)) {
				return true;
			}
		}

		return false;
	}
	/**
	* Run a callback if a move to the given coordinates would overlap with a certain Object.
	* @param (x, y) the 2D coordinates to check
	* @param other_obj the object to check for
	* @param func the callback to run for each overlapped Instance
	*
	* @retval true an overlap will not occur
	* @retval false an overlap will occur
	*/
	bool Instance::is_place_meeting(int x, int y, Object* other_obj, std::function<void(Instance*, Instance*)> func) {
		SDL_Rect mask = get_aabb();
		mask.x = x;
		mask.y = y;

		bool r = false;

		for (auto& i : other_obj->get_instances()) {
			if (i.second == this) {
				continue;
			}

			SDL_Rect other_rect = i.second->get_aabb();

			if (util::check_collision(mask, other_rect)) {
				r = true;
				func(this, i.second);
			}
		}

		return r;
	}
	/**
	* Check whether a move with the given magnitude and direction would be collision-free.
	* @param magnitude the magnitude to check
	* @param direction the 2D direction to check
	*
	* @retval true a collision will not occur
	* @retval false a collision will occur
	*/
	bool Instance::is_move_free(double magnitude, double direction) const {
		double dx = cos(util::degtorad(direction)) * magnitude;
		double dy = -sin(util::degtorad(direction)) * magnitude;
		return is_place_free(
			static_cast<int>(get_x()+dx),
			static_cast<int>(get_y()+dy)
		);
	}
	/**
	* Check whether the current position is aligned to the given grid size.
	* @param hsnap the horizontal grid snap
	* @param vsnap the vertical grid snap
	*
	* @retval true the position is aligned
	* @retval false the position is not aligned
	*/
	bool Instance::is_snapped(int hsnap, int vsnap) const {
		if ((static_cast<int>(get_x()) % hsnap == 0)&&(static_cast<int>(get_y()) % vsnap == 0)) {
			return true;
		}
		return false;
	}

	/**
	* @param hsnap the horizontal grid snap
	* @param vsnap the vertical grid snap
	*
	* @returns the 2D coordinates aligned to the given grid size
	*/
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
	/**
	* @returns the 2D coordinates aligned to a grid sized to the sprite dimensions
	*/
	std::pair<int,int> Instance::get_snapped() const {
		if (get_sprite() == nullptr) {
			return std::make_pair(static_cast<int>(get_x()), static_cast<int>(get_y()));
		}
		return get_snapped(get_sprite()->get_size().first, get_sprite()->get_size().second);
	}
	/**
	* Move to a random 2D position aligned to the given grid size.
	* @param hsnap the horizontal grid snap
	* @param vsnap the vertical grid snap
	*/
	void Instance::move_random(int hsnap, int vsnap) {
		double rx = util::random::get(get_current_room()->get_width());
		double ry = util::random::get(get_current_room()->get_height());

		set_pos(rx, ry, get_z());

		move_snap(hsnap, vsnap);
	}
	/**
	* Move the 2D position to align with the given grid size.
	* @param hsnap the horizontal grid snap
	* @param vsnap the vertical grid snap
	*/
	void Instance::move_snap(int hsnap, int vsnap) {
		double sx, sy;
		std::tie(sx, sy) = get_snapped(hsnap, vsnap);

		set_pos(sx, sy, get_z());
	}
	/**
	* Move the 2D position to align with the grid sized to the sprite dimensions.
	*/
	void Instance::move_snap() {
		if (get_sprite() == nullptr) {
			return;
		}

		move_snap(get_sprite()->get_size().first, get_sprite()->get_size().second);
	}
	/**
	* Wrap the 2D position around the screen with the given margin.
	* @param is_horizontal whether to wrap horizontally
	* @param is_vertical whether to wrap vertically
	* @param margin the pixel margin to wrap by
	*/
	void Instance::move_wrap(bool is_horizontal, bool is_vertical, int margin) {
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

		set_pos(nx, ny, get_z());
	}

	/**
	* @param pos the position vector to measure against
	*
	* @returns the distance to the given position vector
	*/
	double Instance::get_distance(btVector3 pos) const {
		return get_pos().distance(pos);
	}
	/**
	* @param other the Instance to measure against
	*
	* @returns the distance to the given Instance
	*/
	double Instance::get_distance(Instance* other) const {
		return get_distance(other->get_pos());
	}
	/**
	* @param other_obj the Object type to iterate over
	*
	* @returns the distance to the closest Instance of the given Object type
	*/
	double Instance::get_distance(Object* other_obj) const {
		double shortest_distance = 0.0, current_distance = 0.0;
		for (auto& i : other_obj->get_instances()) {
			current_distance = get_distance(i.second->get_pos());;
			if (current_distance < shortest_distance) {
				shortest_distance = current_distance;
			}
		}
		return shortest_distance;
	}
	/**
	* @param pos the position vector to measure against
	*
	* @returns a unit vector in the direction of the given position vector
	*/
	btVector3 Instance::get_direction_of(btVector3 pos) const {
		return util::direction_of(get_pos(), pos);
	}
	/**
	* @param other the Instance to measure against
	*
	* @returns a unit vector in the direction of the given Instance
	*/
	btVector3 Instance::get_direction_of(Instance* other) const {
		return get_direction_of(other->get_pos());
	}
	/**
	* @param other_obj the Object type to iterate over
	*
	* @returns a unit vector in the direction of the closest Instance of the given Object type
	*/
	btVector3 Instance::get_direction_of(Object* other_obj) const {
		double shortest_distance = 0.0, current_distance = 0.0;
		Instance* closest_instance = nullptr;
		for (auto& i : other_obj->get_instances()) {
			current_distance = get_distance(i.second->get_pos());
			if (current_distance < shortest_distance) {
				shortest_distance = current_distance;
				closest_instance = i.second;
			}
		}

		if (closest_instance != nullptr) {
			return get_direction_of(closest_instance->get_pos());
		}
		return btVector3(0.0, 0.0, 0.0);
	}
	/**
	* Determine the 2D relation with a given Instance.
	* @param other the Instance to measure against
	*
	* @returns the relation from the given Instance to this Instance
	*/
	E_RELATION Instance::get_relation(Instance* other) const {
		int w2 = get_aabb().w/2;
		int h2 = get_aabb().h/2;
		int ow2 = other->get_aabb().w/2;
		int oh2 = other->get_aabb().h/2;

		if ((other->get_y() < get_y())&&(abs(other->get_x() - get_x()) < ow2+w2)) {
			return E_RELATION::ABOVE;
		} else if ((other->get_x() > get_x())&&(abs(other->get_y() - get_y()) < oh2+h2)) {
			return E_RELATION::RIGHT;
		} else if ((other->get_y() > get_y())&&(abs(other->get_x() - get_x()) < ow2+w2)) {
			return E_RELATION::BELOW;
		} else if ((other->get_x() < get_x())&&(abs(other->get_y() - get_y()) < oh2+h2)) {
			return E_RELATION::LEFT;
		}
		return E_RELATION::NONE;
	}

	/**
	* Draw the sprite Texture with the given properties.
	* @see Texture::draw() for more information.
	* @param w the width to use
	* @param h the height to use
	* @param angle the angle to draw the sprite at
	* @param color the colorization to use
	*
	* @retval 0 success
	* @retval 1 failed to draw since the sprite was not set
	*/
	int Instance::draw(int w, int h, double angle, RGBA color) {
		if (get_sprite() == nullptr) {
			return 1;
		}

		int cx, cy, xo, yo;
		std::tie(cx, cy) = get_corner();
		std::tie(xo, yo) = object->get_mask_offset();

		return get_sprite()->draw(cx-xo, cy-yo, subimage_time, w, h, angle, color);
	}
	/**
	* Draw the sprite Texture with default properties.
	* @see Texture::draw() for more information.
	*
	* @returns whether the draw succeeded
	*/
	int Instance::draw() {
		return draw(-1, -1, 0.0, {255, 255, 255, 255});
	}
}

#endif // BEE_CORE_INSTANCE
