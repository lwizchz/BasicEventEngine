/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INSTANCEDATA_H
#define _BEE_INSTANCEDATA_H 1

#define ALARM_COUNT 8

#ifndef _BEE_ROOM_H
#define _BEE_ROOM_H
#define _BEE_ROOM_OPERATOR 1
#endif // _BEE_ROOM

#include "../../game.hpp"

class BEE::InstanceData {
		double xstart = 0.0, ystart = 0.0;

		double friction = 0.0;
		double gravity = 0.0, gravity_direction = 180.0;
		double acceleration = 0.0, acceleration_amount = 0.0;

		Path* path = NULL;
		double path_speed = 0.0;
		int path_end_action = 0;
		int path_current_node = 0;
		bool path_is_drawn = false;
		bool path_is_pausable = false;
	public:
		BEE* game = NULL;

		int id = -1;
		Object* object = NULL;
		Uint32 subimage_time = 0;
		Uint32 alarm_end[ALARM_COUNT];
		int depth = 0;

		double x = 0.0, y = 0.0;
		double xprevious = 0.0, yprevious = 0.0;
		std::list<std::pair<double,double>> velocity, old_velocity;
		int path_xstart = 0, path_ystart = 0;

		CollisionPolygon mask;

		InstanceData();
		InstanceData(BEE*, int, Object*, int, int);
		int init(int, Object*, int, int);
		int print();

		bool operator< (const InstanceData& other) const;

		int remove();

		int set_alarm(int, Uint32);

		int set_object(Object*);

		double get_xstart();
		double get_ystart();
		int get_width();
		int get_height();
		double get_center_x();
		double get_center_y();

		int move(double, double);
		int move_to(double, double, double);
		int move_away(double, double, double);
		int set_friction(double);
		int set_gravity(double);
		int set_gravity_direction(double);
		int set_gravity_acceleration(double);
		int reset_gravity_acceleration();
		bool check_collision_polygon(const CollisionPolygon&, const CollisionPolygon&);
		bool check_collision_polygon(const CollisionPolygon&);
		std::pair<double,double> move_outside_polygon(const Line&, CollisionPolygon*, const CollisionPolygon&);
		int move_avoid(const CollisionPolygon&);
		int move_avoid(const SDL_Rect&);

		std::pair<double,double> get_motion();
		std::pair<double,double> get_applied_gravity();
		std::pair<double,double> get_position();
		double get_hspeed();
		double get_vspeed();
		double get_direction();
		double get_speed();
		double get_friction();
		double get_gravity();
		double get_gravity_direction();
		double get_gravity_acceleration();
		double get_gravity_acceleration_amount();

		bool is_place_free(int, int);
		bool is_place_empty(int, int);
		bool is_place_meeting(int, int, Object*);
		bool is_place_meeting(int, int, int);
		bool is_move_free(double, double);
		bool is_snapped(int, int);

		int move_random(int, int);
		std::pair<int,int> get_snapped(int, int);
		std::pair<int,int> get_snapped();
		int move_snap(int, int);
		int move_snap();
		int move_wrap(bool, bool, int);

		double get_distance(int, int);
		double get_distance(InstanceData*);
		double get_distance(Object*);
		double get_direction(int, int);
		double get_direction(InstanceData*);
		double get_direction(Object*);

		int path_start(Path*, double, int, bool);
		int path_end();
		int path_update_node();
		int set_path_drawn(bool);
		int set_path_pausable(bool);
		int handle_path_end();
		bool has_path();
		bool get_path_drawn();
		int get_path_speed();
		int get_path_node();
		std::vector<path_coord> get_path_coords();
		bool get_path_pausable();

		int draw(int, int, double, RGBA, SDL_RendererFlip);
		int draw(int, int, double, rgba_t, SDL_RendererFlip);
		int draw();
		int draw(int, int);
		int draw(double);
		int draw(RGBA);
		int draw(rgba_t);
		int draw(SDL_RendererFlip);

		int draw_path();

		int draw_debug();
};

#ifdef _BEE_ROOM_OPERATOR
#undef _BEE_ROOM_H
#undef _BEE_ROOM_OPERATOR
#include "../room.hpp"
#endif // _BEE_ROOM_OPERATOR

#endif // _BEE_INSTANCEDATA_H
