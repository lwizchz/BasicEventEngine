/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_TEXTENTRY
#define BEE_UI_OBJ_TEXTENTRY 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "text_entry.hpp"

ObjUITextEntry::ObjUITextEntry() : ObjUIElement("obj_ui_text_entry", "/ui/objects/text_entry.cpp") {
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::KEYBOARD_INPUT,
		bee::E_EVENT::DRAW
	});
}
void ObjUITextEntry::create(bee::Instance* self) {
	ObjUIElement::create(self);
	reset(self);
}
void ObjUITextEntry::destroy(bee::Instance* self) {
	delete static_cast<bee::TextData*>(_p("input_td"));
	ObjUIElement::destroy(self);
}
void ObjUITextEntry::keyboard_input(bee::Instance* self, SDL_Event* e) {
	if (!_i("has_focus")) {
		return;
	}

	std::string input = _s("input");
	char c = bee::kb::append_input(&input, &e->key);

	switch (e->key.keysym.sym) {
		case SDLK_TAB: {
			if (_i("rows") == 1) {
				input.pop_back();
				complete(self, input);
			}

			break;
		}
		case SDLK_RETURN: {
			if (
				_i("rows") == 1
				||!(e->key.keysym.mod & KMOD_SHIFT)
			) {
				input.pop_back();

				(*s)["input"] = input;
				bee::ui::text_entry_callback(self, input);
				return;
			}

			break;
		}
		default: {
			if (input.empty()) { // If the input line is now empty (i.e. backspace), clear the completion commands
				reset_completion(self);
			}

			if (c != '\0') {
				std::vector<bee::SIDP>* completions = static_cast<std::vector<bee::SIDP>*>(_p("completions"));
				if (completions->size() > 1) { // Clear the completion commands when additional input is received
					complete(self, input);
					input = _s("input");
				}
			}

			(*s)["input"] = input;
			bee::ui::text_entry_handler(self, input, e);
		}
	}
}
void ObjUITextEntry::draw(bee::Instance* self) {
	if (!_i("is_visible")) {
		return;
	}

	bee::render::set_is_lightable(false);

	bee::Font* font = bee::engine->font_default;
	bee::RGBA c_text (bee::E_RGB::BLACK);
	bee::RGBA c_back = {_i("color_r"), _i("color_g"), _i("color_b"), _i("color_a")};

	int ox = 0, oy = 0;
	bee::ViewPort* v = bee::get_current_room()->get_current_view();
	if (v != nullptr) {
		ox = v->view.x;
		oy = v->view.y;
	}

	bee::draw_rectangle(self->get_corner_x() - ox, self->get_corner_y() - oy, _i("w"), _i("h"), -1, c_back); // Draw a box to contain the completions

	std::string input = _s("input");
	if (!input.empty()) {
		(*s)["input_td"] = static_cast<void*>(font->draw(
			static_cast<bee::TextData*>(_p("input_td")),
			self->get_corner_x() - ox,
			self->get_corner_y() - oy,
			input
		));
	}
	if (_i("has_focus")) {
		if (bee::get_ticks()/500 % 2) { // Draw a blinking cursor that changes every 500 ticks
			font->draw_fast(
				self->get_corner_x() - ox + font->get_string_width(input),
				self->get_corner_y() - oy,
				"_",
				c_text
			);
		}
	}

	if (_i("rows") == 1) {
		// Draw any completions in a box below the input line
		std::vector<bee::SIDP>* completions = static_cast<std::vector<bee::SIDP>*>(_p("completions"));
		if (completions->size() > 1) { // If completions exist, draw them
			bee::draw_rectangle(self->get_corner_x(), self->get_corner_y() + _i("h"), _i("w"), completions->size()*_i("h"), -1, c_back); // Draw a box to contain the completions
			for (size_t i=0; i<completions->size(); ++i) { // Iterate over the completions
				std::string cmd = " " + SIDP_s(completions->at(i)); // Prepend each completion with a space
				if (static_cast<int>(i) == _i("completion_index")) { // If the completion is selected, replace the space with a cursor
					cmd[0] = '>';
				}
				font->draw_fast(self->get_corner_x(), self->get_corner_y() + _i("h") + _i("h")*i, cmd, c_text); // Draw the completions
			}
		}
	}

	bee::render::set_is_lightable(true);
}

void ObjUITextEntry::reset(bee::Instance* self) {
	ObjUIElement::reset(self);

	(*s)["input"] = "";
	(*s)["input_td"] = static_cast<void*>(nullptr);

	(*s)["rows"] = 0;
	(*s)["cols"] = 0;

	(*s)["entry_func"] = static_cast<void*>(nullptr);

	reset_completion(self);
}
void ObjUITextEntry::reset_completion(bee::Instance* self) {
	(*s)["completions"].vector(new std::vector<bee::SIDP>());
	(*s)["completion_index"] = -1;
	(*s)["completions_td"] = static_cast<void*>(nullptr);
}
void ObjUITextEntry::set_size(bee::Instance* self, int rows, int cols) {
	(*s)["rows"] = rows;
	(*s)["cols"] = cols;

	bee::Font* font = bee::engine->font_default;
	(*s)["w"] = font->get_string_width() * cols;
	(*s)["h"] = font->get_string_height() * rows;
}
void ObjUITextEntry::set_input(bee::Instance* self, const std::string& new_input) {
	(*s)["input"] = new_input;
}
std::string ObjUITextEntry::get_input(bee::Instance* self) const {
	return _s("input");
}

void ObjUITextEntry::complete(bee::Instance* self, const std::string& input) {
	std::vector<bee::SIDP> completions = bee::ui::text_entry_completor(self, input);

	std::string new_input = input;
	if (completions.size() == 0) {
		(*s)["completion_index"] = -1;
	} else if (completions.size() == 1) {
		new_input = SIDP_s(completions[0]) + " ";
		completions.clear();
		(*s)["completion_index"] = -1;
	} else {
		// Attempt partial completion if all completions share a common root
		// ! Note that by definition, all completions will share at least as many characters as the length of the input
		std::string matched = ""; // Define a string that contains the matched portion of multiple completions
		bool is_matched = true;
		for (size_t i=new_input.length(); i<SIDP_s(completions[0]).length(); ++i) { // Iterate over the characters of the first completion, beginning at the first character that might not match
			for (size_t j=1; j<completions.size(); ++j) { // Iterate over the other completions
				if (SIDP_s(completions[0])[i] != SIDP_s(completions[j])[i]) { // If the given character does not match each completion, then break
					is_matched = false;
					break;
				}
			}

			if (is_matched) { // If the character matched, append it to the match string
				matched += SIDP_s(completions[0])[i];
			} else { // Otherwise, break;
				break;
			}
		}

		new_input += matched; // Replace the input with the matched portion of a completion
	}

	(*s)["completions"] = completions;
	(*s)["input"] = new_input;
}

#endif // BEE_UI_OBJ_TEXTENTRY
