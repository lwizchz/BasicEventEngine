/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_H
#define BEE_RENDER_PARTICLE_H 1

#include "../../defines.hpp"

#include <list>
#include <vector>
#include <functional>

#include <glm/glm.hpp> // Include the required OpenGL headers

#include "../../enum.hpp"

#include "../rgba.hpp"

namespace bee {
	// Forward declarations
	class Sprite;

	class Particle;
	class ParticleData;
	class ParticleSystem;

	class Particle {
		public:
			Sprite* sprite;

			double scale;
			std::pair<double,double> velocity;

			double angle;
			double angle_increase;
			std::vector<glm::mat4> rotation_cache;

			RGBA color;

			Uint32 max_time;
			std::function<void (ParticleSystem*, ParticleData*, Particle*)> on_death;
			Particle* death_type;
			unsigned int death_amount;
			std::list<ParticleData*> old_particles;

			bool should_reanimate;
			bool is_lightable;
			bool is_sprite_lightable;

			Particle(Sprite*, double, Uint32, bool);
			Particle(E_PT_SHAPE, double, Uint32, bool);
			int init();
			int print();
	};
}

#endif // BEE_RENDER_PARTICLE_H
