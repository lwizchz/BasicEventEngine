/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PARTICLE
#define _BEE_PARTICLE 1

#include "particle.hpp"

BEE::Particle::Particle(BEE* new_game, Sprite* new_sprite, double new_scale, Uint32 new_max_time) {
	game = new_game;
	init(new_sprite, new_scale, new_max_time);
}
BEE::Particle::Particle(BEE* new_game, pt_shape_t new_shape, double new_scale, Uint32 new_max_time) {
	game = new_game;
	Sprite* new_sprite;

	switch (new_shape) {
		case pt_shape_pixel: {
			new_sprite = game->add_sprite("pt_sprite_pixel", "particles/00_pixel.png");
			break;
		}
		case pt_shape_disk: {
			new_sprite = game->add_sprite("pt_sprite_disk", "particles/01_disk.png");
			break;
		}
		case pt_shape_square: {
			new_sprite = game->add_sprite("pt_sprite_square", "particles/02_square.png");
			break;
		}
		case pt_shape_line: {
			new_sprite = game->add_sprite("pt_sprite_line", "particles/03_line.png");
			break;
		}
		case pt_shape_star: {
			new_sprite = game->add_sprite("pt_sprite_star", "particles/04_star.png");
			break;
		}
		case pt_shape_circle: {
			new_sprite = game->add_sprite("pt_sprite_circle", "particles/05_circle.png");
			break;
		}
		case pt_shape_ring: {
			new_sprite = game->add_sprite("pt_sprite_ring", "particles/06_ring.png");
			break;
		}
		case pt_shape_sphere: {
			new_sprite = game->add_sprite("pt_sprite_sphere", "particles/07_sphere.png");
			break;
		}
		case pt_shape_flare: {
			new_sprite = game->add_sprite("pt_sprite_flare", "particles/08_flare.png");
			break;
		}
		case pt_shape_spark: {
			new_sprite = game->add_sprite("pt_sprite_spark", "particles/09_spark.png");
			break;
		}
		case pt_shape_explosion: {
			new_sprite = game->add_sprite("pt_sprite_explosion", "particles/10_explosion.png");
			break;
		}
		case pt_shape_cloud: {
			new_sprite = game->add_sprite("pt_sprite_cloud", "particles/11_cloud.png");
			break;
		}
		case pt_shape_smoke: {
			new_sprite = game->add_sprite("pt_sprite_smoke", "particles/12_smoke.png");
			break;
		}
		case pt_shape_snow: {
			new_sprite = game->add_sprite("pt_sprite_snow", "particles/13_snow.png");
			break;
		}
		default: // This should never happen
			return;
	}

	init(new_sprite, new_scale, new_max_time);
}
int BEE::Particle::init(Sprite* new_sprite, double new_scale, Uint32 new_max_time) {
	sprite = new_sprite;
	scale = new_scale;
	max_time = new_max_time;

	on_death = [] (BEE::ParticleSystem* sys, BEE::ParticleData* pd, BEE::Particle* p) {
		for (int i = 0; i < pd->particle_type->death_amount; i++) {
			p->game->get_current_room()->add_particle(sys, p, pd->x, pd->y);
		}
	};

	return 0;
}
int BEE::Particle::print() {
	std::cout <<
	"Particle { "
	"\n	sprite			" << sprite <<
	"\n	scale			" << scale <<
	"\n	velocity:\n" <<
	"\n		magnitude	" << velocity.first <<
	"\n		direction	" << velocity.second <<
	"\n	angle			" << angle <<
	"\n	angle_increase		" << angle_increase <<
	"\n	color (rgba)		" << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a <<
	"\n	max_time		" << max_time <<
	"\n	death_type		" << death_type <<
	"\n	death_amount		" << death_amount <<
	"\n}\n";

	return 0;
}

BEE::ParticleData::ParticleData(Particle* new_particle_type, int new_x, int new_y) {
	particle_type = new_particle_type;

	x = mean<int>(abs(new_x), abs(new_x) + (int)random(25) - 12) * sign(new_x);
	y = mean<int>(abs(new_y), abs(new_y) + (int)random(25) - 12) * sign(new_y);
	double s = particle_type->scale * random(100)/100;
	w = particle_type->sprite->get_width() * s;
	h = particle_type->sprite->get_height() * s;

	velocity = particle_type->velocity;
	creation_time = SDL_GetTicks();

	randomness = random(3) + 1;
}
int BEE::ParticleData::draw(int sx, int sy, Uint32 ticks) {
	double a = particle_type->angle + particle_type->angle_increase * ticks * ((double)randomness / 2);
	return particle_type->sprite->draw((sx+x) - w/2, (sy+y) - h/2, creation_time, w, h, a, particle_type->color, SDL_FLIP_NONE, false);
}
bool BEE::ParticleData::is_dead(Uint32 ticks) {
	if (particle_type->sprite->get_subimage_amount() > 1) {
		if (!particle_type->sprite->get_is_animated()) {
			return true;
		}
	} else if (ticks + (int)random(particle_type->max_time/4) > particle_type->max_time) {
		return true;
	}
	return false;
}
bool BEE::ParticleData::operator< (const ParticleData& other) {
	if (depth == other.depth) {
		return (creation_time <= other.creation_time);
	}
	return (depth > other.depth);
}

