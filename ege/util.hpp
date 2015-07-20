/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _EGE_UTIL_H
#define _EGE_UTIL_H 1

#include <string>

bool check_collision(SDL_Rect* a, SDL_Rect* b) {
	int a_left, a_right, a_top, a_bottom;
	int b_left, b_right, b_top, b_bottom;

	a_left = a->x;
	a_right = a->x + a->w;
	a_top = a->y;
	a_bottom = a->y + a->h;

	b_left = b->x;
	b_right = b->x + b->w;
	b_top = b->y;
	b_bottom = b->y + b->h;

	if (a_bottom <= b_top) {
		return false;
	} else if (a_top >= b_bottom) {
		return false;
	} else if (a_right <= b_left) {
		return false;
	} else if (a_left >= b_right) {
		return false;
	}

	return true;

	//return (SDL_HasIntersection(a, b) == SDL_TRUE) ? true : false;
}

std::pair<int,int> coord_approach(int x1, int y1, int x2, int y2, int speed) {
	float distance = sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
	if (distance <= speed) {
		return std::make_pair(x2, y2);
	}
 	float ratio = speed/distance;

	int x3 = x1 + (x2-x1)*ratio;
	int y3 = y1 + (y2-y1)*ratio;

	return std::make_pair(x3, y3);
}

#endif // _EGE_UTIL_H
