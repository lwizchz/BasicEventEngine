/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_TRANSITION
#define _BEE_GAME_TRANSITION 1

#include "../game.hpp"

int BEE::set_render_target(Sprite* sprite_target, int w, int h) {
	if (sprite_target == NULL) {
		SDL_SetRenderTarget(renderer, NULL);
	} else {
		sprite_target->set_as_target();
	}

	return 0;
}
int BEE::set_render_target(Sprite* sprite_target) {
	return set_render_target(sprite_target, get_width(), get_height());
}
int BEE::draw_transition() {
	switch (transition_type) {
		case 1: { // Create from left
			break;
		}
		case 2: { // Create from right
			break;
		}
		case 3: { // Create from top
			break;
		}
		case 4: { // Create from bottom
			break;
		}
		case 5: { // Create from center
			break;
		}
		case 6: { // Shift from left
			for (int i=-width; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 7: { // Shift from right
			for (int i=width; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 8: { // Shift from top
			for (int i=-height; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(0, i, 0);
				render();
			}
			break;
		}
		case 9: { // Shift from bottom
			for (int i=height; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(0, 0, 0);
				texture_after->draw(0, i, 0);
				render();
			}
			break;
		}
		case 10: { // Interlaced from left
			break;
		}
		case 11: { // Interlaced from right
			break;
		}
		case 12: { // Interlaced from top
			break;
		}
		case 13: { // Interlaced from bottom
			break;
		}
		case 14: { // Push from left
			for (int i=-width; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(i+width, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 15: { // Push from right
			for (int i=width; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(i-width, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 16: { // Push from top
			for (int i=-height; i<0; i+=transition_speed) {
				render_clear();
				texture_before->draw(i+height, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 17: { // Push from bottom
			for (int i=height; i>=0; i-=transition_speed) {
				render_clear();
				texture_before->draw(i-height, 0, 0);
				texture_after->draw(i, 0, 0);
				render();
			}
			break;
		}
		case 18: { // Rotate to left
			break;
		}
		case 19: { // Rotate to right
			break;
		}
		case 20: { // Blend
			break;
		}
		case 21: { // Fade out and in
			break;
		}

	}
	transition_type++;
	for (auto& b : current_room->get_backgrounds()) {
		b.second->background->set_time_update();
	}

	return 0;
}

#endif // _BEE_GAME_TRANSITION
