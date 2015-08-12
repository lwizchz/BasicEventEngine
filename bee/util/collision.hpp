/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_COLLISION_H
#define _BEE_UTIL_COLLISION_H 1

// Collision checking functions

#include <SDL2/SDL.h>

#include "real.hpp"

class Line {
        public:
                double x1, y1, x2, y2;
};

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
bool check_collision_circle(double x1, double y1, double r1, double x2, double y2, double r2) {
        if (distance(x1, y1, x2, y2) > r1+r2) {
                return false;
        }
        return true;
}
bool check_collision_line(Line line1, Line line2) {
        double m1 = (line1.y2-line1.y1) / (line1.x2-line1.x1); // Slope
        double m2 = (line2.y2-line2.y1) / (line2.x2-line2.x1);
        double b1 = line1.y1 - m1*line1.x1; // y-intercept
        double b2 = line2.y1 - m2*line2.x1;

        if ((m1 == m2)&&(b1 == b2)) {
                return true;
        }

        double cx = (b2-b1) / (m2-m1); // Possible collision

        if ((cx >= line1.x1)&&(cx <= line1.x2)) {
                if ((cx >= line2.x1)&&(cx <= line2.x2)) {
                        return true;
                }
        }

        return false;
}

#endif // _BEE_UTIL_COLLISION_H
