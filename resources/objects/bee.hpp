/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_BEE_H
#define RES_OBJ_BEE_H 1

class ObjBee : public bee::Object {
	public:
		ObjBee();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void commandline_input(bee::Instance*, const std::string&);
		void outside_room(bee::Instance*);
		void draw(bee::Instance*);
};
ObjBee::ObjBee() : Object("obj_bee", "bee.hpp") {
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
	(*s)["text_id"] = static_cast<void*>(nullptr);

	bee::Instance* ui_handle = bee::ui::create_handle(self->get_corner_x(), self->get_corner_y(), 100, 100, self);
	ObjUIHandle* obj_ui_handle = dynamic_cast<ObjUIHandle*>(ui_handle->get_object());
	obj_ui_handle->set_is_visible(ui_handle, false);
	(*s)["ui_handle"] = static_cast<void*>(ui_handle);

	if (self == obj_bee->get_instance(0)) {
		(*s)["serialdata"] = self->serialize();

		//bee::console::add_keybind(SDLK_y, bee::KeyBind("StartPath"), [this, self] (std::shared_ptr<bee::MessageContents> msg) {
		bee::console::add_keybind(SDLK_y, bee::KeyBind("StartPath"), [this, self] (std::shared_ptr<bee::MessageContents> msg) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			self->set_mass(1.0);
			self->path_start(path_bee, 100.0, bee::E_PATH_END::STOP, true);
			self->set_path_drawn(true);
		});

		//bee::console::add_keybind(SDLK_b, bee::KeyBind("StartSerialize"), [this, self] (std::shared_ptr<bee::MessageContents> msg) {
		bee::console::add_keybind(SDLK_b, bee::KeyBind("StartSerialize"), [this, self] (std::shared_ptr<bee::MessageContents> msg) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			bee::messenger::send({"bee"}, bee::E_MESSAGE::INFO, self->serialize(true));
			(*s)["serialdata"] = self->serialize();
		});
		//bee::console::add_keybind(SDLK_v, bee::KeyBind("StartDeserialize"), [this, self] (std::shared_ptr<bee::MessageContents> msg) {
		bee::console::add_keybind(SDLK_v, bee::KeyBind("StartDeserialize"), [this, self] (std::shared_ptr<bee::MessageContents> msg) mutable {
			self = obj_bee->get_instance(0);
			s = &self->get_data();

			self->deserialize(_s("serialdata"));
		});
	}
}
void ObjBee::destroy(bee::Instance* self) {
	// Unbind keybindings
	if (self == obj_bee->get_instance(0)) {
		bee::console::unbind(bee::KeyBind("StartPath"), true);
		bee::console::unbind(bee::KeyBind("StartSerialize"), true);
		bee::console::unbind(bee::KeyBind("StartDeserialize"), true);
	}

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
	self->draw(size, size, r, bee::E_RGB::WHITE, SDL_FLIP_NONE);

	(*s)["text_id"] = static_cast<void*>(font_liberation->draw(static_cast<bee::TextData*>(_p("text_id")), self->get_corner_x(), self->get_corner_y(), bee_itos(self->id)));

	lt_bee->set_position(glm::vec4(self->get_x(), self->get_y(), 0.0, 1.0));
	lt_bee->set_color({static_cast<Uint8>(self->id*50), static_cast<Uint8>(self->id*20), 255, 255});
	lt_bee->queue();
}

#endif // RES_OBJ_BEE_H
