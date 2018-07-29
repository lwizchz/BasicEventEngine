/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
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
	class Texture;

	class Particle;
	class ParticleData;
	class ParticleSystem;

	class Particle {
			Texture* texture;
			bool has_own_texture;

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
			bool is_texture_lightable;

			Particle(Texture*, double, Uint32, unsigned int);
			Particle(E_PT_SHAPE, double, Uint32, unsigned int);
			Particle(Texture*, double, Uint32);
			Particle(E_PT_SHAPE, double, Uint32);
			~Particle();

			int init();
			int print();

			Texture* get_texture();
			unsigned int get_deviation();

			int set_death_type(Particle*);
			int on_death(ParticleSystem*, ParticleData*);

			ParticleData* reuse_particle(double, double, Uint32);
			int remove_old_particles();
	};
}

#endif // BEE_RENDER_PARTICLE_H
