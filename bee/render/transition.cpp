/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_TRANSITION
#define BEE_RENDER_TRANSITION 1

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "transition.hpp" // Include the engine headers

#include "../engine.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/window.hpp"

#include "../render/renderer.hpp"

#include "../resource/sprite.hpp"
#include "../resource/background.hpp"
#include "../resource/room.hpp"

namespace bee {
	/*
	* reset_render_target() - Set the rendering target back to the screen
	* ! See https://wiki.libsdl.org/SDL_SetRenderTarget for details
	*/
	int reset_render_target() {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		if (get_options().renderer_type != E_RENDERER::SDL) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Reset the bound framebuffer
			engine->renderer->target = 0; // Reset the target
		} else {
			SDL_SetRenderTarget(engine->renderer->sdl_renderer, nullptr); // Reset the SDL render target
		}

		return 0;
	}
	/*
	* set_render_target() - Set the given sprite as the render target with the given width and height
	* @sprite_target: the sprite to use as the render target
	* @w: the width to use
	* @h: the height to use
	*/
	int set_render_target(Sprite* sprite_target, int w, int h) {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		if (sprite_target == nullptr) { // If the given target is nullptr then reset the render target
			reset_render_target();
		} else {
			engine->renderer->target = sprite_target->set_as_target(w, h);
		}

		return 0;
	}
	/*
	* set_render_target() - Set the given sprite as the render target
	* ! When the function is called without a width and height, simply call the function again using the width and height of the window
	* @sprite_target: the sprite to use as the render target
	*/
	int set_render_target(Sprite* sprite_target) {
		return set_render_target(sprite_target, get_width(), get_height());
	}
	/*
	* set_render_target() - Set the given background as the render target with the given width and height
	* @background_target: the background to use as the render target
	* @w: the width to use
	* @h: the height to use
	*/
	int set_render_target(Background* background_target, int w, int h) {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		if (background_target == nullptr) { // If the given target is nullptr then reset the render target
			reset_render_target();
		} else {
			engine->renderer->target = background_target->set_as_target(w, h);
		}

		return 0;
	}
	/*
	* set_render_target() - Set the given background as the render target
	* ! When the function is called without a width and height, simply call the function again using the width and height of the window
	*/
	int set_render_target(Background* background_target) {
		return set_render_target(background_target, get_width(), get_height());
	}

	/*
	* get_transition_type() - Return the transition type for draw_transition()
	*/
	E_TRANSITION get_transition_type() {
		return engine->transition_type;
	}
	/*
	* set_transition_type() - Set the transition type for draw_transition()
	* @new_type: the new transition type to use
	*/
	int set_transition_type(E_TRANSITION new_type) {
		engine->transition_type = new_type; // Set the new type
		return 0; // Return 0 on success
	}
	/*
	* set_transition_custom() - Set the transition type to a custom function for draw_transition()
	* @new_custom: the new custom transition type to use
	*/
	int set_transition_custom(std::function<void (Sprite*, Sprite*)> new_custom) {
		engine->transition_custom_func = new_custom; // Set the new custom function
		engine->transition_type = E_TRANSITION::CUSTOM;
		return 0; // Return 0 on success
	}
	/*
	* get_transition_speed() - Return the transition speed for draw_transition()
	*/
	double get_transition_speed() {
		return engine->transition_speed;
	}
	/*
	* set_transition_speed() - Set the transition speed for draw_transition()
	* @new_speed: the new transition speed to use
	*/
	int set_transition_speed(double new_speed) {
		engine->transition_speed = new_speed/get_fps_goal(); // Set the new speed
		return 0; // Return 0 on success
	}
	/*
	* draw_transition() - Draw the defined transition type
	*/
	int draw_transition() {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		SDL_RendererFlip f = SDL_FLIP_NONE;
		if (get_options().renderer_type != E_RENDERER::SDL) {
			f = SDL_FLIP_VERTICAL;
		}

		switch (engine->transition_type) {
			case E_TRANSITION::NONE: { // No transition
				break;
			}
			case E_TRANSITION::CREATE_LEFT: { // Create from left
				for (double i=0; i<get_width(); i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);
					engine->texture_after->crop_image_width(static_cast<int>(i));
					engine->texture_after->draw(0, 0, 0, static_cast<int>(i), -1, 0.0, {255, 255, 255, 255}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::CREATE_RIGHT: { // Create from right
				for (double i=get_width(); i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_after->draw(0, 0, 0, f);
					engine->texture_before->crop_image_width(static_cast<int>(i));
					engine->texture_before->draw(0, 0, 0, static_cast<int>(i), -1, 0.0, {255, 255, 255, 255}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::CREATE_TOP: { // Create from top
				for (double i=0; i<get_height(); i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);
					engine->texture_after->crop_image_height(static_cast<int>(i));
					engine->texture_after->draw(0, 0, 0, -1, static_cast<int>(i), 0.0, {255, 255, 255, 255}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::CREATE_BOTTOM: { // Create from bottom
				for (double i=get_height(); i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_after->draw(0, 0, 0, f);
					engine->texture_before->crop_image_height(static_cast<int>(i));
					engine->texture_before->draw(0, 0, 0, -1, static_cast<int>(i), 0.0, {255, 255, 255, 255}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::CREATE_CENTER: { // Create from center
				const int w = get_width(), h = get_height();
				for (double i=0; i<w; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);

					double ih = i/w * h;
					int x = (w - static_cast<int>(i))/2;
					int y = (h - static_cast<int>(ih))/2;
					engine->texture_after->crop_image({x, y, static_cast<int>(i), static_cast<int>(ih)});
					engine->texture_after->draw(x, y, 0, static_cast<int>(i), static_cast<int>(ih), 0.0, {255, 255, 255, 255}, f);

					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_LEFT: { // Shift from left
				for (double i=-get_width(); i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);
					engine->texture_after->draw(static_cast<int>(i), 0, 0,  f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_RIGHT: { // Shift from right
				for (double i=get_width(); i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);
					engine->texture_after->draw(static_cast<int>(i), 0, 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_TOP: { // Shift from top
				for (double i=-get_height(); i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);
					engine->texture_after->draw(0, static_cast<int>(i), 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_BOTTOM: { // Shift from bottom
				for (double i=get_height(); i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, f);
					engine->texture_after->draw(0, static_cast<int>(i), 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::INTERLACE_LEFT: { // Interlaced from left
				// Not yet implemented
				messenger::send({"engine"}, E_MESSAGE::WARNING, "The interlace transitions are not yet implemented.");
				break;
			}
			case E_TRANSITION::INTERLACE_RIGHT: { // Interlaced from right
				// Not yet implemented
				messenger::send({"engine"}, E_MESSAGE::WARNING, "The interlace transitions are not yet implemented.");
				break;
			}
			case E_TRANSITION::INTERLACE_TOP: { // Interlaced from top
				// Not yet implemented
				messenger::send({"engine"}, E_MESSAGE::WARNING, "The interlace transitions are not yet implemented.");
				break;
			}
			case E_TRANSITION::INTERLACE_BOTTOM: { // Interlaced from bottom
				// Not yet implemented
				messenger::send({"engine"}, E_MESSAGE::WARNING, "The interlace transitions are not yet implemented.");
				break;
			}
			case E_TRANSITION::PUSH_LEFT: { // Push from left
				const int w = get_width();
				for (double i=-w; i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(static_cast<int>(i)+w, 0, 0, f);
					engine->texture_after->draw(static_cast<int>(i), 0, 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::PUSH_RIGHT: { // Push from right
				const int w = get_width();
				for (double i=w; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(static_cast<int>(i)-w, 0, 0, f);
					engine->texture_after->draw(static_cast<int>(i), 0, 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::PUSH_TOP: { // Push from top
				const int h = get_height();
				for (double i=-h; i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, static_cast<int>(i)+h, 0, f);
					engine->texture_after->draw(0, static_cast<int>(i), 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::PUSH_BOTTOM: { // Push from bottom
				const int h = get_height();
				for (double i=h; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, static_cast<int>(i)-h, 0, f);
					engine->texture_after->draw(0, static_cast<int>(i), 0, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::ROTATE_LEFT: { // Rotate to left
				engine->texture_before->set_rotate_xy(0.0, 1.0);
				engine->texture_after->set_rotate_xy(0.0, 1.0);
				for (double a=0.0; a<90.0; a+=engine->transition_speed*get_delta()/20.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, -a, {255, 255, 255, 255}, f);
					engine->texture_after->draw(0, 0, 0, -1, -1, 90.0-a, {255, 255, 255, 255}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::ROTATE_RIGHT: { // Rotate to right
				engine->texture_before->set_rotate_xy(1.0, 1.0);
				engine->texture_after->set_rotate_xy(1.0, 1.0);
				for (double a=0.0; a<90.0; a+=engine->transition_speed*get_delta()/20.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, a, {255, 255, 255, 255}, f);
					engine->texture_after->draw(0, 0, 0, -1, -1, a-90.0, {255, 255, 255, 255}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::BLEND: { // Blend (crossfade)
				for (double a=0.0; a<255.0; a+=engine->transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(255.0-a)}, f);
					engine->texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(a)}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::FADE: { // Fade out and in
				for (double a=0.0; a<255.0; a+=engine->transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(255.0-a)}, f);
					engine->renderer->render();
				}
				for (double a=0.0; a<255.0; a+=engine->transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					engine->renderer->render_clear();
					engine->texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(a)}, f);
					engine->renderer->render();
				}
				break;
			}
			case E_TRANSITION::CUSTOM: { // Run a custom transition
				if (engine->transition_custom_func != nullptr) {
					engine->transition_custom_func(engine->texture_before, engine->texture_after);
				}
				break;
			}
			default: {
				engine->transition_type = E_TRANSITION::NONE;
				break;
			}
		}

		for (auto& b : engine->current_room->get_backgrounds()) {
			b->background->set_time_update();
		}

		return 0;
	}
	/*
	* compute_check_quit() - Check whether we received an SDL_QUIT event in the middle of heavy data-processing
	* ! This function should be periodically called by any loop that retains control for more than 500ms
	* ! Note that this will not handle custom close events such as the Escape key
	*/
	bool compute_check_quit() {
		SDL_PumpEvents();
		if (SDL_PeepEvents(nullptr, 1, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT) > 0) {
			engine->quit = true;
		}

		return engine->quit;
	}
}

#endif // BEE_RENDER_TRANSITION
