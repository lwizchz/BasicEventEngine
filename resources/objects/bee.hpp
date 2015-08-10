/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
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
		void mouse(BEE::InstanceData*, SDL_Event*);
		void mouse_press(BEE::InstanceData*, SDL_Event*);
		void collision(BEE::InstanceData*, BEE::InstanceData*);
		void draw(BEE::InstanceData*);
};
ObjBee::ObjBee() : Object("obj_bee", "bee.hpp") {
	set_sprite(spr_bee);

	fps_display = NULL;
}
ObjBee::~ObjBee() {
	delete fps_display;
}
void ObjBee::create(BEE::InstanceData* self) {
	// create event
	std::cout << "u r a b " << self->id << ", : " << game->get_display_refresh_rate() << "\n";
	//self->set_alarm(0, 2000);
	spr_bee->set_alpha(0.5);
	//self->set_gravity(7.0);
	//show_message(font_liberation->get_fontname());

	std::cerr << execute_string<int>("3+5");
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
	SDL_GetMouseState(&mx, &my);
	std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10);
	self->x = c.first;
	self->y = c.second;*/

	/*int mx, my;
	SDL_GetMouseState(&mx, &my);
	self->move_to(10.0, mx, my);*/
}
void ObjBee::keyboard_press(BEE::InstanceData* self, SDL_Event* e) {
	switch (e->key.keysym.sym) {
		case SDLK_RETURN: {
			game->restart_room();
			break;
		}
		case SDLK_q: {
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
		case SDLK_r: {
			if (self->id == 0) {
				if (game->get_sprite(54) == NULL) {
					show_error("No such sprite");
				}
			}
			break;
		}

		case SDLK_1: {
			snd_music->effect_add(se_none);
			snd_music->play();
			break;
		}
		case SDLK_2: {
			snd_music->effect_add(se_chorus);
			snd_music->play();
			break;
		}
		case SDLK_3: {
			snd_music->effect_add(se_echo);
			snd_music->play();
			break;
		}

		case SDLK_p: {
			game->save_screenshot("screenshot.bmp");
			break;
		}
	}
}
void ObjBee::mouse(BEE::InstanceData* self, SDL_Event* e) {
	if (e->type == SDL_MOUSEMOTION) {
		//spr_bee->set_alpha(0.25);
	}
	if (e->motion.state & SDL_BUTTON_LMASK) {
		/*SDL_Rect a = {self->x, self->y, get_mask()->get_subimage_width(), get_mask()->get_height()};
		SDL_Rect b = {e->button.x, e->button.y, 20, 20};
		if (check_collision(&a, &b)) {
			int mx, my;
			SDL_GetMouseState(&mx, &my);
			std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10);
			self->x = c.first;
			self->y = c.second;
		}*/
		if (self->id == 0) {
			game->get_current_room()->add_instance(-1, this, e->button.x, e->button.y);
		}
	}
}
void ObjBee::mouse_press(BEE::InstanceData* self, SDL_Event* e) {
	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = {(int)self->x, (int)self->y, get_mask()->get_subimage_width(), get_mask()->get_height()};
			SDL_Rect b = {e->button.x, e->button.y, 20, 20};
			if (check_collision(&a, &b)) {
				int mx, my;
				SDL_GetMouseState(&mx, &my);
				std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10);
				self->x = c.first;
				self->y = c.second;
			} else if (self->id == 0) {
				game->get_current_room()->add_instance(-1, this, e->button.x, e->button.y);
			}
			break;
		}
	}
}
void ObjBee::collision(BEE::InstanceData* self, BEE::InstanceData* other) {
	self->move_away(2.0, other->x, other->y);
}
void ObjBee::draw(BEE::InstanceData* self) {
	// draw event
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	int s = 100;//distance(self->x, self->y, mx, my)/2;
	self->draw(s, s, direction_of(self->x, self->y, mx, my), c_aqua);

	//font_liberation->draw_fast(self->x, self->y, std::to_string(self->id));
	font_liberation->draw_fast(self->x, self->y, std::to_string(self->get_path_node()));

	if (self->id == 0) {
		fps_display = font_liberation->draw(fps_display, 0, 0, "FPS: \n" + std::to_string(game->fps_stable));
		//font_liberation->draw_fast(0, 0, "FPS: " + std::to_string(game->fps_stable));
	}
}
