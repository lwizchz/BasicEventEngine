/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER
#define BEE_RENDER 1

#include "../defines.hpp"

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "render.hpp" // Include the engine headers

#include "../util/real.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/window.hpp"

#include "camera.hpp"
#include "renderer.hpp"
#include "shader.hpp"

namespace bee { namespace render {
	/*
	* set_is_lightable() - Set whether to enable lighting or not
	* ! This should be used to disable lighting only on specific elements, e.g. the HUD
	* ! After calling this function it is the user's job to reset the lighting to the previous state
	* ! See Sprite::set_is_lightable() in bee/resources/sprite.cpp for more specific usage
	* @is_lightable: whether to enable lighting
	*/
	int set_is_lightable(bool is_lightable) {
		if (get_options().renderer_type == E_RENDERER::SDL) {
			return 1;
		}

		glUniform1i(engine->renderer->program->get_location("is_lightable"), (is_lightable) ? 1 : 0);

		return 0;
	}

	std::string opengl_prepend_version(const std::string& shader) {
		switch (get_options().renderer_type) {
			case E_RENDERER::OPENGL4: {
				if (GL_VERSION_4_1) {
					return "#version 410 core\n" + shader;
				}
			}
			case E_RENDERER::OPENGL3: {
				if (GL_VERSION_3_3) {
					return "#version 330 core\n" + shader;
				}
			}
			case E_RENDERER::SDL:
			default: {
				engine->options->renderer_type = E_RENDERER::SDL;
				return shader;
			}
		}
	}

	/*
	* set_3d() - Set whether 3D mode is enabled or not
	* @is_3d: whether to enable 3D mode
	*/
	int set_3d(bool is_3d) {
		if (get_options().renderer_type == E_RENDERER::SDL) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::WARNING, "Cannot enable 3D rendering in SDL mode");
			return 1;
		}

		engine->renderer->render_is_3d = is_3d;

		if (engine->renderer->render_camera == nullptr) {
			set_camera(nullptr);
		}

		if (engine->renderer->render_is_3d) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}

		calc_projection();

		return 0;
	}
	/*
	* set_camera() - Set the camera position and angle for 3D mode
	* @camera: the new camera to render as
	*/
	int set_camera(Camera* camera) {
		if (engine->renderer->render_camera != nullptr) {
			if (engine->renderer->render_camera == camera) {
				return 1;
			}

			delete engine->renderer->render_camera;
			engine->renderer->render_camera = nullptr;
		}

		if (camera == nullptr) {
			if (engine->renderer->render_is_3d) {
				engine->renderer->render_camera = new Camera(glm::vec3(0.0f, 0.0f, -540.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			} else {
				engine->renderer->render_camera = new Camera(static_cast<float>(get_width()), static_cast<float>(get_height()));
			}
		} else {
			engine->renderer->render_camera = camera;
		}

		if (engine->renderer->render_camera->width == 0.0) {
			engine->renderer->render_camera->width = static_cast<float>(get_width());
		}
		if (engine->renderer->render_camera->height == 0.0) {
			engine->renderer->render_camera->height = static_cast<float>(get_height());
		}

		calc_projection();

		return 0;
	}
	/*
	* get_3d() - Return whether 3D mode is enabled or not
	*/
	bool get_3d() {
		return engine->renderer->render_is_3d;
	}
	/*
	* get_projection() - Get the projection matrix of the current camera
	*/
	glm::mat4 get_projection() {
		if (engine->renderer->render_camera == nullptr) {
			set_camera(nullptr);
		}

		if (engine->renderer->projection_cache == nullptr) {
			return calc_projection();
		}

		return *(engine->renderer->projection_cache);
	}
	/*
	* calc_projection() - Recalculate the projection matrix for the current camera
	*/
	glm::mat4 calc_projection() {
		if (engine->renderer->render_camera == nullptr) {
			set_camera(nullptr);
		}

		glm::mat4 projection = glm::mat4(1.0f);
		if (engine->renderer->render_is_3d) {
			projection = glm::perspective(static_cast<float>(degtorad(engine->renderer->render_camera->fov)), engine->renderer->render_camera->width/engine->renderer->render_camera->height, engine->renderer->render_camera->z_near, engine->renderer->render_camera->z_far);
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
	* get_camera() - Get a copy of the camera values
	*/
	Camera get_camera() {
		return *(engine->renderer->render_camera);
	}
}}

#endif // BEE_RENDER
