/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER
#define BEE_RENDER 1

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../engine.hpp" // Include the engine headers

#include "camera.hpp"
#include "renderer.hpp"

#include "../util/real.hpp"

#include "../init/gameoptions.hpp"

#include "../core/enginestate.hpp"

namespace bee {
	/*
	* set_is_lightable() - Set whether to enable lighting or not
	* ! This should be used to disable lighting only on specific elements, e.g. the HUD
	* ! After calling this function it is the user's job to reset the lighting to the previous state
	* ! See Sprite::set_is_lightable() in bee/resources/sprite.cpp for more specific usage
	* @new_is_lightable: whether to enable lighting
	*/
	int set_is_lightable(bool new_is_lightable) {
		if (engine->options->renderer_type == E_RENDERER::SDL) {
			return 1;
		}

		glUniform1i(engine->renderer->is_lightable_location, (new_is_lightable) ? 1 : 0);

		return 0;
	}

	/*
	* render_set_3d() - Set whether 3D mode is enabled or not
	* @new_is_3d: whether to enable 3D mode
	*/
	int render_set_3d(bool new_is_3d) {
		if (engine->options->renderer_type == E_RENDERER::SDL) {
			messenger_send({"engine", "renderer"}, E_MESSAGE::WARNING, "Cannot enable 3D rendering in SDL mode");
			return 1;
		}

		engine->renderer->render_is_3d = new_is_3d;

		if (engine->renderer->render_camera == nullptr) {
			render_set_camera(nullptr);
		}

		if (engine->renderer->render_is_3d) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}

		render_calc_projection();

		return 0;
	}
	/*
	* render_set_camera() - Set the camera position and angle for 3D mode
	* @new_camera: the new camera to render as
	*/
	int render_set_camera(Camera* new_camera) {
		if (engine->renderer->render_camera != nullptr) {
			if (engine->renderer->render_camera == new_camera) {
				return 1;
			}

			delete engine->renderer->render_camera;
			engine->renderer->render_camera = nullptr;
		}

		if (new_camera == nullptr) {
			if (engine->renderer->render_is_3d) {
				engine->renderer->render_camera = new Camera(glm::vec3(0.0f, 0.0f, -540.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			} else {
				engine->renderer->render_camera = new Camera(get_width(), get_height());
			}
		} else {
			engine->renderer->render_camera = new_camera;
		}

		if (engine->renderer->render_camera->width == 0.0) {
			engine->renderer->render_camera->width = get_width();
		}
		if (engine->renderer->render_camera->height == 0.0) {
			engine->renderer->render_camera->height = get_height();
		}

		render_calc_projection();

		return 0;
	}
	/*
	* render_get_3d() - Return whether 3D mode is enabled or not
	*/
	bool render_get_3d() {
		return engine->renderer->render_is_3d;
	}
	/*
	* render_get_projection() - Get the projection matrix of the current camera
	*/
	glm::mat4 render_get_projection() {
		if (engine->renderer->render_camera == nullptr) {
			render_set_camera(nullptr);
		}

		if (engine->renderer->projection_cache == nullptr) {
			return render_calc_projection();
		}

		return *(engine->renderer->projection_cache);
	}
	/*
	* render_calc_projection() - Recalculate the projection matrix for the current camera
	*/
	glm::mat4 render_calc_projection() {
		if (engine->renderer->render_camera == nullptr) {
			render_set_camera(nullptr);
		}

		glm::mat4 projection = glm::mat4(1.0f);
		if (engine->renderer->render_is_3d) {
			projection = glm::perspective((float)degtorad(engine->renderer->render_camera->fov), engine->renderer->render_camera->width/engine->renderer->render_camera->height, engine->renderer->render_camera->z_near, engine->renderer->render_camera->z_far);
			projection *= glm::lookAt(engine->renderer->render_camera->position, engine->renderer->render_camera->position+engine->renderer->render_camera->direction, engine->renderer->render_camera->orientation);
		} else {
			projection = glm::ortho(0.0f, engine->renderer->render_camera->width, engine->renderer->render_camera->height, 0.0f, 0.0f, engine->renderer->render_camera->z_far);
		}

		if (engine->renderer->projection_cache == nullptr) {
			engine->renderer->projection_cache = new glm::mat4(1.0f);
		}
		*(engine->renderer->projection_cache) = projection;

		return projection;
	}
	/*
	* render_get_camera() - Get a copy of the camera values
	*/
	Camera render_get_camera() {
		return *(engine->renderer->render_camera);
	}
}

#endif // BEE_RENDER
