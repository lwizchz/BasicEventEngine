/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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

#include "../render/render.hpp"
#include "../render/renderer.hpp"

#include "../resource/texture.hpp"
#include "../resource/room.hpp"

namespace bee {
	/*
	* get_transition_type() - Return the transition type for draw_transition()
	*/
	E_TRANSITION get_transition_type() {
		return engine->transition_type;
	}
	/*
	* set_transition_type() - Set the transition type for draw_transition()
	* @type: the new transition type to use
	*/
	int set_transition_type(E_TRANSITION type) {
		engine->transition_type = type; // Set the new type
		return 0; // Return 0 on success
	}
	/*
	* set_transition_custom() - Set the transition type to a custom function for draw_transition()
	* @custom_func: the new custom transition type to use
	*/
	int set_transition_custom(std::function<void (Texture*, Texture*)> custom_func) {
		engine->transition_custom_func = custom_func; // Set the new custom function
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
	* @speed: the new transition speed to use
	*/
	int set_transition_speed(double speed) {
		engine->transition_speed = speed/get_fps_goal(); // Set the new speed
		return 0; // Return 0 on success
	}
	/*
	* draw_transition() - Draw the defined transition type
	*/
	int draw_transition() {
		if (get_option("is_headless").i) {
			return 1; // Return 1 when in headless mode
		}

		switch (engine->transition_type) {
			case E_TRANSITION::NONE: { // No transition
				break;
			}
			case E_TRANSITION::CREATE_LEFT: { // Create from left
				for (double i=0; i<get_window().w; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);
					engine->texture_after->crop_image({-1, -1, static_cast<int>(i), -1});
					engine->texture_after->draw(0, 0, 0, static_cast<int>(i), -1, 0.0, {255, 255, 255, 255});
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_RIGHT: { // Create from right
				for (double i=get_window().w; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_after->draw(0, 0, 0);
					engine->texture_after->crop_image({-1, -1, static_cast<int>(i), -1});
					engine->texture_before->draw(0, 0, 0, static_cast<int>(i), -1, 0.0, {255, 255, 255, 255});
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_TOP: { // Create from top
				for (double i=0; i<get_window().h; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);
					engine->texture_after->crop_image({-1, -1, -1, static_cast<int>(i)});
					engine->texture_after->draw(0, 0, 0, -1, static_cast<int>(i), 0.0, {255, 255, 255, 255});
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_BOTTOM: { // Create from bottom
				for (double i=get_window().h; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_after->draw(0, 0, 0);
					engine->texture_after->crop_image({-1, -1, -1, static_cast<int>(i)});
					engine->texture_before->draw(0, 0, 0, -1, static_cast<int>(i), 0.0, {255, 255, 255, 255});
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_CENTER: { // Create from center
				const int w = get_window().w, h = get_window().h;
				for (double i=0; i<w; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);

					double ih = i/w * h;
					int x = (w - static_cast<int>(i))/2;
					int y = (h - static_cast<int>(ih))/2;
					engine->texture_after->crop_image({x, y, static_cast<int>(i), static_cast<int>(ih)});
					engine->texture_after->draw(x, y, 0, static_cast<int>(i), static_cast<int>(ih), 0.0, {255, 255, 255, 255});

					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_LEFT: { // Shift from left
				for (double i=-get_window().w; i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);
					engine->texture_after->draw(static_cast<int>(i), 0, 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_RIGHT: { // Shift from right
				for (double i=get_window().h; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);
					engine->texture_after->draw(static_cast<int>(i), 0, 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_TOP: { // Shift from top
				for (double i=-get_window().h; i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);
					engine->texture_after->draw(0, static_cast<int>(i), 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_BOTTOM: { // Shift from bottom
				for (double i=get_window().h; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0);
					engine->texture_after->draw(0, static_cast<int>(i), 0);
					render::render();
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
				const int w = get_window().w;
				for (double i=-w; i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(static_cast<int>(i)+w, 0, 0);
					engine->texture_after->draw(static_cast<int>(i), 0, 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::PUSH_RIGHT: { // Push from right
				const int w = get_window().w;
				for (double i=w; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(static_cast<int>(i)-w, 0, 0);
					engine->texture_after->draw(static_cast<int>(i), 0, 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::PUSH_TOP: { // Push from top
				const int h = get_window().h;
				for (double i=-h; i<0; i+=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, static_cast<int>(i)+h, 0);
					engine->texture_after->draw(0, static_cast<int>(i), 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::PUSH_BOTTOM: { // Push from bottom
				const int h = get_window().h;
				for (double i=h; i>=0; i-=engine->transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, static_cast<int>(i)-h, 0);
					engine->texture_after->draw(0, static_cast<int>(i), 0);
					render::render();
				}
				break;
			}
			case E_TRANSITION::ROTATE_LEFT: { // Rotate to left
				engine->texture_before->set_rotate(0.0, 1.0);
				engine->texture_after->set_rotate(0.0, 1.0);
				for (double a=0.0; a<90.0; a+=engine->transition_speed*get_delta()/20.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, -a, {255, 255, 255, 255});
					engine->texture_after->draw(0, 0, 0, -1, -1, 90.0-a, {255, 255, 255, 255});
					render::render();
				}
				break;
			}
			case E_TRANSITION::ROTATE_RIGHT: { // Rotate to right
				engine->texture_before->set_rotate(1.0, 1.0);
				engine->texture_after->set_rotate(1.0, 1.0);
				for (double a=0.0; a<90.0; a+=engine->transition_speed*get_delta()/20.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, a, {255, 255, 255, 255});
					engine->texture_after->draw(0, 0, 0, -1, -1, a-90.0, {255, 255, 255, 255});
					render::render();
				}
				break;
			}
			case E_TRANSITION::BLEND: { // Blend (crossfade)
				for (double a=0.0; a<255.0; a+=engine->transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(255.0-a)});
					engine->texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(a)});
					render::render();
				}
				break;
			}
			case E_TRANSITION::FADE: { // Fade out and in
				for (double a=0.0; a<255.0; a+=engine->transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(255.0-a)});
					render::render();
				}
				for (double a=0.0; a<255.0; a+=engine->transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					engine->texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(a)});
					render::render();
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
