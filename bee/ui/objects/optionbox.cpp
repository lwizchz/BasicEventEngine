/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_OPTIONBOX
#define BEE_UI_OBJ_OPTIONBOX 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "optionbox.hpp"

ObjUIOptionBox::ObjUIOptionBox() : ObjUIElement("obj_ui_optionbox", "/ui/objects/optionbox.cpp") {
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::MOUSE_INPUT,
		bee::E_EVENT::MOUSE_RELEASE,
		bee::E_EVENT::DRAW
	});
}
void ObjUIOptionBox::create(bee::Instance* self) {
	ObjUIElement::create(self);

	_i("type") = 0; // 0=checkbox, 1=radiobox
	_i("option_height") = 32;

	_i("adaptive_height") = false;

	reset_options(self);
}
void ObjUIOptionBox::mouse_input(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_input(self, e);
}
void ObjUIOptionBox::mouse_release(bee::Instance* self, SDL_Event* e) {
	ObjUIElement::mouse_release(self, e);

	if (_i("has_focus")) {
		const std::pair<int,int> mpos (bee::mouse::get_pos());
		int index = get_option_at(self, mpos.first, mpos.second);

		if (index >= 0) {
			bool state = _v("option_state")[index].i;
			set_option_state(self, index, !state);
		}
	}
}
void ObjUIOptionBox::draw(bee::Instance* self) {
	if (!_i("is_visible")) {
		return;
	}

	bee::render::set_is_lightable(false);

	if (_i("adaptive_height")) {
		_i("h") = _v("options").size() * _i("option_height");
	}
	int w = _i("w");
	int h = _i("h");

	bee::RGBA c_border = {0, 0, 0, 255};
	bee::RGBA c_back = {_i("color_r"), _i("color_g"), _i("color_b"), _i("color_a")};
	bee::RGBA c_highlight = c_back;
	c_highlight.add_value(-0.20f);

	int ox = 0, oy = 0;
	bee::ViewPort* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view.x;
		oy = v->view.y;
	}

	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, w, h, -1, c_back);
	if (_i("has_hover")) {
		const std::pair<int,int> mpos (bee::mouse::get_pos());
		bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() + get_option_at(self, mpos.first, mpos.second)*_i("option_height") - oy, w, _i("option_height"), -1, c_highlight);
	}
	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, w, h, 6, c_border);

	bee::Font* font = bee::engine->font_default;
	size_t i = 0;
	for (auto& option : _v("options")) {
		if (_i("type") == 0) {
			bee::draw_rectangle(self->get_corner_x() + 16 - ox, self->get_corner_y() + i*_i("option_height") + 8 - oy, 16, 16, 1, c_border);
			if (_v("option_state")[i].i) {
				bee::draw_line(
					self->get_corner_x() + 18 - ox, self->get_corner_y() + i*_i("option_height") + 16 - oy,
					self->get_corner_x() + 22 - ox, self->get_corner_y() + i*_i("option_height") + 22 - oy,
					c_border
				);
				bee::draw_line(
					self->get_corner_x() + 22 - ox, self->get_corner_y() + i*_i("option_height") + 22 - oy,
					self->get_corner_x() + 30 - ox, self->get_corner_y() + i*_i("option_height") + 10 - oy,
					c_border
				);
			}
		} else if (_i("type") == 1) {
			bee::draw_circle(glm::vec3(self->get_corner_x() + 24 - ox, self->get_corner_y() + i*_i("option_height") + 16 - oy, 0), 8, 1, c_border);
			if (_v("option_state")[i].i) {
				bee::draw_circle(glm::vec3(self->get_corner_x() + 24 - ox, self->get_corner_y() + i*_i("option_height") + 16 - oy, 0), 6, -1, c_border);
			}
		}

		font->draw_fast(self->get_corner_x() + 40 - ox, self->get_corner_y() + i*_i("option_height") + 8 - oy, option.s, c_border);

		bee::draw_line(self->get_corner_x() - ox, self->get_corner_y() + (i+1)*_i("option_height") - oy, self->get_corner_x() + _i("w") - ox, self->get_corner_y() + (i+1)*_i("option_height") - oy, c_border);

		++i;
	}

	bee::render::set_is_lightable(true);
}

void ObjUIOptionBox::set_type(bee::Instance* self, int new_type) {
	_i("type") = new_type;

}
void ObjUIOptionBox::reset_options(bee::Instance* self) {
	_a("options") = bee::Variant(bee::E_DATA_TYPE::VECTOR);
	_a("option_state") = bee::Variant(bee::E_DATA_TYPE::VECTOR);
}
void ObjUIOptionBox::push_option(bee::Instance* self, const std::string& option, bool initial_state, std::function<void (bee::Instance*, bool)> callback) {
	_v("options").push_back(bee::Variant(option));
	_v("option_state").push_back(bee::Variant(initial_state));
	bee::ui::push_optionbox_option(self, callback);
}
void ObjUIOptionBox::pop_option(bee::Instance* self) {
	_v("options").pop_back();
	_v("option_state").pop_back();
	bee::ui::pop_optionbox_option(self);
}

std::string ObjUIOptionBox::get_option(bee::Instance* self, size_t index) {
	return _v("options")[index].s;
}
bool ObjUIOptionBox::get_option_state(bee::Instance* self, size_t index) {
	return _v("option_state")[index].i;
}
int ObjUIOptionBox::get_option_at(bee::Instance* self, int mx, int my) {
	if (_i("adaptive_height")) {
		_i("h") = _v("options").size() * _i("option_height");
	}

	if (
		(mx < self->get_corner_x()-1)
		||(my < self->get_corner_y()-1)
		||(mx > self->get_corner_x()+1 + _i("w"))
		||(my > self->get_corner_y()+1 + _i("h"))
	) {
		return -1;
	}

	return util::fit_bounds<int>((my - self->get_corner_y()) / _i("option_height"), 0, _v("options").size()-1);
}
std::vector<int> ObjUIOptionBox::get_selected_options(bee::Instance* self) {
	std::vector<int> v;
	for (size_t i=0; i<_v("option_state").size(); ++i) {
		if (_v("option_state")[i].i) {
			v.push_back(i);
		}
	}
	return v;
}

void ObjUIOptionBox::set_option_state(bee::Instance* self, size_t index, bool new_state) {
	if (_i("type") == 0) {
		_v("option_state")[index] = new_state;
		bee::ui::optionbox_callback(self, index, new_state);
	} else if (_i("type") == 1) {
		if (new_state) {
			for (size_t i=0; i<_v("option_state").size(); ++i) {
				if (i == index) {
					continue;
				}

				if (_v("option_state")[i].i) {
					_v("option_state")[i] = false;
					bee::ui::optionbox_callback(self, i, false);
				}
			}

			if (!_v("option_state")[index].i) {
				_v("option_state")[index] = true;
				bee::ui::optionbox_callback(self, index, true);
			}
		}
	}
}

#endif // BEE_UI_OBJ_OPTIONBOX
