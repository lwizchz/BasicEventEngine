/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
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

#include "../game.hpp"

template <typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B>& p) {
	return std::pair<B,A>(p.second, p.first);
}
template std::pair<BEE::InstanceData*,int> flip_pair<int,BEE::InstanceData*>(const std::pair<int,BEE::InstanceData*>&);

class BEE::Room: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string room_path;
		int width, height;
		bool is_isometric;
		int speed;
		bool is_persistent;

		RGBA background_color;
		bool is_background_color_enabled;
		std::map<int,BackgroundData*> backgrounds;
		bool is_views_enabled;
		std::map<int,ViewData*> views;
		std::map<int,InstanceData*> instances;
		std::map<InstanceData*,int> instances_sorted;
		std::map<int,ParticleSystem*> particles;
		int particle_count = 0;
	public:
		Room();
		Room(std::string, std::string);
		~Room();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		bool get_is_isometric();
		int get_speed();
		bool get_is_persistent();
		RGBA get_background_color();
		bool get_is_background_color_enabled();
		std::map<int,BackgroundData*> get_backgrounds();
		std::string get_background_string();
		bool get_is_views_enabled();
		std::map<int,ViewData*> get_views();
		std::string get_view_string();
		std::map<int,InstanceData*> get_instances();
		std::string get_instance_string();

		int set_name(std::string);
		int set_path(std::string);
		int set_width(int);
		int set_height(int);
		int set_is_isometric(bool);
		int set_speed(int);
		int set_is_persistent(bool);
		int set_background_color(RGBA);
		int set_background_color(Uint8, Uint8, Uint8);
		int set_is_background_color_enabled(bool);
		int set_background(int, BackgroundData*);
		int add_background(int, Background*, bool, bool, int, int, bool, bool, int, int, bool);
		int set_is_views_enabled(bool);
		int set_view(int, ViewData*);
		int set_instance(int, InstanceData*);
		int add_instance(int, Object*, int, int);
		int remove_instance(int);
		int sort_instances();
		int add_particle_system(ParticleSystem*);
		int add_particle(ParticleSystem*, Particle*, int, int);
		int clear_particles();

		int load_media();
		int free_media();
		int reset_properties();

		int create();
		int destroy();
		int check_alarms();
		int step_begin();
		int step_mid();
		int step_end();
		int keyboard(SDL_Event*);
		int mouse(SDL_Event*);
		int keyboard_press(SDL_Event*);
		int mouse_press(SDL_Event*);
		int keyboard_release(SDL_Event*);
		int mouse_release(SDL_Event*);
		int check_paths();
		int outside_room();
		int intersect_boundary();
		int collision();
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
