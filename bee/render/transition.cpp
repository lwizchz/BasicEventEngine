/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
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

#include "../core/window.hpp"

#include "../render/render.hpp"
#include "../render/renderer.hpp"
#include "../render/shader.hpp"

#include "../resource/texture.hpp"
#include "../resource/room.hpp"

namespace bee { namespace render {
	namespace internal {
		Texture* texture_before = nullptr;
		Texture* texture_after = nullptr;
		E_TRANSITION transition_type = E_TRANSITION::NONE;
		double transition_speed = 1024.0/DEFAULT_GAME_FPS;
		std::function<void (Texture*, Texture*)> transition_custom_func = nullptr;
	}
	int internal::init_transitions() {
		if (texture_before != nullptr) {
			delete texture_before;
			texture_before = nullptr;
		}
		if (texture_after != nullptr) {
			delete texture_after;
			texture_after = nullptr;
		}

		texture_before = new Texture();
		texture_before->load_as_target(get_window().w, get_window().h);
		texture_after = new Texture();
		texture_after->load_as_target(get_window().w, get_window().h);

		return 0;
	}
	int internal::transition_target_before() {
		return set_target(texture_before);
	}
	int internal::transition_target_after() {
		return set_target(texture_after);
	}

	/**
	* @returns the transition type used when changing Rooms
	*/
	E_TRANSITION get_transition_type() {
		return internal::transition_type;
	}
	/**
	* Set the transition type used when changing Rooms.
	* @param type the new transition type to use
	*/
	void set_transition_type(E_TRANSITION type) {
		internal::transition_type = type;
	}
	/**
	* Set the transition type to a custom function.
	* @param custom_func the new custom transition type to use
	*/
	void set_transition_custom(std::function<void (Texture*, Texture*)> custom_func) {
		internal::transition_custom_func = custom_func;
		internal::transition_type = E_TRANSITION::CUSTOM;
	}
	/**
	* @returns the transition speed when drawing Room transitions
	*/
	double get_transition_speed() {
		return internal::transition_speed;
	}
	/**
	* Set the transition speed when drawing Room transitions.
	* @speed: the new transition speed to use
	*/
	void set_transition_speed(double speed) {
		internal::transition_speed = speed/get_fps_goal();
	}

