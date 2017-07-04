/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PATH
#define BEE_PATH 1

#include <sstream> // Include the required library headers

#include "path.hpp" // Include the class resource header

#include "../debug.hpp"
#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"

#include "../core/messenger/messenger.hpp"

#include "../render/drawing.hpp"
#include "../render/rgba.hpp"

namespace bee {
	/*
	* Path::Path() - Default construct the path
	* ! This constructor should only be directly used for temporary paths, the other constructor should be used for all other cases
	*/
	Path::Path () :
		id(-1),
		name(),
		path(),
		coordinate_list(),
		is_curved(false),
		is_closed(true)
	{}
	/*
	* Path::Path() - Construct the path, add it to the path resource list, and set the new name and path
	* @new_name: the name of the path to use
	* @new_path: the path filename of the path's config file
	*/
	Path::Path (const std::string& new_name, const std::string& new_path) :
		Path() // Default initialize all variables
	{
		add_to_resources(); // Add the path to the appropriate resource list
		if (id < 0) { // If the path could not be added to the resource list, output a warning
			messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add path resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the path name
		set_path(new_path); // Set the config file path
	}
	/*
	* Path::~Path() - Remove the path from the resource list
	*/
	Path::~Path() {
		resource_list->paths.remove_resource(id);
	}
	/*
	* Path::add_to_resources() - Add the path to the appropriate resource list
	*/
	int Path::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = resource_list->paths.add_resource(this); // Add the resource and get the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Path::reset() - Reset all resource variables for reinitialization
	*/
	int Path::reset() {
		// Reset all properties
		name = "";
		path = "";
		coordinate_list.clear();
		is_curved = false;
		is_closed = true;

		return 0; // Return 0 on success
	}
	/*
	* Path::print() - Print all relevant information about the resource
	*/
	int Path::print() const {
		std::string coordinate_string = get_coordinate_string(); // Get the list of coordinates in string form

		std::stringstream s; // Declare the output stream
		s << // Append all info to the ouput
		"Path { "
		"\n	id              " << id <<
		"\n	name            " << name <<
		"\n	path            " << path <<
		"\n	coordinate_list\n" << debug_indent(coordinate_string, 2) <<
		"	is_curved	" << is_curved <<
		"\n	is_closed       " << is_closed <<
		"\n}\n";
		messenger_send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Path::get_*() - Return the requested resource information
	*/
	int Path::get_id() const {
		return id;
	}
	std::string Path::get_name() const {
		return name;
	}
	std::string Path::get_path() const {
		return path;
	}
	std::vector<path_coord_t> Path::get_coordinate_list() const {
		return coordinate_list;
	}
	std::string Path::get_coordinate_string() const {
		if (coordinate_list.empty()) { // If there are no coordinates in the list, return a none-string
			return "none\n";
		}

		std::vector<std::vector<std::string>> table; // Declare a table to hold the coordinates
		table.push_back({"(x", "y", "z", "speed)"}); // Append the table header

		for (auto it = coordinate_list.begin(); it != coordinate_list.end(); ++it) { // Iterate over the coordinates and add each of them to the table
			table.push_back({bee_itos(std::get<0>(*it)), bee_itos(std::get<1>(*it)), bee_itos(std::get<2>(*it)), bee_itos(std::get<3>(*it))});
		}

		return string_tabulate(table); // Return the table as a properly spaced string
	}
	bool Path::get_is_curved() const {
		return is_curved;
	}
	bool Path::get_is_closed() const {
		return is_closed;
	}

	/*
	* Path::set_*() - Set the requested resource data
	*/
	int Path::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Path::set_path(const std::string& new_path) {
		path = "resources/paths/"+new_path; // Append the filename to the path directory
		return 0;
	}
	int Path::set_is_curved(bool new_is_curved) {
		is_curved = new_is_curved;
		return 0;
	}
	int Path::set_is_closed(bool new_is_closed) {
		is_closed = new_is_closed;
		return 0;
	}
	int Path::add_coordinate(path_coord_t new_coordinate) {
		coordinate_list.push_back(new_coordinate);
		return 0;
	}
	int Path::add_coordinate(double x, double y, double z, double speed) {
		return add_coordinate(path_coord_t(x, y, z, speed)); // Construct the path coordinate and return the attempt to add it
	}
	int Path::add_coordinate(double x, double y, double speed) {
		return add_coordinate(x, y, 0.0, speed); // Return the attempt to add a coordinate with z=0.0
	}
	int Path::remove_last_coordinate() {
		if (coordinate_list.empty()) {
			return 1; // Return 1 when the list is empty
		}

		coordinate_list.pop_back(); // Remove the element

		return 0; // Return 0 on success
	}
	int Path::remove_coordinate(unsigned int index) {
		if (index >= coordinate_list.size()) {
			return 1; // Return 1 if the desired index is not in the list
		}

		coordinate_list.erase(coordinate_list.begin()+index); // Erase the element at the given index

		return 0; // Return 0 on success
	}

	/*
	* Path::draw() - Draw the path for debugging purposes
	* @xstart: the x-coordinate to offset the path by
	* @ystart: the y-coordinate to offset the path by
	* @zstart: the z-coordinate to offset the path by
	*/
	int Path::draw(double xstart, double ystart, double zstart) {
		RGBA c_line = get_enum_color(E_RGB::AQUA); // Define the drawing color
		glm::vec3 vs (xstart, ystart, zstart); // Define the start point
		glm::vec3 v1, v2; // Declare two vectors to use for the start and end points of each line

		for (auto it = coordinate_list.begin(); it != --coordinate_list.end(); ) { // Iterate over the coordinates
			v1 = glm::vec3(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it)); // Get the start point from the current coordinate
			++it; // Move to the next coordinate
			v2 = glm::vec3(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it)); // Get the end point from the next coordinate

			draw_line(v1+vs, v2+vs, c_line); // Draw the line with the start point added
		}

		return 0; // Return 0 on success
	}
	/*
	* Path::draw() - Draw the path for debugging purposes
	* ! When the zstart is not provided, simply draw it with it set to 0.0
	* @xstart: the x-coordinate to offset the path by
	* @ystart: the y-coordinate to offset the path by
	*/
	int Path::draw(double xstart, double ystart) {
		return draw(xstart, ystart, 0.0); // Return the draw call with no zstart
	}
}

#endif // BEE_PATH
