/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_FILTER_H
#define BEE_PHYSICS_FILTER_H 1

#include <btBulletDynamicsCommon.h>

namespace bee {
	class PhysicsFilter : public btOverlapFilterCallback {
		virtual bool needBroadphaseCollision(btBroadphaseProxy*, btBroadphaseProxy*) const;
	};
}

#endif // BEE_PHYSICS_FILTER_H
