/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_LIGHT_H
#define BEE_LIGHT_H 1

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include <glm/glm.hpp> // Include the required OpenGL headers

#include "resource.hpp"

#include "../enum.hpp"

#include "../data/variant.hpp"

#include "../render/rgba.hpp"

namespace bee {
	/// Used to store all relevant rendering information
	struct LightData {
		E_LIGHT_TYPE type; ///< The type of lighting to render
		glm::vec4 position; ///< The position of the light
		glm::vec4 direction; ///< The direction of the light
		glm::vec4 attenuation; ///< The components of attenuation: x=brightness, y=cone width, z=range, all roughly in pixels
		RGBA color; ///< The light color, where the alpha value is treated as the light intensity

		LightData();
	};

	/// Used to define a 2D object that can cast shadows
	struct LightableData {
		glm::vec4 position; ///< The position of the lightable object
		std::vector<glm::vec4> mask; ///< The mask for the object relative to the position
	};

	/// Used to render lighting effects
	class Light: public Resource {
		static std::map<size_t,Light*> list;
		static size_t next_id;

		size_t id; ///< The unique Light identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the config file used for the lighting

		LightData lighting; ///< The properties that define the light
	public:
		// See bee/resource/light.cpp for function comments
		Light();
		Light(const std::string&, const std::string&);
		~Light();

		static size_t get_amount();
		static Light* get(size_t);
		static Light* get_by_name(const std::string&);
		static Light* add(const std::string&, const std::string&);

		size_t add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		size_t get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		E_LIGHT_TYPE get_type() const;
		glm::vec4 get_position() const;
		glm::vec4 get_direction() const;
		glm::vec4 get_attenuation() const;
		RGBA get_color() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
		void set_type(E_LIGHT_TYPE);
		void set_position(const glm::vec4&);
		void set_direction(const glm::vec4&);
		void set_attenuation(const glm::vec4&);
		void set_color(RGBA);

		int load();

		int queue();
	};
}

#endif // BEE_LIGHT_H
