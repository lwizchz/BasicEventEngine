/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

class ObjBee : public bee::Object/*<ObjBee>*/ {
	public:
		ObjBee();
		~ObjBee();

		void update(bee::Instance*);
		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void alarm(bee::Instance*, int);
		void step_mid(bee::Instance*);
		void keyboard_press(bee::Instance*, SDL_Event*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void mouse_input(bee::Instance*, SDL_Event*);
		void commandline_input(bee::Instance*, const std::string&);
		void outside_room(bee::Instance*);
		void collision(bee::Instance*, bee::Instance*);
		void draw(bee::Instance*);
};
ObjBee::ObjBee() : Object/*<ObjBee>*/("obj_bee", "bee.hpp") {
	implemented_events[bee::E_EVENT::UPDATE] = true;
	implemented_events[bee::E_EVENT::CREATE] = true;
	implemented_events[bee::E_EVENT::DESTROY] = true;
	implemented_events[bee::E_EVENT::ALARM] = true;
	implemented_events[bee::E_EVENT::STEP_MID] = true;
	implemented_events[bee::E_EVENT::KEYBOARD_PRESS] = true;
	implemented_events[bee::E_EVENT::MOUSE_PRESS] = true;
	implemented_events[bee::E_EVENT::MOUSE_INPUT] = true;
	implemented_events[bee::E_EVENT::COMMANDLINE_INPUT] = true;
	implemented_events[bee::E_EVENT::OUTSIDE_ROOM] = true;
	implemented_events[bee::E_EVENT::COLLISION] = true;
	implemented_events[bee::E_EVENT::DRAW] = true;

	set_sprite(spr_bee);
}
ObjBee::~ObjBee() {}
void ObjBee::update(bee::Instance* self) {
	if (!instance_data.empty()) {
		s = &instance_data[self->id];
	}
}
void ObjBee::create(bee::Instance* self) {
	instance_data[self->id].clear();
	update(self);

	self->get_physbody()->set_mass(0.0);
	self->get_physbody()->set_shape(bee::E_PHYS_SHAPE::BOX, new double[3] {100.0, 100.0, 100.0});
	self->get_physbody()->add_constraint(bee::E_PHYS_CONSTRAINT::FLAT, nullptr);

	// create event
	std::cout << "u r a b " << self->id << "\n";
	(*s)["text_id"] = (void*)nullptr;
	(*s)["text_fps"] = (void*)nullptr;

	//self->set_alarm(0, 2000);
	//spr_bee->set_alpha(0.5);
	//self->set_gravity(7.0);
	//show_message(font_liberation->get_fontname());

	if (self->id == 0) {
		(*s)["camx"] = 0.0;
		(*s)["camy"] = 0.0;
		(*s)["camz"] = 0.0;
		(*s)["camspeed"] = 2.0;

		(*s)["serialdata"] = self->serialize();

		bool enable_partsys = false;
		if (enable_partsys) {
			bee::ParticleSystem* part_system = new bee::ParticleSystem();
			//part_system->following = nullptr;

			//bee::Particle* part_done = new bee::Particle(bee::E_PT_SHAPE::EXPLOSION, 0.5, 100);

			bee::Particle* part_firework = new bee::Particle(bee::E_PT_SHAPE::SNOW, 0.5, 10000, true);
			//bee::Particle* part_firework = new bee::Particle(spr_bee, 0.5, 10000, true);
			part_firework->velocity = {20.0, 270.0};
			part_firework->angle_increase = 0.3;
			//part_firework->color = get_enum_color(c_orange, 100);
			//part_firework->death_type = part_done;
			//part_firework->is_sprite_lightable = true;

			bee::ParticleEmitter* part_emitter = new bee::ParticleEmitter();
			part_emitter->w = 1920;
			part_emitter->particle_type = part_firework;
			part_emitter->number = 3;
			part_system->emitters.push_back(part_emitter);

			bee::ParticleDestroyer* part_destroyer = new bee::ParticleDestroyer();
			part_destroyer->y = 1000;
			part_destroyer->w = 1920;
			part_system->destroyers.push_back(part_destroyer);

			bee::ParticleAttractor* part_attr = new bee::ParticleAttractor();
			part_attr->x = 300;
			part_attr->y = 1000;
			part_attr->w = 200;
			part_attr->h = 50;
			part_attr->max_distance = 500;
			part_system->attractors.push_back(part_attr);

			bee::get_current_room()->add_particle_system(part_system);
			//part_system->fast_forward(300);
		}
	}
}
void ObjBee::destroy(bee::Instance* self) {
	delete (bee::TextData*) _p("text_id");

	if (self->id == 0) {
		if (bee::net_get_is_connected()) {
			bee::net_session_end();
		}
		delete (bee::TextData*) _p("text_fps");
	}

	instance_data.erase(self->id);
}
void ObjBee::alarm(bee::Instance* self, int a) {
	switch (a) {
		case 0: { // Alarm 0
			//snd_chirp->play();
			//self->set_alarm(0, 2000);
			break;
		}
		default: {}
	}

}
void ObjBee::step_mid(bee::Instance* self) {
	/*int mx, my;
	std::tie(mx, my) = get_mouse_position();
	std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10, get_delta());
	self->x = c.first;
	self->y = c.second;*/

	if (self->id == 0) {
		if (bee::render_get_3d()) {
			//bee::render_set_camera(new bee::Camera(glm::vec3(1920.0/2.0, 1080.0/2.0, -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			//bee::render_set_camera(new bee::Camera(glm::vec3(1920.0/2.0 + s->camx, 1080.0/2.0 + s->camy, -540.0 + s->camz), glm::vec3((1920.0/2.0-bee::get_mouse_global_x())/1920.0, (1080.0/2.0-get_mouse_global_y())/1080.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			bee::render_set_camera(new bee::Camera(glm::vec3(_d("camx"), _d("camy"), -540.0 + _d("camz")), glm::vec3((-1920.0/2.0+bee::get_mouse_global_x())/1920.0*2.0, (-1080.0/2.0+bee::get_mouse_global_y())/1080.0*2.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			//bee::render_set_camera(new bee::Camera(glm::vec3(1920.0-bee::get_mouse_global_x(), 1080.0-bee::get_mouse_global_y(), -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		}
	}
}
void ObjBee::keyboard_press(bee::Instance* self, SDL_Event* e) {
	if (self->id != 0) {
		return;
	}

	if (bee::console_get_is_open()) {
		return;
	}

	switch (e->key.keysym.sym) {
		case SDLK_RETURN: {
			bee::set_transition_type((bee::E_TRANSITION)((int)bee::get_transition_type()+1));
			bee::restart_room();
			break;
		}

		/*case SDLK_w: {
			sound_stop_all();
			break;
		}
		case SDLK_a: {
			set_volume(get_volume()/2);
			break;
		}
		case SDLK_d: {
			set_volume(get_volume()*2);
			break;
		}*/
		case SDLK_w: {
			(*s)["camz"] += (*s)["camspeed"];
			break;
		}
		case SDLK_s: {
			(*s)["camz"] -= (*s)["camspeed"];
			break;
		}
		case SDLK_a: {
			(*s)["camx"] -= (*s)["camspeed"];
			break;
		}
		case SDLK_d: {
			(*s)["camx"] += (*s)["camspeed"];
			break;
		}
		case SDLK_q: {
			(*s)["camy"] += (*s)["camspeed"];
			break;
		}
		case SDLK_e: {
			(*s)["camy"] -= (*s)["camspeed"];
			break;
		}

		case SDLK_y: {
			self->set_mass(1.0);
			self->path_start(path_bee, 100.0, bee::E_PATH_END::STOP, true);
			self->set_path_drawn(true);
			break;
		}

		case SDLK_z: {
			bee::net_session_start("test_session", 4, "hostplayer");
			break;
		}
		case SDLK_x: {
			//net_session_join("127.0.0.1", "clientplayer");
			bee::net_session_join("192.168.1.155", "clientplayer");
			break;
		}
		case SDLK_c: {
			std::map<std::string,std::string> servers = bee::net_session_find();
			if (!servers.empty()) {
				std::cerr << "Available servers:\n";
				for (auto& srv : servers) {
					std::cerr << "\t" << srv.second << "\t" << srv.first << "\n";
				}
			} else {
				std::cerr << "No servers available\n";
			}
			break;
		}

		case SDLK_1: {
			//snd_chirp->stop();
			snd_chirp->effect_set((int)bee::E_SOUNDEFFECT::NONE);

			if (snd_chirp->get_is_playing()) {
				snd_chirp->rewind();
			} else {
				snd_chirp->play();
			}
			break;
		}
		case SDLK_2: {
			snd_chirp->stop();
			snd_chirp->effect_set((int)bee::E_SOUNDEFFECT::ECHO);
			snd_chirp->play();
			break;
		}

		case SDLK_n: {
			bee::render_set_3d(true);
			break;
		}
		case SDLK_m: {
			bee::render_set_3d(false);
			break;
		}
		case SDLK_b: {
			bee::messenger_send({"bee"}, bee::E_MESSAGE::INFO, self->serialize(true));
			(*s)["serialdata"] = self->serialize();
			break;
		}
		case SDLK_v: {
			self->deserialize(_s("serialdata"));
			break;
		}
	}
}
void ObjBee::mouse_press(bee::Instance* self, SDL_Event* e) {
	if (bee::console_get_is_open()) {
		return;
	}

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = {(int)self->get_corner_x(), (int)self->get_corner_y(), get_mask()->get_subimage_width(), get_mask()->get_height()};
			SDL_Rect b = {e->button.x-10, e->button.y-10, 20, 20};
			if ((self->id == 0)&&(!check_collision(a, b))) {
				if (self->is_place_empty(e->button.x, e->button.y)) {
					bee::Instance* bee = bee::get_current_room()->add_instance(-1, this, e->button.x, e->button.y, 0.0);
					bee->set_mass(1.0);
				}
			}
			break;
		}
	}
}
void ObjBee::mouse_input(bee::Instance* self, SDL_Event* e) {
	if (bee::console_get_is_open()) {
		return;
	}

	if (e->type == SDL_MOUSEMOTION) {
		//spr_bee->set_alpha(0.25);
	}
	if (e->motion.state & SDL_BUTTON_LMASK) {
		SDL_Rect a = {(int)self->get_corner_x(), (int)self->get_corner_y(), get_mask()->get_subimage_width(), get_mask()->get_height()};
		SDL_Rect b = {e->button.x-10, e->button.y-10, 20, 20};
		if (check_collision(a, b)) {
			self->set_position(e->button.x, e->button.y, 0.0);
		}
	}
}

