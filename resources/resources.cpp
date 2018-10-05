/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef RES
#define RES 1

#include "resources.hpp"

// Declare Objects

// Declare Rooms

/**
* Initialize all game resources.
* @note Loading is not required at this stage, just initialization.
*
* @retval 0 success
* @retval 1 failed
*/
int init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init Objects

		// Init Rooms

		bee::is_initialized = true; // Set the engine initialization flag
	} catch (...) {
		return 1;
	}

	return 0;
}

#define DEL(x) if (x!=nullptr) {delete x; x=nullptr;}
/**
* Destroy all game resources.
*
* @retval 0 success
*/
int close_resources() {
	// Destroy Objects

	// Destroy Rooms

	bee::is_initialized = false; // Unset the engine initialization flag

	return 0;
}
#undef DEL

#endif // RES
