/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_H
#define BEE_PHYSICS_H 1

#include <cstddef>

#include "../enum.hpp"

namespace bee { namespace physics {
	size_t get_shape_param_amount(E_PHYS_SHAPE, int);
	size_t get_shape_param_amount(E_PHYS_SHAPE);

	size_t get_constraint_param_amount(E_PHYS_CONSTRAINT);
}}

#endif // BEE_PHYSICS_H
