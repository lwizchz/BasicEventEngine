/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_CAMERA
#define _BEE_RENDER_CAMERA 1

#include "camera.hpp"

namespace bee {
	Camera::Camera() :
		position(),
		direction(),
		orientation(),

		width(0.0),
		height(0.0),

		fov(90.0),
		z_near(1.0),
		z_far(10000.0)
	{}
	Camera::Camera(float w, float h) :
		position(),
		direction(),
		orientation(),

		width(w),
		height(h),

		fov(90.0),
		z_near(1.0),
		z_far(10000.0)
	{}
	Camera::Camera(glm::vec3 p, glm::vec3 d, glm::vec3 o) :
		position(p),
		direction(d),
		orientation(o),

		width(0.0),
		height(0.0),

		fov(90.0),
		z_near(1.0),
		z_far(10000.0)
	{}
}

#endif // _BEE_RENDER_CAMERA
