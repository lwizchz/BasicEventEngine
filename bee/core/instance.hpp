/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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

#include "../data/variant.hpp"

#include "../render/rgba.hpp"

#include "../resource/path.hpp"

namespace bee {
	// Forward declarations
	class Texture;
	class Object;
	class PhysicsBody;

 	/// Used to hold Object instantiation data
	class Instance {
		btVector3 pos_start; ///< The starting position

		Object* object; ///< The Object type
		Texture* sprite; ///< The sprite to draw

		PhysicsBody* body; ///< The associated PhysicsBody
		E_COMPUTATION computation_type; ///< Determines the desired amount of processing
		bool is_persistent; ///< Whether to persist between Rooms

		Path* path;
		double path_speed;
		E_PATH_END path_end_action;
		int path_current_node;
		bool path_is_drawn;
		bool path_is_pausable;
		double path_previous_mass;

		std::map<std::string,Variant> data; ///< The Instance-specific data
	public:
		int id; ///< Semi-unique identifier, old IDs may be reused
		Uint32 subimage_time; ///< The timestamp of the animation start
		int depth; ///< Determines the order of event processing, higher depths first

		btVector3 pos_previous; ///< The previous position, currently unimplemented
		btVector3 path_pos_start; ///< The offset to the Path starting position

		// See bee/core/instance.cpp for function comments
		Instance();
		Instance(int, Object*, double, double, double);
		Instance(const Instance&);
		virtual ~Instance();
		void init(int, Object*, double, double, double);

		bool operator<(const Instance&) const;
		Instance& operator=(const Instance&);

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		int print();

		std::vector<Uint8> serialize_net();
		int deserialize_net(std::vector<Uint8>);

		void set_alarm(const std::string&, int);

		void set_object(Object*);
		void set_sprite(Texture*);
		void add_physbody();
		void set_computation_type(E_COMPUTATION);
		void set_is_persistent(bool);

		std::map<std::string,Variant>& get_data();
		const Variant& get_data(const std::string&, const Variant&, bool) const;
		Variant get_data(const std::string&) const;
		void set_data(const std::map<std::string,Variant>&);
		void set_data(const std::string&, Variant);

		template <typename T>
		void set_data(const std::string&, T);

		btVector3 get_pos() const;
		double get_x() const;
		double get_y() const;
		double get_z() const;
		SDL_Rect get_aabb() const;
		std::pair<double,double> get_corner() const;
		btVector3 get_start() const;

		Object* get_object() const;
		Texture* get_sprite() const;
		PhysicsBody* get_physbody() const;
		bool get_is_solid() const;
		double get_mass() const;
		E_COMPUTATION get_computation_type() const;
		bool get_is_persistent() const;

		void set_pos(btVector3);
		void set_pos(double, double, double);
		void set_to_start();
		void set_corner(double, double);
		void set_mass(double);
		void move(btVector3);
		void move(double, btVector3);
		void move(double, double);
		void move_to(double, btVector3);
		void move_away(double, btVector3);
		void set_friction(double);
		void set_gravity(btVector3);
		void set_is_solid(bool);
		void set_velocity(btVector3);
		void set_velocity(double, double);
		void add_velocity(btVector3);
		void add_velocity(double, double);
		btVector3 limit_velocity(btVector3);
		int limit_velocity(double);

		double get_speed() const;
		btVector3 get_velocity() const;
		btVector3 get_velocity_ang() const;
		double get_friction() const;
		btVector3 get_gravity() const;

		bool is_place_free(int, int) const;
		bool is_place_empty(int, int) const;
		bool is_place_meeting(int, int, Object*) const;
		bool is_place_meeting(int, int, Object*, std::function<void(Instance*, Instance*)>);
		bool is_move_free(double, double);
		bool is_snapped(int, int) const;

		std::pair<int,int> get_snapped(int, int) const;
		std::pair<int,int> get_snapped() const;
		void move_random(int, int);
		void move_snap(int, int);
		void move_snap();
		void move_wrap(bool, bool, int);

		double get_distance(btVector3) const;
		double get_distance(Instance*) const;
		double get_distance(Object*) const;
		btVector3 get_direction_of(btVector3) const;
		btVector3 get_direction_of(Instance*) const;
		btVector3 get_direction_of(Object*) const;
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

	template <typename T>
	void Instance::set_data(const std::string& field, T value) {
		set_data(field, Variant(value));
	}
}

#endif // BEE_CORE_INSTANCE_H
