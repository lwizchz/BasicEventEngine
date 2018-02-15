/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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

#include "../../core/rooms.hpp"

#include "particledata.hpp"
#include "system.hpp"

#include "../../resource/texture.hpp"
#include "../../resource/room.hpp"

namespace bee {
	Particle::Particle(Texture* _texture, double _scale, Uint32 _max_time, unsigned int _deviation) :
		texture(_texture),
		has_own_texture(false),

		deviation(_deviation),

		on_death_func(nullptr),
		death_type(nullptr),
		old_particles(),

		scale(_scale),
		velocity(),

		angle(0.0),
		angle_increase(0.0),

		rotation_cache(),

		color({255, 255, 255, 255}),

		max_time(_max_time),
		death_amount(1),

		should_reanimate(true),
		is_lightable(true),
		is_texture_lightable(false)
	{
		if (texture != nullptr) {
			init();
		}
	}
	Particle::Particle(E_PT_SHAPE shape, double _scale, Uint32 _max_time, unsigned int _deviation) :
		Particle(nullptr, _scale, _max_time, _deviation)
	{
		has_own_texture = true;

		const std::string root ("/bee/resources/textures/particles");
		switch (shape) {
			case E_PT_SHAPE::PIXEL: {
				texture = Texture::add("pt_texture_pixel", root+"/00_pixel.png");
				break;
			}
			case E_PT_SHAPE::DISK: {
				texture = Texture::add("pt_texture_disk", root+"/01_disk.png");
				break;
			}
			case E_PT_SHAPE::SQUARE: {
				texture = Texture::add("pt_texture_square", root+"/02_square.png");
				break;
			}
			case E_PT_SHAPE::LINE: {
				texture = Texture::add("pt_texture_line", root+"/03_line.png");
				break;
			}
			case E_PT_SHAPE::STAR: {
				texture = Texture::add("pt_texture_star", root+"/04_star.png");
				break;
			}
			case E_PT_SHAPE::CIRCLE: {
				texture = Texture::add("pt_texture_circle", root+"/05_circle.png");
				break;
			}
			case E_PT_SHAPE::RING: {
				texture = Texture::add("pt_texture_ring", root+"/06_ring.png");
				break;
			}
			case E_PT_SHAPE::SPHERE: {
				texture = Texture::add("pt_texture_sphere", root+"/07_sphere.png");
				break;
			}
			case E_PT_SHAPE::FLARE: {
				texture = Texture::add("pt_texture_flare", root+"/08_flare.png");
				break;
			}
			case E_PT_SHAPE::SPARK: {
				texture = Texture::add("pt_texture_spark", root+"/09_spark.png");
				break;
			}
			case E_PT_SHAPE::EXPLOSION: {
				texture = Texture::add("pt_texture_explosion", root+"/10_explosion.png");
				break;
			}
			case E_PT_SHAPE::CLOUD: {
				texture = Texture::add("pt_texture_cloud", root+"/11_cloud.png");
				break;
			}
			case E_PT_SHAPE::SMOKE: {
				texture = Texture::add("pt_texture_smoke", root+"/12_smoke.png");
				break;
			}
			case E_PT_SHAPE::SNOW: {
				texture = Texture::add("pt_sprite_snow", root+"/13_snow.png");
				break;
			}
		}

		init();
	}
	Particle::Particle(Texture* _texture, double _scale, Uint32 _max_time) :
		Particle(_texture, _scale, _max_time, 50)
	{}
	Particle::Particle(E_PT_SHAPE _shape, double _scale, Uint32 _max_time) :
		Particle(_shape, _scale, _max_time, 50)
	{}
	Particle::~Particle() {
		remove_old_particles();

		if (has_own_texture) {
			delete texture;
		}
	}

	int Particle::init() {
		rotation_cache.reserve(360);
		for (int a=0; a<360; a++) {
			glm::mat4 r = glm::translate(glm::mat4(1.0f), glm::vec3(static_cast<float>(texture->get_subimage_width())/2.0f, static_cast<float>(texture->get_height())/2.0f, 0.0f));
			r = glm::rotate(r, static_cast<float>(util::degtorad(a)), glm::vec3(0.0f, 0.0f, 1.0f));
			r = glm::translate(r, glm::vec3(-static_cast<float>(texture->get_subimage_width())/2.0f, -static_cast<float>(texture->get_height())/2.0f, 0.0f));
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
		"\n	texture                  " << texture <<
		"\n	scale                   " << scale <<
		"\n	deviation               " << deviation <<
		"\n	velocity:" <<
		"\n		magnitude       " << velocity.first <<
		"\n		direction       " << velocity.second <<
		"\n	angle                   " << angle <<
		"\n	angle_increase          " << angle_increase <<
		"\n	color (rgba)            " << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b) << ", " << static_cast<int>(color.a) <<
		"\n	max_time                " << max_time <<
		"\n	death_type              " << death_type <<
		"\n	death_amount            " << death_amount <<
		"\n	should_reanimate        " << should_reanimate <<
		"\n	is_lightable            " << is_lightable <<
		"\n	is_texture_lightable	" << is_texture_lightable <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str());

		return 0;
	}

	Texture* Particle::get_texture() {
		return texture;
	}
	unsigned int Particle::get_deviation() {
		return deviation;
	}

	int Particle::set_death_type(Particle* _death_type) {
		death_type = _death_type;
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
	ParticleData* Particle::reuse_particle(double x, double y, Uint32 timestamp) {
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
