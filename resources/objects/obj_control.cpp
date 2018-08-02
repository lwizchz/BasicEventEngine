/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_CONTROL
#define RES_OBJ_CONTROL 1

#include "../resources.hpp"

#include "obj_control.hpp"

ObjControl::ObjControl() : Object("obj_control", "obj_control.hpp") {
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_MID,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::DRAW,
		bee::E_EVENT::ROOM_START,
		bee::E_EVENT::GAME_START
	});
	this->set_is_persistent(true);
}
void ObjControl::create(bee::Instance* self) {
	scr_test->run_func("main", {}, nullptr);

	_p("text_fps") = nullptr;

	_d("camx") = 1920.0/2.0;
	_d("camy") = 1080.0/2.0;
	_d("camz") = 0.0;
	_d("camspeed") = 2.0;

	bee::mouse::set_pos(1920/2, 1080/2);

	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("RoomRestart", [] (const SDL_Event* e) {
		bee::set_transition_type(static_cast<bee::E_TRANSITION>(static_cast<int>(bee::get_transition_type())+1));
		bee::restart_room();
	}));

	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("MoveForward", true, [this] (const SDL_Event* e) {
		_d("camz") += _d("camspeed");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("MoveBackward", true, [this] (const SDL_Event* e) {
		_d("camz") -= _d("camspeed");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("MoveLeft", true, [this] (const SDL_Event* e) {
		_d("camx") -= _d("camspeed");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("MoveRight", true, [this] (const SDL_Event* e) {
		_d("camx") += _d("camspeed");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("MoveDown", true, [this] (const SDL_Event* e) {
		_d("camy") += _d("camspeed");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("MoveUp", true, [this] (const SDL_Event* e) {
		_d("camy") -= _d("camspeed");
	}));


	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("NetworkStartSession", [] (const SDL_Event* e) {
		bee::net::session_start("test_session", 4, "hostplayer");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("NetworkJoinSession", [] (const SDL_Event* e) {
		//bee::net::session_join("127.0.0.1", "clientplayer");
		bee::net::session_join("192.168.1.155", "clientplayer");
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("NetworkFindSessions", [] (const SDL_Event* e) {
		const std::map<std::string,std::string>& servers = bee::net::session_find();
		if (!servers.empty()) {
			std::cerr << "Available servers:\n";
			for (auto& srv : servers) {
				std::cerr << "\t" << srv.second << "\t" << srv.first << "\n";
			}
		} else {
			std::cerr << "No servers available\n";
		}
	}));

	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartSound", [] (const SDL_Event* e) {
		snd_chirp->stop();
		snd_chirp->effect_remove_all();
		snd_chirp->play();
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartSoundEcho", [] (const SDL_Event* e) {
		snd_chirp->stop();
		snd_chirp->effect_remove_all();
		snd_chirp->effect_add(bee::SoundEffect("echo", bee::E_SOUNDEFFECT::ECHO, {}));
		snd_chirp->play();
	}));

	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("Start3D", [] (const SDL_Event* e) {
		bee::render::set_3d(true);
	}));
	bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("End3D", [] (const SDL_Event* e) {
		bee::render::set_3d(false);
	}));
}
void ObjControl::destroy(bee::Instance* self) {
	if (bee::net::get_is_connected()) {
		bee::net::session_end();
	}

	delete static_cast<bee::TextData*>(_p("text_fps"));

	Object::destroy(self);
}
void ObjControl::step_mid(bee::Instance* self) {
	if (bee::render::get_3d()) {
		const std::pair<int,int> mpos (bee::mouse::get_pos());
		bee::render::set_camera(new bee::Camera(glm::vec3(_d("camx"), _d("camy"), -540.0 + _d("camz")), glm::vec3((-1920.0/2.0+mpos.first)/1920.0*2.0, (-1080.0/2.0+mpos.second)/1080.0*2.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	}
}
void ObjControl::mouse_press(bee::Instance* self, SDL_Event* e) {
	if (bee::console::get_is_open()) {
		return;
	}

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = self->get_aabb();
			SDL_Rect b = {e->button.x-10, e->button.y-10, 20, 20};
			if ((self->id == 0)&&(!util::check_collision(a, b))) {
				if (self->is_place_empty(e->button.x, e->button.y)) {
					bee::Instance* bee = bee::get_current_room()->add_instance(-1, obj_bee, btVector3(e->button.x, e->button.y, 0));
					bee->set_computation_type(bee::E_COMPUTATION::SEMIPLAYER);
					bee->set_mass(1.0);
				}
			}
			break;
		}
	}
}
void ObjControl::draw(bee::Instance* self) {
	lt_ambient->queue();

	_p("text_fps") = font_liberation->draw(static_cast<bee::TextData*>(_p("text_fps")), 0, 0, "FPS: " + std::to_string(bee::engine->fps_stable));
}
void ObjControl::room_start(bee::Instance* self) {
	bee::get_current_room()->add_instance(-1, obj_mesh, btVector3(0.0, 0.0, 0.0));
}
void ObjControl::game_start(bee::Instance* self) {
	bee::console::run("execfile(\"cfg/config.py\")"); // Configure default binds
}

#endif // RES_OBJ_CONTROL
