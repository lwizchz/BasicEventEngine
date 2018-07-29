/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_TRANSITION_H
#define BEE_RENDER_TRANSITION_H 1

#include <functional>

#include "../enum.hpp"

namespace bee {
	// Forward declaration
	class Texture;

	E_TRANSITION get_transition_type();
	int set_transition_type(E_TRANSITION);
	int set_transition_custom(std::function<void (Texture*, Texture*)>);
	double get_transition_speed();
	int set_transition_speed(double);
	int draw_transition();
	bool compute_check_quit();
}

#endif // BEE_RENDER_TRANSITION_H
