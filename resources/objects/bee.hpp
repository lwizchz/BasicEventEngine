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
		/*void update(BEE::InstanceData*) override final;
		void create(BEE::InstanceData*) override final;
		void destroy(BEE::InstanceData*) override final;
		void alarm(BEE::InstanceData*, int) override final;
		void step_mid(BEE::InstanceData*) override final;
		void keyboard_press(BEE::InstanceData*, SDL_Event*) override final;
		void mouse_press(BEE::InstanceData*, SDL_Event*) override final;
		void mouse_input(BEE::InstanceData*, SDL_Event*) override final;
		void commandline_input(BEE::InstanceData*, const std::string&) override final;
		void collision(BEE::InstanceData*, BEE::InstanceData*) override final;
		void draw(BEE::InstanceData*) override final;*/

		void update(BEE::InstanceData*);
		void create(BEE::InstanceData*);
		void destroy(BEE::InstanceData*);
		void alarm(BEE::InstanceData*, int);
		void step_mid(BEE::InstanceData*);
		void keyboard_press(BEE::InstanceData*, SDL_Event*);
		void mouse_press(BEE::InstanceData*, SDL_Event*);
		void mouse_input(BEE::InstanceData*, SDL_Event*);
		void commandline_input(BEE::InstanceData*, const std::string&);
		void collision(BEE::InstanceData*, BEE::InstanceData*);
		void draw(BEE::InstanceData*);
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
	implemented_events[BEE_EVENT_COLLISION] = true;
	implemented_events[BEE_EVENT_DRAW] = true;

	set_sprite(spr_bee);
}
ObjBee::~ObjBee() {}
void ObjBee::update(BEE::InstanceData* self) {
	if (!instance_data.empty()) {
		s = &instance_data[self->id];
	}
}
void ObjBee::create(BEE::InstanceData* self) {
	instance_data[self->id].clear();
	update(self);

	//self->get_physbody()->set_mass(1.0);
	double p[3] = {100.0, 100.0, 100.0};
	self->get_physbody()->set_shape(BEE_PHYS_SHAPE_BOX, p);

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

		BEE::ParticleSystem* part_system = new BEE::ParticleSystem(game);
		//part_system->following = nullptr;

		//BEE::Particle* part_done = new BEE::Particle(game, pt_shape_explosion, 0.5, 100);

		BEE::Particle* part_firework = new BEE::Particle(game, pt_shape_snow, 0.5, 10000, true);
		//BEE::Particle* part_firework = new BEE::Particle(game, spr_bee, 0.5, 10000, true);
		part_firework->velocity = {0.75, 270.0};
		part_firework->angle_increase = 0.3;
		//part_firework->color = game->get_enum_color(c_orange, 100);
		//part_firework->death_type = part_done;

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

		//game->get_current_room()->add_particle_system(part_system);
		//part_system->fast_forward(300);
	}
}
void ObjBee::destroy(BEE::InstanceData* self) {
	if (self->id == 0) {
		delete (BEE::TextData*)(*s)["fps_display"].p();
	}
	instance_data.erase(self->id);
}
void ObjBee::alarm(BEE::InstanceData* self, int a) {
	switch (a) {
		case 0: { // Alarm 0
			//snd_chirp->play();
			//self->set_alarm(0, 2000);
			break;
		}
	}

}
void ObjBee::step_mid(BEE::InstanceData* self) {
	/*int mx, my;
	std::tie(mx, my) = game->get_mouse_position();
	std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10, game->get_delta());
	self->x = c.first;
	self->y = c.second;*/

	if (self->id == 0) {
		if (game->render_get_3d()) {
			//game->render_set_camera(new BEE::Camera(glm::vec3(1920.0/2.0, 1080.0/2.0, -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			//game->render_set_camera(new BEE::Camera(glm::vec3(1920.0/2.0 + s->camx, 1080.0/2.0 + s->camy, -540.0 + s->camz), glm::vec3((1920.0/2.0-game->get_mouse_global_x())/1920.0, (1080.0/2.0-game->get_mouse_global_y())/1080.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			game->render_set_camera(new BEE::Camera(glm::vec3((*s)["camx"].d(), (*s)["camy"].d(), -540.0 + (*s)["camz"].d()), glm::vec3((-1920.0/2.0+game->get_mouse_global_x())/1920.0*2.0, (-1080.0/2.0+game->get_mouse_global_y())/1080.0*2.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			//game->render_set_camera(new BEE::Camera(glm::vec3(1920.0-game->get_mouse_global_x(), 1080.0-game->get_mouse_global_y(), -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		}
	}
}
void ObjBee::keyboard_press(BEE::InstanceData* self, SDL_Event* e) {
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

		/*case SDLK_e: {
			self->path_start(path_bee, 5.0, 3, true);
			self->set_path_drawn(true);
			break;
		}*/

		case SDLK_z: {
			if (self->id == 0) {
				game->net_session_start("test_session", 0, "hostplayer");
				if (game->net_get_is_connected()) {
					std::cout << "Session started\n";
				} else {
					std::cout << "Failed to host\n";
				}
			}
			break;
		}
		case SDLK_x: {
			if (self->id == 0) {
				game->net_session_join("127.0.0.1", "clientplayer");
				if (game->net_get_is_connected()) {
					std::cout << "Connected to session\n";
				} else {
					std::cout << "Failed to connect\n";
				}
			}
			break;
		}
		case SDLK_c: {
			if (self->id == 0) {
				std::map<std::string,std::string> servers = game->net_session_find();
				if (!servers.empty()) {
					std::cerr << "Available servers:\n";
					for (auto& srv : servers) {
						std::cerr << "\t" << srv.second << "\t" << srv.first << "\n";
					}
				} else {
					std::cerr << "No servers available\n";
				}
			}
			break;
		}

		case SDLK_1: {
			if (self->id == 0) {
				//snd_chirp->stop();
				snd_chirp->effect_set(bee_se_none);

				if (snd_chirp->get_is_playing()) {
					snd_chirp->rewind();
				} else {
					snd_chirp->play();
				}
			}
			break;
		}
		case SDLK_2: {
			if (self->id == 0) {
				snd_chirp->stop();
				snd_chirp->effect_set(bee_se_echo);
				snd_chirp->play();
			}
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
			mesh_monkey->print();
			break;
		}
	}
}
void ObjBee::mouse_press(BEE::InstanceData* self, SDL_Event* e) {
	if (game->console_get_is_open()) {
		return;
	}

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = {(int)self->get_x(), (int)self->get_y(), get_mask()->get_subimage_width(), get_mask()->get_height()};
			SDL_Rect b = {e->button.x, e->button.y, 20, 20};
			if (check_collision(a, b)) {
				double nx = 0.0, ny = 0.0;
				std::tie(nx, ny) = coord_approach(self->get_x(), self->get_y(), game->get_mouse_x(), game->get_mouse_y(), 10, game->get_delta());
				self->set_position(nx, ny, 0.0);
			} else if (self->id == 0) {
				if (self->is_place_empty(game->get_mouse_x(), game->get_mouse_y())) {
					BEE::InstanceData* bee = game->get_current_room()->add_instance(-1, this, game->get_mouse_x(), game->get_mouse_y(), 0.0);
					bee->get_physbody()->set_mass(1.0);
				}
			}
			break;
		}
	}
}
void ObjBee::mouse_input(BEE::InstanceData* self, SDL_Event* e) {
	if (game->console_get_is_open()) {
		return;
	}

	if (e->type == SDL_MOUSEMOTION) {
		//spr_bee->set_alpha(0.25);
	}
	if (e->motion.state & SDL_BUTTON_LMASK) {
		SDL_Rect a = {(int)self->get_x(), (int)self->get_y(), get_mask()->get_subimage_width(), get_mask()->get_height()};
		SDL_Rect b = {e->button.x, e->button.y, 20, 20};
		if (check_collision(a, b)) {
			double nx = 0.0, ny = 0.0;
			std::tie(nx, ny) = coord_approach(self->get_x(), self->get_y(), game->get_mouse_x(), game->get_mouse_y(), 10, game->get_delta());
			self->set_position(nx, ny, 0.0);
		} else if (self->id == 0) {
			/*if (self->is_place_empty(game->get_mouse_x(), game->get_mouse_y())) {
				game->get_current_room()->add_instance(-1, this, game->get_mouse_x(), game->get_mouse_y());
			}*/
		}
	}
}

