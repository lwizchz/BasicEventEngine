/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

class ObjBee : public BEE::Object/*<ObjBee>*/ {
	public:
		ObjBee();
		~ObjBee();

		void update(BEE::Instance*);
		void create(BEE::Instance*);
		void destroy(BEE::Instance*);
		void alarm(BEE::Instance*, int);
		void step_mid(BEE::Instance*);
		void keyboard_press(BEE::Instance*, SDL_Event*);
		void mouse_press(BEE::Instance*, SDL_Event*);
		void mouse_input(BEE::Instance*, SDL_Event*);
		void commandline_input(BEE::Instance*, const std::string&);
		void outside_room(BEE::Instance*);
		void collision(BEE::Instance*, BEE::Instance*);
		void draw(BEE::Instance*);
};
ObjBee::ObjBee() : Object/*<ObjBee>*/("obj_bee", "bee.hpp") {
	implemented_events[BEE_EVENT_UPDATE] = true;
	implemented_events[BEE_EVENT_CREATE] = true;
	implemented_events[BEE_EVENT_DESTROY] = true;
	implemented_events[BEE_EVENT_ALARM] = true;
	implemented_events[BEE_EVENT_STEP_MID] = true;
	implemented_events[BEE_EVENT_KEYBOARD_PRESS] = true;
	implemented_events[BEE_EVENT_MOUSE_PRESS] = true;
	implemented_events[BEE_EVENT_MOUSE_INPUT] = true;
	implemented_events[BEE_EVENT_COMMANDLINE_INPUT] = true;
	implemented_events[BEE_EVENT_OUTSIDE_ROOM] = true;
	implemented_events[BEE_EVENT_COLLISION] = true;
	implemented_events[BEE_EVENT_DRAW] = true;

	set_sprite(spr_bee);
}
ObjBee::~ObjBee() {}
void ObjBee::update(BEE::Instance* self) {
	if (!instance_data.empty()) {
		s = &instance_data[self->id];
	}
}
void ObjBee::create(BEE::Instance* self) {
	instance_data[self->id].clear();
	update(self);

	self->get_physbody()->set_mass(0.0);
	self->get_physbody()->set_shape(BEE_PHYS_SHAPE_BOX, new double[3] {100.0, 100.0, 100.0});
	self->get_physbody()->add_constraint(BEE_PHYS_CONSTRAINT_2D, nullptr);

	// create event
	std::cout << "u r a b " << self->id << "\n";
	(*s)["fps_display"] = (void*)nullptr;

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
			BEE::ParticleSystem* part_system = new BEE::ParticleSystem(game);
			//part_system->following = nullptr;

			//BEE::Particle* part_done = new BEE::Particle(game, BEE_PT_SHAPE_EXPLOSION, 0.5, 100);

			BEE::Particle* part_firework = new BEE::Particle(game, BEE_PT_SHAPE_SNOW, 0.5, 10000, true);
			//BEE::Particle* part_firework = new BEE::Particle(game, spr_bee, 0.5, 10000, true);
			part_firework->velocity = {20.0, 270.0};
			part_firework->angle_increase = 0.3;
			//part_firework->color = game->get_enum_color(c_orange, 100);
			//part_firework->death_type = part_done;
			//part_firework->is_sprite_lightable = true;

			BEE::ParticleEmitter* part_emitter = new BEE::ParticleEmitter();
			part_emitter->w = 1920;
			part_emitter->particle_type = part_firework;
			part_emitter->number = 3;
			part_system->emitters.push_back(part_emitter);

			BEE::ParticleDestroyer* part_destroyer = new BEE::ParticleDestroyer();
			part_destroyer->y = 1000;
			part_destroyer->w = 1920;
			part_system->destroyers.push_back(part_destroyer);

			BEE::ParticleAttractor* part_attr = new BEE::ParticleAttractor();
			part_attr->x = 300;
			part_attr->y = 1000;
			part_attr->w = 200;
			part_attr->h = 50;
			part_attr->max_distance = 500;
			part_system->attractors.push_back(part_attr);

			game->get_current_room()->add_particle_system(part_system);
			//part_system->fast_forward(300);
		}
	}
}
void ObjBee::destroy(BEE::Instance* self) {
	if (self->id == 0) {
		if (game->net_get_is_connected()) {
			game->net_session_end();
		}

		delete (BEE::TextData*) _p("fps_display");
	}
	instance_data.erase(self->id);
}
void ObjBee::alarm(BEE::Instance* self, int a) {
	switch (a) {
		case 0: { // Alarm 0
			//snd_chirp->play();
			//self->set_alarm(0, 2000);
			break;
		}
	}

}
void ObjBee::step_mid(BEE::Instance* self) {
	/*int mx, my;
	std::tie(mx, my) = game->get_mouse_position();
	std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10, game->get_delta());
	self->x = c.first;
	self->y = c.second;*/

	if (self->id == 0) {
		if (game->render_get_3d()) {
			//game->render_set_camera(new BEE::Camera(glm::vec3(1920.0/2.0, 1080.0/2.0, -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			//game->render_set_camera(new BEE::Camera(glm::vec3(1920.0/2.0 + s->camx, 1080.0/2.0 + s->camy, -540.0 + s->camz), glm::vec3((1920.0/2.0-game->get_mouse_global_x())/1920.0, (1080.0/2.0-game->get_mouse_global_y())/1080.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			game->render_set_camera(new BEE::Camera(glm::vec3(_d("camx"), _d("camy"), -540.0 + _d("camz")), glm::vec3((-1920.0/2.0+game->get_mouse_global_x())/1920.0*2.0, (-1080.0/2.0+game->get_mouse_global_y())/1080.0*2.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			//game->render_set_camera(new BEE::Camera(glm::vec3(1920.0-game->get_mouse_global_x(), 1080.0-game->get_mouse_global_y(), -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		}
	}
}
void ObjBee::keyboard_press(BEE::Instance* self, SDL_Event* e) {
	if (self->id != 0) {
		return;
	}

	if (game->console_get_is_open()) {
		return;
	}

	switch (e->key.keysym.sym) {
		case SDLK_RETURN: {
			game->set_transition_type((bee_transition_t)(game->get_transition_type()+1));
			game->restart_room();
			break;
		}

		/*case SDLK_w: {
			game->sound_stop_all();
			break;
		}
		case SDLK_a: {
			game->set_volume(game->get_volume()/2);
			break;
		}
		case SDLK_d: {
			game->set_volume(game->get_volume()*2);
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
			self->path_start(path_bee, 100.0, BEE_PATH_END_STOP, true);
			self->set_path_drawn(true);
			break;
		}

		case SDLK_z: {
			game->net_session_start("test_session", 4, "hostplayer");
			break;
		}
		case SDLK_x: {
			//game->net_session_join("127.0.0.1", "clientplayer");
			game->net_session_join("192.168.1.155", "clientplayer");
			break;
		}
		case SDLK_c: {
			std::map<std::string,std::string> servers = game->net_session_find();
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
			snd_chirp->effect_set(BEE_SE_NONE);

			if (snd_chirp->get_is_playing()) {
				snd_chirp->rewind();
			} else {
				snd_chirp->play();
			}
			break;
		}
		case SDLK_2: {
			snd_chirp->stop();
			snd_chirp->effect_set(BEE_SE_ECHO);
			snd_chirp->play();
			break;
		}

		case SDLK_n: {
			game->render_set_3d(true);
			break;
		}
		case SDLK_m: {
			game->render_set_3d(false);
			break;
		}
		case SDLK_b: {
			game->messenger_send({"bee"}, BEE_MESSAGE_INFO, self->serialize(true));
			(*s)["serialdata"] = self->serialize();
			break;
		}
		case SDLK_v: {
			self->deserialize(_s("serialdata"));
			break;
		}
	}
}
void ObjBee::mouse_press(BEE::Instance* self, SDL_Event* e) {
	if (game->console_get_is_open()) {
		return;
	}

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = {(int)self->get_corner_x(), (int)self->get_corner_y(), get_mask()->get_subimage_width(), get_mask()->get_height()};
			SDL_Rect b = {e->button.x-10, e->button.y-10, 20, 20};
			if ((self->id == 0)&&(!check_collision(a, b))) {
				if (self->is_place_empty(e->button.x, e->button.y)) {
					BEE::Instance* bee = game->get_current_room()->add_instance(-1, this, e->button.x, e->button.y, 0.0);
					bee->set_mass(1.0);
				}
			}
			break;
		}
	}
}
void ObjBee::mouse_input(BEE::Instance* self, SDL_Event* e) {
	if (game->console_get_is_open()) {
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

void ObjBee::commandline_input(BEE::Instance* self, const std::string& str) {
	std::cout << "bee" << self->id << ":~~~" << str << "~~~\n";
}
void ObjBee::outside_room(BEE::Instance* self) {
	game->get_current_room()->destroy(self);
}
void ObjBee::collision(BEE::Instance* self, BEE::Instance* other) {
	//self->move_away(2.0, other->get_x(), other->get_y());
}
void ObjBee::draw(BEE::Instance* self) {
	int size = 100;
	double r = radtodeg(self->get_physbody()->get_rotation_z());
	self->draw(size, size, r, c_white, SDL_FLIP_NONE);

	font_liberation->draw_fast(self->get_corner_x(), self->get_corner_y(), bee_itos(self->id));

	lt_bee->set_position(glm::vec4(self->get_x(), self->get_y(), 0.0, 1.0));
	lt_bee->set_color({(Uint8)(self->id*50), (Uint8)(self->id*20), 255, 255});
	lt_bee->queue();

	if (self->id == 0) {
		lt_ambient->queue();

		float t = (float)game->get_ticks()/1000.0f;
		float a = 180.0f + radtodeg(sin(t));
		mesh_monkey->draw(glm::vec3(1000.0f+500.0f*cos(t), 500.0f+300.0f*sin(t), 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, a, 180.0f), {255, 255, 0, 255}, false);

		(*s)["fps_display"] = (void*)font_liberation->draw((BEE::TextData*) _p("fps_display"), 0, 0, "FPS: " + bee_itos(game->fps_stable));
	}
}
