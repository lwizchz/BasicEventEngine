/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DEFINES_H
#define BEE_DEFINES_H 1

#define GLM_FORCE_RADIANS // Force all GLM functions to use radians instead of degrees

#define GLEW_STATIC // Statically link GLEW

#define DEFAULT_WINDOW_WIDTH 1920 // Define the default window dimensions
#define DEFAULT_WINDOW_HEIGHT 1080

#define DEFAULT_GAME_FPS 60 // Define the default game fps goal

#define BEE_ALARM_COUNT 8

#define BEE_MAX_LIGHTS 8 // Define the maximum amount of processed lights
#define BEE_MAX_LIGHTABLES 96
#define BEE_MAX_MASK_VERTICES 8

#define MACRO_TO_STR_(x) #x
#define MACRO_TO_STR(x) MACRO_TO_STR_(x)

#ifndef BEE_GAME_ID // BEE_GAME_ID should always be defined but just in case
	#define BEE_GAME_ID 4294967295 // pow(2,32)-1, the maximum value
#endif // BEE_GAME_ID

#define BIT(x) (1u << x)

#endif // BEE_DEFINES_H
