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
		SDL_SetRenderTarget(renderer, nullptr); // Reset the SDL render target
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
	if (sprite_target == nullptr) { // If the given target is nullptr then reset the render target
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
	if (background_target == nullptr) { // If the given target is nullptr then reset the render target
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
* BEE::get_transition_type() - Return the transition type for draw_transition()
*/
int BEE::get_transition_type() const {
	return transition_type;
}
/*
* BEE::set_transition_type() - Set the transition type for draw_transition()
* @new_type: the new transition type to use
*/
int BEE::set_transition_type(bee_transition_t new_type) {
	transition_type = new_type; // Set the new type
	return 0; // Return 0 on success
}
/*
* BEE::set_transition_custom() - Set the transition type to a custom function for draw_transition()
* @new_custom: the new custom transition type to use
*/
int BEE::set_transition_custom(std::function<void (BEE*, Sprite*, Sprite*)> new_custom) {
	transition_custom_func = new_custom; // Set the new custom function
	transition_type = BEE_TRANSITION_CUSTOM;
	return 0; // Return 0 on success
}
/*
* BEE::get_transition_speed() - Return the transition speed for draw_transition()
*/
double BEE::get_transition_speed() const {
	return transition_speed;
}
/*
* BEE::set_transition_speed() - Set the transition speed for draw_transition()
* @new_speed: the new transition speed to use
*/
int BEE::set_transition_speed(double new_speed) {
	transition_speed = new_speed/get_fps_goal(); // Set the new speed
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
		case BEE_TRANSITION_NONE: { // No transition
			break;
		}
		case BEE_TRANSITION_CREATE_LEFT: { // Create from left
			for (double i=0; i<get_width(); i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->crop_image_width(i);
				texture_after->draw(0, 0, 0, i, -1, 0.0, {255, 255, 255, 255}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_CREATE_RIGHT: { // Create from right
			for (double i=get_width(); i>=0; i-=transition_speed*get_delta()) {
				render_clear();
				texture_after->draw(0, 0, 0, f);
				texture_before->crop_image_width(i);
				texture_before->draw(0, 0, 0, i, -1, 0.0, {255, 255, 255, 255}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_CREATE_TOP: { // Create from top
			for (double i=0; i<get_height(); i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->crop_image_height(i);
				texture_after->draw(0, 0, 0, -1, i, 0.0, {255, 255, 255, 255}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_CREATE_BOTTOM: { // Create from bottom
			for (double i=get_height(); i>=0; i-=transition_speed*get_delta()) {
				render_clear();
				texture_after->draw(0, 0, 0, f);
				texture_before->crop_image_height(i);
				texture_before->draw(0, 0, 0, -1, i, 0.0, {255, 255, 255, 255}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_CREATE_CENTER: { // Create from center
			const int w = get_width(), h = get_height();
			for (double i=0; i<w; i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);

				double ih = i/w * h;
				int x = (w - i)/2;
				int y = (h - ih)/2;
				texture_after->crop_image({x, y, (int)i, (int)ih});
				texture_after->draw(x, y, 0, i, ih, 0.0, {255, 255, 255, 255}, f, false);

				render();
			}
			break;
		}
		case BEE_TRANSITION_SHIFT_LEFT: { // Shift from left
			for (double i=-get_width(); i<0; i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(i, 0, 0,  f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_SHIFT_RIGHT: { // Shift from right
			for (double i=get_width(); i>=0; i-=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_SHIFT_TOP: { // Shift from top
			for (double i=-get_height(); i<0; i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(0, i, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_SHIFT_BOTTOM: { // Shift from bottom
			for (double i=get_height(); i>=0; i-=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, 0, 0, f);
				texture_after->draw(0, i, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_INTERLACE_LEFT: { // Interlaced from left
			// Not yet implemented
			messenger_send({"engine"}, BEE_MESSAGE_WARNING, "The interlace transitions are not yet implemented.");
			break;
		}
		case BEE_TRANSITION_INTERLACE_RIGHT: { // Interlaced from right
			// Not yet implemented
			messenger_send({"engine"}, BEE_MESSAGE_WARNING, "The interlace transitions are not yet implemented.");
			break;
		}
		case BEE_TRANSITION_INTERLACE_TOP: { // Interlaced from top
			// Not yet implemented
			messenger_send({"engine"}, BEE_MESSAGE_WARNING, "The interlace transitions are not yet implemented.");
			break;
		}
		case BEE_TRANSITION_INTERLACE_BOTTOM: { // Interlaced from bottom
			// Not yet implemented
			messenger_send({"engine"}, BEE_MESSAGE_WARNING, "The interlace transitions are not yet implemented.");
			break;
		}
		case BEE_TRANSITION_PUSH_LEFT: { // Push from left
			const int w = get_width();
			for (double i=-w; i<0; i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(i+w, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_PUSH_RIGHT: { // Push from right
			const int w = get_width();
			for (double i=w; i>=0; i-=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(i-w, 0, 0, f);
				texture_after->draw(i, 0, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_PUSH_TOP: { // Push from top
			const int h = get_height();
			for (double i=-h; i<0; i+=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, i+h, 0, f);
				texture_after->draw(0, i, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_PUSH_BOTTOM: { // Push from bottom
			const int h = get_height();
			for (double i=h; i>=0; i-=transition_speed*get_delta()) {
				render_clear();
				texture_before->draw(0, i-h, 0, f);
				texture_after->draw(0, i, 0, f);
				render();
			}
			break;
		}
		case BEE_TRANSITION_ROTATE_LEFT: { // Rotate to left
			texture_before->set_rotate_xy(0.0, 1.0);
			texture_after->set_rotate_xy(0.0, 1.0);
			for (double a=0.0; a<90.0; a+=transition_speed*get_delta()/20.0) {
				render_clear();
				texture_before->draw(0, 0, 0, -1, -1, -a, {255, 255, 255, 255}, f, false);
				texture_after->draw(0, 0, 0, -1, -1, 90.0-a, {255, 255, 255, 255}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_ROTATE_RIGHT: { // Rotate to right
			texture_before->set_rotate_xy(1.0, 1.0);
			texture_after->set_rotate_xy(1.0, 1.0);
			for (double a=0.0; a<90.0; a+=transition_speed*get_delta()/20.0) {
				render_clear();
				texture_before->draw(0, 0, 0, -1, -1, a, {255, 255, 255, 255}, f, false);
				texture_after->draw(0, 0, 0, -1, -1, a-90.0, {255, 255, 255, 255}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_BLEND: { // Blend (crossfade)
			for (double a=0.0; a<255.0; a+=transition_speed*get_delta()/5.0) {
				render_clear();
				texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, (Uint8)(255.0-a)}, f, false);
				texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, (Uint8)a}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_FADE: { // Fade out and in
			for (double a=0.0; a<255.0; a+=transition_speed*get_delta()/5.0) {
				render_clear();
				texture_before->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, (Uint8)(255.0-a)}, f, false);
				render();
			}
			for (double a=0.0; a<255.0; a+=transition_speed*get_delta()/5.0) {
				render_clear();
				texture_after->draw(0, 0, 0, -1, -1, 0.0, {255, 255, 255, (Uint8)a}, f, false);
				render();
			}
			break;
		}
		case BEE_TRANSITION_CUSTOM: { // Run a custom transition
			if (transition_custom_func != nullptr) {
				transition_custom_func(this, texture_before, texture_after);
			}
			break;
		}
		default: {
			transition_type = BEE_TRANSITION_NONE;
			break;
		}
	}

	for (auto& b : current_room->get_backgrounds()) {
		b.second->background->set_time_update();
	}

	return 0;
}

#endif // _BEE_GAME_TRANSITION
