/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_WORLD_H
#define BEE_PHYSICS_WORLD_H 1

#include <map>
#include <memory>

#include <btBulletDynamicsCommon.h>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class PhysicsBody;
	namespace internal {
		class PhysicsFilter;
		class PhysicsDraw;
	}

	/// Used to simulate physics via the Bullet Physics library
	class PhysicsWorld {
		btDefaultCollisionConfiguration* collision_configuration; ///< The collision config used by the collision dispatcher and the dynamics world
		btCollisionDispatcher* dispatcher; ///< The collision dispatcher used by the dynamics world
		btBroadphaseInterface* broadphase; ///< The broadphase interface used by the dynamics world
		btSequentialImpulseConstraintSolver* solver; ///< The constraint solver used by the dynamics world
		btDiscreteDynamicsWorld* world; ///< The dynamics world which contains the physics simulation

		internal::PhysicsFilter* filter_callback; ///< The filter callback run by the broadphase test

		internal::PhysicsDraw* debug_draw; ///< The drawer used for Bullet debug graphics

		btVector3 gravity; ///< The world gravity vector
		double scale; ///< The world scale

		std::map<const btRigidBody*,std::weak_ptr<PhysicsBody>> bodies; ///< A map of the btRigidBodys in the world with their associated PhysicsBody
	public:
		// See bee/physics/world.cpp for function comments
		PhysicsWorld(const btVector3&, double);
		PhysicsWorld();
		PhysicsWorld(const PhysicsWorld&);
		~PhysicsWorld();

		PhysicsWorld& operator=(const PhysicsWorld&);

		btVector3 get_gravity() const;
		double get_scale() const;
		btDispatcher* get_dispatcher() const;
		std::weak_ptr<PhysicsBody> get_physbody(const btRigidBody*) const;

		void set_gravity(btVector3);
		int set_scale(double);

		int add_physbody(std::shared_ptr<PhysicsBody>);
		btTypedConstraint* add_constraint(E_PHYS_CONSTRAINT, btRigidBody*, double*);
		btTypedConstraint* add_constraint(E_PHYS_CONSTRAINT, btRigidBody*, btRigidBody*, double*);
		void add_constraint_external(btTypedConstraint*);

		void remove_body(btRigidBody*);
		void remove_constraint(btTypedConstraint*);

		int step(double);

		void draw_debug();
	};
}

#endif // BEE_PHYSICS_WORLD_H
