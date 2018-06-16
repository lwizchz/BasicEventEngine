/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_LIGHT
#define BEE_LIGHT 1

#include "light.hpp" // Include the class resource header

#include "../util/files.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/rooms.hpp"

#include "room.hpp"

namespace bee {
	/**
	* Construct the data struct and initialize all values.
	*/
	LightData::LightData() :
		type(E_LIGHT_TYPE::AMBIENT),
		position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		direction(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		attenuation(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		color({255, 255, 255, 255})
	{}

	std::map<int,Light*> Light::list;
	int Light::next_id = 0;

	/**
	* Default construct the Light.
	* @note This constructor should only be directly used for temporary Lights, the other constructor should be used for all other cases.
	*/
	Light::Light() :
		Resource(),

		id(-1),
		name(),
		path(),

		lighting()
	{}
	/**
	* Construct the Light, add it to the Light resource list, and set the new name and path.
	* @param _name the name of the Light to use
	* @param _path the path of the Light's config file
	*
	* @throws int(-1) Failed to initialize Resource
	*/
	Light::Light(const std::string& _name, const std::string& _path) :
		Light() // Default initialize all variables
	{
		if (add_to_resources() < 0) { // Attempt to add the Light to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add Light resource: \"" + _name + "\" from " + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Remove the Light from the resource list.
	*/
	Light::~Light() {
		list.erase(id);
	}

	/**
	* @returns the number of Light resources
	*/
	size_t Light::get_amount() {
		return list.size();
	}
	/**
	* @oaran id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Light* Light::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Light
	*
	* @returns the Light resource with the given name or nullptr if not found
	*/
	Light* Light::get_by_name(const std::string& name) {
		for (auto& light : list) { // Iterate over the Lights in order to find the first one with the given name
			Light* l = light.second;
			if (l != nullptr) {
				if (l->get_name() == name) {
					return l; // Return the desired Light on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze, load, and return a newly created Light resource
	* @param name the name to initialize the Light with
	* @param path the path to initialize the Light with
	*
	* @returns the newly loaded Light
	*/
	Light* Light::add(const std::string& name, const std::string& path) {
		Light* new_light = new Light(name, path);
		new_light->load();
		return new_light;
	}

	/**
	* Add the Light to the appropriate resource list.
	*
	* @returns the Light id
	*/
	int Light::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with its new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Light::reset() {
		// Reset all properties
		name = "";
		path = "";

		lighting.type = E_LIGHT_TYPE::AMBIENT;
		lighting.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lighting.direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lighting.color = {255, 255, 255, 255};

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Light
	*/
	std::map<Variant,Variant> Light::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["type"] = static_cast<int>(lighting.type);
		info["position"] = {Variant(lighting.position.x), Variant(lighting.position.y), Variant(lighting.position.z), Variant(lighting.position.w)};
		info["direction"] = {Variant(lighting.direction.x), Variant(lighting.direction.y), Variant(lighting.direction.z), Variant(lighting.direction.w)};
		info["attenuation"] = {Variant(lighting.attenuation.x), Variant(lighting.attenuation.y), Variant(lighting.attenuation.z), Variant(lighting.attenuation.w)};
		info["color"] = {
			Variant(static_cast<int>(lighting.color.r)),
			Variant(static_cast<int>(lighting.color.g)),
			Variant(static_cast<int>(lighting.color.b)),
			Variant(static_cast<int>(lighting.color.a))
		};

		return info;
	}
	/**
	* Restore the Light from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	*/
	int Light::deserialize(std::map<Variant,Variant>& m) {
		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		lighting.type = static_cast<E_LIGHT_TYPE>(m["type"].i);
		lighting.position = glm::vec4(
			m["position"].v[0].f,
			m["position"].v[1].f,
			m["position"].v[2].f,
			m["position"].v[3].f
		);
		lighting.direction = glm::vec4(
			m["direction"].v[0].f,
			m["direction"].v[1].f,
			m["direction"].v[2].f,
			m["direction"].v[3].f
		);
		lighting.attenuation = glm::vec4(
			m["attenuation"].v[0].f,
			m["attenuation"].v[1].f,
			m["attenuation"].v[2].f,
			m["attenuation"].v[3].f
		);
		lighting.color = RGBA(
			m["color"].v[0].i,
			m["color"].v[1].i,
			m["color"].v[2].i,
			m["color"].v[3].i
		);

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Light::print() const {
		Variant m (serialize());
		messenger::send({"engine", "light"}, E_MESSAGE::INFO, "Light " + m.to_str(true));
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
	E_LIGHT_TYPE Light::get_type() const {
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

	void Light::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Fonts resource directory.
	*/
	void Light::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Light directory if not root
			path = "resources/lights/"+_path;
		}
	}
	void Light::set_type(E_LIGHT_TYPE _type) {
		lighting.type = _type;
	}
	void Light::set_position(const glm::vec4& _position) {
		lighting.position = _position;
	}
	void Light::set_direction(const glm::vec4& _direction) {
		lighting.direction = _direction;
	}
	void Light::set_attenuation(const glm::vec4& _attenuation) {
		lighting.attenuation = _attenuation;
	}
	void Light::set_color(RGBA _color) {
		lighting.color = _color;
	}

	/**
	* Load the Light from its path.
	*
	* @retval 0 success
	* @retval 1 failed to load the file
	* @retval 2 invalid value
	*/
	int Light::load() {
		std::string cfg (util::file_get_contents(path));
		if (cfg.empty()) { // If the file could not be loaded, output a warning
			messenger::send({"engine", "light"}, E_MESSAGE::WARNING, "Failed to load Light \"" + name + "\" from file \"" + path + "\"");
			return 1;
		}

		// Parse the config file
		Variant m;
		m.interpret(cfg);

		// Clear the old data
		lighting = LightData();

		// Load the new data
		std::string type;
		if (m.m.find("type") != m.m.end()) {
			type = m.m["type"].s;
		}

		if (type == "ambient") {
			lighting.type = E_LIGHT_TYPE::AMBIENT;
		} else if (type == "diffuse") {
			lighting.type = E_LIGHT_TYPE::DIFFUSE;
		} else if (type == "point") {
			lighting.type = E_LIGHT_TYPE::POINT;
		} else if (type == "spot") {
			lighting.type = E_LIGHT_TYPE::SPOT;
		} else {
			messenger::send({"engine", "light"}, E_MESSAGE::WARNING, "Failed to load Light \"" + name + "\": invalid light type");
			return 2;
		}

		if (m.m.find("position") != m.m.end()) {
			lighting.position = glm::vec4(
				m.m["position"].v[0].d,
				m.m["position"].v[1].d,
				m.m["position"].v[2].d,
				m.m["position"].v[3].d
			);
		}
		if (m.m.find("direction") != m.m.end()) {
			lighting.direction = glm::vec4(
				m.m["direction"].v[0].d,
				m.m["direction"].v[1].d,
				m.m["direction"].v[2].d,
				m.m["direction"].v[3].d
			);
		}
		if (m.m.find("attenuation") != m.m.end()) {
			lighting.attenuation = glm::vec4(
				m.m["attenuation"].v[0].d,
				m.m["attenuation"].v[1].d,
				m.m["attenuation"].v[2].d,
				m.m["attenuation"].v[3].d
			);
		}
		if (m.m.find("color") != m.m.end()) {
			lighting.color = RGBA(
				m.m["color"].v[0].i,
				m.m["color"].v[1].i,
				m.m["color"].v[2].i,
				m.m["color"].v[3].i
			);
		}

		return 0;
	}

	/**
	* Queue the Light for drawing in the Room rendering loop.
	*
	* @retval 0 success
	* @retval 1 failed to queue since the engine is in headless mode
	*/
	int Light::queue() {
		if (get_option("is_headless").i) {
			return 1;
		}

		get_current_room()->add_light(lighting); // Add the Light to the Room lighting queue

		return 0;
	}
}

#endif // BEE_LIGHT
