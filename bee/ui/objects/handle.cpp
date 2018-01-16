/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_END,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	});
}
void ObjUIHandle::create(bee::Instance* self) {
	ObjUIElement::create(self);

	_p("parent") = nullptr;
	_i("parent_xoffset") = 0;
	_i("parent_yoffset") = 0;
	_d("parent_mass") = 0.0;

	_i("mouse_xoffset") = 0;
	_i("mouse_yoffset") = 0;
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
		_i("mouse_xoffset") = static_cast<int>(self->get_corner_x()) - e->button.x;
		_i("mouse_yoffset") = static_cast<int>(self->get_corner_y()) - e->button.y;

		bee::Instance* parent_inst = static_cast<bee::Instance*>(_p("parent"));
		if (parent_inst != nullptr) {
			_d("parent_mass") = parent_inst->get_physbody()->get_mass();
			parent_inst->set_mass(0.0);
		}
	}
}
void ObjUIHandle::mouse_input(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_input(self, e);

	if (!_i("is_pressed")) {
		return;
	}

	if ((e->type == SDL_MOUSEMOTION)&&(e->motion.state & SDL_BUTTON_LMASK)) {
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

	bee::render::set_is_lightable(false);

	bee::RGBA c_back = {_i("color_r"), _i("color_g"), _i("color_b"), _i("color_a")};
	bee::RGBA c_stripe = {255, 255, 255, 255};
	if ((c_back.r+c_back.g+c_back.b)/3 > 127) {
		c_stripe = {0, 0, 0, 255};
	}

	int ox = 0, oy = 0;
	bee::ViewPort* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view.x;
		oy = v->view.y;
	}

	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, _i("w"), _i("h"), -1, c_back); // Draw a box to contain the completions

	int stripe_width = 10;
	size_t stripe_amount = _i("w") / stripe_width;
	for (size_t i=0; i<stripe_amount; ++i) {
		bee::draw_line(
			self->get_corner_x() + i*stripe_width - ox,
			self->get_corner_y() + _i("h") - oy,
			self->get_corner_x() + (i+1)*stripe_width - ox,
			self->get_corner_y() - oy,
			c_stripe
		);
	}

	bee::render::set_is_lightable(true);
}

void ObjUIHandle::bind(bee::Instance* self, bee::Instance* parent_inst) {
	_p("parent") = parent_inst;
	if (parent_inst != nullptr) {
		_i("parent_xoffset") = parent_inst->get_corner_x() - self->get_corner_x();
		_i("parent_yoffset") = parent_inst->get_corner_y() - self->get_corner_y();
	} else {
		_i("parent_xoffset") = 0;
		_i("parent_yoffset") = 0;
	}
}

#endif // BEE_UI_OBJ_HANDLE
