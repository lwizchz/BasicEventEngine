/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PARTICLE
#define _BEE_PARTICLE 1

#include "particle.hpp"

BEE::Particle::Particle(BEE* new_game, Sprite* new_sprite, double new_scale, Uint32 new_max_time, bool new_should_reanimate) {
	game = new_game;
	init(new_sprite, new_scale, new_max_time, new_should_reanimate);
}
BEE::Particle::Particle(BEE* new_game, pt_shape_t new_shape, double new_scale, Uint32 new_max_time, bool new_should_reanimate) {
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
			game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Couldn't initialize particle: unknown particle type");
			return;
	}

	init(new_sprite, new_scale, new_max_time, new_should_reanimate);
}
int BEE::Particle::init(Sprite* new_sprite, double new_scale, Uint32 new_max_time, bool new_should_reanimate) {
	sprite = new_sprite;
	scale = new_scale;
	max_time = new_max_time;

	rotation_cache.clear();
	rotation_cache.reserve(360);
	for (int a=0; a<360; a++) {
		glm::mat4 r = glm::translate(glm::mat4(1.0f), glm::vec3((float)sprite->get_subimage_width()/2.0f, (float)sprite->get_height()/2.0f, 0.0f));
		r = glm::rotate(r, (float)degtorad(a), glm::vec3(0.0f, 0.0f, 1.0f));
		r = glm::translate(r, glm::vec3(-(float)sprite->get_subimage_width()/2.0f, -(float)sprite->get_height()/2.0f, 0.0f));
		rotation_cache.push_back(r);
	}

	on_death = [] (BEE::ParticleSystem* sys, BEE::ParticleData* pd, BEE::Particle* p) {
		for (size_t i = 0; i < pd->particle_type->death_amount; i++) {
			p->game->get_current_room()->add_particle(sys, p, pd->x, pd->y);
		}
	};

	should_reanimate = new_should_reanimate;

	return 0;
}
int BEE::Particle::print() {
	std::stringstream s;
	s <<
	"Particle { "
	"\n	sprite            " << sprite <<
	"\n	scale             " << scale <<
	"\n	velocity:" <<
	"\n		magnitude " << velocity.first <<
	"\n		direction " << velocity.second <<
	"\n	angle             " << angle <<
	"\n	angle_increase    " << angle_increase <<
	"\n	color (rgba)      " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a <<
	"\n	max_time          " << max_time <<
	"\n	death_type        " << death_type <<
	"\n	death_amount      " << death_amount <<
	"\n	should_reanimate  " << should_reanimate <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}

