/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_BODY
#define BEE_PHYSICS_BODY 1

#include "body.hpp"

#include "physics.hpp"
#include "world.hpp"

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/template/string.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"
#include "../core/rooms.hpp"

#include "../data/variant.hpp"
#include "../data/serialdata.hpp"

#include "../resource/room.hpp"

namespace bee {
	/**
	* Construct the PhysicsBody.
	* @param _world the PhysicsWorld to attach to
	* @param _inst the Instance to attach to
	* @param _type the shape type to initialize
	* @param _mass the body mass to use
	* @param pos the position to start at
	* @param p the shape params to use
	*/
	PhysicsBody::PhysicsBody(std::shared_ptr<PhysicsWorld> _world, Instance* _inst, E_PHYS_SHAPE _type, double _mass, btVector3 pos, double* p) :
		type(_type),
		shape(nullptr),
		shape_param_amount(0),
		shape_params(nullptr),

		motion_state(nullptr),
		body(nullptr),

		attached_world(_world),
		attached_instance(_inst),
		constraints(),

		scale(1.0),
		mass(_mass),
		friction(0.5)
	{
		set_shape(_type, p);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(pos/btScalar(scale));
		motion_state = new btDefaultMotionState(transform);

		btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
		rb_info.m_friction = btScalar(friction);
		body = new btRigidBody(rb_info);

		body->setSleepingThresholds(body->getLinearSleepingThreshold()/btScalar(scale), body->getAngularSleepingThreshold());
	}
	/**
	* Construct the PhysicsBody from another one.
	* @param other the other body to construct from
	*/
	PhysicsBody::PhysicsBody(const PhysicsBody& other) :
		type(other.type),
		shape(nullptr),
		shape_param_amount(other.shape_param_amount),
		shape_params(nullptr),

		motion_state(nullptr),
		body(nullptr),

		attached_world(other.attached_world),
		attached_instance(other.attached_instance),
		constraints(),

		scale(other.scale),
		mass(other.mass),
		friction(other.friction)
	{
		*this = other;
	}
	/**
	* Free the body and shape data.
	*/
	PhysicsBody::~PhysicsBody() {
		delete motion_state;

		remove();
		remove_constraints();
		delete body;

		if (shape_params != nullptr) {
			delete[] shape_params;
			shape_params = nullptr;
		}

		delete shape;
	}

