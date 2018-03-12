/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	});
}
ObjUIElement::~ObjUIElement() {}
void ObjUIElement::create(bee::Instance* self) {
	reset(self);
}
void ObjUIElement::destroy(bee::Instance* self) {
	Object::destroy(self);
}
void ObjUIElement::mouse_press(bee::Instance* self, SDL_Event* e) {
	_i("is_pressed") = false;
	_i("has_focus") = false;

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = self->get_aabb();
			a.w = _i("w");
			a.h = _i("h");
			SDL_Rect b = {e->button.x-1, e->button.y-1, 2, 2};
			if (util::check_collision(a, b)) {
				_i("is_pressed") = true;
			}
			break;
		}
	}
}
void ObjUIElement::mouse_input(bee::Instance* self, SDL_Event* e) {
	if (e->type != SDL_MOUSEMOTION) {
		return;
	}

	_i("has_hover") = false;

	SDL_Rect a = self->get_aabb();
	a.w = _i("w");
	a.h = _i("h");
	SDL_Rect b = {e->motion.x-1, e->motion.y-1, 2, 2};
	if (util::check_collision(a, b)) {
		_i("has_hover") = true;
	}
}
void ObjUIElement::mouse_release(bee::Instance* self, SDL_Event* e) {
	_i("has_focus") = false;

	switch (e->button.button) {
		case SDL_BUTTON_LEFT: {
			SDL_Rect a = self->get_aabb();
			a.w = _i("w");
			a.h = _i("h");
			SDL_Rect b = {e->button.x-1, e->button.y-1, 2, 2};
			if (util::check_collision(a, b)) {
				if (_i("is_pressed")) {
					_i("has_focus") = true;
				}
			}
			break;
		}
	}

	_i("is_pressed") = false;
}

void ObjUIElement::reset(bee::Instance* self) {
	_i("color_r") = 255;
	_i("color_g") = 255;
	_i("color_b") = 255;
	_i("color_a") = 255;

	_i("w") = 0;
	_i("h") = 0;

	bee::Texture* spr = self->get_sprite();
	if (spr != nullptr) {
		_i("w") = spr->get_size().first;
		_i("h") = spr->get_size().second;
	}

	_i("is_visible") = true;
	_i("is_pressed") = false;
	_i("has_focus") = false;
	_i("has_hover") = false;
}
void ObjUIElement::set_focus(bee::Instance* self, bool new_has_focus) {
	_i("has_focus") = new_has_focus;
}
void ObjUIElement::set_is_visible(bee::Instance* self, bool new_is_visibile) {
	_i("is_visible") = new_is_visibile;
}
void ObjUIElement::set_color(bee::Instance* self, const bee::RGBA& new_color) {
	_i("color_r") = new_color.r;
	_i("color_g") = new_color.g;
	_i("color_b") = new_color.b;
	_i("color_a") = new_color.a;
}

#endif // BEE_UI_OBJ_ELEMENT
