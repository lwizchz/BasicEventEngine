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
}

void ObjPlayer::create(bee::Instance* self) {
	(*s)["movement"] = 0;

	(*s)["shoot_last_fire"] = 0;
	(*s)["shoot_delay"] = 100;

	(*s)["super_max_charges"] = 2;
	(*s)["super_charges"] = (*s)["super_max_charges"];
	(*s)["super_last_fire"] = 0;
	(*s)["super_delay"] = 500;

	// State handling
	bee::StateMachine* sm = new bee::StateMachine();

	bee::State state_still ("Still", {"MoveClockwise", "MoveCounterClockwise", "Shoot", "Super"});
	bee::State state_move_cw ("MoveClockwise", {"Still", "MoveCounterClockwise", "Shoot", "Super"});
	state_move_cw.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("movement") <= 0) {
			sm->pop_state_all("MoveClockwise");
			return;
		}

		(*s)["movement"] -= 1;
	};
	bee::State state_move_ccw ("MoveCounterClockwise", {"Still", "MoveClockwise", "Shoot", "Super"});
	state_move_ccw.update_func = [this, self, sm] (Uint32 ticks) {
		if (_i("movement") >= 0) {
			sm->pop_state_all("MoveCounterClockwise");
			return;
		}

		(*s)["movement"] += 1;
	};
	bee::State state_shoot ("Shoot", {"Still", "MoveClockwise", "MoveCounterClockwise", "Super"});
	state_shoot.start_func = [this, self, sm] () {
		if (static_cast<int>(bee::get_ticks()) - _i("shoot_last_fire") > _i("shoot_delay")) {
			(*s)["shoot_last_fire"] = static_cast<int>(bee::get_ticks());
		}
	};
	state_shoot.update_func = [this, self, sm] (Uint32 ticks) {
		if (static_cast<int>(bee::get_ticks()) - _i("shoot_last_fire") > _i("shoot_delay")) {
			sm->pop_state_all("Shoot");
		}
	};
	bee::State state_super ("Super", {"Still", "MoveClockwise", "MoveCounterClockwise", "Shoot"});
	state_super.start_func = [this, self, sm] () {
		if (_i("super_charges") <= 0) {
			sm->pop_state_all("Super");
			return;
		}

		if (static_cast<int>(bee::get_ticks()) - _i("super_last_fire") > _i("super_delay")) {
			(*s)["super_charges"] -= 1;
			(*s)["super_last_fire"] = static_cast<int>(bee::get_ticks());
		}
	};
	state_super.update_func = [this, self, sm] (Uint32 ticks) {
		if (static_cast<int>(bee::get_ticks()) - _i("super_last_fire") > _i("super_delay")) {
			sm->pop_state_all("Super");
		}
	};

	sm->add_state(state_still);
	sm->add_state(state_move_cw);
	sm->add_state(state_move_ccw);
	sm->add_state(state_shoot);
	sm->add_state(state_super);
	sm->push_state("Still");
	(*s)["sm"] = static_cast<void*>(sm);
}
void ObjPlayer::destroy(bee::Instance* self) {
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
	int cx = bee::get_width()/2;
	spr_logo->draw(cx-spr_logo->get_width()/2, 200, -1);

	bee::set_is_lightable(false);
	bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
	bee::engine->font_default->draw_fast(0, 200, sm->get_states(), {255, 255, 255, 255});
	bee::set_is_lightable(true);
}

#endif // RES_OBJ_PLAYER
