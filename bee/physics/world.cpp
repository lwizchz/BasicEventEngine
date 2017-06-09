/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_WORLD
#define BEE_PHYSICS_WORLD 1

#include "world.hpp"

#include "draw.hpp"
#include "body.hpp"

#include "../engine.hpp"

#include "../util/platform.hpp"

#include "../core/room.hpp"
#include "../core/messenger/messenger.hpp"

#include "../resources/room.hpp"

namespace bee {
	PhysicsWorld::PhysicsWorld() {
		collision_configuration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collision_configuration);
		broadphase = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);

		debug_draw = new PhysicsDraw();
		debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		world->setDebugDrawer(debug_draw);
		debug_draw->attach(this);

		set_gravity(gravity);
		set_scale(scale);

		//world->setInternalTickCallback(Room::collision_internal, static_cast<void*>(this), true);
		world->setInternalTickCallback(Room::collision_internal, static_cast<void*>(this));

		//dispatcher->setNearCallback(Room::check_collision_lists);
	}
	PhysicsWorld::~PhysicsWorld() {
		delete debug_draw;

		delete world;
		delete solver;
		delete broadphase;
		delete dispatcher;
		delete collision_configuration;
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

	int PhysicsWorld::set_gravity(btVector3 new_gravity) {
		gravity = new_gravity;
		//world->setGravity(gravity);
		world->setGravity(10.0*gravity/scale);
		return 0;
	}
	int PhysicsWorld::set_scale(double new_scale) {
		if (new_scale != scale) {
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
				messenger_send({"engine", "physics"}, E_MESSAGE::WARNING, "Scale change occurred with " + bee_itos(a) + " non-removed objects and constraints, they have been deleted\n");
			}
		}

		scale = new_scale;
		world->setGravity(10.0*gravity/scale);

		return 0;
	}

	int PhysicsWorld::add_body(PhysicsBody* new_body) {
		if (scale != new_body->get_scale()) {
			messenger_send({"engine", "physics"}, E_MESSAGE::WARNING, "Failed to add body to world: scale mismatch: world(" + bee_itos(scale) + "), body(" + bee_itos(new_body->get_scale()) + ")\n");
			return 1;
		}

		world->addRigidBody(new_body->get_body());
		new_body->attach(this);

		return 0;
	}
	int PhysicsWorld::add_constraint(E_PHYS_CONSTRAINT type, PhysicsBody* body, double* p) {
		btTypedConstraint* constraint = nullptr;

		bool should_disable_collisions = false;
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT: {
				btPoint2PointConstraint* c = new btPoint2PointConstraint(*(body->get_body()), btVector3(p[0], p[1], p[2]));
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::HINGE: {
				btHingeConstraint* c = new btHingeConstraint(*(body->get_body()), btVector3(p[0], p[1], p[2]), btVector3(p[3], p[4], p[5]));
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SLIDER: {
				btSliderConstraint* c = new btSliderConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLowerLinLimit(p[0]);
				c->setUpperLinLimit(p[1]);
				c->setLowerAngLimit(p[2]);
				c->setUpperAngLimit(p[3]);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::CONE: {
				btConeTwistConstraint* c = new btConeTwistConstraint(*(body->get_body()), btTransform::getIdentity());
				world->addConstraint(c, should_disable_collisions);

				c->setLimit(btScalar(p[0]), btScalar(p[1]), btScalar(p[2]), btScalar(p[3]));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SIXDOF: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(p[0], p[1], p[2]));
				c->setLinearUpperLimit(btVector3(p[3], p[4], p[5]));
				c->setAngularLowerLimit(btVector3(p[6], p[7], p[8]));
				c->setAngularUpperLimit(btVector3(p[9], p[10], p[11]));

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

				c->setLinearLowerLimit(btVector3(1, 1, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 1));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::TILE: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body->get_body()), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			default:
				messenger_send({"engine", "physics"}, E_MESSAGE::WARNING, "Invalid constraint type\n");
			case E_PHYS_CONSTRAINT::NONE:
				break;
		}

		body->add_constraint_external(type, p, constraint);

		return 0;
	}
	int PhysicsWorld::add_constraint(E_PHYS_CONSTRAINT type, PhysicsBody* body1, PhysicsBody* body2, double* p) {
		btTypedConstraint* constraint = nullptr;

		bool should_disable_collisions = false;
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT: {
				btPoint2PointConstraint* c = new btPoint2PointConstraint(*(body1->get_body()), *(body2->get_body()), btVector3(p[0], p[1], p[2]), btVector3(p[3], p[4], p[5]));
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::HINGE: {
				btHingeConstraint* c = new btHingeConstraint(*(body1->get_body()), *(body2->get_body()), btVector3(p[0], p[1], p[2]), btVector3(p[3], p[4], p[5]), btVector3(p[6], p[7], p[8]), btVector3(p[9], p[10], p[11]));
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SLIDER: {
				btSliderConstraint* c = new btSliderConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLowerLinLimit(p[0]);
				c->setUpperLinLimit(p[1]);
				c->setLowerAngLimit(p[2]);
				c->setUpperAngLimit(p[3]);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::CONE: {
				btConeTwistConstraint* c = new btConeTwistConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity());
				world->addConstraint(c, should_disable_collisions);

				c->setLimit(btScalar(p[0]), btScalar(p[1]), btScalar(p[2]), btScalar(p[3]));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::SIXDOF: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(p[0], p[1], p[2]));
				c->setLinearUpperLimit(btVector3(p[3], p[4], p[5]));
				c->setAngularLowerLimit(btVector3(p[6], p[7], p[8]));
				c->setAngularUpperLimit(btVector3(p[9], p[10], p[11]));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FIXED: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::FLAT: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 1));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			case E_PHYS_CONSTRAINT::TILE: {
				btGeneric6DofConstraint* c = new btGeneric6DofConstraint(*(body1->get_body()), *(body2->get_body()), btTransform::getIdentity(), btTransform::getIdentity(), true);
				world->addConstraint(c, should_disable_collisions);

				c->setLinearLowerLimit(btVector3(1, 1, 0));
				c->setLinearUpperLimit(btVector3(0, 0, 0));
				c->setAngularLowerLimit(btVector3(0, 0, 0));
				c->setAngularUpperLimit(btVector3(0, 0, 0));

				constraint = c;
				break;
			}
			default:
				messenger_send({"engine", "physics"}, E_MESSAGE::WARNING, "Invalid constraint type\n");
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
		world->stepSimulation(step_size, 10);
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
