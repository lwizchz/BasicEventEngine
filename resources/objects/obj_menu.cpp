/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_MENU
#define RES_OBJ_MENU 1

#include <fstream>

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "obj_menu.hpp"

#include "obj_player.hpp"

ObjMenu::ObjMenu() : Object("obj_menu", "obj_menu.cpp") {
	implemented_events.insert({
		bee::E_EVENT::UPDATE,
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_MID,
		bee::E_EVENT::DRAW,
		bee::E_EVENT::GAME_START
	});
}

void ObjMenu::create(bee::Instance* self) {
	(*s)["controls_td"] = static_cast<void*>(nullptr);
	(*s)["controls_str"] = "";

	bee::console::run("let level_index = 0");
}
void ObjMenu::destroy(bee::Instance* self) {
	if (_p("controls_td") != nullptr) {
		bee::TextData* td = static_cast<bee::TextData*>(_p("controls_td"));
		delete td;
	}

	Object::destroy(self);
}
void ObjMenu::step_mid(bee::Instance* self) {
	bee::render::set_camera(new bee::Camera(glm::vec3(bee::get_width()/2.0, bee::get_height()/2.0, -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
}
void ObjMenu::draw(bee::Instance* self) {
	int cx = bee::get_width()/2;
	spr_logo->draw(cx-spr_logo->get_width()/2, 100, -1);

	if ((_s("controls_str").empty())&&(bee::console::get_keycode("Quit") != SDLK_UNKNOWN)) {
		std::string c;
		for (auto& e : {"MoveClockwise", "MoveCounterClockwise", "FireBeam", "Super", "Quit"}) {
			c += std::string(e) + " : " + bee::keystrings_get_name(bee::console::get_keycode(e)) + "\n";
		}
		c += "\n\nShoot red guys, collect yellow guys";
		(*s)["controls_str"] = c;
	}
	(*s)["controls_td"] = bee::engine->font_default->draw(static_cast<bee::TextData*>(_p("controls_td")), cx-150, 550, _s("controls_str"), {255, 255, 255, 255});
}
void ObjMenu::game_start(bee::Instance* self) {
	snd_main->loop();

	// Add commands
	bee::console::add_command("CreateLevel", [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_lattice->get_instance(0);
		if (self == nullptr) {
			return;
		}
		s = &self->get_data();

		bee::get_current_room()->destroy_all(obj_player);
		bee::get_current_room()->destroy_all(obj_enemy);
		bee::get_current_room()->destroy_all(obj_bee);
		_v("levelvector").clear();

		(*s)["offset_x"] = 0;
		(*s)["offset_y"] = 0;
		(*s)["offset_z"] = 0;
		(*s)["is_creating"] = true;
	});
	bee::console::add_command("SaveLevel", [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_lattice->get_instance(0);
		if (self == nullptr) {
			return;
		}
		s = &self->get_data();

		std::vector<bee::SIDP> params = bee::console::internal::parse_parameters(msg.descr, true); // Parse the parameters from the given command
		if (params.size() < 2) {
			bee::console::log(bee::E_MESSAGE::WARNING, "Failed to save level: no filename provided");
			return;
		}

		const std::string levelpath ("resources/rooms/"+SIDP_s(params[1])+".csv");
		if (file_exists(levelpath)) {
			bee::console::log(bee::E_MESSAGE::WARNING, "Failed to save level: file already exists");
			return;
		}

		std::ofstream levelfile (levelpath, std::ios::trunc);
		if (!levelfile.good()) {
			bee::console::log(bee::E_MESSAGE::WARNING, "Failed to save level: could not open file");
			return;
		}

		(*s)["is_creating"] = false;

		PrimitiveLine* prev_line = nullptr;
		for (auto& p : _v("levelvector")) {
			PrimitiveLine* pl = static_cast<PrimitiveLine*>(SIDP_p(p));
			if (prev_line != nullptr) {
				levelfile <<
					prev_line->p2[0] << "\t" << prev_line->p2[1] << "\t" << prev_line->p2[2] << "\t\t" <<
					pl->p2[0] << "\t" << pl->p2[1] << "\t" << pl->p2[2] << "\n";
			}
			prev_line = pl;
		}
		levelfile.close();

		bee::console::run("LoadLevel " + SIDP_s(params[1]));
	});
	bee::console::add_command("LoadLevel", [this, self] (const bee::MessageContents& msg) mutable {
		std::vector<bee::SIDP> params = bee::console::internal::parse_parameters(msg.descr, true); // Parse the parameters from the given command
		if (params.size() < 2) {
			bee::console::log(bee::E_MESSAGE::WARNING, "Failed to load level: no filename provided");
			return;
		}
		bee::console::set_var("level", params[1]);
		bee::console::set_var("level_index", -1);

		bee::change_room(rm_level_1);
	});
	bee::console::add_command("LoadNextLevel", [this, self] (const bee::MessageContents& msg) mutable {
		if (SIDP_i(bee::console::get_var("$level_index")) < 0) {
			return;
		}
		std::string level (SIDP_s(bee::console::get_var("$levels[$level_index]")));
		bee::console::set_var("level", level);
		bee::change_room(rm_level_1);
	});

	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("Quit"), [this, self] (const bee::MessageContents& msg) mutable {
		if (bee::get_current_room() == rm_menu) {
			bee::end_game();
			return;
		}
		bee::change_room(rm_menu);
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("Reset"), [this, self] (const bee::MessageContents& msg) mutable {
		if (bee::get_current_room() == rm_menu) {
			return;
		}

		if (SIDP_i(bee::console::get_var("$level_index")) >= 0) {
			bee::console::run("let level_index += -1");
		}

		bee::restart_room();
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("MoveClockwise", true), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::console::run("LoadNextLevel");
			}
			return;
		}
		s = &self->get_data();

		self->set_data("movement", SIDP_i(self->get_data("movement"))+1);

		ObjPlayer* obj = static_cast<ObjPlayer*>(obj_player);
		obj->update_movement(self);
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("MoveCounterClockwise", true), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::console::run("LoadNextLevel");
			}
			return;
		}
		s = &self->get_data();

		self->set_data("movement", SIDP_i(self->get_data("movement"))-1);

		ObjPlayer* obj = static_cast<ObjPlayer*>(obj_player);
		obj->update_movement(self);
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("FirePellet"), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::console::run("LoadNextLevel");
			}
			return;
		}
		s = &self->get_data();

		bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
		sm->push_state("FirePellet");
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("FireBeam"), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::console::run("LoadNextLevel");
			}
			return;
		}
		s = &self->get_data();

		bee::StateMachine* sm = static_cast<bee::StateMachine*>(_p("sm"));
		sm->push_state("FireBeam");
	});
	bee::console::add_keybind(SDLK_UNKNOWN, bee::KeyBind("Super"), [this, self] (const bee::MessageContents& msg) mutable {
		self = obj_player->get_instance(0);
		if (self == nullptr) {
			if (bee::get_current_room() == rm_menu) {
				bee::console::run("LoadNextLevel");
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