void ObjBee::commandline_input(BEE::InstanceData* self, const std::string& str) {
	std::cout << "bee" << self->id << ":~~~" << str << "~~~\n";
}
void ObjBee::collision(BEE::InstanceData* self, BEE::InstanceData* other) {
	//self->move_away(2.0, other->get_x(), other->get_y());
}
void ObjBee::draw(BEE::InstanceData* self) {
	int mx, my;
	std::tie(mx, my) = game->get_mouse_position();
	int size = 100;
	//self->draw(size, size, direction_of(self->get_x(), self->get_y(), mx, my), c_white, SDL_FLIP_NONE);

	double r = radtodeg(self->get_physbody()->get_rotation_z());
	self->draw(size, size, r, c_white, SDL_FLIP_NONE);
	if (r != 0.0) {
		//std::cerr << "rotation: " << r << "\n";
	}

	font_liberation->draw_fast(self->get_corner_x(), self->get_corner_y(), bee_itos(self->id));

	lt_bee->set_position(glm::vec4(self->get_x(), self->get_y(), 0.0, 1.0));
	lt_bee->set_color({(Uint8)(self->id*50), (Uint8)(self->id*20), 255, 255});
	lt_bee->queue();

	if (self->id == 0) {
		lt_ambient->queue();
		/*lt_bee->set_position(glm::vec4(mx, my, 0.0, 1.0));
		lt_bee->queue();*/

		float t = (float)game->get_ticks()/1000.0f;
		float a = 180.0f + radtodeg(sin(t));
		mesh_monkey->draw(glm::vec3(1000.0f+500.0f*cos(t), 500.0f+300.0f*sin(t), 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, a, 180.0f), {255, 255, 0, 255}, false);

		(*s)["fps_display"] = (void*)font_liberation->draw((BEE::TextData*)(*s)["fps_display"].p(), 0, 0, "FPS: " + bee_itos(game->fps_stable));
	}
}
