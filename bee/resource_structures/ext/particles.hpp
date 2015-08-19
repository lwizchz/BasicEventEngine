/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PARTICLE_H
#define _BEE_PARTICLE_H 1

#include <list>
#include <functional>

#include "../../game.hpp"

enum pt_shape_t {
	pt_shape_pixel,
	pt_shape_disk,
	pt_shape_square,
	pt_shape_line,
	pt_shape_star,
	pt_shape_circle,
	pt_shape_ring,
	pt_shape_sphere,
	pt_shape_flare,
	pt_shape_spark,
	pt_shape_explosion,
	pt_shape_cloud,
	pt_shape_smoke,
	pt_shape_snow
};

class BEE::Particle {
	public:
		BEE* game = NULL;

		Sprite* sprite = NULL;

		double scale = 1.0;
		std::pair<double,double> velocity = {0.0, 0.0};

		double angle = 0.0;
		double angle_increase = 0.0;

		RGBA color = {255, 255, 255, 255};

		Uint32 max_time = 100;
		std::function<void (ParticleSystem*, ParticleData*, Particle*)> on_death = NULL;
		Particle* death_type = NULL;
		int death_amount = 1;

		Particle(BEE*, Sprite*, double, Uint32);
		Particle(BEE*, pt_shape_t, double, Uint32);
		int init(Sprite*, double, Uint32);
		int print();
};

class BEE::ParticleData {
	public:
		Particle* particle_type;

		int x = 0, y = 0;
		int w = 0, h = 0;

		int depth = 0;

		std::pair<double,double> velocity;

		Uint32 creation_time;

		unsigned int randomness = 1;

		ParticleData(Particle*, int, int);
		int draw(int, int, Uint32);
		bool is_dead(Uint32);

		bool operator< (const ParticleData&);
};

enum ps_shape_t {
	ps_shape_rectangle,
	ps_shape_line,
	ps_shape_circle
};
enum ps_distr_t {
	ps_distr_linear,
	ps_distr_gaussian,
	ps_distr_invgaussian
};

class BEE::ParticleEmitter {
	public:
		int x = 0, y = 0;
		int w = 0, h = 0;

		ps_shape_t shape = ps_shape_rectangle;
		ps_distr_t distribution = ps_distr_linear;

		Particle* particle_type;

		int number = 1;
		int number_count = 0;
};

enum ps_force_t {
	ps_force_constant,
	ps_force_linear,
	ps_force_quadratic
};

class BEE::ParticleAttractor {
	public:
		int x = 0, y = 0;

		double force = 0.0;
		double max_distance = 0.0;
		ps_force_t force_type = ps_force_linear;
};

class BEE::ParticleDestroyer {
	public:
		int x = 0, y = 0;
		int w = 0, h = 0;

		ps_shape_t shape = ps_shape_rectangle;
};

class BEE::ParticleDeflector {
	public:
		int x = 0, y = 0;
		int w = 0, h = 0;

		double friction = 0.0;
};

enum ps_change_t {
	ps_change_motion,
	ps_change_look,
	ps_change_all
};

class BEE::ParticleChanger {
	public:
		int x = 0, y = 0;
		int w = 0, h = 0;

		ps_shape_t shape = ps_shape_rectangle;

		Particle* particle_before;
		Particle* particle_after;

		ps_change_t change_type = ps_change_all;
};

class BEE::ParticleSystem {
	public:
		int id = -1;

		bool is_oldfirst = true;
		int depth = 0;

		int xoffset = 0, yoffset = 0;
		InstanceData* following = NULL;

		std::list<ParticleData*> particles;
		std::list<ParticleEmitter*> emitters;
		std::list<ParticleAttractor*> attractors;
		std::list<ParticleDestroyer*> destroyers;
		std::list<ParticleDeflector*> deflectors;
		std::list<ParticleChanger*> changers;

		ParticleSystem();
		int print();
		int load();
		int draw();
		int clear();

		int add_particle(Particle*, int, int);
};

#endif // _BEE_PARTICLE_H
