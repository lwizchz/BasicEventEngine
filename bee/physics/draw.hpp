/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PHYSICS_DRAW_H
#define _BEE_PHYSICS_DRAW_H 1

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>

#include "../engine.hpp"

namespace bee {
	class PhysicsDraw : public btIDebugDraw {
		private:
			int debug_mode;

			PhysicsWorld* attached_world = nullptr;
		public:
			PhysicsDraw();
			virtual ~PhysicsDraw();
			int attach(PhysicsWorld*);

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
}

#endif // _BEE_PHYSICS_DRAW_H
