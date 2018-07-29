/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DEFINES_H
#define BEE_DEFINES_H 1

#define GLM_FORCE_RADIANS // Force all GLM functions to use radians instead of degrees

#define GLEW_STATIC // Statically link GLEW

#define DEFAULT_WINDOW_WIDTH 1920 // Define the default window dimensions
#define DEFAULT_WINDOW_HEIGHT 1080

#define DEFAULT_GAME_FPS 60 // Define the default game fps goal

#define BEE_MAX_LIGHTS 8 // Define the maximum amount of processed lights
#define BEE_MAX_LIGHTABLES 96
#define BEE_MAX_MASK_VERTICES 8

#define BEE_MAX_BONES 100 // Define Mesh bone maximums
#define BEE_BONES_PER_VERTEX 4

#define MACRO_TO_STR_(x) #x
#define MACRO_TO_STR(x) MACRO_TO_STR_(x)

#define BIT(x) (1u << x)

#endif // BEE_DEFINES_H
