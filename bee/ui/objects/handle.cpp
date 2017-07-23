/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_HANDLE
#define BEE_UI_OBJ_HANDLE 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "handle.hpp"

ObjUIHandle::ObjUIHandle() : ObjUIElement("obj_ui_handle", "/ui/objects/handle.cpp") {
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
	ObjUIElement::create(self);

	(*s)["parent"] = static_cast<void*>(nullptr);
	(*s)["parent_xoffset"] = 0;
	(*s)["parent_yoffset"] = 0;
	(*s)["parent_mass"] = 0.0;

	(*s)["mouse_xoffset"] = 0;
	(*s)["mouse_yoffset"] = 0;
}
void ObjUIHandle::destroy(bee::Instance* self) {
	bee::ui::destroy_handle(self);
	ObjUIElement::destroy(self);
}
void ObjUIHandle::step_end(bee::Instance* self) {
	bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
	if (parent_inst != nullptr) {
		self->set_corner_x(parent_inst->get_corner_x() - _i("parent_xoffset"));
		self->set_corner_y(parent_inst->get_corner_y() - _i("parent_yoffset"));
	}
}
void ObjUIHandle::mouse_press(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_press(self, e);

	if (_i("is_pressed")) {
		(*s)["mouse_xoffset"] = static_cast<int>(self->get_corner_x()) - e->button.x;
		(*s)["mouse_yoffset"] = static_cast<int>(self->get_corner_y()) - e->button.y;

		bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
		if (parent_inst != nullptr) {
			(*s)["parent_mass"] = parent_inst->get_physbody()->get_mass();
			parent_inst->set_mass(0.0);
		}
	}
}
void ObjUIHandle::mouse_input(bee::Instance* self, SDL_Event* e) {
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
	ObjUIElement::mouse_release(self, e);

	bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
	if (parent_inst != nullptr) {
		parent_inst->set_mass(_d("parent_mass"));
	}
}
void ObjUIHandle::draw(bee::Instance* self) {
	if (!_i("is_visible")) {
		return;
	}

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

#endif // BEE_UI_OBJ_HANDLE
