/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_TRANSITION_H
#define BEE_RENDER_TRANSITION_H 1

#include <functional>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class Sprite;
	class Background;

	int reset_render_target();
	int set_render_target(Sprite*, int, int);
	int set_render_target(Sprite*);
	int set_render_target(Background*, int, int);
	int set_render_target(Background*);

	E_TRANSITION get_transition_type();
	int set_transition_type(E_TRANSITION);
	int set_transition_custom(std::function<void (Sprite*, Sprite*)>);
	double get_transition_speed();
	int set_transition_speed(double);
	int draw_transition();
	bool compute_check_quit();
}

#endif // BEE_RENDER_TRANSITION_H
