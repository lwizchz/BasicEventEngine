/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_BEE
#define RES_OBJ_BEE 1

#include "../resources.hpp"

#include "obj_bee.hpp"

ObjBee::ObjBee() : Object("obj_bee", "obj_bee.hpp") {
	implemented_events.insert({
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::COMMANDLINE_INPUT,
		bee::E_EVENT::PATH_END,
		bee::E_EVENT::OUTSIDE_ROOM,
		bee::E_EVENT::DRAW
	});
}
void ObjBee::create(bee::Instance* self) {
	self->set_computation_type(bee::E_COMPUTATION::SEMIPLAYER);
	self->get_physbody()->set_shape(bee::E_PHYS_SHAPE::BOX, new double[3] {100.0, 100.0, 100.0});
	self->get_physbody()->set_mass(0.0);
	self->get_physbody()->add_constraint(bee::E_PHYS_CONSTRAINT::FLAT, nullptr);

	// create event
	std::cout << "u r a b " << self->id << "\n";
	_p("text_id") = nullptr;

	bee::Instance* ui_handle = bee::ui::create_handle(self->get_corner().first, self->get_corner().second, 100, 100, self);
	ObjUIHandle* obj_ui_handle = static_cast<ObjUIHandle*>(ui_handle->get_object());
	obj_ui_handle->set_is_visible(ui_handle, false);
	_p("ui_handle") = ui_handle;

	if (self == obj_bee->get_instance(0)) {
		_a("serialdata") = self->serialize();
		_p("path") = nullptr;
		_d("path_previous_mass") = 0.0;

		bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartPath", [this, self] (const SDL_Event* e) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			if (_p("path") != nullptr) {
				delete static_cast<bee::PathFollower*>(_p("path"));
				_p("path") = nullptr;
			}

			self->set_mass(1.0);
			bee::PathFollower* pf = new bee::PathFollower(path_bee, {100.0, 100.0, 0.0}, 100);
			pf->is_curved = true;
			bee::get_current_room()->automate_path(self, pf);
			_p("path") = pf;
		}));

		bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartSerialize", [this, self] (const SDL_Event* e) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			_m("serialdata") = self->serialize();
			bee::messenger::send({"bee"}, bee::E_MESSAGE::INFO, _a("serialdata").to_str(true));
		}));
		bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartDeserialize", [this, self] (const SDL_Event* e) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			self->deserialize(_m("serialdata"));
		}));
	}
}
void ObjBee::destroy(bee::Instance* self) {
	if (self == obj_bee->get_instance(0)) {
		_a("serialdata").reset();

		if (_p("path") != nullptr) {
			delete static_cast<bee::PathFollower*>(_p("path"));
			_p("path") = nullptr;
		}
	}

	delete static_cast<bee::TextData*>(_p("text_id"));

	bee::ui::destroy_parent(self);

	Object::destroy(self);
}
void ObjBee::commandline_input(bee::Instance* self, const std::string& str) {
	std::cout << "bee" << self->id << ":~~~" << str << "~~~\n";
}
void ObjBee::path_end(bee::Instance* self, bee::PathFollower* pf) {
	bee::get_current_room()->automate_path(self, nullptr);

	delete static_cast<bee::PathFollower*>(_p("path"));
	_p("path") = nullptr;
}
void ObjBee::outside_room(bee::Instance* self) {
	bee::get_current_room()->destroy(self);
}
void ObjBee::draw(bee::Instance* self) {
	int size = 100;
	double r = util::radtodeg(self->get_physbody()->get_rotation_z());
	self->draw(size, size, r, bee::RGBA(bee::E_RGB::WHITE));

	if (static_cast<bee::PathFollower*>(_p("path")) != nullptr) {
		path_bee->draw(static_cast<bee::PathFollower*>(_p("path")));
	}

	_p("text_id") = font_liberation->draw(static_cast<bee::TextData*>(_p("text_id")), self->get_corner().first, self->get_corner().second, std::to_string(self->id));

	lt_bee->set_position(glm::vec4(self->get_x(), self->get_y(), 0.0, 1.0));
	lt_bee->set_color({static_cast<Uint8>(self->id*50), static_cast<Uint8>(self->id*20), 255, 255});
	lt_bee->queue();
}

#endif // RES_OBJ_BEE
