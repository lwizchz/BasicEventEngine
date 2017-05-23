/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER
#define _BEE_RENDER 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::set_is_lightable() - Set whether to enable lighting or not
* ! This should be used to disable lighting only on specific elements, e.g. the HUD
* ! After calling this function it is the user's job to reset the lighting to the previous state
* ! See Sprite::set_is_lightable() in bee/resources/sprite.cpp for more specific usage
* @new_is_lightable: whether to enable lighting
*/
int BEE::set_is_lightable(bool new_is_lightable) {
	if (options->renderer_type == BEE_RENDERER_SDL) {
		return 1;
	}

	glUniform1i(is_lightable_location, (new_is_lightable) ? 1 : 0);

	return 0;
}

/*
* BEE::render_set_3d() - Set whether 3D mode is enabled or not
* @new_is_3d: whether to enable 3D mode
*/
int BEE::render_set_3d(bool new_is_3d) {
	if (options->renderer_type == BEE_RENDERER_SDL) {
		messenger_send({"engine", "renderer"}, BEE_MESSAGE_WARNING, "Cannot enable 3D rendering in SDL mode");
		return 1;
	}

	render_is_3d = new_is_3d;

	if (render_camera == nullptr) {
		render_set_camera(nullptr);
	}

	if (render_is_3d) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	render_calc_projection();

	return 0;
}
/*
* BEE::render_set_camera() - Set the camera position and angle for 3D mode
* @new_camera: the new camera to render as
*/
int BEE::render_set_camera(Camera* new_camera) {
	if (render_camera != nullptr) {
		if (render_camera == new_camera) {
			return 1;
		}

		delete render_camera;
		render_camera = nullptr;
	}

	if (new_camera == nullptr) {
		if (render_is_3d) {
			render_camera = new Camera(glm::vec3(0.0f, 0.0f, -540.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		} else {
			render_camera = new Camera(get_width(), get_height());
		}
	} else {
		render_camera = new_camera;
	}

	if (render_camera->width == 0.0) {
		render_camera->width = get_width();
	}
	if (render_camera->height == 0.0) {
		render_camera->height = get_height();
	}

	render_calc_projection();

	return 0;
}
/*
* BEE::render_get_3d() - Return whether 3D mode is enabled or not
*/
bool BEE::render_get_3d() const {
	return render_is_3d;
}
/*
* BEE::render_get_projection() - Get the projection matrix of the current camera
*/
glm::mat4 BEE::render_get_projection() {
	if (render_camera == nullptr) {
		render_set_camera(nullptr);
	}

	if (projection_cache == nullptr) {
		return render_calc_projection();
	}

	return *projection_cache;
}
/*
* BEE::render_calc_projection() - Recalculate the projection matrix for the current camera
*/
glm::mat4 BEE::render_calc_projection() {
	if (render_camera == nullptr) {
		render_set_camera(nullptr);
	}

	glm::mat4 projection = glm::mat4(1.0f);
	if (render_is_3d) {
		projection = glm::perspective((float)degtorad(render_camera->fov), render_camera->width/render_camera->height, render_camera->z_near, render_camera->z_far);
		projection *= glm::lookAt(render_camera->position, render_camera->position+render_camera->direction, render_camera->orientation);
	} else {
		projection = glm::ortho(0.0f, render_camera->width, render_camera->height, 0.0f, 0.0f, render_camera->z_far);
	}

	if (projection_cache == nullptr) {
		projection_cache = new glm::mat4(1.0f);
	}
	*projection_cache = projection;

	return projection;
}
/*
* BEE::render_get_camera() - Get a copy of the camera values
*/
BEE::Camera BEE::render_get_camera() const {
	return *render_camera;
}

#endif // _BEE_RENDER
