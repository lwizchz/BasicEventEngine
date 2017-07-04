/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_SYSTEM_H
#define BEE_RENDER_PARTICLE_SYSTEM_H 1

#include <list>
#include <map>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	// Forward declarations
	class Instance;
	class SpriteDrawData;

	class Particle;
	class ParticleData;
	class ParticleEmitter;
	class ParticleAttractor;
	class ParticleDestroyer;
	class ParticleDeflector;
	class ParticleChanger;

	class ParticleSystem {
			Instance* following;

			std::list<Particle*> particle_types;
			std::list<ParticleData*> particles;
			std::list<ParticleEmitter*> emitters;
			std::list<ParticleAttractor*> attractors;
			std::list<ParticleDestroyer*> destroyers;
			std::list<ParticleDeflector*> deflectors;
			std::list<ParticleChanger*> changers;

			std::map<Particle*,std::list<SpriteDrawData*>> draw_data;

			int clear_draw_data();
		public:
			bool is_oldfirst;
			int depth;

			double xoffset, yoffset;

			Uint32 time_offset;

			ParticleSystem();
			~ParticleSystem();
			int print();
			int load();

			int add_particle_type(Particle*);
			int add_emitter(ParticleEmitter*);
			int add_attractor(ParticleAttractor*);
			int add_destroyer(ParticleDestroyer*);
			int add_deflector(ParticleDeflector*);
			int add_changer(ParticleChanger*);

			int set_following(Instance*);

			int fast_forward(int);
			int draw(Uint32, double, bool);
			int draw();
			int draw_debug();
			int clear();

			int add_particle(Particle*, int, int);
	};
}

#endif // BEE_RENDER_PARTICLE_SYSTEM_H
