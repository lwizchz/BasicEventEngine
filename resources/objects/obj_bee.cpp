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
		bee::E_EVENT::OUTSIDE_ROOM,
		bee::E_EVENT::DRAW
	});
}
void ObjBee::create(bee::Instance* self) {
	self->get_physbody()->set_shape(bee::E_PHYS_SHAPE::BOX, new double[3] {100.0, 100.0, 100.0});
	self->get_physbody()->set_mass(0.0);
	self->get_physbody()->add_constraint(bee::E_PHYS_CONSTRAINT::FLAT, nullptr);

	// create event
	std::cout << "u r a b " << self->id << "\n";
	_p("text_id") = nullptr;

	bee::Instance* ui_handle = bee::ui::create_handle(self->get_corner_x(), self->get_corner_y(), 100, 100, self);
	ObjUIHandle* obj_ui_handle = static_cast<ObjUIHandle*>(ui_handle->get_object());
	obj_ui_handle->set_is_visible(ui_handle, false);
	_p("ui_handle") = ui_handle;

	if (self == obj_bee->get_instance(0)) {
		_s("serialdata") = self->serialize();

		bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartPath", [this, self] (const SDL_Event* e) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			self->set_mass(1.0);
			self->path_start(path_bee, 100.0, bee::E_PATH_END::STOP, true);
			self->set_path_drawn(true);
		}));

		bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartSerialize", [this, self] (const SDL_Event* e) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			bee::messenger::send({"bee"}, bee::E_MESSAGE::INFO, self->serialize(true));
			_s("serialdata") = self->serialize();
		}));
		bee::kb::bind(SDLK_UNKNOWN, bee::KeyBind("StartDeserialize", [this, self] (const SDL_Event* e) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			self->deserialize(_s("serialdata"));
		}));
	}
}
void ObjBee::destroy(bee::Instance* self) {
	delete static_cast<bee::TextData*>(_p("text_id"));

	bee::ui::destroy_parent(self);

	Object::destroy(self);
}
void ObjBee::commandline_input(bee::Instance* self, const std::string& str) {
	std::cout << "bee" << self->id << ":~~~" << str << "~~~\n";
}
void ObjBee::outside_room(bee::Instance* self) {
	bee::get_current_room()->destroy(self);
}
void ObjBee::draw(bee::Instance* self) {
	int size = 100;
	double r = radtodeg(self->get_physbody()->get_rotation_z());
	self->draw(size, size, r, bee::RGBA(bee::E_RGB::WHITE));

	_p("text_id") = font_liberation->draw(static_cast<bee::TextData*>(_p("text_id")), self->get_corner_x(), self->get_corner_y(), bee_itos(self->id));

	lt_bee->set_position(glm::vec4(self->get_x(), self->get_y(), 0.0, 1.0));
	lt_bee->set_color({static_cast<Uint8>(self->id*50), static_cast<Uint8>(self->id*20), 255, 255});
	lt_bee->queue();
}

#endif // RES_OBJ_BEE
