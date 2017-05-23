/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_SYSTEM_H
#define _BEE_RENDER_PARTICLE_SYSTEM_H 1

#include <list>

#include "../../game.hpp"

class BEE::ParticleSystem {
	public:
		BEE* game;
		int id;

		bool is_oldfirst;
		int depth;

		double xoffset, yoffset;
		Instance* following;

		Uint32 time_offset;

		std::list<ParticleData*> particles;
		std::list<ParticleEmitter*> emitters;
		std::list<ParticleAttractor*> attractors;
		std::list<ParticleDestroyer*> destroyers;
		std::list<ParticleDeflector*> deflectors;
		std::list<ParticleChanger*> changers;

		std::map<Particle*,std::list<SpriteDrawData*>> draw_data;

		ParticleSystem(BEE*);
		int print();
		int load();
		int fast_forward(int);
		int draw();
		int draw(Uint32, double, bool);
		int draw_debug();
		int clear();

		int add_particle(Particle*, int, int);
};

#endif // _BEE_RENDER_PARTICLE_SYSTEM_H
