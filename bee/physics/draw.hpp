/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_DRAW_H
#define BEE_PHYSICS_DRAW_H 1

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>

namespace bee { namespace internal {
	/// Used to draw Bullet Physics debug graphics
	class PhysicsDraw : public btIDebugDraw {
		int debug_mode; ///< The debug mode that is being drawn
	public:
		// See bee/core/instance.cpp for function comments
		explicit PhysicsDraw();

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
}}

#endif // BEE_PHYSICS_DRAW_H
