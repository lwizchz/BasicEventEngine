/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_GAUGE
#define BEE_UI_OBJ_GAUGE 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "gauge.hpp"

ObjUIGauge::ObjUIGauge() : ObjUIElement("obj_ui_gauge", "/ui/objects/gauge.cpp") {
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DRAW
	});
}
void ObjUIGauge::create(bee::Instance* self) {
	ObjUIElement::create(self);

	(*s)["range"] = 100;
	(*s)["value"] = 0;
	(*s)["pulse_state"] = 0;

	(*s)["color_back_r"] = 0;
	(*s)["color_back_g"] = 0;
	(*s)["color_back_b"] = 0;
	(*s)["color_back_a"] = 255;
}
void ObjUIGauge::draw(bee::Instance* self) {
	if (!_i("is_visible")) {
		return;
	}

	bee::set_is_lightable(false);

	int w = _i("w");
	int h = _i("h");

	bee::RGBA c_border = {0, 0, 0, 255};
	bee::RGBA c_back = {_i("color_back_r"), _i("color_back_g"), _i("color_back_b"), _i("color_back_a")};
	bee::RGBA c_front = {_i("color_r"), _i("color_g"), _i("color_b"), _i("color_a")};

	int ox = 0, oy = 0;
	bee::ViewData* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view_x;
		oy = v->view_y;
	}

	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, w, h, -1, c_back);

	if (_i("pulse_state") != 0) {
		int pulse_width = 20;
		bee::draw_rectangle(self->get_corner_x() + w * _i("value") / _i("range") - ox, self->get_corner_y() - oy, w * pulse_width / _i("range"), h, -1, c_front);

		int inc = _i("pulse_state") * 100.0 * bee::get_delta();
		if (
			(_i("pulse_state") > 0)
			&&(_i("value") + pulse_width + inc >= _i("range"))
		) { // If the pulse is increasing and has reached the maximum value, reverse the pulse
			if (_i("value") + pulse_width == _i("range")) {
				(*s)["pulse_state"] = -1;
			} else {
				(*s)["value"] = _i("range") - pulse_width;
			}
		} else if (
			(_i("pulse_state") < 0)
			&&(_i("value") + inc <= 0)
		) { // If the pulse is decreasing and has reached the minimum value, reverse the pulse
			if (_i("value") == 0) {
				(*s)["pulse_state"] = 1;
			} else {
				(*s)["value"] = 0;
			}
		} else { // Increment the pulse in its current direction
			(*s)["value"] += inc;
		}
	} else {
		bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, w * _i("value") / _i("range"), h, -1, c_front);
	}

	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, w, h, 2, c_border);

	bee::set_is_lightable(true);
}

int ObjUIGauge::get_range(bee::Instance* self) {
	return _i("range");
}
int ObjUIGauge::get_value(bee::Instance* self) {
	return _i("value");
}
void ObjUIGauge::set_range(bee::Instance* self, int new_range) {
	(*s)["range"] = new_range;
}
void ObjUIGauge::set_value(bee::Instance* self, int new_value) {
	(*s)["value"] = new_value;
}
void ObjUIGauge::start_pulse(bee::Instance* self) {
	(*s)["range"] = 100;
	(*s)["value"] = 0;
	(*s)["pulse_state"] = 1;
}
void ObjUIGauge::stop_pulse(bee::Instance* self) {
	(*s)["value"] = 0;
	(*s)["pulse_state"] = 0;
}

void ObjUIGauge::set_color_back(bee::Instance* self, bee::RGBA new_color) {
	(*s)["color_back_r"] = new_color.r;
	(*s)["color_back_g"] = new_color.g;
	(*s)["color_back_b"] = new_color.b;
	(*s)["color_back_a"] = new_color.a;
}

#endif // BEE_UI_OBJ_GAUGE
