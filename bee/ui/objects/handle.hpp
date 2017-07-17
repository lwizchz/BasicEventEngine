/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_HANDLE_H
#define BEE_UI_OBJ_HANDLE_H 1

#include "../../util.hpp"
#include "../../all.hpp"

class ObjUIHandle : public bee::Object {
	public:
		ObjUIHandle();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void step_end(bee::Instance*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void mouse_input(bee::Instance*, SDL_Event*);
		void mouse_release(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		void bind(bee::Instance*, bee::Instance*);
};
ObjUIHandle::ObjUIHandle() : Object("obj_ui_handle", "/ui/objects/handle.hpp") {
	implemented_events = {
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_END,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	};
}
void ObjUIHandle::create(bee::Instance* self) {
	(*s)["color_r"] = 255;
	(*s)["color_g"] = 255;
	(*s)["color_b"] = 255;
	(*s)["color_a"] = 255;

	(*s)["parent"] = static_cast<void*>(nullptr);
	(*s)["parent_xoffset"] = 0;
	(*s)["parent_yoffset"] = 0;
	(*s)["parent_mass"] = 0.0;

	(*s)["w"] = self->get_sprite()->get_width();
	(*s)["h"] = self->get_sprite()->get_height();

	(*s)["is_pressed"] = false;

	(*s)["mouse_xoffset"] = 0;
	(*s)["mouse_yoffset"] = 0;
}
void ObjUIHandle::destroy(bee::Instance* self) {
	bee::ui::destroy_handle(self);
}
void ObjUIHandle::step_end(bee::Instance* self) {
	bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
	if (parent_inst != nullptr) {
		self->set_corner_x(parent_inst->get_corner_x() - _i("parent_xoffset"));
		self->set_corner_y(parent_inst->get_corner_y() - _i("parent_yoffset"));
	}
}
void ObjUIHandle::mouse_press(bee::Instance* self, SDL_Event* e) {
	if (bee::console_get_is_open()) {
		return;
	}

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = self->get_aabb();
			a.w = _i("w");
			a.h = _i("h");
			SDL_Rect b = {e->button.x-10, e->button.y-10, 20, 20};
			if (check_collision(a, b)) {
				(*s)["is_pressed"] = true;
				(*s)["mouse_xoffset"] = a.x - e->button.x;
				(*s)["mouse_yoffset"] = a.y - e->button.y;

				bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
				if (parent_inst != nullptr) {
					(*s)["parent_mass"] = parent_inst->get_physbody()->get_mass();
					parent_inst->set_mass(0.0);
				}
			}
			break;
		}
	}
}
void ObjUIHandle::mouse_input(bee::Instance* self, SDL_Event* e) {
	if (bee::console_get_is_open()) {
		return;
	}

	if (!_i("is_pressed")) {
		return;
	}

	if ((e->type == SDL_MOUSEMOTION)&&(e->motion.state & SDL_BUTTON_LMASK)) {
		SDL_Rect a = self->get_aabb();
		a.w = _i("w");
		a.h = _i("h");
		SDL_Rect b = {e->button.x-10, e->button.y-10, 20, 20};
		if (check_collision(a, b)) {
			int new_x = e->button.x + _i("mouse_xoffset");
			int new_y = e->button.y + _i("mouse_yoffset");

			self->set_corner_x(new_x);
			self->set_corner_y(new_y);

			bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
			if (parent_inst != nullptr) {
				parent_inst->set_corner_x(new_x + _i("parent_xoffset"));
				parent_inst->set_corner_y(new_y + _i("parent_yoffset"));
			}
		}
	}
}
void ObjUIHandle::mouse_release(bee::Instance* self, SDL_Event* e) {
	if (bee::console_get_is_open()) {
		return;
	}

	(*s)["is_pressed"] = false;

	bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
	if (parent_inst != nullptr) {
		parent_inst->set_mass(_d("parent_mass"));
	}
}
void ObjUIHandle::draw(bee::Instance* self) {
	int w = _i("w");
	int h = _i("h");

	int r = _i("color_r");
	int g = _i("color_g");
	int b = _i("color_b");
	int a = _i("color_a");

	bee::set_is_lightable(false);
	self->draw(w, h, 0.0, bee::RGBA(r, g, b, a), SDL_FLIP_NONE);
	bee::set_is_lightable(true);
}

void ObjUIHandle::bind(bee::Instance* self, bee::Instance* parent_inst) {
	(*s)["parent"] = parent_inst;
	if (parent_inst != nullptr) {
		(*s)["parent_xoffset"] = parent_inst->get_corner_x() - self->get_corner_x();
		(*s)["parent_yoffset"] = parent_inst->get_corner_y() - self->get_corner_y();
	} else {
		(*s)["parent_xoffset"] = 0;
		(*s)["parent_yoffset"] = 0;
	}
}

#endif // BEE_UI_OBJ_HANDLE_H
