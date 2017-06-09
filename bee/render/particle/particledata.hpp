/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLEDATA_H
#define BEE_RENDER_PARTICLEDATA_H 1

#include <utility>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	class Particle;
	class SpriteDrawData;

	class ParticleData {
		public:
			Particle* particle_type;
			SpriteDrawData* sprite_data;

			double x, y;
			unsigned int w, h;

			int depth;

			std::pair<double,double> velocity;

			Uint32 creation_time;
			bool is_old;

			unsigned int randomness;

			ParticleData(Particle*, int, int, Uint32);
			int init(double, double, Uint32);
			double get_angle(Uint32);
			int draw(int, int, Uint32);
			bool is_dead(Uint32);

			bool operator< (const ParticleData&);
	};
}

#endif // BEE_RENDER_PARTICLEDATA_H
