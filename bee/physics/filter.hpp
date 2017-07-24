/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_FILTER_H
#define BEE_PHYSICS_FILTER_H 1

#include <btBulletDynamicsCommon.h>

namespace bee {
	struct PhysicsFilter : public btOverlapFilterCallback {
		virtual bool needBroadphaseCollision(btBroadphaseProxy*, btBroadphaseProxy*) const;
	};
}

#endif // BEE_PHYSICS_FILTER_H
