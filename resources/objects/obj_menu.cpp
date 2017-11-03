/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_MENU
#define RES_OBJ_MENU 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "obj_menu.hpp"

ObjMenu::ObjMenu() : Object("obj_menu", "obj_menu.cpp") {
	implemented_events.insert({
		bee::E_EVENT::UPDATE,
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DRAW,
		bee::E_EVENT::GAME_START
	});
}

void ObjMenu::create(bee::Instance* self) {

}
void ObjMenu::draw(bee::Instance* self) {
	int cx = bee::get_width()/2;
	spr_logo->draw(cx-spr_logo->get_width()/2, 100, -1);
}
void ObjMenu::game_start(bee::Instance* self) {
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("Quit"), [this, self] (const bee::MessageContents& msg) mutable {
		if (bee::get_current_room() == rm_menu) {
			bee::end_game();
			return;
		}
		bee::change_room(rm_menu);
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("Shoot"), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::change_room(rm_level_1);
			}
			return;
		}
		s = &self->get_data();

		bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
		sm->push_state("Shoot");
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("Super"), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::change_room(rm_level_1);
			}
			return;
		}
		s = &self->get_data();

		bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
		sm->push_state("Super");
	});

	bee::console::run("exec \"config.cfg\""); // Configure default binds
}

#endif // RES_OBJ_MENU
