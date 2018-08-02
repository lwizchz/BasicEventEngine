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

	/// Used as the comparator for the Room::instances_sorted map
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

		std::vector<Background> backgrounds; ///< The list of Backgrounds that should be drawn
		std::vector<ViewPort> views; ///< The list of ViewPorts that should be drawn

		int next_instance_id; ///< The always increasing identifier for the next created Instance
		std::map<int,Instance*> instances; ///< A map of all Instances with their associated ID
		std::map<Instance*,int,InstanceSort> instances_sorted; ///< A map of all Instances sorted by depth, then by ID
		std::vector<Instance*> created_instances; ///< A list of Instances that should have their create event called during the next frame
		std::vector<Instance*> destroyed_instances; ///< A list of Instances that should have their destroy event called after the event loop
		bool should_sort; ///< Whether the sorted Instance list needs to be resorted after the event loop

		static const std::list<E_EVENT> event_list; ///< A list of the available events
		std::map<E_EVENT,std::map<Instance*,int,InstanceSort>> instances_sorted_events; ///< A map of all events and the Instances which implement those events

		PhysicsWorld* physics_world; ///< The PhysicsWorld used to simulate all PhysicsBodys in the Room
		std::map<const btRigidBody*,Instance*> physics_instances; ///< A map of the btRigidBodys in the world with their associated Instance

		ViewPort* view_current; ///< A pointer to the ViewPort that is currently being drawn

		std::map<Instance*,PathFollower> automatic_paths; ///< A map of the Instance Paths to update every step
		std::vector<TimelineIterator> automatic_timelines; ///< A map of the Timelines to run every step
	protected:
		// See bee/resource/room.cpp for function comments
		Room();
		Room(const std::string&, const std::string&);
	public:
		virtual ~Room();

		static size_t get_amount();
		static Room* get(size_t);
		static Room* get_by_name(const std::string&);
		static Room* add(const std::string&, const std::string&);

		int add_to_resources();
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
		const std::vector<Background>& get_backgrounds() const;
		const std::vector<ViewPort>& get_views() const;
		const std::map<int,Instance*>& get_instances() const;
		ViewPort* get_current_view() const;
		PhysicsWorld* get_phys_world() const;
		const std::map<const btRigidBody*,Instance*>& get_phys_instances() const;
		const std::map<Instance*,PathFollower>& get_paths() const;
		const std::vector<TimelineIterator>& get_timelines() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
		void set_width(int);
		void set_height(int);
		void set_is_persistent(bool);

		size_t set_background(size_t, Background);
		size_t set_view(size_t, ViewPort);
		size_t set_instance(size_t, Instance*);
		Instance* add_instance(size_t, Object*, btVector3);
		size_t add_instance_grid(size_t, Object*, btVector3);
		int remove_instance(size_t);
		void sort_instances();
		void request_instance_sort();
		void add_physbody(Instance*, PhysicsBody*);
		void remove_physbody(PhysicsBody*);
		void automate_path(Instance*, PathFollower);
		void automate_timeline(TimelineIterator);

		void reset_properties();
		int transfer_instances(const Room*);

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
		virtual void start() {};
		virtual void end() {};
	};
}

#endif // BEE_ROOM_H
