/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_CAMERA_H
#define BEE_RENDER_CAMERA_H 1

#include "../defines.hpp"

#include <glm/glm.hpp> // Include the required OpenGL headers

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

#endif // BEE_RENDER_CAMERA_H
