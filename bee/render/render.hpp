/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_H
#define BEE_RENDER_H 1

#include "camera.hpp"

namespace bee {
	int set_is_lightable(bool);

	int render_set_3d(bool);
	int render_set_camera(Camera*);
	bool render_get_3d();
	glm::mat4 render_get_projection();
	glm::mat4 render_calc_projection();
	Camera render_get_camera();
}

#endif // BEE_RENDER_H
