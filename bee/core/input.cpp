/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_INPUT
#define BEE_CORE_INPUT 1

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "input.hpp"

#include "../util/collision.hpp"
#include "../util/string.hpp"

#include "enginestate.hpp"
#include "instance.hpp"
#include "room.hpp"

#include "../render/renderer.hpp"
#include "../render/viewdata.hpp"

#include "../resources/sprite.hpp"
#include "../resources/object.hpp"
#include "../resources/room.hpp"

namespace bee {
	/*
	* get_mouse_global_position() - Return the mouse coordinates relative to the window
	* ! See https://wiki.libsdl.org/SDL_GetMouseState for details
	*/
	std::pair<int,int> get_mouse_global_position() {
		int mx, my;
		SDL_GetMouseState(&mx, &my); // Fetch the mouse coordinates into (mx, my)
		return std::make_pair(mx, my); // Return the pair on success
	}
	/*
	* get_mouse_global_x() - Return the mouse x-coordinate relative to the window
	*/
	int get_mouse_global_x() {
		return get_mouse_global_position().first;
	}
	/*
	* get_mouse_global_y() - Return the mouse y-coordinate relative to the window
	*/
	int get_mouse_global_y() {
		return get_mouse_global_position().second;
	}
	/*
	* get_mouse_position() - Return the mouse coordinates relative to the viewport
	*/
	std::pair<int,int> get_mouse_position() {
		int mx, my;
		std::tie(mx, my) = get_mouse_global_position(); // Fetch the global coordinates into (mx, my)

		if (!get_current_room()->get_is_views_enabled()) { // If views are disabled, simply return the coordinates relative to the window
			return std::make_pair(mx, my); // Return the pair on success
		}

		ViewData* v = get_current_room()->get_current_view(); // Get the current view
		if ((v == nullptr)&&(get_current_room()->get_views().size() > 0)) { // If this function is called outside of view drawing then simply use the first view
			v = get_current_room()->get_views().front();
		}

		if (v != nullptr) { // If the view exists then adjust the coordinates
			mx -= v->port_x;
			my -= v->port_y;
		}

		return std::make_pair(mx, my); // Return the pair on success
	}
	/*
	* get_mouse_x() - Return the mouse x-coordinate relative to the viewport
	*/
	int get_mouse_x() {
		return get_mouse_position().first;
	}
	/*
	* get_mouse_y() - Return the mouse x-coordinate relative to the viewport
	*/
	int get_mouse_y() {
		return get_mouse_position().second;
	}
	/*
	* set_mouse_global_position() - Set the mouse coordinates relative to the window
	* ! See https://wiki.libsdl.org/SDL_WarpMouseInWindow for details
	* @new_mx: the x-coordinate to move the mouse to
	* @new_my: the y-coordinate to move the mouse to
	*/
	int set_mouse_global_position(int new_mx, int new_my) {
		SDL_WarpMouseInWindow(engine->renderer->window, new_mx, new_my); // Move the mouse to the given coordinates
		return 0;
	}
	/*
	* set_mouse_global_x() - Set the mouse x-coordinate relative to the window
	* @new_mx: the x-coordinate to move the mouse to
	*/
	int set_mouse_global_x(int new_mx) {
		return set_mouse_global_position(new_mx, get_mouse_global_y());
	}
	/*
	* set_mouse_global_y() - Set the mouse y-coordinate relative to the window
	* @new_my: the y-coordinate to move the mouse to
	*/
	int set_mouse_global_y(int new_my) {
		return set_mouse_global_position(get_mouse_global_x(), new_my);
	}

	/*
	* is_mouse_inside() - Return whether the mouse collides with the instance's bounding box
	* @instance: the instance to check a collision for
	*/
	bool is_mouse_inside(const Instance& instance) {
		Sprite* m = instance.get_object()->get_mask();
		if (m == nullptr) {
			return false;
		}

		SDL_Rect inst = {(int)instance.get_x(), (int)instance.get_y(), (int)m->get_width(), (int)m->get_height()}; // Create a bounding box based on the instance's mask
		SDL_Rect mouse = {get_mouse_x(), get_mouse_y(), 0, 0};
		return check_collision(inst, mouse); // Return whether the instance collides with the mouse
	}

