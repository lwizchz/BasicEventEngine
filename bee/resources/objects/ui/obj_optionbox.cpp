/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_OPTIONBOX
#define BEE_OBJ_UI_OPTIONBOX 1

#include "../../../util.hpp"
#include "../../../all.hpp"

#include "obj_optionbox.hpp"

ObjUIOptionBox::ObjUIOptionBox() : ObjUIElement("__obj_ui_optionbox", "$/ui/obj_optionbox.cpp") {
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
	bee::RGBA c_back = {_c("color_r"), _c("color_g"), _c("color_b"), _c("color_a")};
	bee::RGBA c_highlight = c_back;
	c_highlight.add_value(-0.20f);

	int ox = 0, oy = 0;
	const std::pair<const std::string,bee::ViewPort>* vp = bee::get_current_room()->get_current_viewport();
	if (vp != nullptr) {
		ox = vp->second.view.x;
		oy = vp->second.view.y;
	}

	int cx, cy;
	std::tie(cx, cy) = self->get_corner();

	bee::render::draw_rectangle(cx-ox, cy-oy, w, h, -1, c_back);
	if (_i("has_hover")) {
		const std::pair<int,int> mpos (bee::mouse::get_pos());
		bee::render::draw_rectangle(cx-ox, cy+get_option_at(self, mpos.first, mpos.second)*_i("option_height") - oy, w, _i("option_height"), -1, c_highlight);
	}
	bee::render::draw_rectangle(cx-ox, cy-oy, w, h, 6, c_border);

	bee::Font* font = bee::engine->font_default;
	size_t i = 0;
	for (auto& option : _v("options")) {
		if (_i("type") == 0) {
			bee::render::draw_rectangle(cx + 16 - ox, cy + i*_i("option_height") + 8 - oy, 16, 16, 1, c_border);
			if (_v("option_state")[i].i) {
				bee::render::draw_line(
					cx + 18 - ox, cy + i*_i("option_height") + 16 - oy,
					cx + 22 - ox, cy + i*_i("option_height") + 22 - oy,
					c_border
				);
				bee::render::draw_line(
					cx + 22 - ox, cy + i*_i("option_height") + 22 - oy,
					cx + 30 - ox, cy + i*_i("option_height") + 10 - oy,
					c_border
				);
			}
		} else if (_i("type") == 1) {
			bee::render::draw_circle(glm::vec3(cx + 24 - ox, cy + i*_i("option_height") + 16 - oy, 0), 8, 1, c_border);
			if (_v("option_state")[i].i) {
				bee::render::draw_circle(glm::vec3(cx + 24 - ox, cy + i*_i("option_height") + 16 - oy, 0), 6, -1, c_border);
			}
		}

		font->draw_fast(cx + 40 - ox, cy + i*_i("option_height") + 8 - oy, option.s, c_border);

		bee::render::draw_line(cx - ox, cy + (i+1)*_i("option_height") - oy, cx + _i("w") - ox, cy + (i+1)*_i("option_height") - oy, c_border);

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

	int cx, cy;
	std::tie(cx, cy) = self->get_corner();

	if (
		(mx < cx-1)
		||(my < cy-1)
		||(mx > cx+1 + _i("w"))
		||(my > cy+1 + _i("h"))
	) {
		return -1;
	}

	return util::fit_bounds<int>((my-cy) / _i("option_height"), 0, _v("options").size()-1);
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

#endif // BEE_OBJ_UI_OPTIONBOX
