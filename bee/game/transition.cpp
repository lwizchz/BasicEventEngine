/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_TRANSITION
#define _BEE_GAME_TRANSITION 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::reset_render_target() - Set the rendering target back to the screen
* ! See https://wiki.libsdl.org/SDL_SetRenderTarget for details
*/
int BEE::reset_render_target() {
	if (options->renderer_type != BEE_RENDERER_SDL) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Reset the bound framebuffer
		target = 0; // Reset the target
	} else {
		SDL_SetRenderTarget(renderer, NULL); // Reset the SDL render target
	}
	return 0;
}
/*
* BEE::set_render_target() - Set the given sprite as the render target with the given width and height
* @sprite_target: the sprite to use as the render target
* @w: the width to use
* @h: the height to use
*/
int BEE::set_render_target(Sprite* sprite_target, int w, int h) {
	if (sprite_target == NULL) { // If the given target is NULL then reset the render target
		reset_render_target();
	} else {
		target = sprite_target->set_as_target(w, h);
	}

	return 0;
}
/*
* BEE::set_render_target() - Set the given sprite as the render target
* ! When the function is called without a width and height, simply call the function again using the width and height of the window
* @sprite_target: the sprite to use as the render target
*/
int BEE::set_render_target(Sprite* sprite_target) {
	return set_render_target(sprite_target, get_width(), get_height());
}
/*
* BEE::set_render_target() - Set the given background as the render target with the given width and height
* @background_target: the background to use as the render target
* @w: the width to use
* @h: the height to use
*/
int BEE::set_render_target(Background* background_target, int w, int h) {
	if (background_target == NULL) { // If the given target is NULL then reset the render target
		reset_render_target();
	} else {
		target = background_target->set_as_target(w, h);
	}

	return 0;
}
/*
* BEE::set_render_target() - Set the given background as the render target
* ! When the function is called without a width and height, simply call the function again using the width and height of the window
*/
int BEE::set_render_target(Background* background_target) {
	return set_render_target(background_target, get_width(), get_height());
}

/*
* BEE::set_transition_type() - Set the transition type for draw_transition()
* @new_type: the new transition type to use
*/
int BEE::set_transition_type(int new_type) {
	if (!is_between(new_type, 0, transition_max)) {
		transition_type = 0;
		return 1; // Return 1 if the given type does not exist
	}

	transition_type = new_type; // Set the new type

	return 0; // Return 0 on success
}
/*
* BEE::draw_transition() - Draw the defined transition type
*/
int BEE::draw_transition() {
	SDL_RendererFlip f = SDL_FLIP_NONE;
	if (options->renderer_type != BEE_RENDERER_SDL) {
		f = SDL_FLIP_VERTICAL;
	}

	switch (transition_type) {
		case 0: { // No transition
			break;
		}
		case 1: { // Create from left
			// Not yet implemented
			break;
		}
		case 2: { // Create from right
			// Not yet implemented
			break;
		}
		case 3: { // Create from top
			// Not yet implemented
			break;
		}
		case 4: { // Create from bottom
			// Not yet implemented
			break;
		}
		case 5: { // Create from center
			// Not yet implemented
			break;
		}
		case 6: { // Shift from left
			for (double i=-get_width(); i<0; i+=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(i, 0, 0,  f);
				render();
			}
			break;
		}
		case 7: { // Shift from right
			for (double i=get_width(); i>=0; i-=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case 8: { // Shift from top
			for (double i=-get_height(); i<0; i+=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(0, i, 0, f);
				render();
			}
			break;
		}
		case 9: { // Shift from bottom
			for (double i=get_height(); i>=0; i-=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(0, i, 0, f);
				render();
			}
			break;
		}
		case 10: { // Interlaced from left
			// Not yet implemented
			break;
		}
		case 11: { // Interlaced from right
			// Not yet implemented
			break;
		}
		case 12: { // Interlaced from top
			// Not yet implemented
			break;
		}
		case 13: { // Interlaced from bottom
			// Not yet implemented
			break;
		}
		case 14: { // Push from left
			int w = get_width();
			for (double i=-w; i<0; i+=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(i+w, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case 15: { // Push from right
			int w = get_width();
			for (double i=w; i>=0; i-=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(i-w, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case 16: { // Push from top
			int h = get_height();
			for (double i=-h; i<0; i+=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(i+h, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case 17: { // Push from bottom
			int h = get_height();
			for (double i=h; i>=0; i-=transition_speed*sqr(get_delta())) {
				render_clear();
				texture_before->draw(i-h, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case 18: { // Rotate to left
			// Not yet implemented
			break;
		}
		case 19: { // Rotate to right
			// Not yet implemented
			break;
		}
		case 20: { // Blend
			// Not yet implemented
			break;
		}
		case 21: { // Fade out and in
			// Not yet implemented
			break;
		}
		default: {
			transition_type = 0;
			break;
		}
	}

	for (auto& b : current_room->get_backgrounds()) {
		b.second->background->set_time_update();
	}

	return 0;
}

#endif // _BEE_GAME_TRANSITION
