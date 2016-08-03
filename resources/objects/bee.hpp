/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

class ObjBee : public BEE::Object {
	public:
		TextData* fps_display;

		ObjBee();
		~ObjBee();
		void create(BEE::InstanceData*);
		void alarm(BEE::InstanceData*, int);
		void step_mid(BEE::InstanceData*);
		void keyboard_press(BEE::InstanceData*, SDL_Event*);
		void mouse_press(BEE::InstanceData*, SDL_Event*);
		void mouse_input(BEE::InstanceData*, SDL_Event*);
		void console_input(BEE::InstanceData*, const std::string&);
		void collision(BEE::InstanceData*, BEE::InstanceData*);
		void draw(BEE::InstanceData*);
};
ObjBee::ObjBee() : Object("obj_bee", "bee.hpp") {
	implemented_events[BEE_EVENT_CREATE] = true;
	implemented_events[BEE_EVENT_ALARM] = true;
	implemented_events[BEE_EVENT_STEP_MID] = true;
	implemented_events[BEE_EVENT_KEYBOARD_PRESS] = true;
	implemented_events[BEE_EVENT_MOUSE_PRESS] = true;
	implemented_events[BEE_EVENT_MOUSE_INPUT] = true;
	implemented_events[BEE_EVENT_CONSOLE_INPUT] = true;
	implemented_events[BEE_EVENT_COLLISION] = true;
	implemented_events[BEE_EVENT_DRAW] = true;

	set_sprite(spr_bee);
	fps_display = nullptr;
}
ObjBee::~ObjBee() {
	delete fps_display;
}
void ObjBee::create(BEE::InstanceData* self) {
	// create event
	std::cout << "u r a b " << self->id << "\n";
	//self->set_alarm(0, 2000);
	//spr_bee->set_alpha(0.5);
	//self->set_gravity(7.0);
	//show_message(font_liberation->get_fontname());

	if (self->id == 0) {
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
	int mx, my;
	std::tie(mx, my) = game->get_mouse_position();
	std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10, game->get_delta());
	self->x = c.first;
	self->y = c.second;
}
void ObjBee::keyboard_press(BEE::InstanceData* self, SDL_Event* e) {
	switch (e->key.keysym.sym) {
		case SDLK_RETURN: {
			game->set_transition_type((bee_transition_t)(game->get_transition_type()+1));
			game->restart_room();
			break;
		}
		case SDLK_q: {
			game->set_transition_type(BEE_TRANSITION_NONE);
			game->end_game();
			break;
		}

		case SDLK_w: {
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
		}

		case SDLK_e: {
			self->path_start(path_bee, 5.0, 3, true);
			self->set_path_drawn(true);
			break;
		}

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
					for (auto& s : servers) {
						std::cerr << "\t" << s.second << "\t" << s.first << "\n";
					}
				} else {
					std::cerr << "No servers available\n";
				}
			}
			break;
		}

		case SDLK_1: {
			if (self->id == 0) {
				snd_music->stop();
				snd_music->effect_add(se_none);
				snd_music->play();
			}
			break;
		}
		case SDLK_2: {
			if (self->id == 0) {
				snd_music->stop();
				snd_music->effect_add(se_echo);
				snd_music->play();
			}
			break;
		}

		case SDLK_n: {
			lt_bee->set_attenuation({2.0, 100.0, 2000.0, 0.0});
			break;
		}
		case SDLK_m: {
			lt_bee->set_attenuation({2.0, 100.0, -2000.0, 0.0});
			break;
		}

		case SDLK_p: {
			game->save_screenshot("screenshot.bmp");
			break;
		}
	}
}
void ObjBee::mouse_press(BEE::InstanceData* self, SDL_Event* e) {
	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = {(int)self->x, (int)self->y, get_mask()->get_subimage_width(), get_mask()->get_height()};
			SDL_Rect b = {e->button.x, e->button.y, 20, 20};
			if (check_collision(a, b)) {
				std::tie(self->x, self->y) = coord_approach(self->x, self->y, game->get_mouse_x(), game->get_mouse_y(), 10, game->get_delta());
			} else if (self->id == 0) {
				if (self->is_place_empty(game->get_mouse_x(), game->get_mouse_y())) {
					game->get_current_room()->add_instance(-1, this, game->get_mouse_x(), game->get_mouse_y());
				}
			}
			break;
		}
	}
}
void ObjBee::mouse_input(BEE::InstanceData* self, SDL_Event* e) {
	if (e->type == SDL_MOUSEMOTION) {
		//spr_bee->set_alpha(0.25);
	}
	if (e->motion.state & SDL_BUTTON_LMASK) {
		SDL_Rect a = {(int)self->x, (int)self->y, get_mask()->get_subimage_width(), get_mask()->get_height()};
		SDL_Rect b = {e->button.x, e->button.y, 20, 20};
		if (check_collision(a, b)) {
			std::tie(self->x, self->y) = coord_approach(self->x, self->y, game->get_mouse_x(), game->get_mouse_y(), 10, game->get_delta());
		} else if (self->id == 0) {
			if (self->is_place_empty(game->get_mouse_x(), game->get_mouse_y())) {
				game->get_current_room()->add_instance(-1, this, game->get_mouse_x(), game->get_mouse_y());
			}
		}
	}
}

void ObjBee::console_input(BEE::InstanceData* self, const std::string& s) {
	std::cout << "~~~" << s << "~~~\n";
}
void ObjBee::collision(BEE::InstanceData* self, BEE::InstanceData* other) {
	self->move_away(2.0, other->x, other->y);
}
void ObjBee::draw(BEE::InstanceData* self) {
	int mx, my;
	std::tie(mx, my) = game->get_mouse_position();
	int s = 100;
	self->draw(s, s, direction_of(self->x, self->y, mx, my), c_white, SDL_FLIP_NONE);

	font_liberation->draw_fast(self->x, self->y, bee_itos(self->id), false);

	lt_bee->set_position(glm::vec4(self->x, self->y, 0.0, 1.0));
	lt_bee->set_color({(Uint8)(self->id*50), (Uint8)(self->id*20), 255, 255});
	lt_bee->queue();

	if (self->id == 0) {
		lt_ambient->queue();
		/*lt_bee->set_position(glm::vec4(mx, my, 0.0, 1.0));
		lt_bee->queue();*/

		fps_display = font_liberation->draw(fps_display, 0, 0, "FPS: " + bee_itos(game->fps_stable), true);
	}
}
