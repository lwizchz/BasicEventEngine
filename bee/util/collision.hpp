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

	if (a_bottom < b_top) {
                return false;
        } else if (a_top > b_bottom) {
                return false;
        } else if (a_right < b_left) {
                return false;
        } else if (a_left > b_right) {
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
bool check_collision(SDL_Rect* rect, Line line) {
        double left, right, top, bottom;
        left = rect->x;
	right = rect->x + rect->w;
	top = rect->y;
	bottom = rect->y + rect->h;

        bool has_collision = false;

        Line side = {left, top, right, top};
        has_collision |= check_collision_line(line, side);
        side = {right, top, right, bottom};
        has_collision |= check_collision_line(line, side);
        side = {left, bottom, right, bottom};
        has_collision |= check_collision_line(line, side);
        side = {left, top, left, bottom};
        has_collision |= check_collision_line(line, side);

        return has_collision;
}

double angle_hbounce(double a) {
        if ((a > 0)&&(a < 90)) {
                a = (90 - a) + 270;
        } else if ((a > 270)&&(a < 360)) {
                a = 90 - (a - 270);
        } else if ((a > 90)&&(a < 270)) {
                a = (180 - a) + 180;
        } else {
                a += 180;
        }
        return a;
}
double angle_vbounce(double a) {
        if ((a > 0)&&(a < 180)) {
                a = (90 - a) + 90;
        } else if ((a > 180)&&(a < 360)) {
                a = (270 - a) + 270;
        } else {
                a += 180;
        }
        return a;
}

std::pair<double,double> move_outside(std::pair<double,double> p1, std::pair<double,double> p2, SDL_Rect* r1, SDL_Rect* r2) {
        double dist = distance(p1.first, p1.second, p2.first, p2.second);
        double dir = direction_of(p2.first, p2.second, p1.first, p1.second);
        double x = p2.first, y = p2.second;

        int max_attempts = 10;
        double delta = dist/((double)max_attempts);
        int attempts = 0;

        r1->x = p2.first; r1->y = p2.second;
        while ((check_collision(r1, r2))&&(attempts++ < max_attempts)) {
                x += sin(degtorad(dir)) * delta*dist;
                y += -cos(degtorad(dir)) * delta*dist;
                r1->x = x;
                r1->y = y;
        }

        return std::make_pair(x, y);
}

#endif // _BEE_UTIL_COLLISION_H
