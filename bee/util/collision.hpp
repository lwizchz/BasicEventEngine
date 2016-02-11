/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_COLLISION_H
#define _BEE_UTIL_COLLISION_H 1

// Collision checking functions

#include <SDL2/SDL.h> // Include the required library functions

#include "real.hpp" // Include the required real number functions

/*
* compare_lines() - Return whether the given lines are equal
* @l1: one of the lines
* @l2: the other line
*/
bool compare_lines(const Line& l1, const Line& l2) {
	if ((l1.x1 == l2.x1)&&(l1.y1 == l2.y1)) { // If the lines have the same starting point
		if ((l1.x2 == l2.x2)&&(l1.y2 == l2.y2)) { // If the lines have the same ending point
			return true; // Return true if both of their points match
		}
	}
	return false; // Otherwise, return false
}

/*
* check_collision() - Return whether the given rectangles overlap
* ! See https://wiki.libsdl.org/SDL_Rect for details
* @a: one of the rectangles
* @b: the other rectangle
*/
bool check_collision(const SDL_Rect& a, const SDL_Rect& b) {
	// Define the boundaries for the first rectangle
	int a_left = a.x;
	int a_right = a.x + a.w;
	int a_top = a.y;
	int a_bottom = a.y + a.h;

	// Define the boundaries for the second rectangle
	int b_left = b.x;
	int b_right = b.x + b.w;
	int b_top = b.y;
	int b_bottom = b.y + b.h;

	// Compare the boundaries of the two rectangles
	if (a_bottom < b_top) {
                return false; // Return false if the bottom of the first rectangle is above the second rectangle
        } else if (a_top > b_bottom) {
                return false; // Return false if the first rectangle is below the bottom of the second rectangle
        } else if (a_right < b_left) {
                return false; // Return false if the right edge of the first rectangle is left of the second rectangle
        } else if (a_left > b_right) {
                return false; // Return false if the left edge of the first rectangle is right of the second rectangle
        }

	return true; // Otherwise, return true
}
/*
* check_collision_circle() - Return whether the given circles intersect
* @x1: the x-coordinate of the first circle's center
* @y1: the y-coordinate of the first circle's center
* @r1: the radius of the first circle
* @x2: the x-coordinate of the first circle's center
* @y2: the y-coordinate of the first circle's center
* @r2: the radius of the second circle
*/
bool check_collision_circle(double x1, double y1, double r1, double x2, double y2, double r2) {
        if (distance(x1, y1, x2, y2) > r1+r2) { // If the distance between the two centers is greater than the sum of the radii, then return false
                return false;
        }
        return true; // Otherwise, return true
}
/*
* check_collision_line() - Return whether the given lines intersect
* @line1: one of the lines
* @line2: the other line
*/
bool check_collision_line(const Line& line1, const Line& line2) {
	// Check whether either of the given lines are vertical
	bool v1 = false, v2 = false;
        if (line1.x1 == line1.x2) {
                v1 = true;
        }
        if (line2.x1 == line2.x2) {
                v2 = true;
        }

	// If a given line isn't vertical, then compute its slope and y-intercept
        double m1 = 0.0, m2 = 0.0; // Declare slopes
        double b1 = 0.0, b2 = 0.0; // Declare y-intercepts
        if (!v1) {
                m1 = (line1.y2-line1.y1) / (line1.x2-line1.x1);
                b1 = line1.y1 - m1*line1.x1;
        }
        if (!v2) {
                m2 = (line2.y2-line2.y1) / (line2.x2-line2.x1);
                b2 = line2.y1 - m2*line2.x1;
        }

        if ((v1 ^ v2) == 0) { // If either both or none of the given lines are vertical
                if (m1 == m2) { // If they have the same slope and the same y-intercept then they are the same line
                        if (b1 == b2) {
                                if (v1) { // If the lines are vertical then check the collision externally
                                        return check_collision_aligned_line(line1, line2);
                                } else {
					return true; // Return true if the lines are the same line
				}
                        } else {
				return false; // Return false if they have the same slope but different y-intercept
			}
                }
        } else if (v1) { // If only the first line is vertical then check whether their bounds line up
                if (is_between(line1.x1, line2.x1, line2.x2)) { // Check the vertical line's x-value
			double cy = m2*line1.x1 + b2;
			if (is_between(cy, line1.y1, line1.y2)) { // Check the non-vertical line's y-value
				return true;
			}
                }
		return false;
        } else if (v2) { // If only the first line is vertical then check whether their bounds line up
		if (is_between(line2.x1, line1.x1, line1.x2)) { // Check the vertical line's x-value
			double cy = m1*line2.x1 + b1;
			if (is_between(cy, line2.y1, line2.y2)) { // Check the non-vertical line's y-value
				return true;
			}
                }
		return false;
        }

	// If no lines are vertical then check for intercepts otherwise
        double cx = (b2-b1) / (m1-m2); // Declare a possible collision, see below math
	/*
		y1 = m1*x1 + b1
		y2 = m2*x2 + b2

		for y1 == y2
		m1*x1 + b1 = m2*x2 + b2
		m1*x1 - m2*x2 = b2 - b1

		for cx == x1 == x2
		cx*(m1-m2) = (b2-b1)
		cx = (b2-b1)/(m1-m2)
	*/

        if (is_between(cx, line1.x1, line1.x2)) { // Check whether the x-value is within the bounds of both lines
                if (is_between(cx, line2.x1, line2.x2)) {
                        return true; // Return true if there is a collision
                }
        }

        return false; // Return false if there is no collision
}
/*
* check_collision_aligned_line() - Return whether the given lines intersect
* ! This function assumes that the lines are aligned to the axes and will return false if they are not
* @line1: one of the lines
* @line2: the other line
*/
bool check_collision_aligned_line(const Line& line1, const Line& line2) {
        bool r = false; // Return false by default

        if (line1.x1 == line1.x2) { // If the first line is vertical
                if (line2.x1 == line2.x2) { // If the second line is vertical
                        if (line1.x1 == line2.x1) { // If the two lines have the same x-value then check whether their y-values match up
                                r = is_between(line1.y1, line2.y1, line2.y2) || is_between(line2.y1, line1.y1, line1.y2);
                        }
                } else if (line2.y1 == line2.y2) { // If the second line is horizontal then check whether their bounds match up
                        r = is_between(line2.y1, line1.y1, line1.y2) && is_between(line1.x1, line2.x1, line2.x2);
                }
        } else if (line1.y1 == line1.y2) { // If the first line is horizontal
                if (line2.x1 == line2.x2) { // If the second line is vertical then check whether their bounds match up
                        r = is_between(line1.y1, line2.y1, line2.y2) && is_between(line2.x1, line1.x1, line1.x2);
                } else if (line2.y1 == line2.y2) { // If the second line is horizontal
                        if (line1.y1 == line2.y1) { // If the two lines have the same y-value then check whether their x-values match up
                                r = is_between(line2.x1, line1.x1, line1.x2) || is_between(line1.x1, line2.x1, line2.x2);
                        }
                }
        }

        return r; // Return the result on success
}
/*
* check_collision() - Return whether the given rectangle and given line intersect
* @rect: the rectangle to operate on
* @line: the line to operate on
*/
bool check_collision(const SDL_Rect& rect, const Line& line) {
	// Define the boundaries for the given rectangle
        double left = rect.x;
	double right = rect.x + rect.w;
	double top = rect.y;
	double bottom = rect.y + rect.h;

        bool has_collision = false; // Define whether the tests show a collision

	// Check the top border
        Line side = {left, top, right, top};
        has_collision |= check_collision_line(line, side);
	// Check the right border
        side = {right, top, right, bottom};
        has_collision |= check_collision_line(line, side);
	// Check the bottom border
        side = {left, bottom, right, bottom};
        has_collision |= check_collision_line(line, side);
	// Check the left border
        side = {left, top, left, bottom};
        has_collision |= check_collision_line(line, side);

        return has_collision; // Return whether the tested lines collide
}

/*
* angle_hbounce() - Return the given angle after reflecting it over the y-axis
* @a: the angle to operate on
*/
double angle_hbounce(double a) {
	a = fmod(a, 360.0); // Ensure that the given angle is between 0.0 and 360.0
	if ((a > 0)&&(a < 180)) {
                a = (90 - a) + 90;
        } else if ((a > 180)&&(a < 360)) {
                a = (270 - a) + 270;
        } else {
                a += 180;
        }
        return fmod(a, 360.0); // Return a new angle, ensuring that it is between 0.0 and 360.0
}
/*
* angle_vbounce() - Return the given angle after reflecting it over the x-axis
* @a: the angle to operate on
*/
double angle_vbounce(double a) {
	a = fmod(a, 360.0); // Ensure that the given angle is between 0.0 and 360.0
	if ((a > 0)&&(a < 90)) {
                a = (90 - a) + 270;
        } else if ((a > 270)&&(a < 360)) {
                a = 90 - (a - 270);
        } else if ((a > 90)&&(a < 270)) {
                a = (180 - a) + 180;
        } else {
                a += 180;
        }
        return fmod(a, 360.0); // Return a new angle, ensuring that it is between 0.0 and 360.0
}

#endif // _BEE_UTIL_COLLISION_H
