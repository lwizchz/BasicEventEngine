/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PATH_H
#define BEE_PATH_H 1

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include "resource.hpp"

namespace bee {
	typedef std::tuple<double, double, double, double> path_coord_t; // {x, y, z, speed}

	class Path: public Resource { // The path resource class is used to repeatedly move instances in complex, predefined patterns
		static std::map<int,Path*> list;
		static int next_id;

		int id; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the config file which is used to populate the coordinate list
		std::vector<path_coord_t> coordinate_list; // The list of points which the instance will follow, see bee/engine.hpp for the typedef of path_coord_t
		bool is_curved; // Whether the path should be curved by a quadratic Bézier curve
		bool is_closed; // Whether the path should loop after the end
	public:
		// See bee/resources/path.cpp for function comments
		Path();
		Path(const std::string&, const std::string&);
		~Path();

		static size_t get_amount();
		static Path* get(int);
		static Path* get_by_name(const std::string&);
		static Path* add(const std::string&, const std::string&);

		int add_to_resources();
		int reset();
		int print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		std::vector<path_coord_t> get_coordinate_list() const;
		std::string get_coordinate_string() const;
		bool get_is_curved() const;
		bool get_is_closed() const;

		int set_name(const std::string&);
		int set_path(const std::string&);
		int set_is_curved(bool);
		int set_is_closed(bool);
		int add_coordinate(path_coord_t);
		int add_coordinate(double, double, double, double);
		int add_coordinate(double, double, double);
		int remove_last_coordinate();
		int remove_coordinate(unsigned int);

		int draw(double, double, double);
		int draw(double, double);
	};
}

#endif // BEE_PATH_H
