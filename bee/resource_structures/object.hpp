/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_OBJECT_H
#define _BEE_OBJECT_H 1

#include <string>

#include "../game.hpp"

class BEE::Object: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string object_path;
		Sprite *sprite;
		bool is_solid, is_visible, is_persistent;
		int depth;
		Object *parent;
		Sprite *mask;

		std::map<int,InstanceData*> instances;
	public:
		Object();
		Object(std::string, std::string);
		~Object();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		Sprite* get_sprite();
		int get_sprite_id();
		bool get_is_solid();
		bool get_is_visible();
		bool get_is_persistent();
		int get_depth();
		Object* get_parent();
		int get_parent_id();
		Sprite* get_mask();
		int get_mask_id();

		int set_name(std::string);
		int set_path(std::string);
		int set_sprite(Sprite*);
		int set_sprite_id(int);
		int set_is_solid(bool);
		int set_is_visible(bool);
		int set_is_persistent(bool);
		int set_depth(int);
		int set_parent(Object*);
		int set_parent_id(int);
		int set_mask(Sprite*);
		int set_mask_id(int);

		int add_instance(int, InstanceData*);
		int remove_instance(int);
		int clear_instances();
		std::map<int, InstanceData*> get_instances();
		InstanceData* get_instance(int);
		std::string get_instance_string();

		virtual void update(InstanceData*) {};
		virtual void create(InstanceData*) {};
		virtual void destroy(InstanceData*) {};
		virtual void alarm(InstanceData*, int) {};
		virtual void step_begin(InstanceData*) {};
		virtual void step_mid(InstanceData*) {};
		virtual void step_end(InstanceData*) {};
		virtual void keyboard(InstanceData*) {};
		virtual void mouse(InstanceData*) {};
		virtual void keyboard_press(InstanceData*, SDL_Event*) {};
		virtual void mouse_press(InstanceData*, SDL_Event*) {};
		virtual void keyboard_input(InstanceData*, SDL_Event*) {};
		virtual void mouse_input(InstanceData*, SDL_Event*) {};
		virtual void keyboard_release(InstanceData*, SDL_Event*) {};
		virtual void mouse_release(InstanceData*, SDL_Event*) {};
		virtual void path_end(InstanceData*) {};
		virtual void outside_room(InstanceData*) {};
		virtual void intersect_boundary(InstanceData*) {};
		virtual void collision(InstanceData*, InstanceData*) {};
		virtual void draw(InstanceData*) {};
		virtual void animation_end(InstanceData*) {};
		virtual void room_start(InstanceData*) {};
		virtual void room_end(InstanceData*) {};
		virtual void game_start(InstanceData*) {};
		virtual void game_end(InstanceData*) {};
		virtual void window(InstanceData*, SDL_Event*) {};
};

#endif // _BEE_OBJECT_H
