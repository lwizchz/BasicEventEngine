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
#include <memory>

#include <SDL2/SDL.h> // Include the SDL2 headers for the Uint8 type

#include <btBulletDynamicsCommon.h>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class Variant;
	class Instance;
	class PhysicsWorld;
	class PhysicsDraw;

	/// Used for rigid bodies in PhysicsWorld simulations
	class PhysicsBody {
		E_PHYS_SHAPE type; ///< The type of shape stored by the collision shape
		btCollisionShape* shape; ///< The collision shape used by the rigid body
		size_t shape_param_amount; ///< The number of parameters required by the shape initializer
		double* shape_params; ///< The parameters provided to the shape initializer

		btDefaultMotionState* motion_state; ///< Used to store the body's motion
		btRigidBody* body; ///< The rigid body used in the simulation

		std::weak_ptr<PhysicsWorld> attached_world; ///< A pointer to the attached PhysicsWorld
		Instance* attached_instance; ///< A pointer to the attached Instance
		std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>> constraints; ///< The list of constraints added to the body

		double scale; ///< The scale used by the shape
		double mass; ///< The mass used by the body
		double friction; ///< The friction used by the body
	public:
		// See bee/physics/body.cpp for function comments
		PhysicsBody(std::shared_ptr<PhysicsWorld>, Instance*, E_PHYS_SHAPE, double, btVector3, double*);
		PhysicsBody(const PhysicsBody&);
		~PhysicsBody();

		PhysicsBody& operator=(const PhysicsBody&);

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&, Instance*);

		std::vector<Uint8> serialize_net();
		int deserialize_net(std::vector<Uint8>);

		void attach(std::shared_ptr<PhysicsWorld>);
		void remove();

		E_PHYS_SHAPE get_shape_type() const;
		double get_mass() const;
		double get_scale() const;
		btVector3 get_inertia() const;
		btRigidBody* get_body() const;
		std::shared_ptr<PhysicsWorld> get_world() const;
		Instance* get_instance() const;
		const std::vector<std::tuple<E_PHYS_CONSTRAINT,double*,btTypedConstraint*>>& get_constraints() const;

		btDefaultMotionState* get_motion() const;
		btVector3 get_pos() const;
		btQuaternion get_rotation() const;
		double get_rotation_x() const;
		double get_rotation_y() const;
		double get_rotation_z() const;

		int set_shape(E_PHYS_SHAPE, double*);
		void set_mass(double);
		void set_friction(double);

		btTypedConstraint* add_constraint(E_PHYS_CONSTRAINT, double*);
		void add_constraint_external(E_PHYS_CONSTRAINT, double*, btTypedConstraint*);
		void remove_constraints();

		int update_state();
	};
}

#endif // BEE_PHYSICS_BODY_H
