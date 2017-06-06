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
	if (options->renderer_type == bee::E_RENDERER::SDL) {
		return 1;
	}

	glUniform1i(renderer->is_lightable_location, (new_is_lightable) ? 1 : 0);

	return 0;
}

/*
* BEE::render_set_3d() - Set whether 3D mode is enabled or not
* @new_is_3d: whether to enable 3D mode
*/
int BEE::render_set_3d(bool new_is_3d) {
	if (options->renderer_type == bee::E_RENDERER::SDL) {
		messenger_send({"engine", "renderer"}, bee::E_MESSAGE::WARNING, "Cannot enable 3D rendering in SDL mode");
		return 1;
	}

	renderer->render_is_3d = new_is_3d;

	if (renderer->render_camera == nullptr) {
		render_set_camera(nullptr);
	}

	if (renderer->render_is_3d) {
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
	if (renderer->render_camera != nullptr) {
		if (renderer->render_camera == new_camera) {
			return 1;
		}

		delete renderer->render_camera;
		renderer->render_camera = nullptr;
	}

	if (new_camera == nullptr) {
		if (renderer->render_is_3d) {
			renderer->render_camera = new Camera(glm::vec3(0.0f, 0.0f, -540.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		} else {
			renderer->render_camera = new Camera(get_width(), get_height());
		}
	} else {
		renderer->render_camera = new_camera;
	}

	if (renderer->render_camera->width == 0.0) {
		renderer->render_camera->width = get_width();
	}
	if (renderer->render_camera->height == 0.0) {
		renderer->render_camera->height = get_height();
	}

	render_calc_projection();

	return 0;
}
/*
* BEE::render_get_3d() - Return whether 3D mode is enabled or not
*/
bool BEE::render_get_3d() const {
	return renderer->render_is_3d;
}
/*
* BEE::render_get_projection() - Get the projection matrix of the current camera
*/
glm::mat4 BEE::render_get_projection() {
	if (renderer->render_camera == nullptr) {
		render_set_camera(nullptr);
	}

	if (renderer->projection_cache == nullptr) {
		return render_calc_projection();
	}

	return *(renderer->projection_cache);
}
/*
* BEE::render_calc_projection() - Recalculate the projection matrix for the current camera
*/
glm::mat4 BEE::render_calc_projection() {
	if (renderer->render_camera == nullptr) {
		render_set_camera(nullptr);
	}

	glm::mat4 projection = glm::mat4(1.0f);
	if (renderer->render_is_3d) {
		projection = glm::perspective((float)degtorad(renderer->render_camera->fov), renderer->render_camera->width/renderer->render_camera->height, renderer->render_camera->z_near, renderer->render_camera->z_far);
		projection *= glm::lookAt(renderer->render_camera->position, renderer->render_camera->position+renderer->render_camera->direction, renderer->render_camera->orientation);
	} else {
		projection = glm::ortho(0.0f, renderer->render_camera->width, renderer->render_camera->height, 0.0f, 0.0f, renderer->render_camera->z_far);
	}

	if (renderer->projection_cache == nullptr) {
		renderer->projection_cache = new glm::mat4(1.0f);
	}
	*(renderer->projection_cache) = projection;

	return projection;
}
/*
* BEE::render_get_camera() - Get a copy of the camera values
*/
BEE::Camera BEE::render_get_camera() const {
	return *(renderer->render_camera);
}

#endif // _BEE_RENDER
