/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_LIGHT
#define BEE_LIGHT 1

#include <sstream> // Include the required library headers

#include "light.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../init/gameoptions.hpp"

#include "../core/enginestate.hpp"
#include "../core/room.hpp"
#include "../core/messenger/messenger.hpp"

#include "room.hpp"

namespace bee {
	/*
	* LightData::LightData() - Construct the data struct and initialize all values
	*/
	LightData::LightData() :
		type(E_LIGHT::AMBIENT),
		position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		direction(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		attenuation(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		color({255, 255, 255, 255})
	{}

	std::map<int,Light*> Light::list;
	int Light::next_id = 0;

	/*
	* Light::Light() - Default construct the light
	* ! This constructor should only be directly used for temporary lights, the other constructor should be used for all other cases
	*/
	Light::Light () :
		Resource(),

		id(-1),
		name(),
		path(),

		lighting(),

		has_drawn_sdl(false)
	{}
	/*
	* Light::Light() - Construct the light, add it to the light resource list, and set the new name and path
	*/
	Light::Light (const std::string& new_name, const std::string& new_path) :
		Light() // Default initialize all variables
	{
		add_to_resources(); // Add the light to the appropriate resource list
		if (id < 0) { // If the light could not be added to the resource list, output a warning
			messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add light resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the light name
		set_path(new_path); // Set the light path
	}
	/*
	* Light::~Light() - Remove the light from the resouce list
	*/
	Light::~Light() {
		if (list.find(id) != list.end()) { // Remove the light from the resource list
			list.erase(id);
		}
	}

	/*
	* Light::add_to_resources() - Add the sprite to the appropriate resource list
	*/
	int Light::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Light::get_amount() - Return the amount of light resources
	*/
	size_t Light::get_amount() {
		return list.size();
	}
	/*
	* Light::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Light* Light::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Light::reset() - Reset all resource variables for reinitialization
	*/
	int Light::reset() {
		// Reset all properties
		name = "";
		path = "";

		lighting.type = E_LIGHT::AMBIENT;
		lighting.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lighting.direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lighting.color = {255, 255, 255, 255};

		has_drawn_sdl = false;

		return 0; // Return 0 on success
	}
	/*
	* Light::print() - Print all relevant information about the resource
	*/
	int Light::print() const {
		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
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
		messenger_send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Light::get_*() - Return the requested resource information
	*/
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

	/*
	* Light::set_*() - Set the requested resource data
	*/
	int Light::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Light::set_path(const std::string& new_path) {
		path = "resources/lights/"+new_path; // Append the path to the light directory
		return 0;
	}
	int Light::set_type(E_LIGHT new_type) {
		lighting.type = new_type;
		return 0;
	}
	int Light::set_position(const glm::vec4& new_position) {
		lighting.position = new_position;
		return 0;
	}
	int Light::set_direction(const glm::vec4& new_direction) {
		lighting.direction = new_direction;
		return 0;
	}
	int Light::set_attenuation(const glm::vec4& new_attenuation) {
		lighting.attenuation = new_attenuation;
		return 0;
	}
	int Light::set_color(RGBA new_color) {
		lighting.color = new_color;
		return 0;
	}

	/*
	* Light::queue() - Queue the light for drawing in the Room rendering loop
	*/
	int Light::queue() {
		if (engine->options->renderer_type == E_RENDERER::SDL) { // If the SDL rendering is being used, output a warning
			if (!has_drawn_sdl) { // If the SDL draw call hasn't been called before, output a warning
				messenger_send({"engine", "light"}, E_MESSAGE::WARNING, "Lighting is not fully supported in SDL mode");
				has_drawn_sdl = true; // Set the SDL drawing boolean
			}
		}

		get_current_room()->add_light(lighting); // Add the light to the Room lighting queue

		return 0; // Return 0 on success
	}
}

#endif // BEE_LIGHT
