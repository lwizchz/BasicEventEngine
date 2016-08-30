/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_LIGHT
#define _BEE_LIGHT 1

#include "light.hpp"

BEE::Light::Light () {
	if (BEE::resource_list->lights.game != nullptr) {
		game = BEE::resource_list->lights.game;
	}

	reset();
}
BEE::Light::Light (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/lights/"+path);
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add light resource: " + path);
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Light::~Light() {
	BEE::resource_list->lights.remove_resource(id);
}
int BEE::Light::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == light_path) {
			return 1;
		}
		BEE::resource_list->lights.remove_resource(id);
		id = -1;
	}

	id = BEE::resource_list->lights.add_resource(this);
	BEE::resource_list->lights.set_resource(id, this);

	if (BEE::resource_list->lights.game != nullptr) {
		game = BEE::resource_list->lights.game;
	}

	return 0;
}
int BEE::Light::reset() {
	name = "";
	light_path = "";

	lighting.type = BEE_LIGHT_AMBIENT;
	lighting.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.color = {255, 255, 255, 255};

	has_drawn_sdl = false;

	return 0;
}
int BEE::Light::print() {
	std::stringstream s;
	s <<
	"Light { "
	"\n	id          " << id <<
	"\n	name        " << name <<
	"\n	light_path  " << light_path;
	switch (lighting.type) {
		case BEE_LIGHT_AMBIENT: {
			s << "\n	type        ambient";
			break;
		}
		case BEE_LIGHT_DIFFUSE: {
			s << "\n	type        diffuse";
			break;
		}
		case BEE_LIGHT_POINT: {
			s << "\n	type        point";
			break;
		}
		case BEE_LIGHT_SPOT: {
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
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}

int BEE::Light::get_id() {
	return id;
}
std::string BEE::Light::get_name() {
	return name;
}
std::string BEE::Light::get_path() {
	return light_path;
}
bee_light_t BEE::Light::get_type() {
	return lighting.type;
}
glm::vec4 BEE::Light::get_position() {
	return lighting.position;
}
glm::vec4 BEE::Light::get_direction() {
	return lighting.direction;
}
glm::vec4 BEE::Light::get_attenuation() {
	return lighting.attenuation;
}
BEE::RGBA BEE::Light::get_color() {
	return lighting.color;
}

int BEE::Light::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Light::set_path(std::string new_path) {
	light_path = new_path;
	return 0;
}
int BEE::Light::set_type(bee_light_t new_type) {
	lighting.type = new_type;
	return 0;
}
int BEE::Light::set_position(glm::vec4 new_position) {
	lighting.position = new_position;
	return 0;
}
int BEE::Light::set_direction(glm::vec4 new_direction) {
	lighting.direction = new_direction;
	return 0;
}
int BEE::Light::set_attenuation(glm::vec4 new_attenuation) {
	lighting.attenuation = new_attenuation;
	return 0;
}
int BEE::Light::set_color(RGBA new_color) {
	lighting.color = new_color;
	return 0;
}

int BEE::Light::queue() {
	if (game->options->renderer_type == BEE_RENDERER_SDL) {
		if (!has_drawn_sdl) {
			game->messenger_send({"engine", "light"}, BEE_MESSAGE_WARNING, "Lighting is not fully supported in SDL mode");
			has_drawn_sdl = true;
		}
	}

	game->get_current_room()->add_light(lighting);

	return 0;
}

#endif // _BEE_LIGHT
