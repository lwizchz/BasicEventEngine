/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PATH_H
#define _BEE_PATH_H 1

#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

#include "../game.hpp"

class BEE::Path: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string path;
		std::vector<bee_path_coord> coordinate_list; // [x, y, speed]
		int connection_type; // Straight vs smooth curve
		bool is_closed;
	public:
		Path();
		Path(std::string, std::string);
		~Path();
		int add_to_resources();
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		std::vector<bee_path_coord> get_coordinate_list();
		std::string get_coordinate_string();
		int get_connection_type();
		bool get_is_closed();

		int set_name(std::string);
		int set_path(std::string);
		int add_coordinate(bee_path_coord);
		int add_coordinate(int, int, double);
		int remove_last_coordinate();
		int remove_coordinate(unsigned int);
		int set_connection_type(int);
		int set_is_closed(bool);

		int draw(int, int);
};

#endif // _BEE_PATH_H
