/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_WINDOW
#define _BEE_GAME_WINDOW 1

#include "../game.hpp"

bool BEE::get_is_visible() {
	return options->is_visible;
}
bool BEE::get_is_fullscreen() {
	return options->is_fullscreen;
}
bool BEE::get_is_borderless() {
	return options->is_borderless;
}
bool BEE::get_is_resizable() {
	return options->is_resizable;
}
std::string BEE::get_window_title() {
	return SDL_GetWindowTitle(window);
}
SDL_Cursor* BEE::get_cursor() {
	return cursor;
}
int BEE::get_window_x() {
	int wx;
	SDL_GetWindowPosition(window, &wx, NULL);
	return wx;
}
int BEE::get_window_y() {
	int wy;
	SDL_GetWindowPosition(window, NULL, &wy);
	return wy;
}
int BEE::get_width() {
	return width;
}
int BEE::get_height() {
	return height;
}
int BEE::set_is_visible(bool new_is_visible) {
	options->is_visible = new_is_visible;
	if (options->is_visible) {
		SDL_ShowWindow(window);
	} else {
		SDL_HideWindow(window);
	}
	return 0;
}
int BEE::set_is_fullscreen(bool new_is_fullscreen) {
	options->is_fullscreen = new_is_fullscreen;
	if (options->is_fullscreen) {
		if (options->is_resizable) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		} else {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		}
	} else {
		SDL_SetWindowFullscreen(window, 0);
	}
	return 0;
}
int BEE::set_window_title(std::string new_title) {
	SDL_SetWindowTitle(window, new_title.c_str());
	return 0;
}
int BEE::set_cursor(SDL_SystemCursor cid) {
	SDL_FreeCursor(cursor);
	cursor = SDL_CreateSystemCursor(cid);
	SDL_SetCursor(cursor);
	return 0;
}
int BEE::set_show_cursor(bool new_show_cursor) {
	SDL_ShowCursor((new_show_cursor) ? SDL_ENABLE : SDL_DISABLE);
	return 0;
}
int BEE::set_window_position(int new_x, int new_y) {
	SDL_SetWindowPosition(window, new_x, new_y);
	return 0;
}
int BEE::set_window_x(int new_x) {
	return set_window_position(new_x, get_window_y());
}
int BEE::set_window_y(int new_y) {
	return set_window_position(get_window_x(), new_y);
}
int BEE::set_window_center() {
	return set_window_position(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
int BEE::set_window_size(int new_width, int new_height) {
	width = new_width;
	height = new_height;
	SDL_SetWindowSize(window, width, height);
	return 0;
}
int BEE::set_width(int new_width) {
	return set_window_size(new_width, height);
}
int BEE::set_height(int new_height) {
	return set_window_size(width, new_height);
}

#endif // _BEE_GAME_WINDOW
