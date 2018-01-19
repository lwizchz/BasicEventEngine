/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_WORLD
#define BEE_PHYSICS_WORLD 1

#include "world.hpp"

#include "filter.hpp"
#include "draw.hpp"
#include "body.hpp"

#include "../engine.hpp"

#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

#include "../core/rooms.hpp"

#include "../resource/room.hpp"

namespace bee {
	PhysicsWorld::PhysicsWorld() :
		collision_configuration(new btDefaultCollisionConfiguration()),
		dispatcher(new btCollisionDispatcher(collision_configuration)),
		broadphase(new btDbvtBroadphase()),
		solver(new btSequentialImpulseConstraintSolver()),
		world(new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration)),

		filter_callback(new PhysicsFilter()),

		debug_draw(new PhysicsDraw(this)),

		gravity({0.0, -10.0, 0.0}),
		scale(10.0)
	{
		debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		world->setDebugDrawer(debug_draw);

		set_gravity(gravity);
		set_scale(scale);

		world->setInternalTickCallback(Room::collision_internal, static_cast<void*>(this));

		world->getPairCache()->setOverlapFilterCallback(filter_callback);
	}
	PhysicsWorld::PhysicsWorld(const PhysicsWorld& other) :
		collision_configuration(new btDefaultCollisionConfiguration()),
		dispatcher(new btCollisionDispatcher(collision_configuration)),
		broadphase(new btDbvtBroadphase()),
		solver(new btSequentialImpulseConstraintSolver()),
		world(new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration)),

		filter_callback(new PhysicsFilter()),

		debug_draw(new PhysicsDraw(this)),

		gravity(other.gravity),
		scale(other.scale)
	{
		debug_draw->setDebugMode(other.debug_draw->getDebugMode());
		world->setDebugDrawer(debug_draw);

		set_gravity(gravity);
		set_scale(scale);

		world->setInternalTickCallback(Room::collision_internal, static_cast<void*>(this));

		world->getPairCache()->setOverlapFilterCallback(filter_callback);
	}
	PhysicsWorld::~PhysicsWorld() {
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

			this->filter_callback = new PhysicsFilter();

			this->debug_draw = new PhysicsDraw(this);

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

	int PhysicsWorld::set_gravity(btVector3 _gravity) {
		gravity = _gravity;
		//world->setGravity(gravity);
		world->setGravity(gravity*btScalar(10.0/scale));
		return 0;
	}
	int PhysicsWorld::set_scale(double _scale) {
		if (_scale != scale) {
			int a = 0;

			for (int i=world->getNumConstraints()-1; i>=0; --i, ++a) {
				btTypedConstraint* c = world->getConstraint(i);
				remove_constraint(c);
				delete c;
			}

			for (int i=world->getNumCollisionObjects()-1; i>=0; --i, ++a) {
				btCollisionObject* obj = world->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);

				if ((body)&&(body->getMotionState())) {
					delete body->getMotionState();
				}

				world->removeRigidBody(body);
				delete obj;
			}

			if (a > 0) {
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Scale change occurred with " + std::to_string(a) + " non-removed objects and constraints, they have been deleted\n");
			}
		}

		scale = _scale;
		world->setGravity(gravity*btScalar(10.0/scale));

		return 0;
	}

	int PhysicsWorld::add_body(PhysicsBody* new_body) {
		if (scale != new_body->get_scale()) {
			messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Failed to add body to world: scale mismatch: world(" + std::to_string(scale) + "), body(" + std::to_string(new_body->get_scale()) + ")\n");
			return 1;
		}

		world->addRigidBody(new_body->get_body());
		new_body->attach(this);

		return 0;
	}
	/*
	* add_constraint() - Add a constraint of the given type to the body with the given parameters
	* ! See http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints for more information
	* @type: the constraint type
	* @body: the body to constrain
	* @p: the constraint parameters
	*/
	int PhysicsWorld::add_constraint(E_PHYS_CONSTRAINT type, PhysicsBody* body, double* p) {
		btTypedConstraint* constraint = nullptr;

		btScalar s = 2.0*scale;

		bool should_disable_collisions = false;
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT: {
				/*
				* p[0], p[1], p[2]: the relative coordinates of the pivot in the body
				*/
				btPoint2PointConstraint* c = new btPoint2PointConstraint(
					*(body->get_body()),
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
					*(body->get_body()),
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
				btSliderConstraint* c = new btSliderConstraint(*(body->get_body()), btTransform::getIdentity(), true);
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
				btConeTwistConstraint* c = new btConeTwistConstraint(*(body->get_body()), btTransform::getIdentity());
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
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(btScalar(p[0]), btScalar(p[1]), btScalar(p[2])) / s);
				c->setLinearUpperLimit(btVector3(btScalar(p[3]), btScalar(p[4]), btScalar(p[5])) / s);
				c->setAngularLowerLimit(btVector3(btScalar(p[6]), btScalar(p[7]), btScalar(p[8])) / s);
				c->setAngularUpperLimit(btVector3(btScalar(p[9]), btScalar(p[10]), btScalar(p[11])) / s);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FIXED: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(0, 0, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FLAT: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0) / s);
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 1) / s);
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::TILE: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0) / s);
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			default:
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Invalid constraint type\n");
			case E_PHYS_CONSTRAINT::NONE:
				break;
		}

		body->add_constraint_external(type, p, constraint);

		return 0;
	}
	/*
	* add_constraint() - Add a constraint of the given type between the bodies with the given parameters
	* ! See http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints for more information
	* @type: the constraint type
	* @body1: the first body to constrain
	* @body2: the second body to constrain
	* @p: the constraint parameters
	*/
	int PhysicsWorld::add_constraint(E_PHYS_CONSTRAINT type, PhysicsBody* body1, PhysicsBody* body2, double* p) {
		btTypedConstraint* constraint = nullptr;

		btScalar s = 2.0*scale;

		bool should_disable_collisions = false;
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT: {
				/*
				* p[0], p[1], p[2]: the relative coordinates of the pivot in body1
				* p[3], p[4], p[5]: the relative coordinates of the pivot in body2
				*/
				btPoint2PointConstraint* c = new btPoint2PointConstraint(
					*(body1->get_body()), *(body2->get_body()),
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
					*(body1->get_body()), *(body2->get_body()),
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
					*(body1->get_body()), *(body2->get_body()),
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
					*(body1->get_body()), *(body2->get_body()),
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
					*(body1->get_body()), *(body2->get_body()),
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
					*(body1->get_body()), *(body2->get_body()),
					btTransform::getIdentity(), btTransform::getIdentity(),
					true
				);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FLAT: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(
					*(body1->get_body()), *(body2->get_body()),
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
					*(body1->get_body()), *(body2->get_body()),
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
				messenger::send({"engine", "physics"}, E_MESSAGE::WARNING, "Invalid constraint type\n");
			case E_PHYS_CONSTRAINT::NONE:
				break;
		}

		body1->add_constraint_external(type, p, constraint);
		body2->add_constraint_external(type, p, constraint);

		return 0;
	}
	int PhysicsWorld::add_constraint_external(btTypedConstraint* c) {
		bool should_disable_collisions = false;
		world->addConstraint(c, should_disable_collisions);
		return 0;
	}

	int PhysicsWorld::remove_body(PhysicsBody* body) {
		world->removeRigidBody(body->get_body());
		get_current_room()->remove_physbody(body);
		return 0;
	}
	int PhysicsWorld::remove_constraint(btTypedConstraint* constraint) {
		world->removeConstraint(constraint);
		return 0;
	}

	int PhysicsWorld::step(double step_size) {
		world->stepSimulation(btScalar(step_size), 10);
		return 0;
	}

	int PhysicsWorld::draw_debug() {
		world->debugDrawWorld();
		return 0;
	}

	size_t PhysicsWorld::get_constraint_param_amount(E_PHYS_CONSTRAINT constraint) const {
		switch (constraint) {
			case E_PHYS_CONSTRAINT::POINT:  return 6;
			case E_PHYS_CONSTRAINT::HINGE:  return 12;
			case E_PHYS_CONSTRAINT::SLIDER: return 4;
			case E_PHYS_CONSTRAINT::CONE:   return 4;
			case E_PHYS_CONSTRAINT::SIXDOF:   return 12;

			default:
			case E_PHYS_CONSTRAINT::FIXED:
			case E_PHYS_CONSTRAINT::FLAT:
			case E_PHYS_CONSTRAINT::TILE:
			case E_PHYS_CONSTRAINT::NONE:   return 0;
		}
	}
}

#endif // BEE_PHYSICS_WORLD
