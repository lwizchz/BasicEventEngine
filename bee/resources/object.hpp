/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJECT_H
#define BEE_OBJECT_H 1

#include <string> // Include the required library headers
#include <map>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "../resources.hpp"
#include "../enum.hpp"

#include "../core/sidp.hpp"

namespace bee {
	// Forward declarations
	class Instance;
	class Sprite;

	class Object: public Resource {
			// Add new variables to the print() debugging method
			int id = -1;
			std::string name;
			std::string path;
			Sprite* sprite;
			bool is_solid, is_visible, is_persistent;
			int depth;
			Object* parent;
			Sprite* mask;
			int xoffset, yoffset;
			bool is_pausable;

			std::map<int,Instance*> instances;
		protected:
			std::map<int,std::map<std::string,SIDP>> instance_data;
			std::map<std::string,SIDP>* s;
			Instance* current_instance;

			Object();
			Object(const std::string&, const std::string&);
		public:
			virtual ~Object();
			int add_to_resources();
			int reset();
			int print() const;

			int get_id() const;
			std::string get_name() const;
			std::string get_path() const;
			Sprite* get_sprite() const;
			int get_sprite_id() const;
			bool get_is_solid() const;
			bool get_is_visible() const;
			bool get_is_persistent() const;
			int get_depth() const;
			Object* get_parent() const;
			int get_parent_id() const;
			Sprite* get_mask() const;
			int get_mask_id() const;
			std::pair<int,int> get_mask_offset() const;
			bool get_is_pausable() const;

			int set_name(const std::string&);
			int set_path(const std::string&);
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
			int set_mask_offset(std::pair<int,int>);
			int set_mask_offset(int, int);
			int set_is_pausable(bool);

			int add_instance(int, Instance*);
			int remove_instance(int);
			int clear_instances();
			std::map<int, Instance*> get_instances() const;
			size_t get_instance_amount() const;
			Instance* get_instance(int) const;
			std::string get_instance_string() const;

			SIDP get_data(int, const std::string&, const SIDP&, bool) const;
			SIDP get_data(int, const std::string&) const;
			int set_data(int, const std::string&, SIDP);

			std::map<E_EVENT,bool> implemented_events;
			virtual void update(Instance*) =0;
			virtual void create(Instance*) {};
			virtual void destroy(Instance*) =0;
			virtual void alarm(Instance*, size_t) {};
			virtual void step_begin(Instance*) {};
			virtual void step_mid(Instance*) {};
			virtual void step_end(Instance*) {};
			virtual void keyboard_press(Instance*, SDL_Event*) {};
			virtual void mouse_press(Instance*, SDL_Event*) {};
			virtual void keyboard_input(Instance*, SDL_Event*) {};
			virtual void mouse_input(Instance*, SDL_Event*) {};
			virtual void keyboard_release(Instance*, SDL_Event*) {};
			virtual void mouse_release(Instance*, SDL_Event*) {};
			virtual void controller_axis(Instance*, SDL_Event*) {};
			virtual void controller_press(Instance*, SDL_Event*) {};
			virtual void controller_release(Instance*, SDL_Event*) {};
			virtual void controller_modify(Instance*, SDL_Event*) {};
			virtual void commandline_input(Instance*, const std::string&) {};
			virtual void path_end(Instance*) {};
			virtual void outside_room(Instance*) {};
			virtual void intersect_boundary(Instance*) {};
			virtual void collision(Instance*, Instance*) {};
			virtual bool check_collision_list(const Instance&, const Instance&) const {return true;};
			virtual void draw(Instance*) {};
			virtual void animation_end(Instance*) {};
			virtual void room_start(Instance*) {};
			virtual void room_end(Instance*) {};
			virtual void game_start(Instance*) {};
			virtual void game_end(Instance*) {};
			virtual void window(Instance*, SDL_Event*) {};
	};
}

#endif // BEE_OBJECT_H
