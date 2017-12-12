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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render.hpp" // Include the engine headers

#include "../engine.hpp"

#include "../util/real.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/rooms.hpp"
#include "../core/window.hpp"

#include "camera.hpp"
#include "drawing.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "viewport.hpp"

#include "../resource/texture.hpp"

namespace bee { namespace render {
	namespace internal {
		GLuint target = 0;

		std::map<const Texture*,std::list<TextureDrawData>> textures;

		ShaderProgram* program = nullptr;
	}

	/*
	* set_is_lightable() - Set whether to enable lighting or not
	* ! This should be used to disable lighting only on specific elements, e.g. the HUD
	* ! After calling this function it is the user's job to reset the lighting to the previous state
	* ! See Sprite::set_is_lightable() in bee/resources/sprite.cpp for more specific usage
	* @is_lightable: whether to enable lighting
	*/
	int set_is_lightable(bool is_lightable) {
		glUniform1i(get_program()->get_location("is_lightable"), (is_lightable) ? 1 : 0);
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
			default: {
				return shader;
			}
		}
	}

	/*
	* set_3d() - Set whether 3D mode is enabled or not
	* @is_3d: whether to enable 3D mode
	*/
	int set_3d(bool is_3d) {
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

	/*
	* set_viewport() - Set the new drawing viewport within the window
	* ! See https://wiki.libsdl.org/SDL_RenderSetViewport for details
	* @viewport: the rectangle defining the desired viewport
	*/
	int set_viewport(ViewPort* viewport) {
		glm::mat4 projection (get_projection());
		glm::mat4 view;
		glm::vec4 port;

		if (viewport == nullptr) { // If the viewport is not defined then set the drawing area to the entire screen
			view = glm::mat4(1.0f);
			port = glm::vec4(0.0f, 0.0f, get_width(), get_height());
		} else { // If the viewport is defined then use it
			view = glm::translate(glm::mat4(1.0f), glm::vec3(viewport->view.x, viewport->view.y, 0.0f));
			port = glm::vec4(viewport->port.x, viewport->port.y, viewport->port.w, viewport->port.h);
		}

		glUniformMatrix4fv(get_program()->get_location("view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform4fv(get_program()->get_location("port"), 1, glm::value_ptr(port));
		glUniformMatrix4fv(get_program()->get_location("projection"), 1, GL_FALSE, glm::value_ptr(projection));

		return 0;
	}

	int internal::render_texture(const TextureDrawData& td) {
		glUniformMatrix4fv(get_program()->get_location("model"), 1, GL_FALSE, glm::value_ptr(td.model));
		glUniformMatrix4fv(get_program()->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(td.rotation));
		glUniform4fv(get_program()->get_location("colorize"), 1, glm::value_ptr(td.color));

		// Bind the texture coordinates
		glEnableVertexAttribArray(get_program()->get_location("v_texcoord"));
		glBindBuffer(GL_ARRAY_BUFFER, td.buffer);
		glVertexAttribPointer(
			get_program()->get_location("v_texcoord"),
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		// Draw the triangles which form the rectangular subimage
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		return 0;
	}
	int queue_texture(const Texture* texture, const TextureDrawData& data) {
		if (internal::textures.find(texture) == internal::textures.end()) {
			internal::textures.emplace(texture, std::list<TextureDrawData>({data}));
		} else {
			internal::textures.at(texture).push_back(data);
		}
		return 0;
	}
	int render_textures() {
		for (auto t : internal::textures) {
			glBindVertexArray(t.second.front().vao); // Bind the VAO for the texture

			glUniform1i(get_program()->get_location("f_texture"), 0);
			glBindTexture(GL_TEXTURE_2D, t.second.front().texture);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t.second.front().ibo);

			for (auto& td : t.second) {
				internal::render_texture(td);
			}

			glBindVertexArray(0); // Unbind the VAO
		}

		glUniformMatrix4fv(get_program()->get_location("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the partial transformation matrix
		glUniformMatrix4fv(get_program()->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the rotation matrix

		internal::textures.clear();

		return 0;
	}

	/*
	* reset_target() - Set the rendering target back to the screen
	* ! See https://wiki.libsdl.org/SDL_SetRenderTarget for details
	*/
	int reset_target() {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Reset the bound framebuffer
		internal::target = 0; // Reset the target

		return 0;
	}
	/*
	* set_target() - Set the given Texture as the render target with the given width and height
	* @target: the Texture to use as the render target
	*/
	int set_target(Texture* target) {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		if (target == nullptr) { // If the given target is nullptr then reset the render target
			reset_target();
		} else {
			internal::target = target->set_as_target();
		}

		return 0;
	}
	int set_program(ShaderProgram* new_program) {
		internal::program = new_program;
		return 0;
	}
	ShaderProgram* get_program() {
		return internal::program;
	}

	int clear() {
		draw_set_color(*(engine->color));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return 0;
	}
	int render() {
		SDL_GL_SwapWindow(engine->renderer->window);
		return 0;
	}
}}

#endif // BEE_RENDER
