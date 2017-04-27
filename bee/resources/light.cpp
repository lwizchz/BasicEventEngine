/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
BEE::Light::Light (const std::string& new_name, const std::string& new_path) {
	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->lights.game != nullptr) {
		game = BEE::resource_list->lights.game;
	}

	reset();

	add_to_resources();
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add light resource: \"" + new_name + "\" from " + new_path);
		throw(-1);
	}

	set_name(new_name);
	set_path(new_path);
}
BEE::Light::~Light() {
	BEE::resource_list->lights.remove_resource(id);
}
int BEE::Light::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->lights.add_resource(this); // Add the resource and get the new id
	}

	return 0;
}
int BEE::Light::reset() {
	name = "";
	path = "";

	lighting.type = BEE_LIGHT_AMBIENT;
	lighting.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lighting.color = {255, 255, 255, 255};

	has_drawn_sdl = false;

	return 0;
}
int BEE::Light::print() const {
	std::stringstream s;
	s <<
	"Light { "
	"\n	id          " << id <<
	"\n	name        " << name <<
	"\n	path        " << path;
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

int BEE::Light::get_id() const {
	return id;
}
std::string BEE::Light::get_name() const {
	return name;
}
std::string BEE::Light::get_path() const {
	return path;
}
bee_light_t BEE::Light::get_type() const {
	return lighting.type;
}
glm::vec4 BEE::Light::get_position() const {
	return lighting.position;
}
glm::vec4 BEE::Light::get_direction() const {
	return lighting.direction;
}
glm::vec4 BEE::Light::get_attenuation() const {
	return lighting.attenuation;
}
BEE::RGBA BEE::Light::get_color() const {
	return lighting.color;
}

int BEE::Light::set_name(const std::string& new_name) {
	name = new_name;
	return 0;
}
int BEE::Light::set_path(const std::string& new_path) {
	path = new_path;
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
