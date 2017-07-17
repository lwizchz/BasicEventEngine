/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_BUTTON_H
#define BEE_UI_OBJ_BUTTON_H 1

#include "../../util.hpp"
#include "../../all.hpp"

class ObjUIButton : public bee::Object {
	public:
		ObjUIButton();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void mouse_release(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		void center_width(bee::Instance*);
};
ObjUIButton::ObjUIButton() : Object("obj_ui_button", "/ui/objects/button.hpp") {
	implemented_events = {
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	};
}
void ObjUIButton::create(bee::Instance* self) {
	(*s)["font"] = static_cast<void*>(nullptr);
	(*s)["text"] = "";
	(*s)["textdata"] = static_cast<void*>(nullptr);

	(*s)["color_r"] = 255;
	(*s)["color_g"] = 255;
	(*s)["color_b"] = 255;
	(*s)["color_a"] = 255;

	(*s)["w"] = self->get_sprite()->get_width();
	(*s)["h"] = self->get_sprite()->get_height();

	(*s)["is_pressed"] = false;
	(*s)["press_func"] = static_cast<void*>(nullptr);
}
void ObjUIButton::destroy(bee::Instance* self) {
	delete static_cast<bee::TextData*>(_p("textdata"));
	Object::destroy(self);
}
void ObjUIButton::mouse_press(bee::Instance* self, SDL_Event* e) {
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
				bee::ui::internal::snd_button_press->play();
				(*s)["is_pressed"] = true;
			}
			break;
		}
	}
}
void ObjUIButton::mouse_release(bee::Instance* self, SDL_Event* e) {
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
				if (_i("is_pressed")) {
					bee::ui::internal::snd_button_release->play();
					bee::ui::button_callback(self);
				}
			}
			break;
		}
	}

	(*s)["is_pressed"] = false;
}
void ObjUIButton::draw(bee::Instance* self) {
	int w = _i("w");
	int h = _i("h");

	int r = _i("color_r");
	int g = _i("color_g");
	int b = _i("color_b");
	int a = _i("color_a");

	bee::set_is_lightable(false);

	self->draw(w, h, 0.0, bee::RGBA(r, g, b, a), SDL_FLIP_NONE);

	std::string text = _s("text");
	if (!text.empty()) {
		bee::Font* font = static_cast<bee::Font*>(_p("font"));
		bee::TextData* textdata = static_cast<bee::TextData*>(_p("textdata"));
		if (font == nullptr) {
			font = bee::engine->font_default;
		}

		int ox = 0, oy = 0;
		bee::ViewData* v = bee::get_current_room()->get_current_view();
		if (v != nullptr) {
			ox = v->view_x;
			oy = v->view_y;
		}

		(*s)["textdata"] = static_cast<void*>(font->draw(
			textdata,
			self->get_corner_x() + (w-font->get_string_width(_s("text")))/2 - ox,
			self->get_corner_y() + (h-h/1.25)/2 - oy,
			text
		));
	} else {
		bee::messenger::log("empty button");
	}

	bee::set_is_lightable(true);
}

void ObjUIButton::center_width(bee::Instance* self) {
	bee::Font* font = static_cast<bee::Font*>(_p("font"));
	if (font == nullptr) {
		font = bee::engine->font_default;
	}

	int ox = 0;
	bee::ViewData* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view_x;
	}

	int w = font->get_string_width(" "+_s("text")+" ");
	int h = font->get_string_height(_s("text"));
	(*s)["w"] = w;
	(*s)["h"] = h*1.25;

	self->set_corner_x((bee::get_width() - w)/2 - ox);
}

#endif // BEE_UI_OBJ_BUTTON_H
