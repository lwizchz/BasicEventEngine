/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_INPUT
#define _BEE_GAME_INPUT 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::get_mouse_global_position() - Return the mouse coordinates relative to the window
* ! See https://wiki.libsdl.org/SDL_GetMouseState for details
*/
std::pair<int,int> BEE::get_mouse_global_position() const {
	int mx, my;
	SDL_GetMouseState(&mx, &my); // Fetch the mouse coordinates into (mx, my)

	double xscale = (double)get_room_width()/get_width(); // Scale the coordinates if the room size if different from the window
	double yscale = (double)get_room_height()/get_height();

	return std::make_pair(mx*xscale, my*yscale); // Return the pair on success
}
/*
* BEE::get_mouse_global_x() - Return the mouse x-coordinate relative to the window
*/
int BEE::get_mouse_global_x() const {
	return get_mouse_global_position().first;
}
/*
* BEE::get_mouse_global_y() - Return the mouse y-coordinate relative to the window
*/
int BEE::get_mouse_global_y() const {
	return get_mouse_global_position().second;
}
/*
* BEE::get_mouse_position() - Return the mouse coordinates relative to the viewport
*/
std::pair<int,int> BEE::get_mouse_position() const {
	if (!get_current_room()->get_is_views_enabled()) { // If views are disabled, simply return the coordinates relative to the window
		return get_mouse_global_position();
	}

	int mx, my;
	std::tie(mx, my) = get_mouse_global_position(); // Fetch the global coordinates into (mx, my)

	ViewData* v = get_current_room()->get_current_view(); // Get the current view
	if ((v == NULL)&&(get_current_room()->get_views().size() > 0)) { // If this function is called outside of view drawing then simply use the first view
		v = get_current_room()->get_views().begin()->second;
	}

	if (v != NULL) { // If the view exists then adjust the coordinates
		mx -= v->port_x;
		my -= v->port_y;
	}

	return std::make_pair(mx, my); // Return the pair on success
}
/*
* BEE::get_mouse_x() - Return the mouse x-coordinate relative to the viewport
*/
int BEE::get_mouse_x() const {
	return get_mouse_position().first;
}
/*
* BEE::get_mouse_y() - Return the mouse x-coordinate relative to the viewport
*/
int BEE::get_mouse_y() const {
	return get_mouse_position().second;
}
/*
* BEE::set_mouse_global_position() - Set the mouse coordinates relative to the window
* ! See https://wiki.libsdl.org/SDL_WarpMouseInWindow for details
* @new_mx: the x-coordinate to move the mouse to
* @new_my: the y-coordinate to move the mouse to
*/
int BEE::set_mouse_global_position(int new_mx, int new_my) const {
	SDL_WarpMouseInWindow(window, new_mx, new_my); // Move the mouse to the given coordinates
	return 0;
}
/*
* BEE::set_mouse_global_x() - Set the mouse x-coordinate relative to the window
* @new_mx: the x-coordinate to move the mouse to
*/
int BEE::set_mouse_global_x(int new_mx) const {
	return set_mouse_global_position(new_mx, get_mouse_global_y());
}
/*
* BEE::set_mouse_global_y() - Set the mouse y-coordinate relative to the window
* @new_my: the y-coordinate to move the mouse to
*/
int BEE::set_mouse_global_y(int new_my) const {
	return set_mouse_global_position(get_mouse_global_x(), new_my);
}

/*
* BEE::is_mouse_inside() - Return whether the mouse collides with the instance's bounding box
* @instance: the instance to check a collision for
*/
bool BEE::is_mouse_inside(const InstanceData& instance) const {
	SDL_Rect i = {(int)instance.x, (int)instance.y, (int)instance.mask.w, (int)instance.mask.h}; // Create a bounding box based on the instance's CollisionPolygon
	SDL_Rect m = {get_mouse_x(), get_mouse_y(), 0, 0};
	return check_collision(i, m); // Return whether the instance collides with the mouse
}

/*
* BEE::get_key_state() - Return whether the given scancode is pressed
* @k: the scancode to check
*/
bool BEE::get_key_state(SDL_Scancode k) const {
	return (bool)keystate[k]; // Return the keystate of the given key as a boolean value
}
/*
* BEE::get_key_state() - Return whether the given keycode is pressed
* ! When the function is called with a SDL_Keycode, simply convert it into an SDL_Scancode and call the function again
* @k: the keycode to check
*/
bool BEE::get_key_state(SDL_Keycode k) const {
	return get_key_state(SDL_GetScancodeFromKey(k));
}
/*
* BEE::get_mod_state() - Return whether the given modifier key is pressed
* ! See https://wiki.libsdl.org/SDL_GetModState for details
* @k: the modifier key to check
*/
bool BEE::get_mod_state(Uint8 k) const {
	return (SDL_GetModState() & k); // Return the keystate of the given modifier as a boolean value
}
/*
* BEE::get_mouse_state() - Return whether the given mouse button is pressed
* ! See https://wiki.libsdl.org/SDL_GetMouseState for details
* @b: the given button to check
*/
bool BEE::get_mouse_state(Uint8 b) const {
	return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(b)); // Return the button state of the given button as a boolean value
}

