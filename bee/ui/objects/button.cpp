/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_BUTTON
#define BEE_UI_OBJ_BUTTON 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "button.hpp"

ObjUIButton::ObjUIButton() : ObjUIElement("obj_ui_button", "/ui/objects/button.cpp") {
	implemented_events = {
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	};
}
void ObjUIButton::create(bee::Instance* self) {
	ObjUIElement::create(self);

	(*s)["font"] = static_cast<void*>(nullptr);
	(*s)["text"] = "";
	(*s)["text_td"] = static_cast<void*>(nullptr);

	(*s)["press_func"] = static_cast<void*>(nullptr);
}
void ObjUIButton::destroy(bee::Instance* self) {
	delete static_cast<bee::TextData*>(_p("text_td"));
	ObjUIElement::destroy(self);
}
void ObjUIButton::mouse_press(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_press(self, e);

	if (_i("is_pressed")) {
		bee::ui::internal::snd_button_press->play();
	}
}
void ObjUIButton::mouse_release(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_release(self, e);

	if (_i("has_focus")) {
		bee::ui::internal::snd_button_release->play();
		bee::ui::button_callback(self);
	}
}
void ObjUIButton::draw(bee::Instance* self) {
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

	std::string text = _s("text");
	if (!text.empty()) {
		bee::Font* font = static_cast<bee::Font*>(_p("font"));
		if (font == nullptr) {
			font = bee::engine->font_default;
		}

		int ox = 0, oy = 0;
		bee::ViewData* v = bee::get_current_room()->get_current_view();
		if (v != nullptr) {
			ox = v->view_x;
			oy = v->view_y;
		}

		(*s)["text_td"] = static_cast<void*>(font->draw(
			static_cast<bee::TextData*>(_p("text_td")),
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

#endif // BEE_UI_OBJ_BUTTON
