/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_H
#define _BEE_RENDER_PARTICLE_H 1

#include <list>
#include <functional>

#include "../../game.hpp"

class BEE::Particle {
	public:
		BEE* game;

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

		Particle(BEE*, Sprite*, double, Uint32, bool);
		Particle(BEE*, bee_pt_shape_t, double, Uint32, bool);
		int init();
		int print();
};

#endif // _BEE_RENDER_PARTICLE_H
