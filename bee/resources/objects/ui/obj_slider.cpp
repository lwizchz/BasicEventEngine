/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_SLIDER
#define BEE_OBJ_UI_SLIDER 1

#include "../../../util.hpp"
#include "../../../all.hpp"

#include "obj_slider.hpp"

ObjUISlider::ObjUISlider() : ObjUIElement("__obj_ui_slider", "/bee/resources/objects/ui/obj_slider.cpp") {
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	});
}
void ObjUISlider::create(bee::Instance* self) {
	ObjUIElement::create(self);

	(*s)["range"] = 100;
	(*s)["value"] = 0;

	(*s)["slider_size"] = 16;

	(*s)["show_text"] = false;

	(*s)["is_continuous"] = false;

	(*s)["color_track_r"] = 0;
	(*s)["color_track_g"] = 0;
	(*s)["color_track_b"] = 0;
	(*s)["color_track_a"] = 255;
}
void ObjUISlider::mouse_input(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_input(self, e);

	if (!_i("is_pressed")) {
		if ((_i("has_hover"))&&(e->type == SDL_MOUSEWHEEL)) {
			int flip = bee::mouse::get_wheel_flip(e->wheel);

			int v = _i("value");

			v += flip * e->wheel.y;
			(*s)["value"] = util::fit_bounds(v, 0, _i("range"));

			bee::ui::slider_callback(self, _i("value"));
		}

		return;
	}

	if ((e->type == SDL_MOUSEMOTION)&&(e->motion.state & SDL_BUTTON_LMASK)) {
		int cx = self->get_corner().first;
		if (
			(e->button.x >= cx - _i("slider_size")/2)
			&&(e->button.x <= cx + _i("w") + _i("slider_size")/2)
		) {
			int v = (e->button.x - cx) * _i("range") / _i("w");
			(*s)["value"] = util::fit_bounds(v, 0, _i("range"));

			if (_i("is_continuous")) {
				bee::ui::slider_callback(self, _i("value"));
			}
		}
	}
}
void ObjUISlider::mouse_release(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_release(self, e);

	if ((_i("has_focus"))&&(!_i("is_continuous"))) {
		bee::ui::slider_callback(self, _i("value"));
	}
}
void ObjUISlider::draw(bee::Instance* self) {
	if (!_i("is_visible")) {
		return;
	}

	bee::render::set_is_lightable(false);

	int w = _i("w");
	int h = _i("h");

	bee::RGBA c_track = {_i("color_track_r"), _i("color_track_g"), _i("color_track_b"), _i("color_track_a")};
	bee::RGBA c_slider = {_i("color_r"), _i("color_g"), _i("color_b"), _i("color_a")};

	int ox = 0, oy = 0;
	const std::pair<const std::string,bee::ViewPort>* vp = bee::get_current_room()->get_current_viewport();
	if (vp != nullptr) {
		ox = vp->second.view.x;
		oy = vp->second.view.y;
	}

	int cx, cy;
	std::tie(cx, cy) = self->get_corner();

	bee::draw_rectangle(cx-ox, cy + h/2 - oy, w, 2, -1, c_track);

	int slider_size = _i("slider_size");
	int slider_x = cx + w * _i("value") / _i("range") - slider_size/2;
	bee::draw_circle(glm::vec3(slider_x + slider_size/2 - ox, cy + h/2 - oy, 0), slider_size/2, -1, c_slider);

	if (_i("show_text")) {
		bee::Font* font = bee::engine->font_default;
		font->draw_fast(cx-ox, cy + h*2/3 - oy, "0");
		font->draw_fast(cx + w-16 - ox, cy + h*2/3 - oy, std::to_string(_i("range")));
		font->draw_fast(slider_x + slider_size/2 - 8 - ox, cy - h/3 - oy, std::to_string(_i("value")));
	}

	bee::render::set_is_lightable(true);
}

int ObjUISlider::get_range(bee::Instance* self) {
	return _i("range");
}
int ObjUISlider::get_value(bee::Instance* self) {
	return _i("value");
}
void ObjUISlider::set_range(bee::Instance* self, int new_range) {
	(*s)["range"] = new_range;
}
void ObjUISlider::set_value(bee::Instance* self, int new_value) {
	(*s)["value"] = new_value;
}
void ObjUISlider::show_text(bee::Instance* self, bool new_show_text) {
	(*s)["show_text"] = new_show_text;
}
void ObjUISlider::set_is_continuous(bee::Instance* self, bool new_is_continuous) {
	(*s)["is_continuous"] = new_is_continuous;
}

void ObjUISlider::set_track_color(bee::Instance* self, bee::RGBA new_color) {
	(*s)["color_track_r"] = new_color.r;
	(*s)["color_track_g"] = new_color.g;
	(*s)["color_track_b"] = new_color.b;
	(*s)["color_track_a"] = new_color.a;
}

#endif // BEE_UI_OBJ_SLIDER