void ObjBee::commandline_input(bee::Instance* self, const std::string& str) {
	std::cout << "bee" << self->id << ":~~~" << str << "~~~\n";
}
void ObjBee::outside_room(bee::Instance* self) {
	bee::get_current_room()->destroy(self);
}
void ObjBee::collision(bee::Instance* self, bee::Instance* other) {
	//self->move_away(2.0, other->get_x(), other->get_y());
}
void ObjBee::draw(bee::Instance* self) {
	int size = 100;
	double r = radtodeg(self->get_physbody()->get_rotation_z());
	self->draw(size, size, r, bee::E_RGB::WHITE, SDL_FLIP_NONE);

	(*s)["text_id"] = (void*)font_liberation->draw((bee::TextData*) _p("text_id"), self->get_corner_x(), self->get_corner_y(), bee_itos(self->id));

	lt_bee->set_position(glm::vec4(self->get_x(), self->get_y(), 0.0, 1.0));
	lt_bee->set_color({(Uint8)(self->id*50), (Uint8)(self->id*20), 255, 255});
	lt_bee->queue();

	if (self->id == 0) {
		lt_ambient->queue();

		float t = (float)bee::get_ticks()/1000.0f;
		float a = 180.0f + radtodeg(sin(t));
		mesh_monkey->draw(glm::vec3(1000.0f+500.0f*cos(t), 500.0f+300.0f*sin(t), 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, a, 180.0f), {255, 255, 0, 255}, false);

		(*s)["text_fps"] = (void*)font_liberation->draw((bee::TextData*) _p("text_fps"), 0, 0, "FPS: " + bee_itos(bee::engine->fps_stable));
	}
}
