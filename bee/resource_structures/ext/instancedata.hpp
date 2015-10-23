/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INSTANCEDATA_H
#define _BEE_INSTANCEDATA_H 1

#define ALARM_COUNT 8

#include "../../game.hpp"

class BEE::InstanceData {
		double xstart = 0.0, ystart = 0.0;

		double friction = 0.0;
		double gravity = 0.0, gravity_direction = 180.0;

		Path* path = NULL;
		double path_speed = 0.0;
		int path_end_action = 0;
		int path_current_node = 0;
		bool path_is_drawn = false;
	public:
		BEE* game = NULL;

		int id = -1;
		Object* object = NULL;
		Uint32 subimage_time = 0;
		Uint32 alarm_end[ALARM_COUNT];
		int depth = 0;

		double x = 0.0, y = 0.0;
		double xprevious = 0.0, yprevious = 0.0;
		std::list<std::pair<double,double>> velocity;
		int path_xstart = 0, path_ystart = 0;

		InstanceData();
		InstanceData(BEE*, int, Object*, int, int);
		int init(int, Object*, int, int);
		int print();

		bool operator< (const InstanceData&);

		int remove();

		int set_alarm(int, Uint32);

		int move(double, double);
		int move_to(double, double, double);
		int move_away(double, double, double);
		int set_friction(double);
		int set_gravity(double);
		int set_gravity_direction(double);

		std::pair<double,double> get_motion();
		std::pair<double,double> get_position();
		double get_hspeed();
		double get_vspeed();
		double get_direction();
		double get_speed();
		double get_friction();
		double get_gravity();
		double get_gravity_direction();

		bool is_place_free(int, int);
		bool is_place_empty(int, int);
		bool is_place_meeting(int, int, Object*);
		bool is_place_meeting(int, int, int);
		bool is_snapped(int, int);

		int move_random(int, int);
		int move_snap(int, int);
		int move_wrap(bool, bool, int);

		double get_distance(int, int);
		double get_distance(InstanceData*);
		double get_distance(Object*);

		int path_start(Path*, double, int, bool);
		int path_end();
		int path_update_node();
		int set_path_drawn(bool);
		int handle_path_end();
		bool has_path();
		bool get_path_drawn();
		int get_path_speed();
		int get_path_node();
		std::vector<path_coord> get_path_coords();

		int draw(int, int, double, RGBA);
		int draw(int, int, double, rgba_t);
		int draw();
		int draw(int, int);
		int draw(double);
		int draw(RGBA);
		int draw(rgba_t);
};

#endif // _BEE_INSTANCEDATA_H
