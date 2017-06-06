/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE
#define _BEE_RENDER_PARTICLE 1

#include "particle.hpp"

BEE::Particle::Particle(BEE* new_game, Sprite* new_sprite, double new_scale, Uint32 new_max_time, bool new_should_reanimate) :
	game(new_game),
	sprite(new_sprite),

	scale(new_scale),
	velocity(),

	angle(0.0),
	angle_increase(0.0),
	rotation_cache(),

	color({255, 255, 255, 255}),

	max_time(new_max_time),
	on_death(nullptr),
	death_type(nullptr),
	death_amount(1),
	old_particles(),

	should_reanimate(new_should_reanimate),
	is_lightable(true),
	is_sprite_lightable(false)
{
	if (sprite != nullptr) {
		init();
	}
}
BEE::Particle::Particle(BEE* new_game, bee::E_PT_SHAPE new_shape, double new_scale, Uint32 new_max_time, bool new_should_reanimate) :
	Particle(new_game, nullptr, new_scale, new_max_time, new_should_reanimate)
{
	switch (new_shape) {
		case bee::E_PT_SHAPE::PIXEL: {
			sprite = game->add_sprite("pt_sprite_pixel", "particles/00_pixel.png");
			break;
		}
		case bee::E_PT_SHAPE::DISK: {
			sprite = game->add_sprite("pt_sprite_disk", "particles/01_disk.png");
			break;
		}
		case bee::E_PT_SHAPE::SQUARE: {
			sprite = game->add_sprite("pt_sprite_square", "particles/02_square.png");
			break;
		}
		case bee::E_PT_SHAPE::LINE: {
			sprite = game->add_sprite("pt_sprite_line", "particles/03_line.png");
			break;
		}
		case bee::E_PT_SHAPE::STAR: {
			sprite = game->add_sprite("pt_sprite_star", "particles/04_star.png");
			break;
		}
		case bee::E_PT_SHAPE::CIRCLE: {
			sprite = game->add_sprite("pt_sprite_circle", "particles/05_circle.png");
			break;
		}
		case bee::E_PT_SHAPE::RING: {
			sprite = game->add_sprite("pt_sprite_ring", "particles/06_ring.png");
			break;
		}
		case bee::E_PT_SHAPE::SPHERE: {
			sprite = game->add_sprite("pt_sprite_sphere", "particles/07_sphere.png");
			break;
		}
		case bee::E_PT_SHAPE::FLARE: {
			sprite = game->add_sprite("pt_sprite_flare", "particles/08_flare.png");
			break;
		}
		case bee::E_PT_SHAPE::SPARK: {
			sprite = game->add_sprite("pt_sprite_spark", "particles/09_spark.png");
			break;
		}
		case bee::E_PT_SHAPE::EXPLOSION: {
			sprite = game->add_sprite("pt_sprite_explosion", "particles/10_explosion.png");
			break;
		}
		case bee::E_PT_SHAPE::CLOUD: {
			sprite = game->add_sprite("pt_sprite_cloud", "particles/11_cloud.png");
			break;
		}
		case bee::E_PT_SHAPE::SMOKE: {
			sprite = game->add_sprite("pt_sprite_smoke", "particles/12_smoke.png");
			break;
		}
		case bee::E_PT_SHAPE::SNOW: {
			sprite = game->add_sprite("pt_sprite_snow", "particles/13_snow.png");
			break;
		}
		default: // This should never happen
			game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::WARNING, "Couldn't initialize particle: unknown particle type " + bee_itos((int)new_shape));
			return;
	}

	init();
}
int BEE::Particle::init() {
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

	return 0;
}
int BEE::Particle::print() {
	std::stringstream s;
	s <<
	"Particle { "
	"\n	sprite                  " << sprite <<
	"\n	scale                   " << scale <<
	"\n	velocity:" <<
	"\n		magnitude       " << velocity.first <<
	"\n		direction       " << velocity.second <<
	"\n	angle                   " << angle <<
	"\n	angle_increase          " << angle_increase <<
	"\n	color (rgba)            " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a <<
	"\n	max_time                " << max_time <<
	"\n	death_type              " << death_type <<
	"\n	death_amount            " << death_amount <<
	"\n	should_reanimate        " << should_reanimate <<
	"\n	is_lightable            " << is_lightable <<
	"\n	is_sprite_lightable	" << is_sprite_lightable <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::INFO, s.str());

	return 0;
}

#endif // _BEE_RENDER_PARTICLE
