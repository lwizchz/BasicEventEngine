/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
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
		BEE* game = nullptr;

		Sprite* sprite = nullptr;

		double scale = 1.0;
		std::pair<double,double> velocity = {0.0, 0.0};

		double angle = 0.0;
		double angle_increase = 0.0;
		std::vector<glm::mat4> rotation_cache;

		RGBA color = {255, 255, 255, 255};

		Uint32 max_time = 100;
		std::function<void (ParticleSystem*, ParticleData*, Particle*)> on_death = nullptr;
		Particle* death_type = nullptr;
		int death_amount = 1;
		std::list<ParticleData*> old_particles;

		bool should_reanimate = true;
		bool is_lightable = true;

		Particle(BEE*, Sprite*, double, Uint32, bool);
		Particle(BEE*, pt_shape_t, double, Uint32, bool);
		int init(Sprite*, double, Uint32, bool);
		int print();
};

class BEE::ParticleData {
	public:
		Particle* particle_type;
		SpriteDrawData* sprite_data = nullptr;

		double x = 0.0, y = 0.0;
		int w = 1, h = 1;

		int depth = 0;

		std::pair<double,double> velocity;

		Uint32 creation_time;
		bool is_old = true;

		unsigned int randomness = 1;

		ParticleData(Particle*, int, int, Uint32);
		int init(int, int, Uint32);
		double get_angle(Uint32);
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
		double x = 0, y = 0;
		int w = 1, h = 1;
		InstanceData* following = nullptr;

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
		double x = 0, y = 0;
		int w = 1, h = 1;
		InstanceData* following = nullptr;

		double force = 1.0;
		double max_distance = 100.0;
		ps_force_t force_type = ps_force_linear;
};

class BEE::ParticleDestroyer {
	public:
		double x = 0, y = 0;
		int w = 1, h = 1;
		InstanceData* following = nullptr;

		ps_shape_t shape = ps_shape_rectangle;
};

class BEE::ParticleDeflector {
	public:
		double x = 0, y = 0;
		int w = 1, h = 1;
		InstanceData* following = nullptr;

		double friction = 1.0;
};

enum ps_change_t {
	ps_change_motion,
	ps_change_look,
	ps_change_all
};

class BEE::ParticleChanger {
	public:
		double x = 0, y = 0;
		int w = 1, h = 1;
		InstanceData* following = nullptr;

		ps_shape_t shape = ps_shape_rectangle;

		Particle* particle_before;
		Particle* particle_after;

		ps_change_t change_type = ps_change_all;
};

class BEE::ParticleSystem {
	public:
		BEE* game = nullptr;
		int id = -1;

		bool is_oldfirst = true;
		int depth = 0;

		double xoffset = 0, yoffset = 0;
		InstanceData* following = nullptr;

		Uint32 time_offset = 0;

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
		int draw(Uint32, bool);
		int draw_debug();
		int clear();

		int add_particle(Particle*, int, int);
};

#endif // _BEE_PARTICLE_H