/*
* BEE::append_input() - Append the given keyboard event's key to the given string and return the added character
* @output: the string to append to
* @k: the keyboard event to append
*/
char BEE::append_input(std::string* output, SDL_KeyboardEvent* k) {
	std::string s = "";
	switch (k->keysym.sym) {
		// Handle capitalization for all alphabetical characters based on shift and caps lock
		case SDLK_a:
		case SDLK_b:
		case SDLK_c:
		case SDLK_d:
		case SDLK_e:
		case SDLK_f:
		case SDLK_g:
		case SDLK_h:
		case SDLK_i:
		case SDLK_j:
		case SDLK_k:
		case SDLK_l:
		case SDLK_m:
		case SDLK_n:
		case SDLK_o:
		case SDLK_p:
		case SDLK_q:
		case SDLK_r:
		case SDLK_s:
		case SDLK_t:
		case SDLK_u:
		case SDLK_v:
		case SDLK_w:
		case SDLK_x:
		case SDLK_y:
		case SDLK_z: {
			s = SDL_GetKeyName(k->keysym.sym);

			bool should_capitalize = false;
			if (k->keysym.mod & KMOD_SHIFT) {
				should_capitalize = true;
			}
			if (k->keysym.mod & KMOD_CAPS) {
				should_capitalize = !should_capitalize;
			}
			if (!should_capitalize) {
				s = string_lower(s);
			}

			break;
		}

		// Handle backspace
		case SDLK_BACKSPACE: {
			if (!output->empty()) {
				output->pop_back();
			}
			break;
		}

		// Handle spacing keys
		case SDLK_SPACE: {
			s = " ";
			break;
		}
		case SDLK_TAB: {
			s = "	";
			break;
		}

		// Handle numpad keys which aren't effected by numlock
		case SDLK_KP_DIVIDE: {
			s = "/";
			break;
		}
		case SDLK_KP_MULTIPLY: {
			s = "*";
			break;
		}
		case SDLK_KP_MINUS: {
			s = "-";
			break;
		}
		case SDLK_KP_PLUS: {
			s = "+";
			break;
		}

		// Handle shifted numbers
		case SDLK_0: {
			s = "0";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = ")";
			}
			break;
		}
		case SDLK_1: {
			s = "1";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "!";
			}
			break;
		}
		case SDLK_2: {
			s = "2";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "@";
			}
			break;
		}
		case SDLK_3: {
			s = "3";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "#";
			}
			break;
		}
		case SDLK_4: {
			s = "4";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "$";
			}
			break;
		}
		case SDLK_5: {
			s = "5";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "%";
			}
			break;
		}
		case SDLK_6: {
			s = "6";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "^";
			}
			break;
		}
		case SDLK_7: {
			s = "7";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "&";
			}
			break;
		}
		case SDLK_8: {
			s = "8";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "*";
			}
			break;
		}
		case SDLK_9: {
			s = "9";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "(";
			}
			break;
		}

		// Handle shifted symbols
		case SDLK_BACKSLASH: {
			s = "\\";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "|";
			}
			break;
		}
		case SDLK_BACKQUOTE: {
			s = "`";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "~";
			}
			break;
		}
		case SDLK_QUOTE: {
			s = "'";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "\"";
			}
			break;
		}
		case SDLK_COMMA: {
			s = ",";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "<";
			}
			break;
		}
		case SDLK_EQUALS: {
			s = "=";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "+";
			}
			break;
		}
		case SDLK_LEFTBRACKET: {
			s = "[";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "{";
			}
			break;
		}
		case SDLK_MINUS: {
			s = "-";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "_";
			}
			break;
		}
		case SDLK_PERIOD: {
			s = ".";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = ">";
			}
			break;
		}
		case SDLK_RIGHTBRACKET: {
			s = "]";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "}";
			}
			break;
		}
		case SDLK_SEMICOLON: {
			s = ";";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = ":";
			}
			break;
		}
		case SDLK_SLASH: {
			s = "/";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "?";
			}
			break;
		}
	}

	if ((s.empty())&&(k->keysym.mod & KMOD_NUM)) {
		switch(k->keysym.sym) {
			// Handle the numpad
			case SDLK_KP_0:
			case SDLK_KP_00:
			case SDLK_KP_000: {
				s = "0";
				break;
			}
			case SDLK_KP_1: {
				s = "1";
				break;
			}
			case SDLK_KP_2: {
				s = "2";
				break;
			}
			case SDLK_KP_3: {
				s = "3";
				break;
			}
			case SDLK_KP_4: {
				s = "4";
				break;
			}
			case SDLK_KP_5: {
				s = "5";
				break;
			}
			case SDLK_KP_6: {
				s = "6";
				break;
			}
			case SDLK_KP_7: {
				s = "7";
				break;
			}
			case SDLK_KP_8: {
				s = "8";
				break;
			}
			case SDLK_KP_9: {
				s = "9";
				break;
			}
			case SDLK_KP_PERIOD: {
				s = ".";
				break;
			}
		}
	}

	output->append(s); // Append the character to the given string
	return s.c_str()[0]; // Return the character
}

#endif // _BEE_GAME_INPUT
