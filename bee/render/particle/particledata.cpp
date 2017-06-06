/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLEDATA
#define _BEE_RENDER_PARTICLEDATA 1

#include "particledata.hpp"

namespace bee {
	ParticleData::ParticleData(Particle* new_particle_type, int new_x, int new_y, Uint32 now) :
		particle_type(new_particle_type),
		sprite_data(new SpriteDrawData()),

		x(new_x),
		y(new_y),
		w(1),
		h(1),

		depth(0),

		velocity(),

		creation_time(now),
		is_old(true),

		randomness(1)
	{
		init(x, y, now);
	}
	int ParticleData::init(double new_x, double new_y, Uint32 now) {
		x = new_x;
		y = new_y;

		randomness = random(75) + 25;
		double s = particle_type->scale * randomness/100;
		randomness %= 3;
		randomness++;

		w = particle_type->sprite->get_subimage_width() * s;
		h = particle_type->sprite->get_height() * s;

		velocity = particle_type->velocity;
		creation_time = now;
		is_old = false;

		return 0;
	}
	double ParticleData::get_angle(Uint32 ticks) {
		return particle_type->angle + particle_type->angle_increase * ticks * (sin(randomness)+2)/randomness/4 * (((int)randomness - 3 >= 0)  ? 1 : -1);
	}
	int ParticleData::draw(int sx, int sy, Uint32 ticks) {
		return particle_type->sprite->draw((sx+x) - w/2, (sy+y) - h/2, creation_time, w, h, get_angle(ticks), particle_type->color, SDL_FLIP_NONE);
	}
	bool ParticleData::is_dead(Uint32 ticks) {
		if ((!particle_type->should_reanimate)&&(particle_type->sprite->get_subimage_amount() > 1)) {
			if (!particle_type->sprite->get_is_animated()) {
				return true;
			}
		} else if (ticks > particle_type->max_time - w*1000) {
			return true;
		}

		if (ticks + 1000 > particle_type->max_time - w*1000) {
			w /= 1.0 + (double)randomness/50;
			h /= 1.0 + (double)randomness/50;
		}

		if ((w <= 1.0) || (h <= 1.0)) {
			return true;
		}

		return false;
	}
	bool ParticleData::operator< (const ParticleData& other) {
		if (depth == other.depth) {
			return (creation_time <= other.creation_time);
		}
		return (depth > other.depth);
	}
}

#endif // _BEE_RENDER_PARTICLEDATA
