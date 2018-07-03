/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KB
#define BEE_INPUT_KB 1

#include <unordered_map>
#include <algorithm>

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "kb.hpp"

#include "../engine.hpp"

#include "../util/string.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/console.hpp"
#include "../core/enginestate.hpp"

#include "keybind.hpp"

#include "../render/transition.hpp"

namespace bee { namespace kb {
	namespace internal {
		const Uint8* keystate;

		std::map<std::string,KeyBind> allbinds;
		std::unordered_multimap<SDL_Keycode,KeyBind> bindings;
	}

	/**
	* Initialize the keyboard state array.
	*/
	void init() {
		internal::keystate = SDL_GetKeyboardState(nullptr);

		bee::kb::bind(SDLK_BACKQUOTE, bee::KeyBind("ConsoleToggle", [] (const SDL_Event* e) {
			console::toggle();
		}));
		bee::kb::bind(SDLK_ESCAPE, bee::KeyBind("Quit", [] (const SDL_Event* e) {
			messenger::send({"engine"}, E_MESSAGE::INFO, "Quitting...");
			set_transition_type(E_TRANSITION::NONE);
			end_game();
		}));
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
	char append_input(std::string* output, const SDL_KeyboardEvent* k) {
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
					s = util::string::lower(s);
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
		if (key == SDLK_UNKNOWN) {
			return "SDLK_UNKNOWN";
		}

		std::string name (SDL_GetKeyName(key));
		return "SDLK_" + ((name.length() > 1) ? util::string::upper(name) : util::string::lower(name));
	}
	/**
	* @param key the keycode
	* @returns the name of the given SDL keycode
	*/
	std::string keystrings_get_name(SDL_Keycode key) {
		std::string keystring = keystrings_get_string(key);
		return util::string::upper(util::string::replace(keystring.substr(5), "_", " "));
	}

	/**
	* Run the KeyBind callbacks for all bindings of the given event's key
	* @param e the key event to handle
	*/
	void internal::handle_input(const SDL_Event* e) {
		if (console::get_is_open()) {
			console::internal::handle_input(e);
			return;
		}

		auto binds = internal::bindings.equal_range(e->key.keysym.sym);
		std::for_each(binds.first, binds.second, [e] (auto& kb) {
			if ((!e->key.repeat)||(kb.second.is_repeatable)) {
				kb.second.call(e);
			}
		});
	}

	/**
	* Bind a key to a given KeyBind.
	* @param key the keycode to bind to
	* @param keybind the keybind to bind to
	*
	* @retval 0 success
	* @retval 1 failed since key is already bound
	*/
	int bind(SDL_Keycode key, KeyBind keybind) {
		internal::allbinds.emplace(keybind.name, keybind);

		if (key == SDLK_UNKNOWN) {
			return 0;
		}

		if (internal::bindings.find(key) != internal::bindings.end()) { // If the key has already been bound, output a warning
			messenger::send({"engine", "kb"}, E_MESSAGE::WARNING, "Failed to bind key \"" + kb::keystrings_get_string(key) + "\", the key is already bound.");
			return 1;
		}

		keybind.key = key;

		internal::bindings.emplace(key, keybind);

		return 0;
	}
	/**
	* Return a KeyBind that is bound to the given key.
	* @note Since multiple KeyBinds are allowed to be bound to the same key, this function may not return the same KeyBind everytime.
	* @param key the keycode to find the bind of
	*
	* @returns the bound keybind or an empty bind if none was found
	*/
	KeyBind get_keybind(SDL_Keycode key) {
		std::unordered_map<SDL_Keycode,KeyBind>::iterator bind (internal::bindings.find(key));
		if (bind == internal::bindings.end()) { // If the key has not been bound, then return an empty bind
			return KeyBind();
		}
		return bind->second;
	}
	/**
	* Return the Keybind with the given name.
	* @param name the name of the keybind to find
	*
	* @returns the keybind or an empty bind if none was found
	*/
	KeyBind get_keybind(const std::string& name) {
		for (auto& kb : internal::bindings) {
			if (kb.second.name == name) {
				return kb.second;
			}
		}

		if (internal::allbinds.find(name) != internal::allbinds.end()) {
			return internal::allbinds.at(name);
		}

		return KeyBind();
	}
	/**
	* Unbind a key from a KeyBind.
	* @param key the keycode to unbind
	*/
	void unbind(SDL_Keycode key) {
		if (internal::bindings.find(key) != internal::bindings.end()) {
			auto binds = internal::bindings.equal_range(key);
			std::for_each(binds.first, binds.second, [] (auto& kb) {
				internal::allbinds.at(kb.second.name).key = SDLK_UNKNOWN;
			});

			internal::bindings.erase(key);
		}
	}
	/**
	* Unbind a KeyBind from a key.
	* @param keybind the keybind to unbind
	*
	* @retval 0 success
	* @retval 1 failed to unbind since no key was bound
	*/
	int unbind(KeyBind keybind) {
		for (auto it=internal::bindings.begin(); it!=internal::bindings.end(); ++it) {
			if (it->second.name == keybind.name) {
				internal::allbinds.at(it->second.name).key = SDLK_UNKNOWN;

				internal::bindings.erase(it);
				return 0;
			}
		}

		return 1;
	}
	/**
	* Unbind all keys from their KeyBinds.
	*/
	void unbind_all() {
		for (auto& kb : internal::allbinds) {
			kb.second.key = SDLK_UNKNOWN;
		}

		internal::bindings.clear();
	}
}}

#endif // BEE_INPUT_KB
