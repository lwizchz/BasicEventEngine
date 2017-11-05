/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_PLAYER
#define RES_OBJ_PLAYER 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "obj_player.hpp"

ObjPlayer::ObjPlayer() : Object("obj_player", "obj_player.cpp") {
	implemented_events.insert({
		bee::E_EVENT::UPDATE,
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_MID,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::DRAW
	});
	set_depth(1);
}

void ObjPlayer::create(bee::Instance* self) {
	(*s)["position"] = 0;
	(*s)["max_positions"] = 0; // This is set on first run by update_position()
	(*s)["movement"] = 0;

	(*s)["gun_cooldown"] = 0; // TODO: Change to use milliseconds instead of frames
	(*s)["pellet_delay"] = 10;
	(*s)["beam_duration"] = 40;
	(*s)["beam_position"] = -1;

	(*s)["super_max_charges"] = 2;
	(*s)["super_charges"] = (*s)["super_max_charges"];
	(*s)["super_cooldown"] = 0;
	(*s)["super_duration"] = 60;

	VectorSprite* vs = new VectorSprite("resources/sprites/player.csv");
	(*s)["vsprite"] = static_cast<void*>(vs);

	this->update_position(self);

	// State handling
	bee::StateMachine* sm = new bee::StateMachine();

	bee::State state_still ("Still", {"MoveClockwise", "MoveCounterClockwise", "FirePellet", "FireBeam", "Super"});
	state_still.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("gun_cooldown") > 0) {
			(*s)["gun_cooldown"] -= 1;
		}
		if (_i("super_cooldown") > 0) {
			(*s)["super_cooldown"] -= 1;
		}
	};
	bee::State state_move_cw ("MoveClockwise", {"Still", "MoveCounterClockwise", "FirePellet", "FireBeam", "Super"});
	state_move_cw.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("movement") <= 0) {
			sm->pop_state_all("MoveClockwise");
			return;
		}

		(*s)["position"] += 5;
		(*s)["movement"] -= 1;

		(*s)["position"] = qmod(_i("position"), _i("max_positions")*10);
		this->update_position(self);
	};
	bee::State state_move_ccw ("MoveCounterClockwise", {"Still", "MoveClockwise", "Shoot", "Super"});
	state_move_ccw.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("movement") >= 0) {
			sm->pop_state_all("MoveCounterClockwise");
			return;
		}

		(*s)["position"] -= 5;
		(*s)["movement"] += 1;

		(*s)["position"] = qmod(_i("position"), _i("max_positions")*10);
		this->update_position(self);
	};
	bee::State state_firepellet ("FirePellet", {"Still", "MoveClockwise", "MoveCounterClockwise", "Super"});
	state_firepellet.start_func = [this, self, sm] () {
		if (_i("gun_cooldown") > 0) {
			sm->pop_state_all("FirePellet");
			return;
		}

		(*s)["gun_cooldown"] = _i("pellet_delay");
	};
	state_firepellet.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("gun_cooldown") <= 0) {
			sm->pop_state_all("FirePellet");
			return;
		}
	};
	bee::State state_firebeam ("FireBeam", {"Still", "MoveClockwise", "MoveCounterClockwise", "Super"});
	state_firebeam.start_func = [this, self, sm] () {
		if (_i("gun_cooldown") > 0) {
			sm->pop_state_all("FireBeam");
			return;
		}

		(*s)["gun_cooldown"] = _i("beam_duration");
		(*s)["beam_position"] = _i("position") / 10;
	};
	state_firebeam.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("gun_cooldown") <= 0) {
			(*s)["beam_position"] = -1;
			sm->pop_state_all("FireBeam");
			return;
		}
	};
	bee::State state_super ("Super", {"Still", "MoveClockwise", "MoveCounterClockwise", "FirePellet", "FireBeam"});
	state_super.start_func = [this, self, sm] () {
		/*if (_i("super_charges") <= 0) {
			sm->pop_state_all("Super");
			return;
		}*/

		if (_i("super_cooldown") > 0) {
			sm->pop_state_all("Super");
			return;
		}

		(*s)["super_charges"] -= 1;
		(*s)["super_cooldown"] = _i("super_duration");

		bee::Instance* lat = obj_lattice->get_instance(0);
		if (lat == nullptr) {
			return;
		}

		lat->set_data("color_r", 255);
		lat->set_data("color_g", 0);
		lat->set_data("color_b", 0);
	};
	state_super.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("super_cooldown") <= 0) {
			sm->pop_state_all("Super");
			return;
		}

		bee::Instance* lat = obj_lattice->get_instance(0);
		if (lat == nullptr) {
			return;
		}

		int x = _i("super_duration") - _i("super_cooldown");
		x /= 10;
		bee::RGBA c (
			SIDP_i(lat->get_data("color_r")),
			SIDP_i(lat->get_data("color_g")),
			SIDP_i(lat->get_data("color_b")),
			255
		);
		c.set_hue(qmod(exp(-x) * cos(2*M_PI*x) * 1800.0 + 90.0, 360));
		lat->set_data("color_r", c.r);
		lat->set_data("color_g", c.g);
		lat->set_data("color_b", c.b);
	};
	state_super.end_func = [this, self, sm] () {
		bee::Instance* lat = obj_lattice->get_instance(0);
		if (lat == nullptr) {
			return;
		}

		lat->set_data("color_r", 255);
		lat->set_data("color_g", 255);
		lat->set_data("color_b", 255);
	};

	sm->add_state(state_still);
	sm->add_state(state_move_cw);
	sm->add_state(state_move_ccw);
	sm->add_state(state_firepellet);
	sm->add_state(state_firebeam);
	sm->add_state(state_super);
	sm->push_state("Still");
	(*s)["sm"] = static_cast<void*>(sm);
}
void ObjPlayer::destroy(bee::Instance* self) {
	VectorSprite* vs = static_cast<VectorSprite*>(_p("vsprite"));
	delete vs;

	bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
	delete sm;

	Object::destroy(self);
}
void ObjPlayer::step_mid(bee::Instance* self) {
	bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
	sm->update_all();
}
void ObjPlayer::mouse_input(bee::Instance* self, SDL_Event* e) {
	if (e->type == SDL_MOUSEWHEEL) {
		int flip = bee::get_mouse_wheel_flip(e->wheel);
		int m = _i("movement");
		if (flip * m > 0) {
			m += flip * e->wheel.y;
		} else {
			m = flip * e->wheel.y;
		}

		(*s)["movement"] = m;

		bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
		if (m > 0) {
			sm->pop_state_all("MoveCounterClockwise");
			sm->push_state("MoveClockwise");
		} else if (m < 0) {
			sm->pop_state_all("MoveClockwise");
			sm->push_state("MoveCounterClockwise");
		} else {
			sm->pop_state_all("MoveClockwise");
			sm->pop_state_all("MoveCounterClockwise");
		}
	}
}
void ObjPlayer::draw(bee::Instance* self) {
	bee::RGBA c (255, 0, 0, 255);
	c.set_hsv({qmod(static_cast<float>(bee::get_ticks()/16), 360), 1.0, 1.0});
	
	VectorSprite* vs = static_cast<VectorSprite*>(_p("vsprite"));
	vs->draw(glm::vec3(self->get_x(), self->get_y(), 0.0), glm::vec3(0.0, 0.0, 0.0), c);

	bee::set_is_lightable(false);
	bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
	bee::engine->font_default->draw_fast(0, 0, sm->get_states(), {255, 255, 255, 255});
	bee::set_is_lightable(true);
}

void ObjPlayer::update_position(bee::Instance* self) {
	bee::Instance* lat = obj_lattice->get_instance(0);
	if (lat == nullptr) {
		return;
	}

	if (SIDP_v(lat->get_data("levelvector")).size() < static_cast<unsigned int>(_i("position")/10)) {
		return;
	}

	PrimitiveLine* l = static_cast<PrimitiveLine*>(SIDP_cp(lat->get_data("levelvector"), _i("position")/10));
	self->set_position(
		l->p1.x + SIDP_d(lat->get_data("offset_x")),
		l->p1.y + SIDP_d(lat->get_data("offset_y")),
		l->p1.z + SIDP_d(lat->get_data("offset_z"))
	);
	if (_i("max_positions") == 0) {
		(*s)["max_positions"] = bee::SIDP(static_cast<int>(SIDP_v(lat->get_data("levelvector")).size()));
	}
}

#endif // RES_OBJ_PLAYER
