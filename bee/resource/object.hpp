/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJECT_H
#define BEE_OBJECT_H 1

#include <string> // Include the required library headers
#include <map>
#include <set>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "resource.hpp"

#include "../enum.hpp"

#include "../data/variant.hpp"

namespace bee {
	// Forward declarations
	class Instance;
	class Texture;

	struct NetworkEvent;
	struct PathFollower;

	/// Used to handle all events and to store Instance data
	class Object: public Resource {
		static std::map<size_t,Object*> list;
		static size_t next_id;

		size_t id; ///< The unique Object identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the Object's derived header

		Texture* sprite; ///< The Texture to draw for the object
		bool is_persistent; ///< Whether the Object's Instances should persist between Rooms
		int depth; ///< The sorting depth of the Object, lower values are processed first
		Object* parent; ///< The parent of the Object, all parent events can be called before the child's
		std::pair<int,int> draw_offset; ///< How far the sprite should be offset from the Instance position
		bool is_pausable; ///< Whether the Object's events are pausable or not

		std::map<size_t,Instance*> instances; ///< A list of all the Instances of this Object type
	protected:
		Instance* current_instance; ///< A pointer to the Instance that is currently being processed
		std::map<std::string,Variant>* s; ///< A pointer to the data map for the current Instance

		std::set<E_EVENT> implemented_events; ///< A list of all the events that the Object implements

		// See bee/resource/object.cpp for function comments
		Object();
		Object(const std::string&, const std::string&);
	public:
		virtual ~Object();

		static size_t get_amount();
		static Object* get(size_t);
		static Object* get_by_name(const std::string&);
		static Object* add(const std::string&, const std::string&);

		size_t add_to_resources();
		int reset();

		virtual std::map<Variant,Variant> serialize() const;
		virtual int deserialize(std::map<Variant,Variant>&);
		void print() const;

		size_t get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		Texture* get_sprite() const;
		bool get_is_persistent() const;
		int get_depth() const;
		Object* get_parent() const;
		std::pair<int,int> get_mask_offset() const;
		bool get_is_pausable() const;
		const std::set<E_EVENT>& get_events() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
		void set_sprite(Texture*);
		void set_is_persistent(bool);
		void set_depth(int);
		void set_parent(Object*);
		void set_mask_offset(const std::pair<int,int>&);
		void set_is_pausable(bool);

		int add_instance(int, Instance*);
		void remove_instance(int);
		void clear_instances();
		const std::map<size_t,Instance*>& get_instances() const;
		size_t get_instance_amount() const;
		Instance* get_instance_at(int) const;

		virtual void update(Instance*);
		virtual void create(Instance*) =0;
		virtual void destroy(Instance*);
		virtual void alarm(Instance*, const std::string&) {};
		virtual void step_begin(Instance*);
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
		virtual void path_end(Instance*, const PathFollower&) {};
		virtual void outside_room(Instance*) {};
		virtual void intersect_boundary(Instance*) {};
		virtual void collision(Instance*, Instance*) {};
		virtual bool check_collision_filter(const Instance*, const Instance*) const;
		virtual void draw(Instance*) {};
		virtual void animation_end(Instance*) {};
		virtual void room_start(Instance*) {};
		virtual void room_end(Instance*) {};
		virtual void game_start(Instance*) {};
		virtual void game_end(Instance*) {};
		virtual void window(Instance*, SDL_Event*) {};
		virtual void network(Instance*, const NetworkEvent&) {};
	};
}

#endif // BEE_OBJECT_H
