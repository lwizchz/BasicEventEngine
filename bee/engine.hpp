/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENGINE_H
#define BEE_ENGINE_H 1

// Include the required library headers
#include <string>
#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "defines.hpp"

#include <glm/glm.hpp> // Include the required OpenGL headers

#include "enum.hpp" // Include the required engine enumerations

namespace bee { // The engine namespace
	struct EngineState; // The struct which holds the main game state

	class Resource;
	class ResourceList;
	class MetaResourceList;

	class Sprite; class Sound; class Background; class Font; class Path; class Timeline; class Mesh; class Object; class Room; // The main resource types
	class Particle; class ParticleData; class ParticleEmitter; class ParticleAttractor; class ParticleDestroyer; class ParticleDeflector; class ParticleChanger; class ParticleSystem; // The particle system components
	class Renderer; class Light; class Camera; // The OpenGL-only resources (poor SDL implementations may exist)
	class ProgramFlags; class GameOptions; struct RGBA; // The engine related data
	class Instance; // The additional resource data types
	class PhysicsWorld; class PhysicsDraw; class PhysicsBody; // The classes which interface with the external Physics library
	struct SpriteDrawData; struct TextData; struct LightData; struct LightableData; // Additional resource-related data structs

	struct ViewData; struct BackgroundData; // The configurational structs
	struct NetworkData; struct NetworkClient; // The networking subsystem structs
	struct MessageContents; struct MessageRecipient; // The messaging subsystem structs
	struct SIDP; // The utility structs
	struct Console; // The console subsystem structs

	extern EngineState* engine;
	extern bool is_initialized;

	namespace internal {
		int update_delta();
	}

	// User defined in resources/resources.hpp
	int init_resources();
	int close_resources();

	// bee/engine.cpp
	int init(int, char**, const std::list<ProgramFlags*>&, Room**, GameOptions*);
	int loop();
	int close();

	Uint32 get_ticks();
	Uint32 get_seconds();
	Uint32 get_frame();
	double get_delta();
	Uint32 get_tick_delta();
	unsigned int get_fps_goal();

	int restart_game();
	int end_game();
}

#endif // BEE_ENGINE_H
