/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_ROOM_H
#define _BEE_ROOM_H 1

#include <iostream>
#include <map>
#include <tuple>
#include <algorithm>
#include <fstream>

#include <btBulletDynamicsCommon.h>

#include "../game.hpp"

template <typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B>& p) {
	return std::pair<B,A>(p.second, p.first);
}
template std::pair<BEE::InstanceData*,int> flip_pair<int,BEE::InstanceData*>(const std::pair<int,BEE::InstanceData*>&);

struct InstanceDataSort {
	bool operator() (BEE::InstanceData* lhs, BEE::InstanceData* rhs) {
		return (*lhs) < (*rhs);
	}
};

class BEE::Room: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string path;
		int width, height;
		bool is_isometric;
		bool is_persistent;

		RGBA background_color;
		bool is_background_color_enabled;
		std::map<int,BackgroundData*> backgrounds;
		bool is_views_enabled;
		std::map<int,ViewData*> views;

		std::map<int,InstanceData*> instances;
		std::map<InstanceData*,int,InstanceDataSort> instances_sorted;
		std::vector<InstanceData*> created_instances;
		std::vector<InstanceData*> destroyed_instances;
		bool should_sort = false;

		std::map<bee_event_t,std::map<InstanceData*,int,InstanceDataSort>> instances_sorted_events;

		std::map<int,ParticleSystem*> particles;
		int particle_count = 0;
		int next_instance_id = 0;

		std::vector<LightData> lights;
		std::vector<LightableData*> lightables;
		Sprite* light_map = nullptr;

		PhysicsWorld* physics_world = nullptr;
		std::map<const btRigidBody*,InstanceData*> physics_instances;

		std::string instance_map = "";

		Sprite* view_texture = nullptr;
		ViewData* view_current = nullptr;
	public:
		Room();
		Room(const std::string&, const std::string&);
		~Room();
		int add_to_resources();
		int reset();
		int print() const;
		std::string get_print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		int get_width() const;
		int get_height() const;
		bool get_is_isometric() const;
		bool get_is_persistent() const;
		RGBA get_background_color() const;
		bool get_is_background_color_enabled() const;
		std::map<int,BackgroundData*> get_backgrounds() const;
		std::string get_background_string() const;
		bool get_is_views_enabled() const;
		std::map<int,ViewData*> get_views() const;
		std::string get_view_string() const;
		const std::map<int,InstanceData*>& get_instances() const;
		std::string get_instance_string() const;
		ViewData* get_current_view() const;
		PhysicsWorld* get_phys_world() const;
		const std::map<const btRigidBody*,InstanceData*>& get_phys_instances() const;

		int set_name(const std::string&);
		int set_path(const std::string&);
		int set_width(int);
		int set_height(int);
		int set_is_isometric(bool);
		int set_is_persistent(bool);
		int set_background_color(RGBA);
		int set_background_color(Uint8, Uint8, Uint8);
		int set_background_color(bee_rgba_t);
		int set_is_background_color_enabled(bool);
		int set_background(int, BackgroundData*);
		int add_background(int, Background*, bool, bool, int, int, bool, bool, int, int, bool);
		int set_is_views_enabled(bool);
		int set_view(int, ViewData*);
		int set_instance(int, InstanceData*);
		InstanceData* add_instance(int, Object*, double, double, double);
		int add_instance_grid(int, Object*, double, double, double);
		int remove_instance(int);
		int sort_instances();
		int request_instance_sort();
		int add_physbody(InstanceData*, PhysicsBody*);
		int remove_physbody(PhysicsBody*);
		int add_particle_system(ParticleSystem*);
		int add_particle(ParticleSystem*, Particle*, int, int);
		int clear_particles();
		int add_lightable(LightableData*);
		int add_light(LightData);
		int handle_lights();
		int reset_lights();

		int load_media();
		int free_media();
		int reset_properties();

		int save_instance_map(std::string);
		int load_instance_map(std::string);
		int load_instance_map();
		std::string get_instance_map() const;
		int set_instance_map(std::string);

		int create();
		int destroy();
		int destroy(InstanceData*);
		int destroy_all(Object*);
		int check_alarms();
		int step_begin();
		int step_mid();
		int step_end();
		int keyboard_press(SDL_Event*);
		int mouse_press(SDL_Event*);
		int keyboard_input(SDL_Event*);
		int mouse_input(SDL_Event*);
		int keyboard_release(SDL_Event*);
		int mouse_release(SDL_Event*);
		int controller_axis(SDL_Event*);
		int controller_press(SDL_Event*);
		int controller_release(SDL_Event*);
		int controller_modify(SDL_Event*);
		int commandline_input(const std::string&);
		int check_paths();
		int outside_room();
		int intersect_boundary();
		int collision();
		static void collision_internal(btDynamicsWorld*, btScalar);
		static void check_collision_lists(btBroadphasePair&, btCollisionDispatcher&, const btDispatcherInfo&);
		int draw();
		int draw_view();
		int animation_end();
		int room_start();
		int room_end();
		int game_start();
		int game_end();
		int window(SDL_Event*);

		virtual int init() =0;
};

#endif // _BEE_ROOM_H
