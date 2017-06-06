/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_WINDOW
#define _BEE_CORE_WINDOW 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::get_window_title() - Return the current window title string
* ! See https://wiki.libsdl.org/SDL_GetWindowTitle for details
*/
std::string BEE::get_window_title() const {
	return SDL_GetWindowTitle(renderer->window);
}
/*
* BEE::get_cursor() - Return the current window cursor
*/
SDL_Cursor* BEE::get_cursor()  const {
	return cursor;
}
/*
* BEE::get_window_x() - Return the x-coordinate of the game window
*/
int BEE::get_window_x() const {
	int wx;
	SDL_GetWindowPosition(renderer->window, &wx, nullptr);
	return wx;
}
/*
* BEE::get_window_y() - Return the y-coordinate of the game window
*/
int BEE::get_window_y() const {
	int wy;
	SDL_GetWindowPosition(renderer->window, nullptr, &wy);
	return wy;
}
/*
* BEE::get_width() - Return the width of the game window
*/
int BEE::get_width() const {
	return width;
}
/*
* BEE::get_height() - Return the height of the game window
*/
int BEE::get_height() const {
	return height;
}

/*
* BEE::set_window_title() - Set the title string of the current window
* @new_title: the string to set the title to
*/
int BEE::set_window_title(const std::string& new_title) const {
	SDL_SetWindowTitle(renderer->window, new_title.c_str());
	return 0;
}
/*
* BEE::set_cursor() - Change the current window cursor to the given type
* ! See https://wiki.libsdl.org/SDL_CreateSystemCursor for details
* @cid: the SDL system cursor enum id
*/
int BEE::set_cursor(SDL_SystemCursor cid) {
	SDL_FreeCursor(cursor);
	cursor = SDL_CreateSystemCursor(cid);
	SDL_SetCursor(cursor);
	return 0;
}
/*
* BEE::set_show_cursor() - Set whether to show the cursor or not
* @new_show_cursor: whether the cursor should be visible or not
*/
int BEE::set_show_cursor(bool new_show_cursor) const {
	SDL_ShowCursor((new_show_cursor) ? SDL_ENABLE : SDL_DISABLE);
	return 0;
}
/*
* BEE::set_window_position() - Set the game window position
* @new_x: the new x-coordinate to move the window to
* @new_y: the new y-coordinate to move the window to
*/
int BEE::set_window_position(int new_x, int new_y) const {
	SDL_SetWindowPosition(renderer->window, new_x, new_y);
	return 0;
}
/*
* BEE::set_window_x() - Set the x-coordinate of the game window
* @new_x: the new x-coordinate to move the window to
*/
int BEE::set_window_x(int new_x) const {
	return set_window_position(new_x, get_window_y());
}
/*
* BEE::set_window_y() - Set the y-coordinate of the game window
*/
int BEE::set_window_y(int new_y) const {
	return set_window_position(get_window_x(), new_y);
}
/*
* BEE::set_window_center() - Center the game window on the screen
*/
int BEE::set_window_center() const {
	return set_window_position(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}
/*
* BEE::set_window_size() - Change the size of the game window
* @new_width: the new width to change the window to
* @new_height: the new height to change the window to
*/
int BEE::set_window_size(int new_width, int new_height) {
	width = new_width;
	height = new_height;
	SDL_SetWindowSize(renderer->window, width, height);
	return 0;
}
/*
* BEE::set_width() - Change the width of the game window
* @new_width: the new width to change the window to
*/
int BEE::set_width(int new_width) {
	return set_window_size(new_width, height);
}
/*
* BEE::set_height() - Change the height of the game window
* @new_height: the new height to change the window to
*/
int BEE::set_height(int new_height) {
	return set_window_size(width, new_height);
}

#endif // _BEE_CORE_WINDOW
