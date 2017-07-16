/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_BODY_H
#define BEE_PHYSICS_BODY_H 1

#include <string>
#include <vector>

#include <btBulletDynamicsCommon.h>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class Instance;
	class PhysicsWorld;
	class PhysicsDraw;

	class PhysicsBody {
		private:
			E_PHYS_SHAPE type;
			btCollisionShape* shape = nullptr;
			size_t shape_param_amount = 0;
			double* shape_params = nullptr;

			btDefaultMotionState* motion_state = nullptr;
			btRigidBody* body = nullptr;

			PhysicsWorld* attached_world = nullptr;
			Instance* attached_instance = nullptr;
			std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>> constraints;

			double scale = 1.0;
			double mass = 0.0;
			double friction  = 0.5;
		public:
			PhysicsBody(PhysicsWorld*, Instance*, E_PHYS_SHAPE, double, double, double, double, double*);
			~PhysicsBody();

			std::string serialize(bool) const;
			std::string serialize() const;
			int deserialize(const std::string&, Instance*);
			int deserialize(const std::string&);

			int attach(PhysicsWorld*);
			int remove();

			double get_mass() const;
			double get_scale() const;
			btVector3 get_inertia() const;
			btRigidBody* get_body() const;
			PhysicsWorld* get_world() const;
			const std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>>& get_constraints() const;

			btDefaultMotionState* get_motion() const;
			btVector3 get_position() const;
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
