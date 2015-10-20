/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
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

#endif // _BEE_GAME_INPUT
