/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLEDATA
#define BEE_RENDER_PARTICLEDATA 1

#include "particledata.hpp"

#include "particle.hpp"

#include "../../util/real.hpp"

#include "../drawing.hpp"

#include "../../resource/texture.hpp"

namespace bee {
	ParticleData::ParticleData(Particle* _particle_type, double _x, double _y, Uint32 now) :
		particle_type(_particle_type),

		w(1),
		h(1),

		depth(0),

		creation_time(now),
		is_old(true),

		deviation(particle_type->get_deviation()),

		x(_x),
		y(_y),

		velocity()
	{
		init(x, y, now);
	}

	int ParticleData::init(double _x, double _y, Uint32 now) {
		x = _x;
		y = _y;

		unsigned int d = particle_type->get_deviation();
		deviation = util::random::get(d) + d/2;
		double s = particle_type->scale * get_deviation_percent();

		w = static_cast<int>(particle_type->get_texture()->get_subimage_width() * s);
		h = static_cast<int>(particle_type->get_texture()->get_size().second * s);

		velocity = particle_type->velocity;
		creation_time = now;
		is_old = false;

		return 0;
	}
	int ParticleData::make_old() {
		is_old = true;
		return 0;
	}
	bool ParticleData::operator< (const ParticleData& other) {
		if (depth == other.depth) {
			return (creation_time <= other.creation_time);
		}
		return (depth > other.depth);
	}

	int ParticleData::set_type(Particle* _type) {
		particle_type = _type;
		return 0;
	}
	int ParticleData::move(double delta) {
		double m = velocity.first * (get_deviation_percent());
		double dir = velocity.second;

		x += cos(util::degtorad(dir)) * m * delta;
		y += -sin(util::degtorad(dir)) * m * delta;

		return 0;
	}
	int ParticleData::set_position(std::pair<double,double> pos) {
		x = pos.first;
		y = pos.second;
		return 0;
	}
	int ParticleData::set_velocity(double magnitude, double direction) {
		velocity.first = magnitude;
		velocity.second = direction;
		return 0;
	}

	Particle* ParticleData::get_type() {
		return particle_type;
	}
	SDL_Rect ParticleData::get_rect() {
		return {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
	}
	double ParticleData::get_w() {
		return w;
	}
	double ParticleData::get_h() {
		return h;
	}
	double ParticleData::get_angle(Uint32 ticks) {
		return particle_type->angle + particle_type->angle_increase * ticks * get_deviation_percent() * ((get_deviation_percent() >= 0.5) ? 1 : -1);
	}
	Uint8 ParticleData::get_alpha(Uint32 ticks) {
		Uint8 max_alpha = particle_type->color.a;
		Uint32 max_time = static_cast<Uint32>(particle_type->max_time * get_deviation_percent());

		double p = 1.0 - 500.0 / max_time;
		if (ticks > max_time * p) { // Use a linear relationship to gradually decrease the alpha for the last 500ms of the particle's life
			return static_cast<Uint8>(
				max_alpha * (max_time - ticks) / (max_time * (1.0-p))
			);
		}

		return max_alpha;
	}
	Uint32 ParticleData::get_creation() {
		return creation_time;
	}
	bool ParticleData::is_dead(Uint32 ticks) {
		if (
			(!particle_type->should_reanimate) // If the particle should not reanimate
			&&(particle_type->get_texture()->get_subimage_amount() > 1) // If the particle has multiple subimages, i.e. it has an animation
			&&(!particle_type->get_texture()->get_is_animated()) // If the particle finished animating
		) {
			return true; // Return true when finished animating
		}

		if (ticks > particle_type->max_time * get_deviation_percent()) {
			return true; // Return true when the particle has expired
		}

		return false; // Otherwise return false
	}
	bool ParticleData::get_is_old() {
		return is_old;
	}
	unsigned int ParticleData::get_deviation() {
		return deviation;
	}
	double ParticleData::get_deviation_percent() {
		return static_cast<double>(deviation) / particle_type->get_deviation();
	}

	int ParticleData::draw(double system_x, double system_y, Uint32 ticks) {
		RGBA c (particle_type->color);
		c.a = get_alpha(ticks);
		return particle_type->get_texture()->draw(static_cast<int>(system_x+x) - w/2, static_cast<int>(system_y+y) - h/2, creation_time, w, h, util::absolute_angle(get_angle(ticks)), c);
	}
	int ParticleData::draw_debug(double system_x, double system_y, E_RGB color) {
		return draw_rectangle(static_cast<int>(system_x+x) - w/2, static_cast<int>(system_y+y) - h/2, w, h, 1, RGBA(color));
	}
}

#endif // BEE_RENDER_PARTICLEDATA
