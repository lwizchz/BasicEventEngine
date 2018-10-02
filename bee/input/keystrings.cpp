/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KEYSTRINGS
#define BEE_INPUT_KEYSTRINGS 1

#include <unordered_map>

#include "keystrings.hpp"

#include "../util/string.hpp"

#define SK(x) {#x, x}

namespace bee { namespace kb {
	namespace internal {
		const std::unordered_map<std::string,SDL_Keycode> stringkeys {
			SK(SDLK_UNKNOWN),
			SK(SDLK_BACKSPACE),
			SK(SDLK_TAB),
			SK(SDLK_RETURN),
			SK(SDLK_ESCAPE),
			SK(SDLK_SPACE),
			SK(SDLK_EXCLAIM),
			SK(SDLK_QUOTEDBL),
			SK(SDLK_HASH),
			SK(SDLK_DOLLAR),
			SK(SDLK_PERCENT),
			SK(SDLK_AMPERSAND),
			SK(SDLK_QUOTE),
			SK(SDLK_LEFTPAREN),
			SK(SDLK_RIGHTPAREN),
			SK(SDLK_ASTERISK),
			SK(SDLK_PLUS),
			SK(SDLK_COMMA),
			SK(SDLK_MINUS),
			SK(SDLK_PERIOD),
			SK(SDLK_SLASH),
			SK(SDLK_0),
			SK(SDLK_1),
			SK(SDLK_2),
			SK(SDLK_3),
			SK(SDLK_4),
			SK(SDLK_5),
			SK(SDLK_6),
			SK(SDLK_7),
			SK(SDLK_8),
			SK(SDLK_9),
			SK(SDLK_COLON),
			SK(SDLK_SEMICOLON),
			SK(SDLK_LESS),
			SK(SDLK_EQUALS),
			SK(SDLK_GREATER),
			SK(SDLK_QUESTION),
			SK(SDLK_AT),
			SK(SDLK_LEFTBRACKET),
			SK(SDLK_BACKSLASH),
			SK(SDLK_RIGHTBRACKET),
			SK(SDLK_CARET),
			SK(SDLK_UNDERSCORE),
			SK(SDLK_BACKQUOTE),
			SK(SDLK_a),
			SK(SDLK_b),
			SK(SDLK_c),
			SK(SDLK_d),
			SK(SDLK_e),
			SK(SDLK_f),
			SK(SDLK_g),
			SK(SDLK_h),
			SK(SDLK_i),
			SK(SDLK_j),
			SK(SDLK_k),
			SK(SDLK_l),
			SK(SDLK_m),
			SK(SDLK_n),
			SK(SDLK_o),
			SK(SDLK_p),
			SK(SDLK_q),
			SK(SDLK_r),
			SK(SDLK_s),
			SK(SDLK_t),
			SK(SDLK_u),
			SK(SDLK_v),
			SK(SDLK_w),
			SK(SDLK_x),
			SK(SDLK_y),
			SK(SDLK_z),
			SK(SDLK_DELETE),
			SK(SDLK_CAPSLOCK),
			SK(SDLK_F1),
			SK(SDLK_F2),
			SK(SDLK_F3),
			SK(SDLK_F4),
			SK(SDLK_F5),
			SK(SDLK_F6),
			SK(SDLK_F7),
			SK(SDLK_F8),
			SK(SDLK_F9),
			SK(SDLK_F10),
			SK(SDLK_F11),
			SK(SDLK_F12),
			SK(SDLK_PRINTSCREEN),
			SK(SDLK_SCROLLLOCK),
			SK(SDLK_PAUSE),
			SK(SDLK_INSERT),
			SK(SDLK_HOME),
			SK(SDLK_PAGEUP),
			SK(SDLK_END),
			SK(SDLK_PAGEDOWN),
			SK(SDLK_RIGHT),
			SK(SDLK_LEFT),
			SK(SDLK_DOWN),
			SK(SDLK_UP),
			SK(SDLK_NUMLOCKCLEAR),
			SK(SDLK_KP_DIVIDE),
			SK(SDLK_KP_MULTIPLY),
			SK(SDLK_KP_MINUS),
			SK(SDLK_KP_PLUS),
			SK(SDLK_KP_ENTER),
			SK(SDLK_KP_1),
			SK(SDLK_KP_2),
			SK(SDLK_KP_3),
			SK(SDLK_KP_4),
			SK(SDLK_KP_5),
			SK(SDLK_KP_6),
			SK(SDLK_KP_7),
			SK(SDLK_KP_8),
			SK(SDLK_KP_9),
			SK(SDLK_KP_0),
			SK(SDLK_KP_PERIOD),
			SK(SDLK_APPLICATION),
			SK(SDLK_POWER),
			SK(SDLK_KP_EQUALS),
			SK(SDLK_F13),
			SK(SDLK_F14),
			SK(SDLK_F15),
			SK(SDLK_F16),
			SK(SDLK_F17),
			SK(SDLK_F18),
			SK(SDLK_F19),
			SK(SDLK_F20),
			SK(SDLK_F21),
			SK(SDLK_F22),
			SK(SDLK_F23),
			SK(SDLK_F24),
			SK(SDLK_EXECUTE),
			SK(SDLK_HELP),
			SK(SDLK_MENU),
			SK(SDLK_SELECT),
			SK(SDLK_STOP),
			SK(SDLK_AGAIN),
			SK(SDLK_UNDO),
			SK(SDLK_CUT),
			SK(SDLK_COPY),
			SK(SDLK_PASTE),
			SK(SDLK_FIND),
			SK(SDLK_MUTE),
			SK(SDLK_VOLUMEUP),
			SK(SDLK_VOLUMEDOWN),
			SK(SDLK_KP_COMMA),
			SK(SDLK_KP_EQUALSAS400),
			SK(SDLK_ALTERASE),
			SK(SDLK_SYSREQ),
			SK(SDLK_CANCEL),
			SK(SDLK_CLEAR),
			SK(SDLK_PRIOR),
			SK(SDLK_RETURN2),
			SK(SDLK_SEPARATOR),
			SK(SDLK_OUT),
			SK(SDLK_OPER),
			SK(SDLK_CLEARAGAIN),
			SK(SDLK_CRSEL),
			SK(SDLK_EXSEL),
			SK(SDLK_KP_00),
			SK(SDLK_KP_000),
			SK(SDLK_THOUSANDSSEPARATOR),
			SK(SDLK_DECIMALSEPARATOR),
			SK(SDLK_CURRENCYUNIT),
			SK(SDLK_CURRENCYSUBUNIT),
			SK(SDLK_KP_LEFTPAREN),
			SK(SDLK_KP_RIGHTPAREN),
			SK(SDLK_KP_LEFTBRACE),
			SK(SDLK_KP_RIGHTBRACE),
			SK(SDLK_KP_TAB),
			SK(SDLK_KP_BACKSPACE),
			SK(SDLK_KP_A),
			SK(SDLK_KP_B),
			SK(SDLK_KP_C),
			SK(SDLK_KP_D),
			SK(SDLK_KP_E),
			SK(SDLK_KP_F),
			SK(SDLK_KP_XOR),
			SK(SDLK_KP_POWER),
			SK(SDLK_KP_PERCENT),
			SK(SDLK_KP_LESS),
			SK(SDLK_KP_GREATER),
			SK(SDLK_KP_AMPERSAND),
			SK(SDLK_KP_DBLAMPERSAND),
			SK(SDLK_KP_VERTICALBAR),
			SK(SDLK_KP_DBLVERTICALBAR),
			SK(SDLK_KP_COLON),
			SK(SDLK_KP_HASH),
			SK(SDLK_KP_SPACE),
			SK(SDLK_KP_AT),
			SK(SDLK_KP_EXCLAM),
			SK(SDLK_KP_MEMSTORE),
			SK(SDLK_KP_MEMRECALL),
			SK(SDLK_KP_MEMCLEAR),
			SK(SDLK_KP_MEMADD),
			SK(SDLK_KP_MEMSUBTRACT),
			SK(SDLK_KP_MEMMULTIPLY),
			SK(SDLK_KP_MEMDIVIDE),
			SK(SDLK_KP_PLUSMINUS),
			SK(SDLK_KP_CLEAR),
			SK(SDLK_KP_CLEARENTRY),
			SK(SDLK_KP_BINARY),
			SK(SDLK_KP_OCTAL),
			SK(SDLK_KP_DECIMAL),
			SK(SDLK_KP_HEXADECIMAL),
			SK(SDLK_LCTRL),
			SK(SDLK_LSHIFT),
			SK(SDLK_LALT),
			SK(SDLK_LGUI),
			SK(SDLK_RCTRL),
			SK(SDLK_RSHIFT),
			SK(SDLK_RALT),
			SK(SDLK_RGUI),
			SK(SDLK_MODE),
			SK(SDLK_AUDIONEXT),
			SK(SDLK_AUDIOPREV),
			SK(SDLK_AUDIOSTOP),
			SK(SDLK_AUDIOPLAY),
			SK(SDLK_AUDIOMUTE),
			SK(SDLK_MEDIASELECT),
			SK(SDLK_WWW),
			SK(SDLK_MAIL),
			SK(SDLK_CALCULATOR),
			SK(SDLK_COMPUTER),
			SK(SDLK_AC_SEARCH),
			SK(SDLK_AC_HOME),
			SK(SDLK_AC_BACK),
			SK(SDLK_AC_FORWARD),
			SK(SDLK_AC_STOP),
			SK(SDLK_AC_REFRESH),
			SK(SDLK_AC_BOOKMARKS),
			SK(SDLK_BRIGHTNESSDOWN),
			SK(SDLK_BRIGHTNESSUP),
			SK(SDLK_DISPLAYSWITCH),
			SK(SDLK_KBDILLUMTOGGLE),
			SK(SDLK_KBDILLUMDOWN),
			SK(SDLK_KBDILLUMUP),
			SK(SDLK_EJECT),
			SK(SDLK_SLEEP)
		};

		auto ks() {
			std::unordered_map<SDL_Keycode,std::string> ksmap;
			for (auto& sk : stringkeys) {
				ksmap.emplace(sk.second, sk.first);
			}
			return ksmap;
		}
		const std::unordered_map<SDL_Keycode,std::string> keystrings (ks());
	}
	/**
	* @param str the key string
	* @returns the SDL keycode from the given string
	*/
	SDL_Keycode keystrings_get_key(const std::string& str) {
		auto k = internal::stringkeys.find(str);
		if (k == internal::stringkeys.end()) {
			return SDLK_UNKNOWN;
		}
		return k->second;
	}
	/**
	* @param key the keycode
	* @returns the string of the given SDL keycode
	*/
	std::string keystrings_get_string(SDL_Keycode key) {
		auto s = internal::keystrings.find(key);
		if (s == internal::keystrings.end()) {
			return "SDLK_UNKNOWN";
		}
		return s->second;
	}
}}

#undef SK

#endif // BEE_INPUT_KEYSTRINGS
