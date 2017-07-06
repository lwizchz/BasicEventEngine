/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE
#define BEE_RENDER_PARTICLE 1

#include "../../defines.hpp"

#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include "particle.hpp"

#include "../../engine.hpp"

#include "../../util/real.hpp"
#include "../../util/platform.hpp"

#include "../../messenger/messenger.hpp"

#include "../../core/resources.hpp"
#include "../../core/room.hpp"

#include "particledata.hpp"
#include "system.hpp"

#include "../../resource/sprite.hpp"
#include "../../resource/room.hpp"

namespace bee {
	Particle::Particle(Sprite* new_sprite, double new_scale, Uint32 new_max_time, unsigned int new_deviation) :
		sprite(new_sprite),
		has_own_sprite(false),

		deviation(new_deviation),

		on_death_func(nullptr),
		death_type(nullptr),
		old_particles(),

		scale(new_scale),
		velocity(),

		angle(0.0),
		angle_increase(0.0),

		rotation_cache(),

		color({255, 255, 255, 255}),

		max_time(new_max_time),
		death_amount(1),

		should_reanimate(true),
		is_lightable(true),
		is_sprite_lightable(false)
	{
		if (sprite != nullptr) {
			init();
		}
	}
	Particle::Particle(E_PT_SHAPE new_shape, double new_scale, Uint32 new_max_time, unsigned int new_deviation) :
		Particle(nullptr, new_scale, new_max_time, new_deviation)
	{
		has_own_sprite = true;

		switch (new_shape) {
			case E_PT_SHAPE::PIXEL: {
				sprite = add_sprite("pt_sprite_pixel", "particles/00_pixel.png");
				break;
			}
			case E_PT_SHAPE::DISK: {
				sprite = add_sprite("pt_sprite_disk", "particles/01_disk.png");
				break;
			}
			case E_PT_SHAPE::SQUARE: {
				sprite = add_sprite("pt_sprite_square", "particles/02_square.png");
				break;
			}
			case E_PT_SHAPE::LINE: {
				sprite = add_sprite("pt_sprite_line", "particles/03_line.png");
				break;
			}
			case E_PT_SHAPE::STAR: {
				sprite = add_sprite("pt_sprite_star", "particles/04_star.png");
				break;
			}
			case E_PT_SHAPE::CIRCLE: {
				sprite = add_sprite("pt_sprite_circle", "particles/05_circle.png");
				break;
			}
			case E_PT_SHAPE::RING: {
				sprite = add_sprite("pt_sprite_ring", "particles/06_ring.png");
				break;
			}
			case E_PT_SHAPE::SPHERE: {
				sprite = add_sprite("pt_sprite_sphere", "particles/07_sphere.png");
				break;
			}
			case E_PT_SHAPE::FLARE: {
				sprite = add_sprite("pt_sprite_flare", "particles/08_flare.png");
				break;
			}
			case E_PT_SHAPE::SPARK: {
				sprite = add_sprite("pt_sprite_spark", "particles/09_spark.png");
				break;
			}
			case E_PT_SHAPE::EXPLOSION: {
				sprite = add_sprite("pt_sprite_explosion", "particles/10_explosion.png");
				break;
			}
			case E_PT_SHAPE::CLOUD: {
				sprite = add_sprite("pt_sprite_cloud", "particles/11_cloud.png");
				break;
			}
			case E_PT_SHAPE::SMOKE: {
				sprite = add_sprite("pt_sprite_smoke", "particles/12_smoke.png");
				break;
			}
			case E_PT_SHAPE::SNOW: {
				sprite = add_sprite("pt_sprite_snow", "particles/13_snow.png");
				break;
			}
			default: // This should never happen
				messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Couldn't initialize particle: unknown particle type " + bee_itos((int)new_shape));
				return;
		}

		init();
	}
	Particle::Particle(Sprite* new_sprite, double new_scale, Uint32 new_max_time) :
		Particle(new_sprite, new_scale, new_max_time, 50)
	{}
	Particle::Particle(E_PT_SHAPE new_shape, double new_scale, Uint32 new_max_time) :
		Particle(new_shape, new_scale, new_max_time, 50)
	{}
	Particle::~Particle() {
		remove_old_particles();

		if (has_own_sprite) {
			delete sprite;
		}
	}

	int Particle::init() {
		rotation_cache.reserve(360);
		for (int a=0; a<360; a++) {
			glm::mat4 r = glm::translate(glm::mat4(1.0f), glm::vec3((float)sprite->get_subimage_width()/2.0f, (float)sprite->get_height()/2.0f, 0.0f));
			r = glm::rotate(r, (float)degtorad(a), glm::vec3(0.0f, 0.0f, 1.0f));
			r = glm::translate(r, glm::vec3(-(float)sprite->get_subimage_width()/2.0f, -(float)sprite->get_height()/2.0f, 0.0f));
			rotation_cache.push_back(r);
		}

		on_death_func = [] (ParticleSystem* sys, ParticleData* pd, Particle* p) {
			for (size_t i = 0; i < pd->get_type()->death_amount; i++) {
				sys->add_particle(p, pd->x, pd->y);
			}
		};

		return 0;
	}
	int Particle::print() {
		std::stringstream s;
		s <<
		"Particle { "
		"\n	sprite                  " << sprite <<
		"\n	scale                   " << scale <<
		"\n	deviation               " << deviation <<
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
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str());

		return 0;
	}

	Sprite* Particle::get_sprite() {
		return sprite;
	}
	unsigned int Particle::get_deviation() {
		return deviation;
	}

	int Particle::set_death_type(Particle* new_death_type) {
		death_type = new_death_type;
		return 0;
	}
	int Particle::on_death(ParticleSystem* sys, ParticleData* pd) {
		if ((death_type != nullptr)&&(on_death_func != nullptr)) {
			on_death_func(sys, pd, death_type);
		}
		add_old_particle(pd);
		return 0;
	}

	int Particle::add_old_particle(ParticleData* pd) {
		pd->make_old();
		old_particles.push_back(pd);
		return 0;
	}
	ParticleData* Particle::reuse_particle(int x, int y, Uint32 timestamp) {
		if (old_particles.empty()) {
			return (new ParticleData(this, x, y, timestamp));
		}

		ParticleData* pd = old_particles.back();
		old_particles.pop_back();
		pd->init(x, y, timestamp);

		return pd;
	}
	int Particle::remove_old_particles() {
		for (auto& pd : old_particles) {
			delete pd;
		}
		old_particles.clear();
		return 0;
	}
}

#endif // BEE_RENDER_PARTICLE
