/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_INSTANCE_H
#define BEE_CORE_INSTANCE_H 1

#include <functional>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <btBulletDynamicsCommon.h>

#include "../defines.hpp"
#include "../enum.hpp"

#include "../data/sidp.hpp"

#include "../render/rgba.hpp"

#include "../resource/path.hpp"

namespace bee {
	// Forward declarations
	class Texture;
	class Object;
	class PhysicsBody;

	class Instance {
			btVector3 pos_start;

			Object* object;
			Texture* sprite;

			PhysicsBody* body;
			bool is_solid;
			E_COMPUTATION computation_type;
			bool is_persistent;

			Path* path;
			double path_speed;
			E_PATH_END path_end_action;
			int path_current_node;
			bool path_is_drawn;
			bool path_is_pausable;
			double path_previous_mass;

			std::map<std::string,SIDP> data;
		public:
			int id;
			Uint32 subimage_time;
			Uint32 alarm_end[BEE_ALARM_COUNT];
			int depth;

			btVector3 pos_previous;
			btVector3 path_pos_start;

			Instance();
			Instance(int, Object*, double, double, double);
			Instance(const Instance&);
			virtual ~Instance();
			int init(int, Object*, double, double, double);
			int print();

			bool operator<(const Instance&) const;
			Instance& operator=(const Instance&);

			std::string serialize(bool) const;
			std::string serialize() const;
			int deserialize(std::map<SIDP,SIDP>&, Object*);
			int deserialize(const std::string&, Object*);
			int deserialize(const std::string&);

			std::vector<Uint8> serialize_net();
			int deserialize_net(std::vector<Uint8>);

			int remove();

			int set_alarm(size_t, Uint32);

			int set_object(Object*);
			int set_sprite(Texture*);
			int add_physbody();
			int set_computation_type(E_COMPUTATION);
			int set_is_persistent(bool);

			std::map<std::string,SIDP>& get_data();
			const SIDP& get_data(const std::string&, const SIDP&, bool) const;
			SIDP get_data(const std::string&) const;
			int set_data(const std::map<std::string,SIDP>&);
			int set_data(const std::string&, const SIDP&);

			btVector3 get_position() const;
			double get_x() const;
			double get_y() const;
			double get_z() const;
			double get_corner_x() const;
			double get_corner_y() const;
			btVector3 get_start() const;
			double get_xstart() const;
			double get_ystart() const;
			double get_zstart() const;

			Object* get_object() const;
			Texture* get_sprite() const;
			PhysicsBody* get_physbody() const;
			bool get_is_solid() const;
			double get_mass() const;
			E_COMPUTATION get_computation_type() const;
			bool get_is_persistent() const;

			int get_width() const;
			int get_height() const;
			SDL_Rect get_aabb() const;

			int set_position(btVector3);
			int set_position(double, double, double);
			int set_to_start();
			int set_corner_x(double);
			int set_corner_y(double);
			int set_mass(double);
			int move(btVector3);
			int move(double, btVector3);
			int move(double, double);
			int move_to(double, double, double, double);
			int move_to(double, double, double);
			int move_away(double, double, double, double);
			int move_away(double, double, double);
			int set_friction(double);
			int set_gravity(btVector3);
			int set_gravity(double, double, double);
			int move_outside(btVector3);
			int set_is_solid(bool);
			int set_velocity(btVector3);
			int set_velocity(double, double);
			int add_velocity(btVector3);
			int add_velocity(double, double);
			int limit_velocity(double);
			int limit_velocity_x(double);
			int limit_velocity_y(double);
			int limit_velocity_z(double);

			double get_speed() const;
			btVector3 get_velocity() const;
			btVector3 get_velocity_ang() const;
			double get_friction() const;
			btVector3 get_gravity() const;

			bool is_place_free(int, int) const;
			bool is_place_empty(int, int) const;
			bool is_place_meeting(int, int, Object*) const;
			bool is_place_meeting(int, int, int) const;
			bool is_place_meeting(int, int, Object*, std::function<void(Instance*,Instance*)>);
			bool is_move_free(double, double);
			bool is_snapped(int, int) const;

			std::pair<int,int> get_snapped(int, int) const;
			std::pair<int,int> get_snapped() const;
			int move_random(int, int);
			int move_snap(int, int);
			int move_snap();
			int move_wrap(bool, bool, int);

			double get_distance(int, int, int) const;
			double get_distance(Instance*) const;
			double get_distance(Object*) const;
			double get_direction_of(int, int) const;
			double get_direction_of(Instance*) const;
			double get_direction_of(Object*) const;
			int get_relation(Instance*) const;

			int path_start(Path*, double, E_PATH_END, bool);
			int path_end();
			int path_reset();
			int path_update_node();
			int set_path_drawn(bool);
			int set_path_pausable(bool);
			int handle_path_end();
			bool has_path();
			bool get_path_drawn();
			double get_path_speed();
			int get_path_node();
			std::vector<path_coord_t> get_path_coords();
			bool get_path_pausable();

			int draw(int, int, double, RGBA);
			int draw();

			int draw_path();
	};
}

#endif // BEE_CORE_INSTANCE_H
