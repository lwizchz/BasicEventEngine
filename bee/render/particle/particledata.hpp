/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLEDATA_H
#define BEE_RENDER_PARTICLEDATA_H 1

#include <utility> // Include the required library header for std::pair
#include <list>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "../../enum.hpp"

namespace bee {
	// Forward declarations
	class Particle;

	class ParticleData {
			Particle* particle_type;

			unsigned int w, h;

			int depth;

			Uint32 creation_time;
			bool is_old;

			unsigned int deviation;
		public:
			double x, y;

			std::pair<double,double> velocity;

			ParticleData(Particle*, double, double, Uint32);

			int init(double, double, Uint32);
			int make_old();
			bool operator< (const ParticleData&);

			int set_type(Particle*);
			int move(double);
			int set_position(std::pair<double,double>);
			int set_velocity(double, double);

			Particle* get_type();
			SDL_Rect get_rect();
			double get_w();
			double get_h();
			double get_angle(Uint32);
			Uint8 get_alpha(Uint32);
			Uint32 get_creation();
			bool is_dead(Uint32);
			bool get_is_old();
			unsigned int get_deviation();
			double get_deviation_percent();

			int draw(double, double, Uint32);
			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLEDATA_H
