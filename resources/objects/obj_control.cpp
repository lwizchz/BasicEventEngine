/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
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
	scr_test->run_func("main");

	(*s)["text_fps"] = static_cast<void*>(nullptr);

	(*s)["camx"] = 0.0;
	(*s)["camy"] = 0.0;
	(*s)["camz"] = 0.0;
	(*s)["camspeed"] = 2.0;

	(*s)["part_system"] = static_cast<void*>(nullptr);
	bool enable_partsys = true;
	//bool enable_partsys = false;
	if (enable_partsys) {
		bee::ParticleSystem* part_system = new bee::ParticleSystem();

		/*bee::Particle* part_done = new bee::Particle(bee::E_PT_SHAPE::EXPLOSION, 0.5, 1000);
		part_done->velocity = {50.0, 90.0};
		part_system->add_particle_type(part_done);*/

		bee::Particle* part_firework = new bee::Particle(bee::E_PT_SHAPE::SNOW, 0.5, 15000);
		part_firework->velocity = {100.0, 270.0};
		part_firework->angle_increase = 0.2;
		part_firework->color = bee::RGBA(bee::E_RGB::ORANGE, 200);
		//part_firework->set_death_type(part_done);
		//part_firework->is_sprite_lightable = true;
		part_system->add_particle_type(part_firework);

		bee::ParticleEmitter* part_emitter = new bee::ParticleEmitter(
			0.0, 0.0,
			1920, 1,
			part_firework
		);
		part_emitter->set_number(3);
		part_system->add_emitter(part_emitter);

		bee::ParticleDestroyer* part_destroyer = new bee::ParticleDestroyer(
			0.0, 1000.0,
			1920, 80
		);
		part_system->add_destroyer(part_destroyer);

		bee::ParticleAttractor* part_attr = new bee::ParticleAttractor(
			300.0, 1000.0,
			200, 50
		);
		part_attr->set_max_distance(500);
		part_system->add_attractor(part_attr);

		(*s)["part_system"] = part_system;
	}

	bee::console::add_keybind(SDLK_RETURN, bee::KeyBind("RoomRestart"), [] (const bee::MessageContents& msg) {
		bee::set_transition_type(static_cast<bee::E_TRANSITION>(static_cast<int>(bee::get_transition_type())+1));
		bee::restart_room();
	});

	bee::console::add_keybind(SDLK_w, bee::KeyBind("MoveForward", true), [this] (const bee::MessageContents& msg) {
		(*s)["camz"] += (*s)["camspeed"];
	});
	bee::console::add_keybind(SDLK_s, bee::KeyBind("MoveBackward", true), [this] (const bee::MessageContents& msg) {
		(*s)["camz"] -= (*s)["camspeed"];
	});
	bee::console::add_keybind(SDLK_a, bee::KeyBind("MoveLeft", true), [this] (const bee::MessageContents& msg) {
		(*s)["camx"] -= (*s)["camspeed"];
	});
	bee::console::add_keybind(SDLK_d, bee::KeyBind("MoveRight", true), [this] (const bee::MessageContents& msg) {
		(*s)["camx"] += (*s)["camspeed"];
	});
	bee::console::add_keybind(SDLK_q, bee::KeyBind("MoveDown", true), [this] (const bee::MessageContents& msg) {
		(*s)["camy"] += (*s)["camspeed"];
	});
	bee::console::add_keybind(SDLK_e, bee::KeyBind("MoveUp", true), [this] (const bee::MessageContents& msg) {
		(*s)["camy"] -= (*s)["camspeed"];
	});


	bee::console::add_keybind(SDLK_z, bee::KeyBind("NetworkStartSession"), [] (const bee::MessageContents& msg) {
		bee::net::session_start("test_session", 4, "hostplayer");
	});
	bee::console::add_keybind(SDLK_x, bee::KeyBind("NetworkJoinSession"), [] (const bee::MessageContents& msg) {
		//bee::net::session_join("127.0.0.1", "clientplayer");
		bee::net::session_join("192.168.1.155", "clientplayer");
	});
	bee::console::add_keybind(SDLK_c, bee::KeyBind("NetworkFindSessions"), [] (const bee::MessageContents& msg) {
		const std::map<std::string,std::string>& servers = bee::net::session_find();
		if (!servers.empty()) {
			std::cerr << "Available servers:\n";
			for (auto& srv : servers) {
				std::cerr << "\t" << srv.second << "\t" << srv.first << "\n";
			}
		} else {
			std::cerr << "No servers available\n";
		}
	});

	bee::console::add_keybind(SDLK_1, bee::KeyBind("StartSound"), [] (const bee::MessageContents& msg) {
		snd_chirp->effect_set(static_cast<int>(bee::E_SOUNDEFFECT::NONE));

		if (snd_chirp->get_is_playing()) {
			snd_chirp->rewind();
		} else {
			snd_chirp->play();
		}
	});
	bee::console::add_keybind(SDLK_2, bee::KeyBind("StartSoundEcho"), [] (const bee::MessageContents& msg) {
		snd_chirp->stop();
		snd_chirp->effect_set(static_cast<int>(bee::E_SOUNDEFFECT::ECHO));
		snd_chirp->play();
	});

	bee::console::add_keybind(SDLK_n, bee::KeyBind("Start3D"), [] (const bee::MessageContents& msg) {
		bee::render::set_3d(true);
	});
	bee::console::add_keybind(SDLK_m, bee::KeyBind("End3D"), [] (const bee::MessageContents& msg) {
		bee::render::set_3d(false);
	});
}
void ObjControl::destroy(bee::Instance* self) {
	//Unbind keybindings
	bee::console::unbind(bee::KeyBind("RoomRestart"));
	bee::console::unbind(bee::KeyBind("MoveForward"));
	bee::console::unbind(bee::KeyBind("MoveBackward"));
	bee::console::unbind(bee::KeyBind("MoveLeft"));
	bee::console::unbind(bee::KeyBind("MoveRight"));
	bee::console::unbind(bee::KeyBind("MoveDown"));
	bee::console::unbind(bee::KeyBind("MoveUp"));
	bee::console::unbind(bee::KeyBind("NetworkStartSession"));
	bee::console::unbind(bee::KeyBind("NetworkJoinSession"));
	bee::console::unbind(bee::KeyBind("NetworkFindSessions"));
	bee::console::unbind(bee::KeyBind("StartSound"));
	bee::console::unbind(bee::KeyBind("StartSoundEcho"));
	bee::console::unbind(bee::KeyBind("Start3D"));
	bee::console::unbind(bee::KeyBind("End3D"));

	if (_p("part_system") != nullptr) {
		delete static_cast<bee::ParticleSystem*>(_p("part_system"));
	}

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
			if ((self->id == 0)&&(!check_collision(a, b))) {
				if (self->is_place_empty(e->button.x, e->button.y)) {
					bee::Instance* bee = bee::get_current_room()->add_instance(-1, obj_bee, e->button.x, e->button.y, 0.0);
					bee->set_mass(1.0);
				}
			}
			break;
		}
	}
}
void ObjControl::draw(bee::Instance* self) {
	lt_ambient->queue();

	float t = static_cast<float>(bee::get_ticks())/1000.0f;
	float a = 180.0f + radtodeg(sin(t));
	mesh_monkey->draw(glm::vec3(1000.0f+500.0f*cos(t), 500.0f+300.0f*sin(t), 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, a, 180.0f), {255, 255, 0, 255}, false);

	(*s)["text_fps"] = static_cast<void*>(font_liberation->draw(static_cast<bee::TextData*>(_p("text_fps")), 0, 0, "FPS: " + bee_itos(bee::engine->fps_stable)));
}
void ObjControl::room_start(bee::Instance* self) {
	bee::ParticleSystem* part_system = static_cast<bee::ParticleSystem*>(_p("part_system"));
	if (part_system != nullptr) {
		//part_system->fast_forward(300);
		bee::get_current_room()->add_particle_system(part_system);
	}
}
void ObjControl::game_start(bee::Instance* self) {
	bee::console::run("execfile(\"cfg/config.py\")"); // Configure default binds
}

#endif // RES_OBJ_CONTROL
