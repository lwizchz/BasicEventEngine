/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_CAMERA_H
#define _BEE_RENDER_CAMERA_H 1

#include "../engine.hpp"

namespace bee {
	struct Camera {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 orientation;

		float width, height;

		float fov;
		float z_near, z_far;

		Camera();
		Camera(float, float);
		Camera(glm::vec3, glm::vec3, glm::vec3);
	};
}

#endif // _BEE_RENDER_CAMERA_H
