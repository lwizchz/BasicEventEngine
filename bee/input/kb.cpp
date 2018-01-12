/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KB
#define BEE_INPUT_KB 1

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "kb.hpp"

#include "../util/string.hpp"

#include "../core/enginestate.hpp"

namespace bee { namespace kb {
	namespace internal {
		const Uint8* keystate;
	}

	/**
	* Initialize the keyboard state array.
	*/
	void init() {
		internal::keystate = SDL_GetKeyboardState(nullptr);
	}

	/**
	* @param sc the scancode to check
	* @returns whether the given scancode is pressed
	*/
	bool get_state(SDL_Scancode sc) {
		return static_cast<bool>(internal::keystate[sc]);
	}
	/**
	* @note If the function is called with SDL_Keycode, then convert it to an SDL_Scancode.
	* @param k the key to check
	* @returns whether the given keycode is pressed
	*/
	bool get_state(SDL_Keycode k) {
		return get_state(SDL_GetScancodeFromKey(k));
	}
	/**
	* @returns the modifier state bitmask
	* @see https://wiki.libsdl.org/SDL_Keymod for details
	*/
	SDL_Keymod get_mod_state() {
		return SDL_GetModState();
	}
	/**
	* @param k the modifier key to check
	* @returns whether the given modifier key is pressed
	* @see https://wiki.libsdl.org/SDL_GetModState for details
	*/
	bool get_mod_state(int k) {
		return (SDL_GetModState() & k);
	}

	/**
	* Append a keyboard event key to the given string
	* @param output the string to append to
	* @param k the keyboard event to append
	* @returns the newly added character
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
				s = "\t";
				break;
			}
			case SDLK_RETURN: {
				s = "\n";
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

	/**
	* @param key the key string
	* @returns the SDL keycode from the given string
	*/
	SDL_Keycode keystrings_get_key(const std::string& key) {
		return SDL_GetKeyFromName(key.substr(5).c_str());
	}
	/**
	* @param key the keycode
	* @returns the string of the given SDL keycode
	*/
	std::string keystrings_get_string(SDL_Keycode key) {
		std::string name (SDL_GetKeyName(key));
		return "SDLK_" + ((name.length() > 1) ? string_upper(name) : string_lower(name));
	}
	/**
	* @param key the keycode
	* @returns the name of the given SDL keycode
	*/
	std::string keystrings_get_name(SDL_Keycode key) {
		std::string keystring = keystrings_get_string(key);
		return string_upper(string_replace(keystring.substr(5), "_", " "));
	}
}}

#endif // BEE_INPUT_KB
