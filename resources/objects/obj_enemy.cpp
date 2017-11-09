/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_ENEMY
#define RES_OBJ_ENEMY 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "obj_enemy.hpp"

#include "obj_player.hpp"

ObjEnemy::ObjEnemy() : Object("obj_enemy", "obj_enemy.cpp") {
	implemented_events.insert({
		bee::E_EVENT::UPDATE,
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_MID,
		bee::E_EVENT::DRAW
	});
	set_depth(1);
}

void ObjEnemy::create(bee::Instance* self) {
	(*s)["health"] = 100;
	(*s)["damage"] = 100;

	(*s)["position"] = 0;
	(*s)["max_positions"] = 0; // This is set on first run by update_position()
	(*s)["movement"] = 0;
	(*s)["movement_speed"] = 40;
	(*s)["approach"] = -1.0 * static_cast<double>(random_range(100, 1000));

	VectorSprite* vs = new VectorSprite("resources/sprites/enemy.csv");
	(*s)["vsprite"] = static_cast<void*>(vs);

	this->update_position(self);

	// State handling
	bee::StateMachine* sm = new bee::StateMachine();

	bee::State state_still ("Still", {"Approach"});
	state_still.update_func = [this, self, sm] (Uint32 ticks) {
		if (_d("approach") < 0.0) {
			(*s)["approach"] += static_cast<double>(random_range(1, 3));
			this->update_position(self);
		} else {
			sm->pop_state_all("Still");
			sm->push_state("Approach");
		}
	};
	bee::State state_approach ("Approach", {"Attack", "Dead"});
	state_approach.update_func = [this, self, sm] (Uint32 ticks) {
		if (_d("approach") < 100.0) {
			(*s)["approach"] += bee::SIDP(1.0);
			this->update_position(self);
		} else {
			(*s)["movement"] = (*s)["movement_speed"];
			sm->pop_state_all("Approach");
			sm->push_state("Attack");
		}
	};
	bee::State state_attack ("Attack", {"Dead"});
	state_attack.update_func = [this, self, sm] (Uint32 ticks) {
		bee::Instance* player = obj_player->get_instance(0);
		if (player == nullptr) {
			return;
		}

		int player_pos = SIDP_i(player->get_data("position"))/10;
		int pos = _i("position");
		if (player_pos == pos) {
			ObjPlayer* obj = static_cast<ObjPlayer*>(obj_player);
			obj->update(player);
			obj->hurt(player, _i("damage"));

			sm->push_state("Dead");

			return;
		}

		if (_i("movement") > 0) {
			(*s)["movement"] -= 1;
			return;
		}

		if (qmod(player_pos-1, _i("max_positions")) >= pos) {
			++pos;
		} else if (qmod(player_pos+1, _i("max_positions")) <= pos) {
			--pos;
		}
		(*s)["position"] = qmod(pos, _i("max_positions"));
		this->update_position(self);

		(*s)["movement"] = (*s)["movement_speed"];
	};
	bee::State state_dead ("Dead", {});
	state_dead.start_func = [this, self, sm] () {
		bee::get_current_room()->destroy(self);
	};

	sm->add_state(state_still);
	sm->add_state(state_approach);
	sm->add_state(state_attack);
	sm->add_state(state_dead);
	sm->push_state("Still");
	(*s)["sm"] = static_cast<void*>(sm);
}
void ObjEnemy::destroy(bee::Instance* self) {
	VectorSprite* vs = static_cast<VectorSprite*>(_p("vsprite"));
	delete vs;

	bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
	delete sm;

	Object::destroy(self);
}
void ObjEnemy::step_mid(bee::Instance* self) {
	bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
	sm->update_all();
}
void ObjEnemy::draw(bee::Instance* self) {
	bee::RGBA c (255, 0, 0, 255);

	VectorSprite* vs = static_cast<VectorSprite*>(_p("vsprite"));
	vs->draw(glm::vec3(self->get_x(), self->get_y(), self->get_z()), glm::vec3(0.0, 0.0, 0.0), c);
}

void ObjEnemy::update_position(bee::Instance* self) {
	bee::Instance* lat = obj_lattice->get_instance(0);
	if (lat == nullptr) {
		return;
	}

	if (SIDP_v(lat->get_data("levelvector")).size() < static_cast<unsigned int>(_i("position"))) {
		return;
	}

	if (_i("max_positions") == 0) {
		(*s)["max_positions"] = bee::SIDP(static_cast<int>(SIDP_v(lat->get_data("levelvector")).size()));
		(*s)["position"] = static_cast<int>(random(_i("max_positions")));
	}

	double approach = 2000.0-2000.0*_d("approach")/100.0;

	PrimitiveLine* l = static_cast<PrimitiveLine*>(SIDP_cp(lat->get_data("levelvector"), _i("position")));
	self->set_position(
		l->p1.x + SIDP_d(lat->get_data("offset_x")),
		l->p1.y + SIDP_d(lat->get_data("offset_y")),
		l->p1.z + SIDP_d(lat->get_data("offset_z")) + approach
	);
}
void ObjEnemy::hurt(bee::Instance* self, int damage) {
	(*s)["health"] -= damage;
	if (_i("health") <= 0) {
		bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
		sm->push_state("Dead");
	}
}

#endif // RES_OBJ_ENEMY