	PhysicsBody& PhysicsBody::operator=(const PhysicsBody& rhs) {
		if (this != &rhs) {
			this->type = rhs.type;
			if (this->shape != nullptr) {
				delete shape;
				this->shape = nullptr;
			}
			this->shape_param_amount = rhs.shape_param_amount;
			if (shape_params != nullptr) {
				delete[] shape_params;
				shape_params = nullptr;
			}

			double* p = new double[shape_param_amount];
			for (size_t i=0; i<shape_param_amount; ++i) {
				p[i] = rhs.shape_params[i];
			}
			this->set_shape(this->type, p);

			this->scale = rhs.scale;
			this->mass = rhs.mass;
			this->friction = rhs.friction;

			this->motion_state = new btDefaultMotionState(*rhs.motion_state);

			btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(this->mass), this->motion_state, this->shape, this->get_inertia());
			rb_info.m_friction = btScalar(this->friction);
			this->body = new btRigidBody(rb_info);

			this->body->setSleepingThresholds(this->body->getLinearSleepingThreshold()/btScalar(this->scale), this->body->getAngularSleepingThreshold());

			this->attached_world = rhs.attached_world;
			this->attached_instance = rhs.attached_instance;

			this->constraints.clear();
			for (auto& c : rhs.constraints) {
				this->add_constraint(std::get<0>(c), std::get<1>(c));
			}
		}
		return *this;
	}

	/**
	* @returns a map of all the information required to restore a PhysicsBody
	*/
	std::map<Variant,Variant> PhysicsBody::serialize() const {
		std::vector<Variant> sp;
		for (size_t i=0; i<shape_param_amount; ++i) {
			sp.push_back(Variant(shape_params[i]));
		}

		std::vector<Variant> cons;
		if (auto world = attached_world.lock()) {
			for (auto& c : constraints) {
				size_t constraint_param_amount = physics::get_constraint_param_amount(std::get<0>(c));
				std::vector<Variant> con;

				for (size_t i=0; i<constraint_param_amount; ++i) {
					con.push_back(Variant(std::get<1>(c)[i]));
				}

				cons.push_back(Variant(con));
			}
		}

		std::map<Variant,Variant> data;

		data["type"] = static_cast<int>(type);
		data["mass"] = mass;
		data["scale"] = scale;
		data["friction"] = friction;
		data["shape_params"] = sp;

		data["attached_instance"] = -1;
		if (attached_instance != nullptr) {
			data["attached_instance"] = static_cast<int>(attached_instance->id);
		}
		data["position"] = {Variant(get_pos().x()), Variant(get_pos().y()), Variant(get_pos().z())};
		data["rotation"] = {Variant(get_rotation_x()), Variant(get_rotation_y()), Variant(get_rotation_z())};

		data["gravity"] = {Variant(body->getGravity().x()), Variant(body->getGravity().y()), Variant(body->getGravity().z())};
		data["velocity"] = {Variant(body->getLinearVelocity().x()), Variant(body->getLinearVelocity().y()), Variant(body->getLinearVelocity().z())};
		data["velocity_ang"] = {Variant(body->getAngularVelocity().x()), Variant(body->getAngularVelocity().y()), Variant(body->getAngularVelocity().z())};

		data["collision_flags"] = body->getCollisionFlags();
		data["constraints"] = cons;

		return data;
	}
	/**
	* Restore a PhysicsBody from its serialized data.
	* @param m the map of data to use
	* @param inst the Instance to attach to
	*
	* @retval 0 success
	* @retval 1 failed to set the shape type
	*/
	int PhysicsBody::deserialize(std::map<Variant,Variant>& m, Instance* inst) {
		mass = m["mass"].d;
		scale = m["scale"].d;
		friction = m["friction"].d;

		E_PHYS_SHAPE previous_type = type;
		type = static_cast<E_PHYS_SHAPE>(m["type"].i);
		if (type != previous_type) {
			shape_param_amount = physics::get_shape_param_amount(type);
			if ((type == E_PHYS_SHAPE::MULTISPHERE)||(type == E_PHYS_SHAPE::CONVEX_HULL)) {
				shape_param_amount = physics::get_shape_param_amount(type, static_cast<int>(m["shape_params"].v[0].d));
			}

			if (shape_params != nullptr) {
				delete[] shape_params;
				shape_params = nullptr;
			}
			if (shape_param_amount > 0) {
				shape_params = new double[shape_param_amount];
				for (size_t i=0; i<shape_param_amount; ++i) {
					shape_params[i] = m["shape_params"].v[i].d;
				}
			}

			if (set_shape(type, shape_params)) {
				return 1;
			}
		}

		attached_instance = inst;
		if (attached_instance == nullptr) {
			auto instances = get_current_room()->get_instances();
			if (instances.find(m["attached_instance"].i) != instances.end()) {
				attached_instance = instances.at(m["attached_instance"].i);
			}
		}

		btVector3 position = btVector3(
			btScalar(m["position"].v[0].f),
			btScalar(m["position"].v[1].f),
			btScalar(m["position"].v[2].f)
		);
		btVector3 rotation = btVector3(
			btScalar(m["rotation"].v[0].f),
			btScalar(m["rotation"].v[1].f),
			btScalar(m["rotation"].v[2].f)
		);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(position/btScalar(scale));
		btQuaternion qt;
		qt.setEuler(rotation.y(), rotation.x(), rotation.z());
		transform.setRotation(qt);
		body->setCenterOfMassTransform(transform);

		btVector3 gravity = btVector3(
			btScalar(m["gravity"].v[0].f),
			btScalar(m["gravity"].v[1].f),
			btScalar(m["gravity"].v[2].f)
		);
		body->setGravity(gravity);

		btVector3 velocity = btVector3(
			btScalar(m["velocity"].v[0].f),
			btScalar(m["velocity"].v[1].f),
			btScalar(m["velocity"].v[2].f)
		);
		btVector3 velocity_ang = btVector3(
			btScalar(m["velocity_ang"].v[0].f),
			btScalar(m["velocity_ang"].v[1].f),
			btScalar(m["velocity_ang"].v[2].f)
		);
		body->setLinearVelocity(velocity);
		body->setAngularVelocity(velocity_ang);

		return 0;
	}

	std::vector<Uint8> PhysicsBody::serialize_net() {
		SerialData data (128);

		data.store_double(mass);
		data.store_double(friction);

		std::vector<double> pos = {get_pos().x(), get_pos().y(), get_pos().z()};
		std::vector<double> rot = {get_rotation_x(), get_rotation_y(), get_rotation_z()};
		data.store_vector(pos);
		data.store_vector(rot);

		std::vector<double> vel = {body->getLinearVelocity().x(), body->getLinearVelocity().y(), body->getLinearVelocity().z()};
		std::vector<double> vel_ang = {body->getAngularVelocity().x(), body->getAngularVelocity().y(), body->getAngularVelocity().z()};
		data.store_vector(vel);
		data.store_vector(vel_ang);

		return data.get();
	}
	int PhysicsBody::deserialize_net(std::vector<Uint8> d) {
		if (d.empty()) {
			return 1;
		}

		SerialData data (d);

		data.store_double(mass);
		data.store_double(friction);

		std::vector<double> pos;
		std::vector<double> rot;
		data.store_vector(pos);
		data.store_vector(rot);
		btVector3 position = {
			static_cast<float>(pos[0]),
			static_cast<float>(pos[1]),
			static_cast<float>(pos[2])
		};
		btVector3 rotation = {
			static_cast<float>(rot[0]),
			static_cast<float>(rot[1]),
			static_cast<float>(rot[2])
		};

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(position/btScalar(scale));
		btQuaternion qt;
		qt.setEuler(rotation.y(), rotation.x(), rotation.z());
		transform.setRotation(qt);
		body->setCenterOfMassTransform(transform);

		std::vector<double> vel;
		std::vector<double> vel_ang;
		data.store_vector(vel);
		data.store_vector(vel_ang);
		btVector3 velocity = {
			static_cast<float>(vel[0]),
			static_cast<float>(vel[1]),
			static_cast<float>(vel[2])
		};
		btVector3 velocity_ang = {
			static_cast<float>(vel_ang[0]),
			static_cast<float>(vel_ang[1]),
			static_cast<float>(vel_ang[2])
		};

		body->setLinearVelocity(velocity);
		body->setAngularVelocity(velocity_ang);

		return 0;
	}

	/**
	* Attach to the given PhysicsWorld and re-add any previous constraints from their parameters.
	* @param world the world to attach to
	*/
	void PhysicsBody::attach(std::shared_ptr<PhysicsWorld> world) {
		if (world == nullptr) {
			remove();
		}

		attached_world = world;
		if (constraints.size() > 0) {
			auto tmp_constraints = constraints;
			constraints.clear();
			for (auto& c : tmp_constraints) {
				btTypedConstraint* constraint = std::get<2>(c);
				if (constraint != nullptr) {
					add_constraint_external(std::get<0>(c), std::get<1>(c), constraint);
				} else {
					add_constraint(std::get<0>(c), std::get<1>(c));
				}
			}
		}

		scale = world->get_scale();
	}
	/**
	* Remove from the attached PhysicsWorld and delete the constructed constraints but not their parameters.
	*/
	void PhysicsBody::remove() {
		if (auto world = attached_world.lock()) {
			for (auto& c : constraints) {
				if (std::get<2>(c) != nullptr) {
					world->remove_constraint(std::get<2>(c));

					delete std::get<2>(c);
					std::get<2>(c) = nullptr;
				}
			}

			world->remove_body(body);
			world = nullptr;
		}
	}

	E_PHYS_SHAPE PhysicsBody::get_shape_type() const {
		return type;
	}
	double PhysicsBody::get_mass() const {
		return mass;
	}
	double PhysicsBody::get_scale() const {
		return scale;
	}
	/**
	* @returns the local inertia with regard to the body mass and scale
	*/
	btVector3 PhysicsBody::get_inertia() const {
		btVector3 local_intertia (0.0, 0.0, 0.0);
		if ((mass != 0.0)&&(shape != nullptr)) {
			shape->calculateLocalInertia(btScalar(mass), local_intertia);
		}
		return local_intertia*btScalar(scale);
	}
	btRigidBody* PhysicsBody::get_body() const {
		return body;
	}
	std::shared_ptr<PhysicsWorld> PhysicsBody::get_world() const {
		return attached_world.lock();
	}
	Instance* PhysicsBody::get_instance() const {
		return attached_instance;
	}
	const std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>>& PhysicsBody::get_constraints() const {
		return constraints;
	}

	btDefaultMotionState* PhysicsBody::get_motion() const {
		return motion_state;
	}
	btVector3 PhysicsBody::get_pos() const {
		return body->getCenterOfMassPosition()*btScalar(scale);
	}
	btQuaternion PhysicsBody::get_rotation() const {
		return body->getCenterOfMassTransform().getRotation();
	}
	double PhysicsBody::get_rotation_x() const {
		return 2.0*asin(get_rotation().x());
	}
	double PhysicsBody::get_rotation_y() const {
		return 2.0*asin(get_rotation().y());
	}
	double PhysicsBody::get_rotation_z() const {
		return 2.0*asin(get_rotation().z());
	}

	/**
	* Change the shape to the given type with the given parameters.
	* @see http://bulletphysics.com/Bullet/BulletFull/classbtCollisionShape.html for details.
	* @param _type the shape type
	* @param p the shape parameters
	*
	* @retval 0 success
	*/
	int PhysicsBody::set_shape(E_PHYS_SHAPE _type, double* p) {
		if (shape != nullptr) {
			delete shape;
			shape = nullptr;
		}
		if ((shape_params != nullptr)&&(shape_params != p)) {
			delete[] shape_params;
			shape_params = nullptr;
		}
		type = _type;
		shape_params = p;
		shape_param_amount = physics::get_shape_param_amount(type);

		if (auto world = attached_world.lock()) {
			scale = world->get_scale();
		}

		btScalar s = 2.0*scale;

		switch (type) {
			case E_PHYS_SHAPE::SPHERE: {
				/*
				* p[0]: the radius
				*/
				shape = new btSphereShape(btScalar(p[0]) / s);
				break;
			}
			case E_PHYS_SHAPE::BOX: {
				/*
				* p[0], p[1], p[2]: the width, height, and depth
				*/
				shape = new btBoxShape(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s);
				break;
			}
			case E_PHYS_SHAPE::CYLINDER: {
				/*
				* p[0], p[1]: the radius and height
				*/
				shape = new btCylinderShape(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[0])) / s);
				break;
			}
			case E_PHYS_SHAPE::CAPSULE: {
				/*
				* p[0], p[1]: the radius and height
				*/
				shape = new btCapsuleShape(btScalar(p[0]) / s, btScalar(p[1]) / s);
				break;
			}
			case E_PHYS_SHAPE::CONE: {
				/*
				* p[0], p[1]: the radius and height
				*/
				shape = new btConeShape(btScalar(p[0]) / s, btScalar(p[1]) / s);
				break;
			}
			case E_PHYS_SHAPE::MULTISPHERE: {
				/*
				* p[0]: the amount of spheres
				* p[1], p[2], p[3], ..., p[p[0]]: the x-, y-, and z-coordinates of each sphere's center
				* p[p[0]+1], p[p[0]+2], p[p[0]+3], ..., p[2*p[0]]: the x-, y-, and z-scaling for each axis of the appropriate sphere
				*/
				const size_t amount = static_cast<size_t>(p[0]);

				btVector3* pos = new btVector3[amount];
				for (size_t i=0; i<amount; i+=3) {
					pos[i] = btVector3(btScalar(p[i+2]), btScalar(p[i+3]), btScalar(p[i+4])) / s;
				}
				btScalar* radii = new btScalar[amount];
				for (size_t i=0; i<amount; ++i) {
					radii[i] = btScalar(p[i+amount+1]) / s;
				}

				shape_param_amount = physics::get_shape_param_amount(type, amount);
				shape = new btMultiSphereShape(pos, radii, amount);

				delete[] pos;
				delete[] radii;

				break;
			}
			case E_PHYS_SHAPE::CONVEX_HULL: {
				/*
				* p[0]: the amount of points
				* p[1], p[2], p[3], ..., p[p[0]]: the x-, y-, and z-coordinates of each point
				*/
				const size_t amount = static_cast<size_t>(p[0]);

				btConvexHullShape* tmp_shape = new btConvexHullShape();

				for (size_t i=0; i<amount; i+=3) {
					tmp_shape->addPoint(btVector3(btScalar(p[i+2]), btScalar(p[i+3]), btScalar(p[i+4])) / s);
				}

				shape_param_amount = physics::get_shape_param_amount(type, amount);
				shape = tmp_shape;

				break;
			}

			default:
				messenger::send({"engine", "physics"}, E_MESSAGE::ERROR, "Invalid shape type");
				[[fallthrough]];
			case E_PHYS_SHAPE::NONE:
				shape_param_amount = 0;
				shape = new btEmptyShape();
		}

		update_state();

		return 0;
	}
	void PhysicsBody::set_mass(double _mass) {
		mass = _mass;
		update_state();
	}
	void PhysicsBody::set_friction(double _friction) {
		friction = _friction;
		update_state();
	}

	/**
	* Add a constraint of the given type with the given parameters.
	* @param constraint_type the constraint type
	* @param p the constraint parameters
	*
	* @returns the initialized constraint
	*/
	btTypedConstraint* PhysicsBody::add_constraint(E_PHYS_CONSTRAINT constraint_type, double* p) {
		btTypedConstraint* constraint = nullptr;
		if (auto world = attached_world.lock()) {
			constraint = world->add_constraint(constraint_type, body, p);
		}

		constraints.emplace_back(constraint_type, p, constraint);

		return constraint;
	}
	/**
	* Add the given constraint and its parameters.
	* @param constraint_type the constraint type
	* @param p the constraint parameters
	* @param constraint the constraint itself
	*/
	void PhysicsBody::add_constraint_external(E_PHYS_CONSTRAINT constraint_type, double* p, btTypedConstraint* constraint) {
		if (auto world = attached_world.lock()) {
			world->add_constraint_external(constraint);
		}

		constraints.emplace_back(constraint_type, p, constraint);
	}
	/**
	* Remove all constraints from the body.
	*/
	void PhysicsBody::remove_constraints() {
		auto world = attached_world.lock();
		while (body->getNumConstraintRefs()) {
			btTypedConstraint* c = body->getConstraintRef(0);
			if (world != nullptr) {
				world->remove_constraint(c);
			}
			body->removeConstraintRef(c);
			delete c;
		}

		for (auto& c : constraints) {
			if (std::get<1>(c) != nullptr) {
				delete[] std::get<1>(c);
			}
			if (std::get<2>(c) != nullptr) {
				delete std::get<2>(c);
			}
		}
		constraints.clear();
	}

	/**
	* Update the body state within the simulation.
	* @note This should not be called very often, mainly just on state changes such as shape type, mass, or friction.
	*
	* @retval 0 success
	* @retval 1 failed since the body and its motion state are not loaded
	*/
	int PhysicsBody::update_state() {
		if ((body == nullptr)||(motion_state == nullptr)) {
			return 1;
		}

		std::weak_ptr<PhysicsWorld> tmp_world = attached_world;
		std::weak_ptr<PhysicsBody> self;
		if (auto world = tmp_world.lock()) {
			self = world->get_physbody(body);
		}
		remove();

		delete body;
		body = nullptr;

		btRigidBody::btRigidBodyConstructionInfo rb_info (btScalar(mass), motion_state, shape, get_inertia());
		rb_info.m_friction = btScalar(friction);

		body = new btRigidBody(rb_info);

		body->setSleepingThresholds(body->getLinearSleepingThreshold()/btScalar(scale), body->getAngularSleepingThreshold());

		if (auto world = tmp_world.lock()) {
			if (auto _self = self.lock()) {
				if (world->add_physbody(_self) == 0) {
					attach(world);
				}
			}
		}

		return 0;
	}
}

#endif // BEE_PHYSICS_BODY
