/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_LIGHT_H
#define BEE_LIGHT_H 1

#include "../defines.hpp"

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include <glm/glm.hpp> // Include the required OpenGL headers

#include "resource.hpp"

#include "../enum.hpp"

#include "../render/rgba.hpp"

namespace bee {
	struct LightData { // The data struct which contains all of the relevant rendering information for the light
		E_LIGHT type; // The type of lighting to render
		glm::vec4 position; // The position of the light
		glm::vec4 direction; // The direction of the light
		glm::vec4 attenuation; // The components of attenuation: x=the brightness, y=the cone width, z=the range, all roughly in pixels
		RGBA color; // The light color, where the alpha value is treated as the light intensity

		LightData();
	};

	struct LightableData { // The data struct which defines an object that can cast shadows
		glm::vec4 position; // The position of the lightable object
		std::vector<glm::vec4> mask; // The mask for the object, relative to the position
	};

	class Light: public Resource { // The light resource class is used to draw all lighting effects
		static std::map<int,Light*> list;
		static int next_id;

		int id; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the file to load the light from

		LightData lighting; // The properties that define the light
	public:
		// See bee/resources/light.cpp for function comments
		Light();
		Light(const std::string&, const std::string&);
		~Light();

		static size_t get_amount();
		static Light* get(int);
		static Light* get_by_name(const std::string&);
		static Light* add(const std::string&, const std::string&);

		int add_to_resources();
		int reset();
		void print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		E_LIGHT get_type() const;
		glm::vec4 get_position() const;
		glm::vec4 get_direction() const;
		glm::vec4 get_attenuation() const;
		RGBA get_color() const;

		int set_name(const std::string&);
		int set_path(const std::string&);
		int set_type(E_LIGHT);
		int set_position(const glm::vec4&);
		int set_direction(const glm::vec4&);
		int set_attenuation(const glm::vec4&);
		int set_color(RGBA);

		int queue();
	};
}

#endif // BEE_LIGHT_H
