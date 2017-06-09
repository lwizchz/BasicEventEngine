/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_LIGHT
#define BEE_LIGHT 1

#include <sstream>

#include "light.hpp"

#include "room.hpp"

#include "../engine.hpp"

#include "../init/gameoptions.hpp"

#include "../core/enginestate.hpp"
#include "../core/room.hpp"
#include "../core/messenger/messenger.hpp"

namespace bee {
	Light::Light () {
		reset();
	}
	Light::Light (const std::string& new_name, const std::string& new_path) {
		reset();

		add_to_resources();
		if (id < 0) {
			messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add light resource: \"" + new_name + "\" from " + new_path);
			throw(-1);
		}

		set_name(new_name);
		set_path(new_path);
	}
	Light::~Light() {
		resource_list->lights.remove_resource(id);
	}
	int Light::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = resource_list->lights.add_resource(this); // Add the resource and get the new id
		}

		return 0;
	}
	int Light::reset() {
		name = "";
		path = "";

		lighting.type = E_LIGHT::AMBIENT;
		lighting.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lighting.direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lighting.color = {255, 255, 255, 255};

		has_drawn_sdl = false;

		return 0;
	}
	int Light::print() const {
		std::stringstream s;
		s <<
		"Light { "
		"\n	id          " << id <<
		"\n	name        " << name <<
		"\n	path        " << path;
		switch (lighting.type) {
			case E_LIGHT::AMBIENT: {
				s << "\n	type        ambient";
				break;
			}
			case E_LIGHT::DIFFUSE: {
				s << "\n	type        diffuse";
				break;
			}
			case E_LIGHT::POINT: {
				s << "\n	type        point";
				break;
			}
			case E_LIGHT::SPOT: {
				s << "\n	type        spot";
				break;
			}
			default: {
				s << "\n	type        unknown";
			}
		}
		s <<
		"\n	position    (" << lighting.position.x << ", " << lighting.position.y << ", " << lighting.position.z << ")" <<
		"\n	direction   (" << lighting.direction.x << ", " << lighting.direction.y << ", " << lighting.direction.z << ")" <<
		"\n	attenuation (" << lighting.attenuation.x << ", " << lighting.attenuation.y << ", " << lighting.attenuation.z << ")" <<
		"\n	color       " << (int)lighting.color.r << ", " << (int)lighting.color.g << ", " << (int)lighting.color.b <<
		"\n}\n";
		messenger_send({"engine", "resource"}, E_MESSAGE::INFO, s.str());

		return 0;
	}

	int Light::get_id() const {
		return id;
	}
	std::string Light::get_name() const {
		return name;
	}
	std::string Light::get_path() const {
		return path;
	}
	E_LIGHT Light::get_type() const {
		return lighting.type;
	}
	glm::vec4 Light::get_position() const {
		return lighting.position;
	}
	glm::vec4 Light::get_direction() const {
		return lighting.direction;
	}
	glm::vec4 Light::get_attenuation() const {
		return lighting.attenuation;
	}
	RGBA Light::get_color() const {
		return lighting.color;
	}

	int Light::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Light::set_path(const std::string& new_path) {
		path = new_path;
		return 0;
	}
	int Light::set_type(E_LIGHT new_type) {
		lighting.type = new_type;
		return 0;
	}
	int Light::set_position(glm::vec4 new_position) {
		lighting.position = new_position;
		return 0;
	}
	int Light::set_direction(glm::vec4 new_direction) {
		lighting.direction = new_direction;
		return 0;
	}
	int Light::set_attenuation(glm::vec4 new_attenuation) {
		lighting.attenuation = new_attenuation;
		return 0;
	}
	int Light::set_color(RGBA new_color) {
		lighting.color = new_color;
		return 0;
	}

	int Light::queue() {
		if (engine->options->renderer_type == E_RENDERER::SDL) {
			if (!has_drawn_sdl) {
				messenger_send({"engine", "light"}, E_MESSAGE::WARNING, "Lighting is not fully supported in SDL mode");
				has_drawn_sdl = true;
			}
		}

		get_current_room()->add_light(lighting);

		return 0;
	}
}

#endif // BEE_LIGHT
