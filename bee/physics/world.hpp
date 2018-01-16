/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_WORLD_H
#define BEE_PHYSICS_WORLD_H 1

#include <btBulletDynamicsCommon.h>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class PhysicsFilter;
	class PhysicsDraw;
	class PhysicsBody;

	class PhysicsWorld {
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* dispatcher;
			btBroadphaseInterface* broadphase;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* world;

			PhysicsFilter* filter_callback;

			PhysicsDraw* debug_draw;

			btVector3 gravity;
			double scale;
		public:
			PhysicsWorld();
			PhysicsWorld(const PhysicsWorld&);
			~PhysicsWorld();

			PhysicsWorld& operator=(const PhysicsWorld&);

			btVector3 get_gravity() const;
			double get_scale() const;
			btDispatcher* get_dispatcher() const;

			int set_gravity(btVector3);
			int set_scale(double);

			int add_body(PhysicsBody*);
			int add_constraint(E_PHYS_CONSTRAINT, PhysicsBody*, double*);
			int add_constraint(E_PHYS_CONSTRAINT, PhysicsBody*, PhysicsBody*, double*);
			int add_constraint_external(btTypedConstraint*);

			int remove_body(PhysicsBody*);
			int remove_constraint(btTypedConstraint*);

			int step(double);

			int draw_debug();

			size_t get_constraint_param_amount(E_PHYS_CONSTRAINT) const;
	};
}

#endif // BEE_PHYSICS_WORLD_H