BEE::ParticleData::ParticleData(Particle* new_particle_type, int new_x, int new_y, Uint32 now) {
	particle_type = new_particle_type;
	sprite_data = new SpriteDrawData();

	init(new_x, new_y, now);
}
int BEE::ParticleData::init(int new_x, int new_y, Uint32 now) {
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
double BEE::ParticleData::get_angle(Uint32 ticks) {
	return particle_type->angle + particle_type->angle_increase * ticks * (sin(randomness)+2)/randomness/4 * (((int)randomness - 3 >= 0)  ? 1 : -1);
}
int BEE::ParticleData::draw(int sx, int sy, Uint32 ticks) {
	return particle_type->sprite->draw((sx+x) - w/2, (sy+y) - h/2, creation_time, w, h, get_angle(ticks), particle_type->color, SDL_FLIP_NONE);
}
bool BEE::ParticleData::is_dead(Uint32 ticks) {
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
bool BEE::ParticleData::operator< (const ParticleData& other) {
	if (depth == other.depth) {
		return (creation_time <= other.creation_time);
	}
	return (depth > other.depth);
}

BEE::ParticleSystem::ParticleSystem(BEE* new_game) {
	game = new_game;
	following = nullptr;
	clear();
}
int BEE::ParticleSystem::load() {
	for (auto& e : emitters) {
		if (!e->particle_type->sprite->get_is_loaded()) {
			e->particle_type->sprite->load();
		}
	}
	return 0;
}
int BEE::ParticleSystem::fast_forward(int frames) { // Fast-forwading more than 500 milliseconds is not recommended due to how long it takes to calculate
	int t = game->get_ticks();
	int step = 1000.0/game->get_fps_goal();
	for (int i=0; i<frames; i++) {
		time_offset += step;
		draw(t+time_offset, false);
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
int BEE::ParticleSystem::draw() {
	return draw(game->get_ticks()+time_offset, true);
}
int BEE::ParticleSystem::draw(Uint32 now, bool should_draw) {
	int sx = xoffset, sy = yoffset;
	if (following != nullptr) {
		sx += following->x;
		sy += following->y;
	}

	for (auto& p : particles) {
		if (p->is_old){
			continue;
		}

		for (auto& d : destroyers) {
			int dx = sx, dy = sy;
			if (d->following != nullptr) {
				dx = d->following->x;
				dy = d->following->y;
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
				cx = c->following->x;
				cy = c->following->y;
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
		p->x += cos(degtorad(dir)) * m * game->get_delta();
		p->y += -sin(degtorad(dir)) * m * game->get_delta();

		for (auto& a : attractors) {
			int ax = sx, ay = sy;
			if (a->following != nullptr) {
				ax = a->following->x;
				ay = a->following->y;
			}

			double ds = dist_sqr(p->x, p->y, ax+a->x+a->w/2, ay+a->y+a->h/2);
			if (ds < sqr(a->max_distance)) {
				double f = 0.0;
				switch (a->force_type) {
					case ps_force_constant: {
						f = a->force * (p->randomness+1.0);
						break;
					}
					case ps_force_linear:
					default: {
						f = a->force * (p->randomness+1.0) * (sqr(a->max_distance) - ds) / sqr(a->max_distance);
						break;
					}
					case ps_force_quadratic: {
						f = a->force * (p->randomness+1.0) * sqr((sqr(a->max_distance) - ds) / sqr(a->max_distance));
						break;
					}
				}
				std::tie(p->x, p->y) = coord_approach(p->x, p->y, ax+a->x+a->w/2, ay+a->y+a->h/2, f, game->get_delta());
			}
		}

		for (auto& d : deflectors) {
			int dx = sx, dy = sy;
			if (d->following != nullptr) {
				dx = d->following->x;
				dy = d->following->y;
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
			ex = e->following->x;
			ey = e->following->y;
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
					p->sprite_data->x += following->x;
					p->sprite_data->y += following->y;
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
				game->set_is_lightable(false);
			}
			s.first->sprite->draw_array(s.second, s.first->rotation_cache, s.first->color, SDL_FLIP_NONE);
			game->set_is_lightable(true);
		}
	}

	return 0;
}
int BEE::ParticleSystem::draw_debug() {
	int sx = xoffset, sy = yoffset;
	if (following != nullptr) {
		sx += following->x;
		sy += following->y;
	}

	for (auto& p : particles) {
		game->draw_rectangle((sx+p->x) - p->w/2, (sy+p->y) - p->h/2, p->w, p->h, false, c_aqua);
	}
	for (auto& e : emitters) {
		int ex = sx, ey = sy;
		if (e->following != nullptr) {
			ex = e->following->x;
			ey = e->following->y;
		}
		game->draw_rectangle(ex+e->x, ey+e->y, e->w, e->h, false, c_green);
	}
	for (auto& a : attractors) {
		int ax = sx, ay = sy;
		if (a->following != nullptr) {
			ax = a->following->x;
			ay = a->following->y;
		}
		game->draw_rectangle(ax+a->x, ay+a->y, a->w, a->h, false, c_magenta);
	}
	for (auto& d : destroyers) {
		int dx = sx, dy = sx;
		if (d->following != nullptr) {
			dx = d->following->x;
			dy = d->following->y;
		}
		game->draw_rectangle(dx+d->x, dy+d->y, d->w, d->h, false, c_red);
	}
	for (auto& d : deflectors) {
		int dx = sx, dy = sx;
		if (d->following != nullptr) {
			dx = d->following->x;
			dy = d->following->y;
		}
		game->draw_rectangle(dx+d->x, dy+d->y, d->w, d->h, false, c_navy);
	}
	for (auto& c : changers) {
		int cx = sx, cy = sx;
		if (c->following != nullptr) {
			cx = c->following->x;
			cy = c->following->y;
		}
		game->draw_rectangle(cx+c->x, cy+c->y, c->w, c->h, false, c_yellow);
	}
	return 0;
}
int BEE::ParticleSystem::clear() {
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

int BEE::ParticleSystem::add_particle(Particle* p, int x, int y) {
	if (!p->old_particles.empty()) {
		ParticleData* pd = p->old_particles.front();
		pd->init(x, y, game->get_ticks()+time_offset);
		p->old_particles.pop_front();
		particles.push_back(pd);
	} else {
		ParticleData* pd = new ParticleData(p, x, y, game->get_ticks()+time_offset);
		particles.push_back(pd);
	}
	return 0;
}

#endif // _BEE_PARTICLE
