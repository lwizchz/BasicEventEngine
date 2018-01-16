/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_MOUSE_H
#define BEE_INPUT_MOUSE_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	// Forward declaration
	class Instance;
	class Texture;
namespace mouse {
		void init();
		void close();

		std::pair<int,int> get_display_pos();
		std::pair<int,int> get_pos();
		std::pair<int,int> get_relative_pos();
		int set_display_pos(int, int);
		void set_pos(int, int);

		bool is_inside(const Instance*);

		bool get_state(Uint8);
		int get_wheel_flip(const SDL_MouseWheelEvent&);

		// Cursor functions
		SDL_Cursor* get_cursor();
		int set_cursor(SDL_SystemCursor);
		int set_cursor(const Texture*, int, int);
		int set_show_cursor(bool);
}}

#endif // BEE_INPUT_MOUSE_H
