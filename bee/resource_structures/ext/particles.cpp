/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PARTICLE
#define _BEE_PARTICLE 1

#include "particles.hpp"

BEE::Particle::Particle(BEE* new_game, Sprite* new_sprite, double new_scale, std::pair<double,double> new_velocity, RGBA new_color, Uint32 new_max_time) {
	game = new_game;
	init(new_sprite, new_scale, new_velocity, new_color, new_max_time);
}
int BEE::Particle::init(Sprite* new_sprite, double new_scale, std::pair<double,double> new_velocity, RGBA new_color, Uint32 new_max_time) {
	sprite = new_sprite;
	scale = new_scale;
	velocity = new_velocity;
	color = new_color;
	max_time = new_max_time;

	return 0;
}
int BEE::Particle::print() {
	std::cout <<
	"Particle { "
	"\n	sprite		" << sprite <<
	"\n	scale		" << scale <<
	"\n	color		" << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a <<
	"\n	max_time	" << max_time <<
	"\n}\n";

	return 0;
}

BEE::ParticleData::ParticleData(Particle* new_particle_type, int new_x, int new_y) {
	particle_type = new_particle_type;

	x = mean<int>(new_x, (int)random(25));
	y = mean<int>(new_y, (int)random(25));
	double s = particle_type->scale * random(100)/100;
	w = particle_type->sprite->get_width() * s;
	h = particle_type->sprite->get_height() * s;

	velocity = particle_type->velocity;
	creation_time = SDL_GetTicks();
}
int BEE::ParticleData::draw(int sx, int sy) {
	return particle_type->sprite->draw(sx+x, sy+y, creation_time, w, h, 0.0, particle_type->color);
}
bool BEE::ParticleData::is_dead() {
	if (particle_type->sprite->get_subimage_amount() > 1) {
		if (!particle_type->sprite->get_is_animated()) {
			return true;
		}
	} else if (SDL_GetTicks() - creation_time > particle_type->max_time) {
		return true;
	}
	return false;
}

BEE::ParticleSystem::ParticleSystem() {}
int BEE::ParticleSystem::load() {
	for (auto& e : emitters) {
		e->particle_type->sprite->load();
	}
	return 0;
}
int BEE::ParticleSystem::draw() {
	for (auto& p : particles) {
		for (auto& c : changers) {
			SDL_Rect a = {p->x, p->y, 0, 0};
			SDL_Rect b = {c->x, c->y, c->w, c->h};
			if (check_collision(&a, &b)) {
				if (p->particle_type == c->particle_before) {
					p->particle_type = c->particle_after;
				}
			}
		}

		int px = p->x, py = p->y;
		double m = p->velocity.first;
		double dir = p->velocity.second;
		p->x += sin(degtorad(dir))*m;
		p->y += -cos(degtorad(dir))*m;

		for (auto& a : attractors) {
			dir = direction_of(px, py, a->x, a->y);
			p->x += sin(degtorad(dir)) * a->force;
			p->y += -cos(degtorad(dir)) * a->force;
		}

		for (auto& d : deflectors) {
			SDL_Rect a = {p->x, p->y, 0, 0};
			SDL_Rect b = {d->x, d->y, d->w, d->h};
			if (check_collision(&a, &b)) {
				dir = direction_of(px, py, p->x, p->y);
				if (true) {
					p->velocity = std::make_pair(p->velocity.first*d->friction, angle_hbounce(dir));
				} else {
					p->velocity = std::make_pair(p->velocity.first*d->friction, angle_vbounce(dir));
				}
			}
		}
	}

	for (auto& e : emitters) {
		if (e->number >= 0) {
			ParticleData* p = new ParticleData(e->particle_type, 0, 100);
			particles.push_back(p);
		}
	}

	std::list<ParticleData*> plist = particles;
	for (auto& p : plist) {
		p->draw(following->x + xoffset, following->y + yoffset);
		if (p->is_dead()) {
			particles.remove(p);
			delete p;
		} else {
			for (auto& d : destroyers) {
				SDL_Rect a = {p->x, p->y, 0, 0};
				SDL_Rect b = {d->x, d->y, d->w, d->y};
				if (check_collision(&a, &b)) {
					particles.remove(p);
					delete p;
				}
			}
		}
	}

	return 0;
}
int BEE::ParticleSystem::clear() {
	for (auto& p : particles) {
		delete p;
	}
	particles.clear();
	emitters.clear();
	attractors.clear();
	destroyers.clear();
	deflectors.clear();
	changers.clear();
	return 0;
}

#endif // _BEE_PARTICLE
