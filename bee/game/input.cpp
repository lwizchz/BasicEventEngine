/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_INPUT
#define _BEE_GAME_INPUT 1

#include "../game.hpp"

std::pair<int,int> BEE::get_mouse_global_position() {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	return std::make_pair(mx, my);
}
int BEE::get_mouse_global_x() {
	return get_mouse_global_position().first;
}
int BEE::get_mouse_global_y() {
	return get_mouse_global_position().second;
}
std::pair<int,int> BEE::get_mouse_position() {
	if (!get_current_room()->get_is_views_enabled()) {
		return get_mouse_global_position();
	}

	int mx, my;
	std::tie(mx, my) = get_mouse_global_position();
	ViewData* v = get_current_room()->get_current_view();

	if ((v == NULL)&&(get_current_room()->get_views().size() > 0)) {
		v = get_current_room()->get_views().begin()->second;
	}

	if (v != NULL) {
		mx -= v->port_x;
		my -= v->port_y;
	}

	return std::make_pair(mx, my);
}
int BEE::get_mouse_x() {
	return get_mouse_position().first;
}
int BEE::get_mouse_y() {
	return get_mouse_position().second;
}
int BEE::set_mouse_global_position(int new_mx, int new_my) {
	SDL_WarpMouseInWindow(window, new_mx, new_my);
	return 0;
}
int BEE::set_mouse_global_x(int new_mx) {
	return set_mouse_global_position(new_mx, get_mouse_global_y());
}
int BEE::set_mouse_global_y(int new_my) {
	return set_mouse_global_position(get_mouse_global_x(), new_my);
}

bool BEE::is_mouse_inside(InstanceData* instance) {
	SDL_Rect i = {(int)instance->x, (int)instance->y, instance->object->get_mask()->get_subimage_width(), instance->object->get_mask()->get_height()};
	SDL_Rect m = {get_mouse_x(), get_mouse_y(), 0, 0};
	return check_collision(&i, &m);
}

bool BEE::get_key_state(SDL_Scancode k) {
	return (bool)keystate[k];
}
bool BEE::get_key_state(SDL_Keycode k) {
	return get_key_state(SDL_GetScancodeFromKey(k));
}
bool BEE::get_mod_state(Uint8 k) {
	return (SDL_GetModState() & k);
}
bool BEE::get_mouse_state(Uint8 b) {
	return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(b));
}

char BEE::append_input(std::string& output, SDL_KeyboardEvent* k) {
	std::string s = "";
	switch (k->keysym.sym) {
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

		case SDLK_BACKSLASH: {
			s = "\\";
			if (k->keysym.mod & KMOD_SHIFT) {
				s = "|";
			}
			break;
		}
		case SDLK_BACKSPACE: {
			if (!output.empty()) {
				output.pop_back();
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
		case SDLK_SPACE: {
			s = " ";
			break;
		}
		case SDLK_TAB: {
			s = "	";
			break;
		}

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
	}

	if (s.empty()) {
		switch(k->keysym.sym) {
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
	}

	if ((s.empty())&&(k->keysym.mod & KMOD_NUM)) {
		switch(k->keysym.sym) {
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

	output += s;
	return s.c_str()[0];
}

#endif // _BEE_GAME_INPUT
