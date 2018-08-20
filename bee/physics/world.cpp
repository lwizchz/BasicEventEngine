/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_WORLD
#define BEE_PHYSICS_WORLD 1

#include "world.hpp"

#include "physics.hpp"
#include "filter.hpp"
#include "draw.hpp"
#include "body.hpp"

#include "../engine.hpp"

#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

#include "../core/rooms.hpp"

#include "../resource/room.hpp"

namespace bee {
	/**
	* Construct the PhysicsWorld with the given gravity and scale.
	* @param _gravity the gravity vector to use
	* @param _scale the world scale to use, recommended values are in [0.05, 10]
	*/
	PhysicsWorld::PhysicsWorld(const btVector3& _gravity, double _scale) :
		collision_configuration(new btDefaultCollisionConfiguration()),
		dispatcher(new btCollisionDispatcher(collision_configuration)),
		broadphase(new btDbvtBroadphase()),
		solver(new btSequentialImpulseConstraintSolver()),
		world(new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration)),

		filter_callback(new internal::PhysicsFilter()),

		debug_draw(new internal::PhysicsDraw()),

		gravity(_gravity),
		scale(_scale),

		bodies()
	{
		debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		world->setDebugDrawer(debug_draw);

		set_gravity(gravity);
		set_scale(scale);

		world->setInternalTickCallback(Room::collision_internal, static_cast<void*>(this));

		world->getPairCache()->setOverlapFilterCallback(filter_callback);
	}
	/**
	* Default construct the PhysicsWorld.
	*/
	PhysicsWorld::PhysicsWorld() :
		PhysicsWorld({0.0, -10.0, 0.0}, 10.0)
	{}
	/**
	* Construct the PhysicsWorld from another one.
	* @param other the other world to construct from
	*/
	PhysicsWorld::PhysicsWorld(const PhysicsWorld& other) :
		PhysicsWorld(other.gravity, other.scale)
	{
		debug_draw->setDebugMode(other.debug_draw->getDebugMode());
	}
	/**
	* Free the simulation data.
	*/
	PhysicsWorld::~PhysicsWorld() {
		bodies.clear();

		delete debug_draw;

		delete filter_callback;

		delete world;
		delete solver;
		delete broadphase;
		delete dispatcher;
		delete collision_configuration;
	}

	PhysicsWorld& PhysicsWorld::operator=(const PhysicsWorld& rhs) {
		if (this != &rhs) {
			if (this->collision_configuration != nullptr) {
				delete this->collision_configuration;
				this->collision_configuration = nullptr;
			}
			if (this->dispatcher != nullptr) {
				delete this->dispatcher;
				this->dispatcher = nullptr;
			}
			if (this->broadphase != nullptr) {
				delete this->broadphase;
				this->broadphase = nullptr;
			}
			if (this->solver != nullptr) {
				delete this->solver;
				this->solver = nullptr;
			}
			if (this->world != nullptr) {
				delete this->world;
				this->world = nullptr;
			}

			this->collision_configuration = new btDefaultCollisionConfiguration();
			this->dispatcher = new btCollisionDispatcher(this->collision_configuration);
			this->broadphase = new btDbvtBroadphase();
			this->solver = new btSequentialImpulseConstraintSolver();
			this->world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);

			this->filter_callback = new internal::PhysicsFilter();

			this->debug_draw = new internal::PhysicsDraw();

			this->gravity = rhs.gravity;
			this->scale = rhs.scale;

			debug_draw->setDebugMode(rhs.debug_draw->getDebugMode());
			world->setDebugDrawer(debug_draw);

			set_gravity(gravity);
			set_scale(scale);

			world->setInternalTickCallback(Room::collision_internal, static_cast<void*>(this));

			world->getPairCache()->setOverlapFilterCallback(filter_callback);
		}
		return *this;
	}

	btVector3 PhysicsWorld::get_gravity() const {
		return gravity;
	}
	double PhysicsWorld::get_scale() const {
		return scale;
	}
	btDispatcher* PhysicsWorld::get_dispatcher() const {
		return world->getDispatcher();
	}
	/**
	* @param body the rigid body to find
	*
	* @returns the PhysicsBody associated with the given rigid body
	*/
	std::weak_ptr<PhysicsBody> PhysicsWorld::get_physbody(const btRigidBody* body) const {
		auto b = bodies.find(body);
		if (b == bodies.end()) {
			return {};
		}
		return b->second;
	}

	/**
	* Set the gravity vector with respect to the world scale.
	* @param _gravity the new gravity vector
	*/
	void PhysicsWorld::set_gravity(btVector3 _gravity) {
		gravity = _gravity;
		world->setGravity(gravity*btScalar(10.0/scale));
	}
	/**
	* Set the world scale to the given value.
	* @param _scale the new world scale
	*
	* @returns how many constraints and rigid bodies were deleted before the scale change
	*/
	int PhysicsWorld::set_scale(double _scale) {
		int a = 0;
		if (_scale != scale) {
			for (int i=world->getNumConstraints()-1; i>=0; --i, ++a) {
				btTypedConstraint* c = world->getConstraint(i);
				remove_constraint(c);
				delete c;
			}

			btAlignedObjectArray<btCollisionObject*> objs = world->getCollisionObjectArray();
			for (int i=world->getNumCollisionObjects()-1; i>=0; --i, ++a) {
				btRigidBody* body = btRigidBody::upcast(objs[i]);
				if ((body)&&(body->getMotionState())) {
					delete body->getMotionState();
				}

				world->removeRigidBody(body);
				delete objs[i];
			}

			bodies.clear();

			if (a > 0) {
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Scale change occurred with " + std::to_string(a) + " non-removed objects and constraints, they have been deleted");
			}
		}

		scale = _scale;
		world->setGravity(gravity*btScalar(10.0/scale));

		return a;
	}

	/**
	* Add the given PhysicsBody to the simulation.
	* @param new_body the body to add
	*
	* @retval 0 success
	* @retval 1 failed since the body's scale doesn't match the world scale
	*/
	int PhysicsWorld::add_physbody(std::shared_ptr<PhysicsBody> new_body) {
		if (scale != new_body->get_scale()) {
			if (new_body->get_shape_type() != E_PHYS_SHAPE::NONE) {
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Failed to add body to world: scale mismatch: world(" + std::to_string(scale) + "), body(" + std::to_string(new_body->get_scale()) + ")");
				return 1;
			}
		}

		world->addRigidBody(new_body->get_body());
		bodies.emplace(new_body->get_body(), new_body);

		return 0;
	}
	/**
	* Add a constraint of the given type to the body with the given parameters.
	* @see http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints for more information.
	* @param type the constraint type
	* @param body the body to constrain
	* @param p the constraint parameters
	*
	* @returns the added btTypedConstraint
	*/
	btTypedConstraint* PhysicsWorld::add_constraint(E_PHYS_CONSTRAINT type, btRigidBody* body, double* p) {
		btTypedConstraint* constraint;

		btScalar s = 2.0*scale;

		bool should_disable_collisions = false;
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT: {
				/*
				* p[0], p[1], p[2]: the relative coordinates of the pivot in the body
				*/
				btPoint2PointConstraint* c = new btPoint2PointConstraint(
					*body,
					btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s
				);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::HINGE: {
				/*
				* p[0], p[1], p[2]: the relative coordinates of the pivot axis in the body
				* p[3], p[4], p[5]: the direction of the pivot axis relative to the body
				*/
				btHingeConstraint* c = new btHingeConstraint(
					*body,
					btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s,
					btVector3(btScalar(p[3]), btScalar(p[4]), btScalar(p[5])) / s
				);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SLIDER: {
				/*
				* p[0]: the lower limit of linear motion
				* p[1]: the upper limit of linear motion
				* p[2]: the lower limit of angular motion
				* p[3]: the upper limit of angular motion
				*/
				btSliderConstraint* c = new btSliderConstraint(*body, btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLowerLinLimit(btScalar(p[0]) / s);
				c->setUpperLinLimit(btScalar(p[1]) / s);
				c->setLowerAngLimit(btScalar(p[2]) / s);
				c->setUpperAngLimit(btScalar(p[3]) / s);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::CONE: {
				/*
				* p[0], p[1]: the swing spans of the cone's rotation for the y- and z-axes (given that the x-axis is along the cone's height)
				* p[2]: the twist span of the x-axis
				* p[3]: the softness
				* p[4]: the bias factor
				* p[5]: the relaxation factor
				*/
				btConeTwistConstraint* c = new btConeTwistConstraint(*body, btTransform::getIdentity());
				world->addConstraint(c, should_disable_collisions);

				c->setLimit(
					btScalar(p[0]) / s, btScalar(p[1]) / s,
					btScalar(p[2]) / s,
					btScalar(p[3]) / s,
					btScalar(p[4]) / s,
					btScalar(p[5]) / s
				);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SIXDOF: {
				/*
				* p[0], p[1], p[2]: the lower limit of linear motion
				* p[3], p[4], p[5]: the upper limit of linear motion
				* p[6], p[7], p[8]: the lower limit of angular motion
				* p[9], p[10], p[11]: the upper limit of angular motion
				*
				* On all 6dof constraints, if:
				*   lower_limit == upper_limit: the axis is locked
				*   lower_limit < upper_limit: the axis is limited to that range
				*   lower_limit > upper_limit: the axis is free
				*/
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*body, btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s);
				c->setLinearUpperLimit(btVector3(btScalar(p[3]), btScalar(p[4]), btScalar(p[5])) / s);
				c->setAngularLowerLimit(btVector3(btScalar(p[6]), btScalar(p[7]), btScalar(p[8])) / s);
				c->setAngularUpperLimit(btVector3(btScalar(p[9]), btScalar(p[10]), btScalar(p[11])) / s);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FIXED: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*body, btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(0, 0, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FLAT: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*body, btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0) / s);
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 1) / s);
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::TILE: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*body, btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0) / s);
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			default:
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Invalid constraint type");
				[[fallthrough]];
			case E_PHYS_CONSTRAINT::NONE:
				constraint = nullptr;
				break;
		}

		return constraint;
	}
	/**
	* Add a constraint of the given type between the bodies with the given parameters.
	* @see http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints for more information.
	* @param type the constraint type
	* @param body1 the first body to constrain
	* @param body2 the second body to constrain
	* @param p the constraint parameters
	*
	* @returns the added btTypedConstraint
	*/
	btTypedConstraint* PhysicsWorld::add_constraint(E_PHYS_CONSTRAINT type, btRigidBody* body1, btRigidBody* body2, double* p) {
		btTypedConstraint* constraint;

		btScalar s = 2.0*scale;

		bool should_disable_collisions = false;
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT: {
				/*
				* p[0], p[1], p[2]: the relative coordinates of the pivot in body1
				* p[3], p[4], p[5]: the relative coordinates of the pivot in body2
				*/
				btPoint2PointConstraint* c = new btPoint2PointConstraint(
					*body1, *body2,
					btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s,
					btVector3(btScalar(p[3]), btScalar(p[4]), btScalar(p[5])) / s
				);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::HINGE: {
				/*
				* p[0], p[1], p[2]: the relative coordinates of the pivot in body1
				* p[3], p[4], p[5]: the relative coordinates of the pivot in body2
				* p[6], p[7], p[8]: the direction of the pivot axis relative to body1
				* p[9], p[10], p[11]: the direction of the pivot axis relative to body2
				*/
				btHingeConstraint* c = new btHingeConstraint(
					*body1, *body2,
					btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s,
					btVector3(btScalar(p[3]), btScalar(p[4]), btScalar(p[5])) / s,
					btVector3(btScalar(p[6]), btScalar(p[7]), btScalar(p[8])) / s,
					btVector3(btScalar(p[9]), btScalar(p[10]), btScalar(p[11])) / s
				);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SLIDER: {
				/*
				* p[0]: the lower limit of linear motion
				* p[1]: the upper limit of linear motion
				* p[2]: the lower limit of angular motion
				* p[3]: the upper limit of angular motion
				*/
				btSliderConstraint* c = new btSliderConstraint(
					*body1, *body2,
					btTransform::getIdentity(), btTransform::getIdentity(),
					true
				);
				world->addConstraint(c, should_disable_collisions);

				c->setLowerLinLimit(btScalar(p[0]) / s);
				c->setUpperLinLimit(btScalar(p[1]) / s);
				c->setLowerAngLimit(btScalar(p[2]) / s);
				c->setUpperAngLimit(btScalar(p[3]) / s);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::CONE: {
				/*
				* p[0], p[1]: the swing spans of the cone's rotation for the y- and z-axes (given that the x-axis is along the cone's height)
				* p[2]: the twist span of the x-axis
				* p[3]: the softness
				* p[4]: the bias factor
				* p[5]: the relaxation factor
				*/
				btConeTwistConstraint* c = new btConeTwistConstraint(
					*body1, *body2,
					btTransform::getIdentity(), btTransform::getIdentity()
				);
				world->addConstraint(c, should_disable_collisions);

				c->setLimit(
					btScalar(p[0]) / s, btScalar(p[1]) / s,
					btScalar(p[2]) / s,
					btScalar(p[3]) / s,
					btScalar(p[4]) / s,
					btScalar(p[5]) / s
				);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SIXDOF: {
				/*
				* p[0], p[1], p[2]: the lower limit of linear motion
				* p[3], p[4], p[5]: the upper limit of linear motion
				* p[6], p[7], p[8]: the lower limit of angular motion
				* p[9], p[10], p[11]: the upper limit of angular motion
				*
				* On all 6dof constraints, if:
				*   lower_limit == upper_limit: the axis is locked
				*   lower_limit < upper_limit: the axis is limited to that range
				*   lower_limit > upper_limit: the axis is free
				*/
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(
					*body1, *body2,
					btTransform::getIdentity(), btTransform::getIdentity(),
					true
				);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s);
				c->setLinearUpperLimit(btVector3(btScalar(p[3]), btScalar(p[4]), btScalar(p[5])) / s);
				c->setAngularLowerLimit(btVector3(btScalar(p[6]), btScalar(p[7]), btScalar(p[8])) / s);
				c->setAngularUpperLimit(btVector3(btScalar(p[9]), btScalar(p[10]), btScalar(p[11])) / s);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FIXED: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(
					*body1, *body2,
					btTransform::getIdentity(), btTransform::getIdentity(),
					true
				);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FLAT: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(
					*body1, *body2,
					btTransform::getIdentity(), btTransform::getIdentity(),
					true
				);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 1));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::TILE: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(
					*body1, *body2,
					btTransform::getIdentity(), btTransform::getIdentity(),
					true
				);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			default:
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Invalid constraint type");
				[[fallthrough]];
			case E_PHYS_CONSTRAINT::NONE:
				constraint = nullptr;
				break;
		}

		return constraint;
	}
	/**
	* Add the given constraint to the simulation.
	* @see http://bulletphysics.org/Bullet/BulletFull/classbtTypedConstraint.html for details.
	* @param constraint the constraint to add
	*/
	void PhysicsWorld::add_constraint_external(btTypedConstraint* constraint) {
		bool should_disable_collisions = false;
		world->addConstraint(constraint, should_disable_collisions);
	}

	/**
	* Remove the given rigid body from the simulation.
	* @param body the body to remove
	*/
	void PhysicsWorld::remove_body(btRigidBody* body) {
		world->removeRigidBody(body);
		bodies.erase(body);
	}
	/**
	* Remove the given constraint from the simulation.
	* @param constraint the constraint to remove
	*/
	void PhysicsWorld::remove_constraint(btTypedConstraint* constraint) {
		world->removeConstraint(constraint);
	}

	/**
	* Step the simulation the given amount of time.
	* @param step_size the time to step forward the simulation
	*
	* @returns the number of steps that were simulated
	*/
	int PhysicsWorld::step(double step_size) {
		return world->stepSimulation(btScalar(step_size), 10);
	}

	/**
	* Draw the simulation debug graphics.
	*/
	void PhysicsWorld::draw_debug() {
		world->debugDrawWorld();
	}
}

#endif // BEE_PHYSICS_WORLD
