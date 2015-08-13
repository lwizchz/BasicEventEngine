/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_INPUT
#define _BEE_GAME_INPUT 1

int BEE::get_mousex() {
	int mx;
	SDL_GetMouseState(&mx, NULL);
	return mx;
}
int BEE::get_mousey() {
	int my;
	SDL_GetMouseState(NULL, &my);
	return my;
}
int BEE::set_mouse_position(int new_mx, int new_my) {
	SDL_WarpMouseInWindow(window, new_mx, new_my);
	return 0;
}
int BEE::set_mousex(int new_mx) {
	return set_mouse_position(new_mx, get_mousey());
}
int BEE::set_mousey(int new_my) {
	return set_mouse_position(get_mousex(), new_my);
}

#endif // _BEE_GAME_INPUT
