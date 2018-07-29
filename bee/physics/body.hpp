/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_BODY_H
#define BEE_PHYSICS_BODY_H 1

#include <string>
#include <vector>
#include <map>

#include <SDL2/SDL.h> // Include the SDL2 headers for the Uint8 type

#include <btBulletDynamicsCommon.h>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class Variant;
	class Instance;
	class PhysicsWorld;
	class PhysicsDraw;

	class PhysicsBody {
		private:
			E_PHYS_SHAPE type;
			btCollisionShape* shape;
			size_t shape_param_amount;
			double* shape_params;

			btDefaultMotionState* motion_state;
			btRigidBody* body;

			PhysicsWorld* attached_world;
			Instance* attached_instance;
			std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>> constraints;

			double scale;
			double mass;
			double friction;
		public:
			PhysicsBody(PhysicsWorld*, Instance*, E_PHYS_SHAPE, double, double, double, double, double*);
			PhysicsBody(const PhysicsBody&);
			~PhysicsBody();

			PhysicsBody& operator=(const PhysicsBody&);

			std::map<Variant,Variant> serialize() const;
			int deserialize(std::map<Variant,Variant>&, Instance*);

			std::vector<Uint8> serialize_net();
			int deserialize_net(std::vector<Uint8>);

			int attach(PhysicsWorld*);
			int remove();

			double get_mass() const;
			double get_scale() const;
			btVector3 get_inertia() const;
			btRigidBody* get_body() const;
			PhysicsWorld* get_world() const;
			Instance* get_instance() const;
			const std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>>& get_constraints() const;

			btDefaultMotionState* get_motion() const;
			btVector3 get_pos() const;
			btQuaternion get_rotation() const;
			double get_rotation_x() const;
			double get_rotation_y() const;
			double get_rotation_z() const;

			int set_shape(E_PHYS_SHAPE, double*);
			int set_mass(double);
			int set_friction(double);

			int add_constraint(E_PHYS_CONSTRAINT, double*);
			int add_constraint_external(E_PHYS_CONSTRAINT, double*, btTypedConstraint*);
			int remove_constraints();

			int update_state();

			size_t get_shape_param_amount(E_PHYS_SHAPE, int) const;
			size_t get_shape_param_amount(E_PHYS_SHAPE) const;
	};
}

#endif // BEE_PHYSICS_BODY_H