	/**
	* Draw the defined transition type.
	*/
	int draw_transition() {
		if (get_option("is_headless").i) {
			return 1; // Return 1 when in headless mode
		}

		render::set_viewport(nullptr);
		glUniform1i(render::get_program()->get_location("flip"), 2);

		switch (internal::transition_type) {
			case E_TRANSITION::NONE: { // No transition
				break;
			}
			case E_TRANSITION::CREATE_LEFT: { // Create from left
				for (double i=0; i<get_window().w; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);
					internal::texture_after->crop_image({-1, -1, static_cast<int>(i), -1});
					internal::texture_after->draw(0, 0, 0, static_cast<int>(i), -1, 0.0, {255, 255, 255, 255});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_RIGHT: { // Create from right
				for (double i=get_window().w; i>=0; i-=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_after->draw(0, 0, 0);
					internal::texture_before->crop_image({-1, -1, static_cast<int>(i), -1});
					internal::texture_before->draw(0, 0, 0, static_cast<int>(i), -1, 0.0, {255, 255, 255, 255});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_TOP: { // Create from top
				for (double i=0; i<get_window().h; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);
					internal::texture_after->crop_image({-1, -1, -1, static_cast<int>(i)});
					internal::texture_after->draw(0, 0, 0, -1, static_cast<int>(i), 0.0, {255, 255, 255, 255});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_BOTTOM: { // Create from bottom
				for (double i=get_window().h; i>=0; i-=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_after->draw(0, 0, 0);
					internal::texture_before->crop_image({-1, -1, -1, static_cast<int>(i)});
					internal::texture_before->draw(0, 0, 0, -1, static_cast<int>(i), 0.0, {255, 255, 255, 255});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::CREATE_CENTER: { // Create from center
				const int w = get_window().w, h = get_window().h;
				for (double i=0; i<w; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);

					double ih = i/w * h;
					int x = (w - static_cast<int>(i))/2;
					int y = (h - static_cast<int>(ih))/2;
					internal::texture_after->crop_image({x, y, static_cast<int>(i), static_cast<int>(ih)});
					internal::texture_after->draw(x, y, 0, static_cast<int>(i), static_cast<int>(ih), 0.0, {255, 255, 255, 255});

					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_LEFT: { // Shift from left
				for (double i=-get_window().w; i<0; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);
					internal::texture_after->draw(static_cast<int>(i), 0, 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_RIGHT: { // Shift from right
				for (double i=get_window().h; i>=0; i-=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);
					internal::texture_after->draw(static_cast<int>(i), 0, 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_TOP: { // Shift from top
				for (double i=-get_window().h; i<0; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);
					internal::texture_after->draw(0, static_cast<int>(i), 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::SHIFT_BOTTOM: { // Shift from bottom
				for (double i=get_window().h; i>=0; i-=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0);
					internal::texture_after->draw(0, static_cast<int>(i), 0);
					render::render_textures();
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
				for (double i=-w; i<0; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(static_cast<int>(i)+w, 0, 0);
					internal::texture_after->draw(static_cast<int>(i), 0, 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::PUSH_RIGHT: { // Push from right
				const int w = get_window().w;
				for (double i=w; i>=0; i-=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(static_cast<int>(i)-w, 0, 0);
					internal::texture_after->draw(static_cast<int>(i), 0, 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::PUSH_TOP: { // Push from top
				const int h = get_window().h;
				for (double i=-h; i<0; i+=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, static_cast<int>(i)+h, 0);
					internal::texture_after->draw(0, static_cast<int>(i), 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::PUSH_BOTTOM: { // Push from bottom
				const int h = get_window().h;
				for (double i=h; i>=0; i-=internal::transition_speed*get_delta()) {
					if ((static_cast<int>(i)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, static_cast<int>(i)-h, 0);
					internal::texture_after->draw(0, static_cast<int>(i), 0);
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::ROTATE_LEFT: { // Rotate to left
				internal::texture_before->set_rotate(0.0, 1.0);
				internal::texture_after->set_rotate(0.0, 1.0);
				for (double a=0.0; a<90.0; a+=internal::transition_speed*get_delta()/20.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0, -1, -1, -a, {255, 255, 255, 255});
					internal::texture_after->draw(0, 0, 0, -1, -1, 90.0-a, {255, 255, 255, 255});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::ROTATE_RIGHT: { // Rotate to right
				internal::texture_before->set_rotate(1.0, 1.0);
				internal::texture_after->set_rotate(1.0, 1.0);
				for (double a=0.0; a<90.0; a+=internal::transition_speed*get_delta()/20.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0, -1, -1, a, {255, 255, 255, 255});
					internal::texture_after->draw(0, 0, 0, -1, -1, a-90.0, {255, 255, 255, 255});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::BLEND: { // Blend (crossfade)
				for (double a=0.0; a<255.0; a+=internal::transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(255.0-a)});
					internal::texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(a)});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::FADE: { // Fade out and in
				for (double a=0.0; a<255.0; a+=internal::transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(255.0-a)});
					render::render_textures();
					render::render();
				}
				for (double a=0.0; a<255.0; a+=internal::transition_speed*get_delta()/5.0) {
					if ((static_cast<int>(a)%10 == 0)&&(compute_check_quit())) {
						break;
					}

					render::clear();
					internal::texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, static_cast<Uint8>(a)});
					render::render_textures();
					render::render();
				}
				break;
			}
			case E_TRANSITION::CUSTOM: { // Run a custom transition
				if (internal::transition_custom_func != nullptr) {
					internal::transition_custom_func(internal::texture_before, internal::texture_after);
				}
				break;
			}
			default: {
				internal::transition_type = E_TRANSITION::NONE;
				break;
			}
		}

		glUniform1i(render::get_program()->get_location("flip"), 0);

		return 0;
	}
}}

#endif // BEE_RENDER_TRANSITION
