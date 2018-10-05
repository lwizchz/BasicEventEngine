/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_BUTTON
#define BEE_OBJ_UI_BUTTON 1

#include "../../../util.hpp"
#include "../../../all.hpp"

#include "obj_button.hpp"

ObjUIButton::ObjUIButton() : ObjUIElement("__obj_ui_button", "$/ui/obj_button.cpp") {
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
	_a("text") = "";
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
	bee::RGBA c_back = {_c("color_r"), _c("color_g"), _c("color_b"), _c("color_a")};
	if (_i("has_hover")) {
		c_back.add_value(-0.20f);
	}

	int press_offset = 0;
	if (_i("is_pressed")) {
		press_offset = 4;
	}

	int ox = 0, oy = 0;
	const std::pair<const std::string,bee::ViewPort>* vp = bee::get_current_room()->get_current_viewport();
	if (vp != nullptr) {
		ox = vp->second.view.x;
		oy = vp->second.view.y;
	}

	int cx, cy;
	std::tie(cx, cy) = self->get_corner();

	bee::render::draw_rectangle(cx-ox, cy+press_offset - oy, w, h, -1, c_back);
	bee::render::draw_rectangle(cx-ox, cy+press_offset - oy, w, h, 6, c_border);

	std::string text = _s("text");
	if (!text.empty()) {
		bee::Font* font = static_cast<bee::Font*>(_p("font"));
		if (font == nullptr) {
			font = bee::engine->font_default;
		}

		_p("text_td") = font->draw(
			static_cast<bee::TextData*>(_p("text_td")),
			cx + (w-font->get_string_width(_s("text")))/2 - ox,
			cy+press_offset + (h-h/1.25)/2 - oy,
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
	const std::pair<const std::string,bee::ViewPort>* vp = bee::get_current_room()->get_current_viewport();
	if (vp != nullptr) {
		ox = vp->second.view.x;
	}

	int w = font->get_string_width(" "+_s("text")+" ");
	int h = font->get_string_height(_s("text"));
	_i("w") = w;
	_i("h") = h*1.25;

	self->set_corner((bee::get_window().w - w)/2 - ox, self->get_corner().second);
}

#endif // BEE_OBJ_UI_BUTTON
