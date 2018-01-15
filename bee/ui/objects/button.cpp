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
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	});
}
void ObjUIButton::create(bee::Instance* self) {
	ObjUIElement::create(self);

	_p("font") = nullptr;
	_s("text") = "";
	_p("text_td") = nullptr;

	_p("press_func") = nullptr;
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

	bee::render::set_is_lightable(false);

	int w = _i("w");
	int h = _i("h");

	bee::RGBA c_border = {0, 0, 0, 255};
	bee::RGBA c_back = {_i("color_r"), _i("color_g"), _i("color_b"), _i("color_a")};
	if (_i("has_hover")) {
		c_back.add_value(-0.20f);
	}

	int press_offset = 0;
	if (_i("is_pressed")) {
		press_offset = 4;
	}

	int ox = 0, oy = 0;
	bee::ViewPort* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view.x;
		oy = v->view.y;
	}

	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y()+press_offset - oy, w, h, -1, c_back);
	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y()+press_offset - oy, w, h, 6, c_border);

	std::string text = _s("text");
	if (!text.empty()) {
		bee::Font* font = static_cast<bee::Font*>(_p("font"));
		if (font == nullptr) {
			font = bee::engine->font_default;
		}

		_p("text_td") = font->draw(
			static_cast<bee::TextData*>(_p("text_td")),
			self->get_corner_x() + (w-font->get_string_width(_s("text")))/2 - ox,
			self->get_corner_y()+press_offset + (h-h/1.25)/2 - oy,
			text
		);
	} else {
		bee::messenger::log("empty button");
	}

	bee::render::set_is_lightable(true);
}

void ObjUIButton::center_width(bee::Instance* self) {
	bee::Font* font = static_cast<bee::Font*>(_p("font"));
	if (font == nullptr) {
		font = bee::engine->font_default;
	}

	int ox = 0;
	bee::ViewPort* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view.x;
	}

	int w = font->get_string_width(" "+_s("text")+" ");
	int h = font->get_string_height(_s("text"));
	_i("w") = w;
	_i("h") = h*1.25;

	self->set_corner_x((bee::get_window().w - w)/2 - ox);
}

#endif // BEE_UI_OBJ_BUTTON
