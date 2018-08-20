/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ROOM_H
#define BEE_ROOM_H 1

#include <string> // Include the required library headers
#include <map>
#include <list>
#include <vector>
#include <memory>

#include <btBulletDynamicsCommon.h> // Include the required Bullet headers

#include "resource.hpp"

#include "../data/variant.hpp"

#include "../render/rgba.hpp"

namespace bee {
	// Forward declarations
	class Object;
	class Instance;

	struct Background;
	struct ViewPort;
	class PhysicsWorld;
	class PhysicsBody;

	struct NetworkEvent;
	struct PathFollower;
	struct TimelineIterator;

	/// Used as the comparator for the Room::instances_sorted_events map
	struct InstanceSort {
		// See bee/resource/room.cpp for function comments
		bool operator()(Instance* lhs, Instance* rhs) const;
	};

	/// Used to handle Instance event calls and initialization
	class Room: public Resource {
		static std::map<size_t,Room*> list;
		static size_t next_id;

		size_t id; ///< The unique Room identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the Room's derived header

		int width; ///< The width of the Room
		int height; ///< The height of the Room
		bool is_persistent; ///< Whether the Room's Instances should persist to the next Room

		std::map<std::string,Background> backgrounds; ///< The map of named Backgrounds that should be drawn
		std::map<std::string,ViewPort> viewports; ///< The map of named ViewPorts that should be drawn
		std::pair<const std::string,ViewPort>* viewport_current; ///< A pointer to the ViewPort that is currently being drawn

		size_t next_instance_id; ///< The always increasing identifier for the next created Instance
		std::map<size_t,Instance*> instances; ///< A map of all Instances with their associated ID
		std::vector<Instance*> created_instances; ///< A list of Instances that should have their create event called during the next frame
		std::vector<Instance*> destroyed_instances; ///< A list of Instances that should have their destroy event called after the event loop
		std::map<E_EVENT,std::map<Instance*,size_t,InstanceSort>> instances_events; ///< A map of all events and the Instances which implement those events

		std::shared_ptr<PhysicsWorld> physics_world; ///< The PhysicsWorld used to simulate all PhysicsBodys in the Room

		std::map<Instance*,PathFollower> automatic_paths; ///< A map of the Instance Paths to update every step
		std::vector<TimelineIterator> automatic_timelines; ///< A map of the Timelines to run every step

		// See bee/resource/room.cpp for function comments
		void set_instance(size_t, Instance*);
		int remove_instance_internal(size_t);
	protected:
		Room();
		Room(const std::string&, const std::string&);
	public:
		virtual ~Room();

		static size_t get_amount();
		static Room* get(size_t);
		static Room* get_by_name(const std::string&);
		static Room* add(const std::string&, const std::string&);

		size_t add_to_resources();
		int reset();

		virtual std::map<Variant,Variant> serialize() const;
		virtual int deserialize(std::map<Variant,Variant>&);
		void print() const;

		size_t get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		int get_width() const;
		int get_height() const;
		bool get_is_persistent() const;
		const std::map<std::string,Background>& get_backgrounds() const;
		const std::map<std::string,ViewPort>& get_viewports() const;
		std::pair<const std::string,ViewPort>* get_current_viewport() const;
		const std::map<size_t,Instance*>& get_instances() const;
		std::shared_ptr<PhysicsWorld> get_phys_world() const;
		const std::map<Instance*,PathFollower>& get_paths() const;
		const std::vector<TimelineIterator>& get_timelines() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
		void set_width(int);
		void set_height(int);
		void set_is_persistent(bool);

		int add_background(const std::string&, Background);
		void remove_background(const std::string&);
		int add_viewport(const std::string&, ViewPort);
		void remove_viewport(const std::string&);
		Instance* add_instance(Object*, btVector3);
		int remove_instance(size_t);
		void automate_path(Instance*, PathFollower);
		void automate_timeline(TimelineIterator);

		void reset_properties();
		int transfer_instances(Room*);

		void create();
		void destroy();
		void destroy(Instance*);
		void destroy_all(Object*);
		void check_alarms();
		void step_begin();
		void step_mid();
		void step_end();
		void keyboard_press(SDL_Event*);
		void mouse_press(SDL_Event*);
		void keyboard_input(SDL_Event*);
		void mouse_input(SDL_Event*);
		void keyboard_release(SDL_Event*);
		void mouse_release(SDL_Event*);
		void controller_axis(SDL_Event*);
		void controller_press(SDL_Event*);
		void controller_release(SDL_Event*);
		void controller_modify(SDL_Event*);
		void commandline_input(const std::string&);
		void check_paths();
		void outside_room();
		void intersect_boundary();
		int collision();
		static void collision_internal(btDynamicsWorld*, btScalar);
		static bool check_collision_filter(btBroadphaseProxy*, btBroadphaseProxy*);
		void draw();
		void draw_view(ViewPort*);
		void animation_end();
		void room_start();
		void room_end();
		void game_start();
		void game_end();
		void window(SDL_Event*);
		void network(const NetworkEvent&);

		virtual void init();
		virtual void start();
		virtual void end();
	};
}

#endif // BEE_ROOM_H
