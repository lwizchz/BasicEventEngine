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
			Sprite* sprite;
			bool has_own_sprite;

			unsigned int deviation;

			std::function<void (ParticleSystem*, ParticleData*, Particle*)> on_death_func;
			Particle* death_type;
			std::vector<ParticleData*> old_particles;

			int add_old_particle(ParticleData*);
		public:
			double scale;
			std::pair<double,double> velocity;

			double angle;
			double angle_increase;

			std::vector<glm::mat4> rotation_cache;

			RGBA color;

			Uint32 max_time;
			unsigned int death_amount;

			bool should_reanimate;
			bool is_lightable;
			bool is_sprite_lightable;

			Particle(Sprite*, double, Uint32, unsigned int);
			Particle(E_PT_SHAPE, double, Uint32, unsigned int);
			Particle(Sprite*, double, Uint32);
			Particle(E_PT_SHAPE, double, Uint32);
			~Particle();

			int init();
			int print();

			Sprite* get_sprite();
			unsigned int get_deviation();

			int set_death_type(Particle*);
			int on_death(ParticleSystem*, ParticleData*);

			ParticleData* reuse_particle(int, int, Uint32);
			int remove_old_particles();
	};
}

#endif // BEE_RENDER_PARTICLE_H
