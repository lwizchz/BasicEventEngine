/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_SYSTEM
#define BEE_RENDER_PARTICLE_SYSTEM 1

#include <algorithm>

#include "system.hpp"

#include "../../engine.hpp"

#include "../../util/real.hpp"
#include "../../util/collision.hpp"

#include "../../core/instance.hpp"

#include "../drawing.hpp"
#include "../render.hpp"
#include "particle.hpp"
#include "particledata.hpp"
#include "emitter.hpp"
#include "attractor.hpp"
#include "destroyer.hpp"
#include "deflector.hpp"
#include "changer.hpp"

#include "../../resources/sprite.hpp"

namespace bee {
	ParticleSystem::ParticleSystem() :
		id(-1),

		is_oldfirst(true),
		depth(0),

		xoffset(0.0),
		yoffset(0.0),
		following(nullptr),

		time_offset(0),

		particles(),
		emitters(),
		attractors(),
		destroyers(),
		deflectors(),
		changers(),

		draw_data()
	{}
	int ParticleSystem::load() {
		for (auto& e : emitters) {
			if (!e->particle_type->sprite->get_is_loaded()) {
				e->particle_type->sprite->load();
			}
		}
		return 0;
	}
	int ParticleSystem::fast_forward(int frames) { // Fast-forwading more than 500 milliseconds is not recommended due to how long it takes to calculate
		int t = get_ticks();
		int step = 1000.0/get_fps_goal();
		for (int i=0; i<frames; i++) {
			time_offset += step;
			draw(t+time_offset, 1.0/step, false);
		}

		particles.erase(
			std::remove_if(
				particles.begin(),
				particles.end(),
				[&] (ParticleData* p) -> bool {
					if (p->is_old){
						return true;
					}

					Uint32 ticks = t+time_offset - p->creation_time;
					if (t+time_offset < p->creation_time) {
						ticks = 0;
					}

					if (p->is_dead(ticks)) {
						if ((p->particle_type->death_type != nullptr)&&(p->particle_type->on_death != nullptr)) {
							p->particle_type->on_death(this, p, p->particle_type->death_type);
						}
						p->is_old = true;
						p->particle_type->old_particles.push_back(p);
						return true;
					}
					return false;
				}
			),
			particles.end()
		);

		return 0;
	}
	int ParticleSystem::draw() {
		return draw(get_ticks()+time_offset, get_delta(), true);
	}
	int ParticleSystem::draw(Uint32 now, double delta, bool should_draw) {
		int sx = xoffset, sy = yoffset;
		if (following != nullptr) {
			sx += following->get_x();
			sy += following->get_y();
		}

		for (auto& p : particles) {
			if (p->is_old){
				continue;
			}

			for (auto& d : destroyers) {
				int dx = sx, dy = sy;
				if (d->following != nullptr) {
					dx = d->following->get_x();
					dy = d->following->get_y();
				}

				SDL_Rect a = {(int)p->x, (int)p->y, (int)p->w, (int)p->h};
				SDL_Rect b = {dx+(int)d->x, dy+(int)d->y, (int)d->w, (int)d->h};
				if (check_collision(a, b)) {
					p->is_old = true;
					p->particle_type->old_particles.push_back(p);
					break;
				}
			}
			if (p->is_old){
				continue;
			}

			for (auto& c : changers) {
				int cx = sx, cy = sy;
				if (c->following != nullptr) {
					cx = c->following->get_x();
					cy = c->following->get_y();
				}

				SDL_Rect a = {(int)p->x, (int)p->y, (int)p->w, (int)p->h};
				SDL_Rect b = {cx+(int)c->x, cy+(int)c->y, (int)c->w, (int)c->h};
				if (check_collision(a, b)) {
					if (p->particle_type == c->particle_before) {
						p->particle_type = c->particle_after;
					}
				}
			}

			int px = p->x, py = p->y;
			double m = p->velocity.first * (p->randomness+1.0);
			double dir = p->velocity.second;
			p->x += cos(degtorad(dir)) * m * delta;
			p->y += -sin(degtorad(dir)) * m * delta;

			for (auto& a : attractors) {
				int ax = sx, ay = sy;
				if (a->following != nullptr) {
					ax = a->following->get_x();
					ay = a->following->get_y();
				}

				double ds = dist_sqr(p->x, p->y, ax+a->x+a->w/2, ay+a->y+a->h/2);
				if (ds < sqr(a->max_distance)) {
					double f = 0.0;
					switch (a->force_type) {
						case E_PS_FORCE::CONSTANT: {
							f = a->force * (p->randomness+1.0);
							break;
						}
						case E_PS_FORCE::LINEAR:
						default: {
							f = a->force * (p->randomness+1.0) * (sqr(a->max_distance) - ds) / sqr(a->max_distance);
							break;
						}
						case E_PS_FORCE::QUADRATIC: {
							f = a->force * (p->randomness+1.0) * sqr((sqr(a->max_distance) - ds) / sqr(a->max_distance));
							break;
						}
					}
					std::tie(p->x, p->y) = coord_approach(p->x, p->y, ax+a->x+a->w/2, ay+a->y+a->h/2, f, delta);
				}
			}

			for (auto& d : deflectors) {
				int dx = sx, dy = sy;
				if (d->following != nullptr) {
					dx = d->following->get_x();
					dy = d->following->get_y();
				}

				SDL_Rect a = {(int)p->x, (int)p->y, (int)p->w, (int)p->h};
				SDL_Rect b = {dx+(int)d->x, dy+(int)d->y, (int)d->w, (int)d->h};
				if (check_collision(a, b)) {
					dir = direction_of(px, py, p->x, p->y);
					p->velocity = std::make_pair(p->velocity.first * d->friction * -1, dir);
				}
			}
		}

		for (auto& e : emitters) {
			int ex = sx, ey = sy;
			if (e->following != nullptr) {
				ex = e->following->get_x();
				ey = e->following->get_y();
			}

			if (e->number >= 0) {
				for (int i=0; i<e->number; i++) {
					add_particle(e->particle_type, ex + e->x + random(e->w), ey + e->y + random(e->h));
				}
			} else {
				if (e->number_count++ >= -e->number) {
					add_particle(e->particle_type, ex + e->x + random(e->w), ey + e->y + random(e->h));
					e->number_count = 0;
				}
			}
		}

		if (should_draw) {
			draw_data.clear();
			particles.remove_if(
				[&] (ParticleData* p) -> bool {
					if (p->is_old) {
						return true;
					}

					Uint32 ticks = now - p->creation_time;
					if (now < p->creation_time) {
						ticks = 0;
					}

					p->sprite_data->x = xoffset + p->x - p->w/2;
					p->sprite_data->y = yoffset + p->y - p->h/2;
					if (following != nullptr) {
						p->sprite_data->x += following->get_x();
						p->sprite_data->y += following->get_y();
					}
					p->sprite_data->subimage_time = p->creation_time;
					p->sprite_data->w = p->w;
					p->sprite_data->h = p->h;
					if ((p->w >= 10.0)&&(p->h >= 10.0)) {
						p->sprite_data->angle = absolute_angle(p->get_angle(ticks));
					}
					draw_data[p->particle_type].push_back(p->sprite_data);

					if (p->is_dead(ticks)) {
						if ((p->particle_type->death_type != nullptr)&&(p->particle_type->on_death != nullptr)) {
							p->particle_type->on_death(this, p, p->particle_type->death_type);
						}
						p->is_old = true;
						p->particle_type->old_particles.push_back(p);
						return true;
					}
					return false;
				}
			);
			for (auto& s : draw_data) {
				if (!s.first->is_lightable) {
					set_is_lightable(false);
				}

				bool is_sprite_lightable = s.first->sprite->get_is_lightable();
				if (!s.first->is_sprite_lightable) {
					s.first->sprite->set_is_lightable(false);
				}

				s.first->sprite->draw_array(s.second, s.first->rotation_cache, s.first->color, SDL_FLIP_NONE);

				set_is_lightable(true);
				s.first->sprite->set_is_lightable(is_sprite_lightable);
			}
		}

		return 0;
	}
	int ParticleSystem::draw_debug() {
		int sx = xoffset, sy = yoffset;
		if (following != nullptr) {
			sx += following->get_x();
			sy += following->get_y();
		}

		for (auto& p : particles) {
			draw_rectangle((sx+p->x) - p->w/2, (sy+p->y) - p->h/2, p->w, p->h, false, E_RGB::AQUA);
		}
		for (auto& e : emitters) {
			int ex = sx, ey = sy;
			if (e->following != nullptr) {
				ex = e->following->get_x();
				ey = e->following->get_y();
			}
			draw_rectangle(ex+e->x, ey+e->y, e->w, e->h, false, E_RGB::GREEN);
		}
		for (auto& a : attractors) {
			int ax = sx, ay = sy;
			if (a->following != nullptr) {
				ax = a->following->get_x();
				ay = a->following->get_y();
			}
			draw_rectangle(ax+a->x, ay+a->y, a->w, a->h, false, E_RGB::MAGENTA);
		}
		for (auto& d : destroyers) {
			int dx = sx, dy = sx;
			if (d->following != nullptr) {
				dx = d->following->get_x();
				dy = d->following->get_y();
			}
			draw_rectangle(dx+d->x, dy+d->y, d->w, d->h, false, E_RGB::RED);
		}
		for (auto& d : deflectors) {
			int dx = sx, dy = sx;
			if (d->following != nullptr) {
				dx = d->following->get_x();
				dy = d->following->get_y();
			}
			draw_rectangle(dx+d->x, dy+d->y, d->w, d->h, false, E_RGB::NAVY);
		}
		for (auto& c : changers) {
			int cx = sx, cy = sx;
			if (c->following != nullptr) {
				cx = c->following->get_x();
				cy = c->following->get_y();
			}
			draw_rectangle(cx+c->x, cy+c->y, c->w, c->h, false, E_RGB::YELLOW);
		}
		return 0;
	}
	int ParticleSystem::clear() {
		for (auto& p : particles) {
			if (p != nullptr) {
				p->particle_type->old_particles.remove_if(
					[&] (ParticleData* pd) -> bool {
						delete pd->sprite_data;
						pd->sprite_data = nullptr;
						delete pd;
						pd = nullptr;
						return true;
					}
				);
			}
		}

		particles.clear();
		emitters.clear();
		attractors.clear();
		destroyers.clear();
		deflectors.clear();
		changers.clear();

		draw_data.clear();

		return 0;
	}

	int ParticleSystem::add_particle(Particle* p, int x, int y) {
		if (!p->old_particles.empty()) {
			ParticleData* pd = p->old_particles.front();
			pd->init(x, y, get_ticks()+time_offset);
			p->old_particles.pop_front();
			particles.push_back(pd);
		} else {
			ParticleData* pd = new ParticleData(p, x, y, get_ticks()+time_offset);
			particles.push_back(pd);
		}
		return 0;
	}
}

#endif // BEE_RENDER_PARTICLE_SYSTEM
