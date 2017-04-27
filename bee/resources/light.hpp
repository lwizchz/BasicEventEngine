/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_LIGHT_H
#define _BEE_LIGHT_H 1

#include "../game.hpp"

class BEE::LightData {
	public:
		bee_light_t type = BEE_LIGHT_AMBIENT;
		glm::vec4 position;
		glm::vec4 direction;
		glm::vec4 attenuation; // The components of attenuation: x=the brightness, y=the cone width, z=the range, all roughly in pixels
		RGBA color; // The alpha value is treated as the light intensity
};
class BEE::LightableData {
	public:
		glm::vec4 position;
		std::vector<glm::vec4> mask;
};

class BEE::Light: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string path;

		LightData lighting;

		bool has_drawn_sdl = false;
	public:
		Light();
		Light(const std::string&, const std::string&);
		~Light();
		int add_to_resources();
		int reset();
		int print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		bee_light_t get_type() const;
		glm::vec4 get_position() const;
		glm::vec4 get_direction() const;
		glm::vec4 get_attenuation() const;
		RGBA get_color() const;

		int set_name(const std::string&);
		int set_path(const std::string&);
		int set_type(bee_light_t);
		int set_position(glm::vec4);
		int set_direction(glm::vec4);
		int set_attenuation(glm::vec4);
		int set_color(RGBA);

		int queue();
};

#endif // _BEE_LIGHT_H
