/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PHYSICS_H
#define _BEE_PHYSICS_H 1

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>

#include "../../game.hpp"

class BEE::PhysicsWorld {
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
		PhysicsWorld(btVector3, double);
		~PhysicsWorld();

		int init(btVector3, double);
		int attach(BEE*);

		btVector3 get_gravity() const;
		double get_scale() const;
		btDispatcher* get_dispatcher() const;

		int set_gravity(btVector3);
		int set_scale(double);

		int add_body(PhysicsBody*);
		int add_constraint(bee_phys_constraint_t, PhysicsBody*, double*);
		int add_constraint(bee_phys_constraint_t, PhysicsBody*, PhysicsBody*, double*);

		int remove_body(PhysicsBody*);

		int step(double);

		int draw_debug();
};

class BEE::PhysicsDraw : public btIDebugDraw {
	private:
		int debug_mode;

		BEE* attached_game = nullptr;
		PhysicsWorld* attached_world = nullptr;
	public:
		PhysicsDraw();
		virtual ~PhysicsDraw();
		int attach(BEE*, PhysicsWorld*);

		virtual void drawLine(const btVector3&, const btVector3&, const btVector3&, const btVector3&);
		virtual void drawLine(const btVector3&, const btVector3&, const btVector3&);
		virtual void drawSphere(const btVector3&, const btVector3&, const btVector3&);
		virtual void drawTriangle(const btVector3&, const btVector3&, const btVector3&, const btVector3&, btScalar);
		virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&);

		virtual void reportErrorWarning(const char*);

		virtual void draw3dText(const btVector3&, const char*);

		virtual void setDebugMode(int);
		virtual int getDebugMode() const;
};

class BEE::PhysicsBody {
	private:
		bee_phys_shape_t type;
		btCollisionShape* shape = nullptr;
		btDefaultMotionState* motion_state = nullptr;
		btRigidBody* body = nullptr;

		PhysicsWorld* attached_world = nullptr;

		double mass = 0.0;
		double scale = 1.0;
	public:
		PhysicsBody(PhysicsWorld*, bee_phys_shape_t, double, double, double, double, double*);
		~PhysicsBody();

		int attach(PhysicsWorld*);

		double get_mass() const;
		double get_scale() const;
		btVector3 get_inertia() const;
		btRigidBody* get_body() const;
		PhysicsWorld* get_world() const;

		btDefaultMotionState* get_motion() const;
		btVector3 get_position() const;
		btQuaternion get_rotation() const;
		double get_rotation_x() const;
		double get_rotation_y() const;
		double get_rotation_z() const;

		int set_shape(bee_phys_shape_t, double*);
		int set_mass(double);
};

#endif // _BEE_PHYSICS_H
