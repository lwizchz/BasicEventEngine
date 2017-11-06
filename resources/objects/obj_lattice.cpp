/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_LATTICE
#define RES_OBJ_LATTICE 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "obj_lattice.hpp"

ObjLattice::ObjLattice() : Object("obj_lattice", "obj_lattice.cpp") {
	implemented_events.insert({
		bee::E_EVENT::UPDATE,
		bee::E_EVENT::CREATE,
		bee::E_EVENT::DESTROY,
		bee::E_EVENT::STEP_MID,
		bee::E_EVENT::MOUSE_PRESS,
		bee::E_EVENT::DRAW
	});
}

void ObjLattice::create(bee::Instance* self) {
	(*s)["offset_x"] = 0.0;
	(*s)["offset_y"] = 0.0;
	(*s)["offset_z"] = 0.0;

	(*s)["rot_x"] = 0.0;
	(*s)["rot_y"] = 0.0;
	(*s)["rot_z"] = 0.0;

	(*s)["color_r"] = 255;
	(*s)["color_g"] = 255;
	(*s)["color_b"] = 255;

	(*s)["is_creating"] = false;

	std::vector<bee::SIDP>* lv = new std::vector<bee::SIDP>();

	std::string datastr = file_get_contents("resources/rooms/" + SIDP_s(bee::console::get_var("level")) + ".csv");
	std::istringstream data_stream (datastr);

	while (!data_stream.eof()) {
		std::string tmp;
		getline(data_stream, tmp);

		if (tmp.empty()) {
			continue;
		}

		std::vector<std::string> p = splitv(trim(tmp), '\t', false);
		std::vector<std::string> params;
		for (auto& e : p) { // Remove empty values
			if (!e.empty()) {
				params.push_back(e);
			}
		}

		if (params[0][0] == '#') {
			continue;
		} else if (params[0] == "offset") {
			(*s)["offset_x"] = std::stod(params[1]);
			(*s)["offset_y"] = std::stod(params[2]);
			(*s)["offset_z"] = std::stod(params[3]);
		} else if (params[0] == "rot") {
			(*s)["rot_x"] = std::stod(params[1]);
			(*s)["rot_y"] = std::stod(params[2]);
			(*s)["rot_z"] = std::stod(params[3]);
		} else {
			lv->push_back(bee::SIDP(new PrimitiveLine(
				glm::vec3(
					std::stod(params[0]),
					std::stod(params[1]),
					std::stod(params[2])
				), glm::vec3(
					std::stod(params[3]),
					std::stod(params[4]),
					std::stod(params[5])
				)
			)));
		}
	}

	(*s)["levelvector"].vector(lv);
}
void ObjLattice::destroy(bee::Instance* self) {
	std::vector<bee::SIDP>* lv = static_cast<std::vector<bee::SIDP>*>(_p("levelvector"));
	for (auto& l : *lv) {
		PrimitiveLine* pl = static_cast<PrimitiveLine*>(SIDP_p(l));
		delete pl;
	}

	Object::destroy(self);
}
void ObjLattice::step_mid(bee::Instance* self) {
	bee::render_set_camera(new bee::Camera(glm::vec3(bee::get_width()/2.0, bee::get_height()/2.0, -540.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
}
void ObjLattice::mouse_press(bee::Instance* self, SDL_Event* e) {
	if ((_i("is_creating"))&&(e->button.button == SDL_BUTTON_LEFT)) {
		_v("levelvector").push_back(bee::SIDP(new PrimitiveLine(glm::vec3(0.0, 0.0, 0.0), glm::vec3(e->button.x, e->button.y, 0.0))));
	}
}
void ObjLattice::draw(bee::Instance* self) {
	glm::vec3 offset (_d("offset_x"), _d("offset_y"), _d("offset_z"));
	glm::vec3 offset3d (_d("offset_x"), _d("offset_y"), _d("offset_z")+2000.0);
	glm::vec3 offset3dfar (_d("offset_x"), _d("offset_y"), _d("offset_z")+10000.0);
	glm::vec3 rot (_d("rot_x"), _d("rot_y"), _d("rot_z"));
	bee::RGBA color (_i("color_r"), _i("color_g"), _i("color_b"), 255);

	size_t beam_line = -1;
	int beam_cooldown = 0;
	bee::Instance* player = obj_player->get_instance(0);
	if (player != nullptr) {
		beam_line = SIDP_i(player->get_data("beam_position"));
		beam_cooldown = SIDP_i(player->get_data("gun_cooldown"));
	}

	size_t i=0;
	for (auto& l : _v("levelvector")) {
		PrimitiveLine* pl = static_cast<PrimitiveLine*>(SIDP_p(l));
		pl->draw(offset, rot, color);
		if (_i("is_creating")) {
			continue;
		}

		pl->draw(offset3d, rot, color);

		if (i == beam_line) {
			glm::vec3 o1 (10.0*sin(degtorad(beam_cooldown*20)), 10.0*cos(degtorad(beam_cooldown*20)), 0.0);
			glm::vec3 o2 (0.0, 0.0, 2000.0-2000.0*beam_cooldown/40);
			bee::draw_triangle(pl->p1+offset+o1+o2, pl->p1+offset-o1+o2, pl->p1+offset3d, {0, 255, 255, 255}, true);
		} else {
			bee::draw_line(pl->p1+offset, pl->p1+offset3d, color);
		}
		bee::draw_line(pl->p1+offset3d, pl->p1+offset3dfar, {30, 30, 30, 255});
		++i;
	}
}

#endif // RES_OBJ_LATTICE
