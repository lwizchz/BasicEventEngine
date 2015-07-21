/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

class ObjBee : public Object {
	public:
		TextData* fps_display;

		ObjBee();
		~ObjBee();
		void create(InstanceData*);
		void alarm(InstanceData*, int);
		void step_mid(InstanceData*);
		void keyboard_press(InstanceData*, SDL_Event*);
		void mouse(InstanceData*, SDL_Event*);
		void mouse_press(InstanceData*, SDL_Event*);
		void collision(InstanceData*, InstanceData*);
		void draw(InstanceData*);
};
ObjBee::ObjBee() : Object("obj_bee", "bee.hpp") {
	set_sprite(spr_bee);

	fps_display = NULL;
}
ObjBee::~ObjBee() {
	delete fps_display;
}
void ObjBee::create(InstanceData* self) {
	// create event
	std::cout << "u r a b " << self->id << "\n";
	self->set_alarm(0, 2000);
	spr_bee->set_alpha(0.5);
}
void ObjBee::alarm(InstanceData* self, int a) {
	switch (a) {
		case 0: { // Alarm 0
			//snd_chirp->play();
			//self->set_alarm(0, 2000);
			break;
		}
	}

}
void ObjBee::step_mid(InstanceData* self) {
	/*int mx, my;
	SDL_GetMouseState(&mx, &my);
	std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10);
	self->x = c.first;
	self->y = c.second;*/
}
void ObjBee::keyboard_press(InstanceData* self, SDL_Event* e) {
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
			float p = (float)self->x/game->width;
			p *= 2.0;
			p -= 1.0;
			snd_chirp->set_pan(p);
			snd_chirp->play();
			break;
		}
	}
}
void ObjBee::mouse(InstanceData* self, SDL_Event* e) {
	if (e->type == SDL_MOUSEMOTION) {
		//spr_bee->set_alpha(0.25);
	}
	if (e->motion.state & SDL_BUTTON_LMASK) {
		SDL_Rect a = {self->x, self->y, get_mask()->get_subimage_width(), get_mask()->get_height()};
		SDL_Rect b = {e->button.x, e->button.y, 20, 20};
		if (check_collision(&a, &b)) {
			int mx, my;
			SDL_GetMouseState(&mx, &my);
			std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10);
			self->x = c.first;
			self->y = c.second;
		}
	}
}
void ObjBee::mouse_press(InstanceData* self, SDL_Event* e) {
	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = {self->x, self->y, get_mask()->get_subimage_width(), get_mask()->get_height()};
			SDL_Rect b = {e->button.x, e->button.y, 20, 20};
			if (check_collision(&a, &b)) {
				int mx, my;
				SDL_GetMouseState(&mx, &my);
				std::pair<int,int> c = coord_approach(self->x, self->y, mx, my, 10);
				self->x = c.first;
				self->y = c.second;
			} else if (self->id == 0) {
				game->get_room()->add_instance(-1, this, e->button.x, e->button.y);
			}
			break;
		}
	}
}
void ObjBee::collision(InstanceData* self, InstanceData* other) {

}
void ObjBee::draw(InstanceData* self) {
	// draw event
	get_sprite()->draw(self->vx, self->vy, self->subimage_time);
	font_liberation->draw_fast(self->vx, self->vy, std::to_string(self->id));

	if (self->id == 0) {
		fps_display = font_liberation->draw(fps_display, 0, 0, "FPS: " + std::to_string(game->fps_stable));
		//font_liberation->draw_fast(0, 0, "FPS: " + std::to_string(game->fps_stable));
	}
}