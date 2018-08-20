/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS
#define BEE_PHYSICS 1

#include "physics.hpp"

namespace bee { namespace physics {
	/**
	* @param type the desired shape type
	* @param p0 the first parameter which specifices the number of points for some types
	*
	* @returns the total number of parameters that should be provided to initialize the given type
	*/
	size_t get_shape_param_amount(E_PHYS_SHAPE type, int p0) {
		switch (type) {
			case E_PHYS_SHAPE::SPHERE:      return 1;
			case E_PHYS_SHAPE::BOX:         return 3;

			case E_PHYS_SHAPE::CYLINDER:
			case E_PHYS_SHAPE::CAPSULE:
			case E_PHYS_SHAPE::CONE:        return 2;

			case E_PHYS_SHAPE::MULTISPHERE: return p0 * 4 + 1;
			case E_PHYS_SHAPE::CONVEX_HULL: return p0 * 3 + 1;

			default:
			case E_PHYS_SHAPE::NONE:        return 0;
		}
	}
	/**
	* @param s the desired shape type
	*
	* @returns the total number of parameters that should be provided to initialize the given type
	*/
	size_t get_shape_param_amount(E_PHYS_SHAPE type) {
		return get_shape_param_amount(type, 0);
	}

	/**
	* @param type the desired constraint type
	*
	* @returns the total number of parameters that should be provided to initialize the given type
	*/
	size_t get_constraint_param_amount(E_PHYS_CONSTRAINT type) {
		switch (type) {
			case E_PHYS_CONSTRAINT::POINT:  return 6;
			case E_PHYS_CONSTRAINT::HINGE:  return 12;
			case E_PHYS_CONSTRAINT::SLIDER: return 4;
			case E_PHYS_CONSTRAINT::CONE:   return 4;
			case E_PHYS_CONSTRAINT::SIXDOF: return 12;

			default:
			case E_PHYS_CONSTRAINT::FIXED:
			case E_PHYS_CONSTRAINT::FLAT:
			case E_PHYS_CONSTRAINT::TILE:
			case E_PHYS_CONSTRAINT::NONE:   return 0;
		}
	}
}}

#endif // BEE_PHYSICS_WORLD
