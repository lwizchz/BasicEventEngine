/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_ELEMENT
#define BEE_UI_OBJ_ELEMENT 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "element.hpp"

ObjUIElement::ObjUIElement(const std::string& new_name, const std::string& new_path) :
	Object(new_name, new_path)
{
	implemented_events = {
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	};
}
ObjUIElement::~ObjUIElement() {}
void ObjUIElement::create(bee::Instance* self) {
	reset(self);
}
void ObjUIElement::destroy(bee::Instance* self) {
	Object::destroy(self);
}
void ObjUIElement::mouse_press(bee::Instance* self, SDL_Event* e) {
	(*s)["is_pressed"] = false;
	(*s)["has_focus"] = false;

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = self->get_aabb();
			a.w = _i("w");
			a.h = _i("h");
			SDL_Rect b = {e->button.x-1, e->button.y-1, 2, 2};
			if (check_collision(a, b)) {
				(*s)["is_pressed"] = true;
			}
			break;
		}
	}
}
void ObjUIElement::mouse_input(bee::Instance* self, SDL_Event* e) {
	if (e->type != SDL_MOUSEMOTION) {
		return;
	}

	(*s)["has_hover"] = false;

	SDL_Rect a = self->get_aabb();
	a.w = _i("w");
	a.h = _i("h");
	SDL_Rect b = {e->motion.x-1, e->motion.y-1, 2, 2};
	if (check_collision(a, b)) {
		(*s)["has_hover"] = true;
	}
}
void ObjUIElement::mouse_release(bee::Instance* self, SDL_Event* e) {
	(*s)["has_focus"] = false;

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = self->get_aabb();
			a.w = _i("w");
			a.h = _i("h");
			SDL_Rect b = {e->button.x-1, e->button.y-1, 2, 2};
			if (check_collision(a, b)) {
				if (_i("is_pressed")) {
					(*s)["has_focus"] = true;
				}
			}
			break;
		}
	}

	(*s)["is_pressed"] = false;
}

void ObjUIElement::reset(bee::Instance* self) {
	(*s)["color_r"] = 255;
	(*s)["color_g"] = 255;
	(*s)["color_b"] = 255;
	(*s)["color_a"] = 255;

	(*s)["w"] = 0;
	(*s)["h"] = 0;

	bee::Sprite* spr = self->get_sprite();
	if (spr != nullptr) {
		(*s)["w"] = spr->get_width();
		(*s)["h"] = spr->get_height();
	}

	(*s)["is_visible"] = true;
	(*s)["is_pressed"] = false;
	(*s)["has_focus"] = false;
	(*s)["has_hover"] = false;
}
void ObjUIElement::set_focus(bee::Instance* self, bool new_has_focus) {
	(*s)["has_focus"] = new_has_focus;
}
void ObjUIElement::set_is_visible(bee::Instance* self, bool new_is_visibile) {
	(*s)["is_visible"] = new_is_visibile;
}
void ObjUIElement::set_color(bee::Instance* self, const bee::RGBA& new_color) {
	(*s)["color_r"] = new_color.r;
	(*s)["color_g"] = new_color.g;
	(*s)["color_b"] = new_color.b;
	(*s)["color_a"] = new_color.a;
}

#endif // BEE_UI_OBJ_ELEMENT
