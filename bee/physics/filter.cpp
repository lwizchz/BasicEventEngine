/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_FILTER
#define BEE_PHYSICS_FILTER 1

#include "filter.hpp"

#include "../resource/room.hpp"

namespace bee {
	bool PhysicsFilter::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const {
		return Room::check_collision_filter(proxy0, proxy1);
	}
}

#endif // BEE_PHYSICS_FILTER