	/*
	* get_key_state() - Return whether the given scancode is pressed
	* @k: the scancode to check
	*/
	bool get_key_state(SDL_Scancode k) {
		return (bool)engine->keystate[k]; // Return the keystate of the given key as a boolean value
	}
	/*
	* get_key_state() - Return whether the given keycode is pressed
	* ! When the function is called with a SDL_Keycode, simply convert it into an SDL_Scancode and call the function again
	* @k: the keycode to check
	*/
	bool get_key_state(SDL_Keycode k) {
		return get_key_state(SDL_GetScancodeFromKey(k));
	}
	/*
	* get_mod_state() - Return whether the given modifier key is pressed
	* ! See https://wiki.libsdl.org/SDL_GetModState for details
	* @k: the modifier key to check
	*/
	bool get_mod_state(Uint8 k) {
		return (SDL_GetModState() & k); // Return the keystate of the given modifier as a boolean value
	}
	/*
	* get_mouse_state() - Return whether the given mouse button is pressed
	* ! See https://wiki.libsdl.org/SDL_GetMouseState for details
	* @b: the given button to check
	*/
	bool get_mouse_state(Uint8 b) {
		return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(b)); // Return the button state of the given button as a boolean value
	}

	/*
	* append_input() - Append the given keyboard event's key to the given string and return the added character
	* @output: the string to append to
	* @k: the keyboard event to append
	*/
	char append_input(std::string* output, SDL_KeyboardEvent* k) {
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

	#define keystring(key) {#key, key}
	#define stringkey(key) {key, #key}
	/*
	* keystrings_populate() - Populate the map of keycodes
	*/
	int keystrings_populate() {
		if (engine->keystrings_keys.empty()) {
			engine->keystrings_keys = std::map<std::string,SDL_Keycode>{
				keystring(SDLK_UNKNOWN),
				keystring(SDLK_BACKSPACE),
				keystring(SDLK_TAB),
				keystring(SDLK_RETURN),
				keystring(SDLK_ESCAPE),
				keystring(SDLK_SPACE),
				keystring(SDLK_EXCLAIM),
				keystring(SDLK_QUOTEDBL),
				keystring(SDLK_HASH),
				keystring(SDLK_DOLLAR),
				keystring(SDLK_PERCENT),
				keystring(SDLK_AMPERSAND),
				keystring(SDLK_QUOTE),
				keystring(SDLK_LEFTPAREN),
				keystring(SDLK_RIGHTPAREN),
				keystring(SDLK_ASTERISK),
				keystring(SDLK_PLUS),
				keystring(SDLK_COMMA),
				keystring(SDLK_MINUS),
				keystring(SDLK_PERIOD),
				keystring(SDLK_SLASH),
				keystring(SDLK_0),
				keystring(SDLK_1),
				keystring(SDLK_2),
				keystring(SDLK_3),
				keystring(SDLK_4),
				keystring(SDLK_5),
				keystring(SDLK_6),
				keystring(SDLK_7),
				keystring(SDLK_8),
				keystring(SDLK_9),
				keystring(SDLK_COLON),
				keystring(SDLK_SEMICOLON),
				keystring(SDLK_LESS),
				keystring(SDLK_EQUALS),
				keystring(SDLK_GREATER),
				keystring(SDLK_QUESTION),
				keystring(SDLK_AT),
				keystring(SDLK_LEFTBRACKET),
				keystring(SDLK_BACKSLASH),
				keystring(SDLK_RIGHTBRACKET),
				keystring(SDLK_CARET),
				keystring(SDLK_UNDERSCORE),
				keystring(SDLK_BACKQUOTE),
				keystring(SDLK_a),
				keystring(SDLK_b),
				keystring(SDLK_c),
				keystring(SDLK_d),
				keystring(SDLK_e),
				keystring(SDLK_f),
				keystring(SDLK_g),
				keystring(SDLK_h),
				keystring(SDLK_i),
				keystring(SDLK_j),
				keystring(SDLK_k),
				keystring(SDLK_l),
				keystring(SDLK_m),
				keystring(SDLK_n),
				keystring(SDLK_o),
				keystring(SDLK_p),
				keystring(SDLK_q),
				keystring(SDLK_r),
				keystring(SDLK_s),
				keystring(SDLK_t),
				keystring(SDLK_u),
				keystring(SDLK_v),
				keystring(SDLK_w),
				keystring(SDLK_x),
				keystring(SDLK_y),
				keystring(SDLK_z),
				keystring(SDLK_DELETE),
				keystring(SDLK_CAPSLOCK),
				keystring(SDLK_F1),
				keystring(SDLK_F2),
				keystring(SDLK_F3),
				keystring(SDLK_F4),
				keystring(SDLK_F5),
				keystring(SDLK_F6),
				keystring(SDLK_F7),
				keystring(SDLK_F8),
				keystring(SDLK_F9),
				keystring(SDLK_F10),
				keystring(SDLK_F11),
				keystring(SDLK_F12),
				keystring(SDLK_PRINTSCREEN),
				keystring(SDLK_SCROLLLOCK),
				keystring(SDLK_PAUSE),
				keystring(SDLK_INSERT),
				keystring(SDLK_HOME),
				keystring(SDLK_PAGEUP),
				keystring(SDLK_END),
				keystring(SDLK_PAGEDOWN),
				keystring(SDLK_RIGHT),
				keystring(SDLK_LEFT),
				keystring(SDLK_DOWN),
				keystring(SDLK_UP),
				keystring(SDLK_NUMLOCKCLEAR),
				keystring(SDLK_KP_DIVIDE),
				keystring(SDLK_KP_MULTIPLY),
				keystring(SDLK_KP_MINUS),
				keystring(SDLK_KP_PLUS),
				keystring(SDLK_KP_ENTER),
				keystring(SDLK_KP_1),
				keystring(SDLK_KP_2),
				keystring(SDLK_KP_3),
				keystring(SDLK_KP_4),
				keystring(SDLK_KP_5),
				keystring(SDLK_KP_6),
				keystring(SDLK_KP_7),
				keystring(SDLK_KP_8),
				keystring(SDLK_KP_9),
				keystring(SDLK_KP_0),
				keystring(SDLK_KP_PERIOD),
				keystring(SDLK_APPLICATION),
				keystring(SDLK_POWER),
				keystring(SDLK_KP_EQUALS),
				keystring(SDLK_F13),
				keystring(SDLK_F14),
				keystring(SDLK_F15),
				keystring(SDLK_F16),
				keystring(SDLK_F17),
				keystring(SDLK_F18),
				keystring(SDLK_F19),
				keystring(SDLK_F20),
				keystring(SDLK_F21),
				keystring(SDLK_F22),
				keystring(SDLK_F23),
				keystring(SDLK_F24),
				keystring(SDLK_EXECUTE),
				keystring(SDLK_HELP),
				keystring(SDLK_MENU),
				keystring(SDLK_SELECT),
				keystring(SDLK_STOP),
				keystring(SDLK_AGAIN),
				keystring(SDLK_UNDO),
				keystring(SDLK_CUT),
				keystring(SDLK_COPY),
				keystring(SDLK_PASTE),
				keystring(SDLK_FIND),
				keystring(SDLK_MUTE),
				keystring(SDLK_VOLUMEUP),
				keystring(SDLK_VOLUMEDOWN),
				keystring(SDLK_KP_COMMA),
				keystring(SDLK_KP_EQUALSAS400),
				keystring(SDLK_ALTERASE),
				keystring(SDLK_SYSREQ),
				keystring(SDLK_CANCEL),
				keystring(SDLK_CLEAR),
				keystring(SDLK_PRIOR),
				keystring(SDLK_RETURN2),
				keystring(SDLK_SEPARATOR),
				keystring(SDLK_OUT),
				keystring(SDLK_OPER),
				keystring(SDLK_CLEARAGAIN),
				keystring(SDLK_CRSEL),
				keystring(SDLK_EXSEL),
				keystring(SDLK_KP_00),
				keystring(SDLK_KP_000),
				keystring(SDLK_THOUSANDSSEPARATOR),
				keystring(SDLK_DECIMALSEPARATOR),
				keystring(SDLK_CURRENCYUNIT),
				keystring(SDLK_CURRENCYSUBUNIT),
				keystring(SDLK_KP_LEFTPAREN),
				keystring(SDLK_KP_RIGHTPAREN),
				keystring(SDLK_KP_LEFTBRACE),
				keystring(SDLK_KP_RIGHTBRACE),
				keystring(SDLK_KP_TAB),
				keystring(SDLK_KP_BACKSPACE),
				keystring(SDLK_KP_A),
				keystring(SDLK_KP_B),
				keystring(SDLK_KP_C),
				keystring(SDLK_KP_D),
				keystring(SDLK_KP_E),
				keystring(SDLK_KP_F),
				keystring(SDLK_KP_XOR),
				keystring(SDLK_KP_POWER),
				keystring(SDLK_KP_PERCENT),
				keystring(SDLK_KP_LESS),
				keystring(SDLK_KP_GREATER),
				keystring(SDLK_KP_AMPERSAND),
				keystring(SDLK_KP_DBLAMPERSAND),
				keystring(SDLK_KP_VERTICALBAR),
				keystring(SDLK_KP_DBLVERTICALBAR),
				keystring(SDLK_KP_COLON),
				keystring(SDLK_KP_HASH),
				keystring(SDLK_KP_SPACE),
				keystring(SDLK_KP_AT),
				keystring(SDLK_KP_EXCLAM),
				keystring(SDLK_KP_MEMSTORE),
				keystring(SDLK_KP_MEMRECALL),
				keystring(SDLK_KP_MEMCLEAR),
				keystring(SDLK_KP_MEMADD),
				keystring(SDLK_KP_MEMSUBTRACT),
				keystring(SDLK_KP_MEMMULTIPLY),
				keystring(SDLK_KP_MEMDIVIDE),
				keystring(SDLK_KP_PLUSMINUS),
				keystring(SDLK_KP_CLEAR),
				keystring(SDLK_KP_CLEARENTRY),
				keystring(SDLK_KP_BINARY),
				keystring(SDLK_KP_OCTAL),
				keystring(SDLK_KP_DECIMAL),
				keystring(SDLK_KP_HEXADECIMAL),
				keystring(SDLK_LCTRL),
				keystring(SDLK_LSHIFT),
				keystring(SDLK_LALT),
				keystring(SDLK_LGUI),
				keystring(SDLK_RCTRL),
				keystring(SDLK_RSHIFT),
				keystring(SDLK_RALT),
				keystring(SDLK_RGUI),
				keystring(SDLK_MODE),
				keystring(SDLK_AUDIONEXT),
				keystring(SDLK_AUDIOPREV),
				keystring(SDLK_AUDIOSTOP),
				keystring(SDLK_AUDIOPLAY),
				keystring(SDLK_AUDIOMUTE),
				keystring(SDLK_MEDIASELECT),
				keystring(SDLK_WWW),
				keystring(SDLK_MAIL),
				keystring(SDLK_CALCULATOR),
				keystring(SDLK_COMPUTER),
				keystring(SDLK_AC_SEARCH),
				keystring(SDLK_AC_HOME),
				keystring(SDLK_AC_BACK),
				keystring(SDLK_AC_FORWARD),
				keystring(SDLK_AC_STOP),
				keystring(SDLK_AC_REFRESH),
				keystring(SDLK_AC_BOOKMARKS),
				keystring(SDLK_BRIGHTNESSDOWN),
				keystring(SDLK_BRIGHTNESSUP),
				keystring(SDLK_DISPLAYSWITCH),
				keystring(SDLK_KBDILLUMTOGGLE),
				keystring(SDLK_KBDILLUMDOWN),
				keystring(SDLK_KBDILLUMUP),
				keystring(SDLK_EJECT),
				keystring(SDLK_SLEEP)
			};
		}
		if (engine->keystrings_strings.empty()) {
			engine->keystrings_strings = std::map<SDL_Keycode,std::string>{
				stringkey(SDLK_UNKNOWN),
				stringkey(SDLK_BACKSPACE),
				stringkey(SDLK_TAB),
				stringkey(SDLK_RETURN),
				stringkey(SDLK_ESCAPE),
				stringkey(SDLK_SPACE),
				stringkey(SDLK_EXCLAIM),
				stringkey(SDLK_QUOTEDBL),
				stringkey(SDLK_HASH),
				stringkey(SDLK_DOLLAR),
				stringkey(SDLK_PERCENT),
				stringkey(SDLK_AMPERSAND),
				stringkey(SDLK_QUOTE),
				stringkey(SDLK_LEFTPAREN),
				stringkey(SDLK_RIGHTPAREN),
				stringkey(SDLK_ASTERISK),
				stringkey(SDLK_PLUS),
				stringkey(SDLK_COMMA),
				stringkey(SDLK_MINUS),
				stringkey(SDLK_PERIOD),
				stringkey(SDLK_SLASH),
				stringkey(SDLK_0),
				stringkey(SDLK_1),
				stringkey(SDLK_2),
				stringkey(SDLK_3),
				stringkey(SDLK_4),
				stringkey(SDLK_5),
				stringkey(SDLK_6),
				stringkey(SDLK_7),
				stringkey(SDLK_8),
				stringkey(SDLK_9),
				stringkey(SDLK_COLON),
				stringkey(SDLK_SEMICOLON),
				stringkey(SDLK_LESS),
				stringkey(SDLK_EQUALS),
				stringkey(SDLK_GREATER),
				stringkey(SDLK_QUESTION),
				stringkey(SDLK_AT),
				stringkey(SDLK_LEFTBRACKET),
				stringkey(SDLK_BACKSLASH),
				stringkey(SDLK_RIGHTBRACKET),
				stringkey(SDLK_CARET),
				stringkey(SDLK_UNDERSCORE),
				stringkey(SDLK_BACKQUOTE),
				stringkey(SDLK_a),
				stringkey(SDLK_b),
				stringkey(SDLK_c),
				stringkey(SDLK_d),
				stringkey(SDLK_e),
				stringkey(SDLK_f),
				stringkey(SDLK_g),
				stringkey(SDLK_h),
				stringkey(SDLK_i),
				stringkey(SDLK_j),
				stringkey(SDLK_k),
				stringkey(SDLK_l),
				stringkey(SDLK_m),
				stringkey(SDLK_n),
				stringkey(SDLK_o),
				stringkey(SDLK_p),
				stringkey(SDLK_q),
				stringkey(SDLK_r),
				stringkey(SDLK_s),
				stringkey(SDLK_t),
				stringkey(SDLK_u),
				stringkey(SDLK_v),
				stringkey(SDLK_w),
				stringkey(SDLK_x),
				stringkey(SDLK_y),
				stringkey(SDLK_z),
				stringkey(SDLK_DELETE),
				stringkey(SDLK_CAPSLOCK),
				stringkey(SDLK_F1),
				stringkey(SDLK_F2),
				stringkey(SDLK_F3),
				stringkey(SDLK_F4),
				stringkey(SDLK_F5),
				stringkey(SDLK_F6),
				stringkey(SDLK_F7),
				stringkey(SDLK_F8),
				stringkey(SDLK_F9),
				stringkey(SDLK_F10),
				stringkey(SDLK_F11),
				stringkey(SDLK_F12),
				stringkey(SDLK_PRINTSCREEN),
				stringkey(SDLK_SCROLLLOCK),
				stringkey(SDLK_PAUSE),
				stringkey(SDLK_INSERT),
				stringkey(SDLK_HOME),
				stringkey(SDLK_PAGEUP),
				stringkey(SDLK_END),
				stringkey(SDLK_PAGEDOWN),
				stringkey(SDLK_RIGHT),
				stringkey(SDLK_LEFT),
				stringkey(SDLK_DOWN),
				stringkey(SDLK_UP),
				stringkey(SDLK_NUMLOCKCLEAR),
				stringkey(SDLK_KP_DIVIDE),
				stringkey(SDLK_KP_MULTIPLY),
				stringkey(SDLK_KP_MINUS),
				stringkey(SDLK_KP_PLUS),
				stringkey(SDLK_KP_ENTER),
				stringkey(SDLK_KP_1),
				stringkey(SDLK_KP_2),
				stringkey(SDLK_KP_3),
				stringkey(SDLK_KP_4),
				stringkey(SDLK_KP_5),
				stringkey(SDLK_KP_6),
				stringkey(SDLK_KP_7),
				stringkey(SDLK_KP_8),
				stringkey(SDLK_KP_9),
				stringkey(SDLK_KP_0),
				stringkey(SDLK_KP_PERIOD),
				stringkey(SDLK_APPLICATION),
				stringkey(SDLK_POWER),
				stringkey(SDLK_KP_EQUALS),
				stringkey(SDLK_F13),
				stringkey(SDLK_F14),
				stringkey(SDLK_F15),
				stringkey(SDLK_F16),
				stringkey(SDLK_F17),
				stringkey(SDLK_F18),
				stringkey(SDLK_F19),
				stringkey(SDLK_F20),
				stringkey(SDLK_F21),
				stringkey(SDLK_F22),
				stringkey(SDLK_F23),
				stringkey(SDLK_F24),
				stringkey(SDLK_EXECUTE),
				stringkey(SDLK_HELP),
				stringkey(SDLK_MENU),
				stringkey(SDLK_SELECT),
				stringkey(SDLK_STOP),
				stringkey(SDLK_AGAIN),
				stringkey(SDLK_UNDO),
				stringkey(SDLK_CUT),
				stringkey(SDLK_COPY),
				stringkey(SDLK_PASTE),
				stringkey(SDLK_FIND),
				stringkey(SDLK_MUTE),
				stringkey(SDLK_VOLUMEUP),
				stringkey(SDLK_VOLUMEDOWN),
				stringkey(SDLK_KP_COMMA),
				stringkey(SDLK_KP_EQUALSAS400),
				stringkey(SDLK_ALTERASE),
				stringkey(SDLK_SYSREQ),
				stringkey(SDLK_CANCEL),
				stringkey(SDLK_CLEAR),
				stringkey(SDLK_PRIOR),
				stringkey(SDLK_RETURN2),
				stringkey(SDLK_SEPARATOR),
				stringkey(SDLK_OUT),
				stringkey(SDLK_OPER),
				stringkey(SDLK_CLEARAGAIN),
				stringkey(SDLK_CRSEL),
				stringkey(SDLK_EXSEL),
				stringkey(SDLK_KP_00),
				stringkey(SDLK_KP_000),
				stringkey(SDLK_THOUSANDSSEPARATOR),
				stringkey(SDLK_DECIMALSEPARATOR),
				stringkey(SDLK_CURRENCYUNIT),
				stringkey(SDLK_CURRENCYSUBUNIT),
				stringkey(SDLK_KP_LEFTPAREN),
				stringkey(SDLK_KP_RIGHTPAREN),
				stringkey(SDLK_KP_LEFTBRACE),
				stringkey(SDLK_KP_RIGHTBRACE),
				stringkey(SDLK_KP_TAB),
				stringkey(SDLK_KP_BACKSPACE),
				stringkey(SDLK_KP_A),
				stringkey(SDLK_KP_B),
				stringkey(SDLK_KP_C),
				stringkey(SDLK_KP_D),
				stringkey(SDLK_KP_E),
				stringkey(SDLK_KP_F),
				stringkey(SDLK_KP_XOR),
				stringkey(SDLK_KP_POWER),
				stringkey(SDLK_KP_PERCENT),
				stringkey(SDLK_KP_LESS),
				stringkey(SDLK_KP_GREATER),
				stringkey(SDLK_KP_AMPERSAND),
				stringkey(SDLK_KP_DBLAMPERSAND),
				stringkey(SDLK_KP_VERTICALBAR),
				stringkey(SDLK_KP_DBLVERTICALBAR),
				stringkey(SDLK_KP_COLON),
				stringkey(SDLK_KP_HASH),
				stringkey(SDLK_KP_SPACE),
				stringkey(SDLK_KP_AT),
				stringkey(SDLK_KP_EXCLAM),
				stringkey(SDLK_KP_MEMSTORE),
				stringkey(SDLK_KP_MEMRECALL),
				stringkey(SDLK_KP_MEMCLEAR),
				stringkey(SDLK_KP_MEMADD),
				stringkey(SDLK_KP_MEMSUBTRACT),
				stringkey(SDLK_KP_MEMMULTIPLY),
				stringkey(SDLK_KP_MEMDIVIDE),
				stringkey(SDLK_KP_PLUSMINUS),
				stringkey(SDLK_KP_CLEAR),
				stringkey(SDLK_KP_CLEARENTRY),
				stringkey(SDLK_KP_BINARY),
				stringkey(SDLK_KP_OCTAL),
				stringkey(SDLK_KP_DECIMAL),
				stringkey(SDLK_KP_HEXADECIMAL),
				stringkey(SDLK_LCTRL),
				stringkey(SDLK_LSHIFT),
				stringkey(SDLK_LALT),
				stringkey(SDLK_LGUI),
				stringkey(SDLK_RCTRL),
				stringkey(SDLK_RSHIFT),
				stringkey(SDLK_RALT),
				stringkey(SDLK_RGUI),
				stringkey(SDLK_MODE),
				stringkey(SDLK_AUDIONEXT),
				stringkey(SDLK_AUDIOPREV),
				stringkey(SDLK_AUDIOSTOP),
				stringkey(SDLK_AUDIOPLAY),
				stringkey(SDLK_AUDIOMUTE),
				stringkey(SDLK_MEDIASELECT),
				stringkey(SDLK_WWW),
				stringkey(SDLK_MAIL),
				stringkey(SDLK_CALCULATOR),
				stringkey(SDLK_COMPUTER),
				stringkey(SDLK_AC_SEARCH),
				stringkey(SDLK_AC_HOME),
				stringkey(SDLK_AC_BACK),
				stringkey(SDLK_AC_FORWARD),
				stringkey(SDLK_AC_STOP),
				stringkey(SDLK_AC_REFRESH),
				stringkey(SDLK_AC_BOOKMARKS),
				stringkey(SDLK_BRIGHTNESSDOWN),
				stringkey(SDLK_BRIGHTNESSUP),
				stringkey(SDLK_DISPLAYSWITCH),
				stringkey(SDLK_KBDILLUMTOGGLE),
				stringkey(SDLK_KBDILLUMDOWN),
				stringkey(SDLK_KBDILLUMUP),
				stringkey(SDLK_EJECT),
				stringkey(SDLK_SLEEP)
			};
		}

		return 0;
	}
	#undef keystring
	#undef stringkey
	/*
	* keystrings_get_key() - Return the SDL keycode from the given string
	*/
	SDL_Keycode keystrings_get_key(const std::string& key) {
		if (engine->keystrings_keys.empty()) {
			keystrings_populate();
		}

		if (engine->keystrings_keys.find(key) != engine->keystrings_keys.end()) {
			return engine->keystrings_keys[key];
		}

		return SDLK_UNKNOWN;
	}
	/*
	* keystrings_get_string() - Return the SDL keycode from the given string
	*/
	std::string keystrings_get_string(SDL_Keycode key) {
		if (engine->keystrings_strings.empty()) {
			keystrings_populate();
		}

		if (engine->keystrings_strings.find(key) != engine->keystrings_strings.end()) {
			return engine->keystrings_strings[key];
		}

		return "SDLK_UNKNOWN";
	}
}

#endif // BEE_CORE_INPUT
