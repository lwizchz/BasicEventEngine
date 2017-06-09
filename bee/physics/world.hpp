/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
	class PhysicsDraw;
	class PhysicsBody;

	class PhysicsWorld {
		private:
			btDefaultCollisionConfiguration* collision_configuration = nullptr;
			btCollisionDispatcher* dispatcher = nullptr;
			btBroadphaseInterface* broadphase = nullptr;
			btSequentialImpulseConstraintSolver* solver = nullptr;
			btDiscreteDynamicsWorld* world = nullptr;

			PhysicsDraw* debug_draw = nullptr;

			btVector3 gravity = {0.0, -10.0, 0.0};
			double scale = 1.0;
		public:
			PhysicsWorld();
			~PhysicsWorld();

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