BEE::ParticleSystem::ParticleSystem() {
	following = NULL;
	clear();
}
int BEE::ParticleSystem::load() {
	for (auto& e : emitters) {
		e->particle_type->sprite->load();
	}
	return 0;
}
int BEE::ParticleSystem::draw() {
	for (auto& p : particles) {
		for (auto& c : changers) {
			int fx = 0, fy = 0;
			if (c->following != NULL) {
				fx = c->following->x;
				fy = c->following->y;
			}

			SDL_Rect a = {p->x, p->y, p->w, p->h};
			SDL_Rect b = {c->x+fx, c->y+fy, c->w, c->h};
			if (check_collision(a, b)) {
				if (p->particle_type == c->particle_before) {
					p->particle_type = c->particle_after;
				}
			}
		}

		int px = p->x, py = p->y;
		double m = p->velocity.first * p->randomness;
		double dir = p->velocity.second;
		p->x += sin(degtorad(dir)) * m;
		p->y += -cos(degtorad(dir)) * m;

		for (auto& a : attractors) {
			int fx = 0, fy = 0;
			if (a->following != NULL) {
				fx = a->following->x;
				fy = a->following->y;
			}

			dir = direction_of(px, py, a->x+fx, a->y+fy);
			p->x += sin(degtorad(dir)) * a->force * p->randomness;
			p->y += -cos(degtorad(dir)) * a->force * p->randomness;
		}

		for (auto& d : deflectors) {
			int fx = 0, fy = 0;
			if (d->following != NULL) {
				fx = d->following->x;
				fy = d->following->y;
			}

			SDL_Rect a = {p->x, p->y, p->w, p->h};
			SDL_Rect b = {d->x+fx, d->y+fy, d->w, d->h};
			if (check_collision(a, b)) {
				dir = direction_of(px, py, p->x, p->y);
				if (true) {
					p->velocity = std::make_pair(p->velocity.first * d->friction, angle_hbounce(dir));
				} else {
					p->velocity = std::make_pair(p->velocity.first * d->friction, angle_vbounce(dir));
				}
			}
		}
	}

	for (auto& e : emitters) {
		int fx = 0, fy = 0;
		if (e->following != NULL) {
			fx = e->following->x;
			fy = e->following->y;
		}

		if (e->number >= 0) {
			for (int i=0; i<e->number; i++) {
				add_particle(e->particle_type, fx + e->x + random(e->w), fy + e->y + random(e->h));
			}
		} else {
			if (e->number_count++ >= -e->number) {
				add_particle(e->particle_type, fx + e->x + random(e->w), fy + e->y + random(e->h));
				e->number_count = 0;
			}
		}
	}

	particles.sort();
	if (!is_oldfirst) {
		particles.reverse();
	}
	particles.erase(
		std::remove_if(
			particles.begin(),
			particles.end(),
			[&] (ParticleData* p) -> bool {
				Uint32 ticks = SDL_GetTicks() - p->creation_time;

				if (following != NULL) {
					p->draw(following->x + xoffset, following->y + yoffset, ticks);
				} else {
					p->draw(xoffset, yoffset, ticks);
				}

				if (p->is_dead(ticks)) {
					if ((p->particle_type->death_type != NULL)&&(p->particle_type->on_death != NULL)) {
						p->particle_type->on_death(this, p, p->particle_type->death_type);
					}
					delete p;
					return true;
				} else {
					for (auto& d : destroyers) {
						int fx = 0, fy = 0;
						if (d->following != NULL) {
							fx = d->following->x;
							fy = d->following->y;
						}

						SDL_Rect a = {p->x, p->y, p->w, p->h};
						SDL_Rect b = {d->x+fx, d->y+fy, d->w, d->h};
						if (check_collision(a, b)) {
							delete p;
							return true;
						}
					}
				}
				return false;
			}
		),
		particles.end()
	);

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

int BEE::ParticleSystem::add_particle(Particle* p, int x, int y) {
	ParticleData* pd = new ParticleData(p, x, y);
	particles.push_back(pd);
	return 0;
}

#endif // _BEE_PARTICLE
