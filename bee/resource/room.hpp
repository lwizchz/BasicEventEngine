/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
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

#include "../core/instance.hpp"

#include "../render/rgba.hpp"

#include "texture.hpp"
#include "light.hpp"

namespace bee {
	// Forward declarations
	struct BackgroundData;
	struct ViewPort;
	class PhysicsWorld;
	class Particle;
	class ParticleSystem;

	struct NetworkEvent;

	// Declare a function to flip <int,Instance*> to <Instance*,int> for use with instances_sorted
	namespace internal {
		std::pair<Instance*,int> flip_instancemap_pair(const std::pair<int,Instance*>&);
	}

	struct BackgroundData { // The data struct which is used to pass data to the Room class
		Texture* texture; // A pointer to the texture to use this data with
		bool is_visible; // Whether to draw the background
		bool is_foreground; // Whether to draw the texture above or below the other sprites
		TextureTransform transform; // The data to transform the texture with

		// See bee/resources/room.cpp for function comments
		BackgroundData();
		BackgroundData(Texture*, bool, bool, int, int, bool, bool, int, int, bool);
	};

	struct InstanceSort { // This struct is used as the comparator for the instances_sorted member of the Room class
		bool operator() (Instance* lhs, Instance* rhs) const {
			return (*lhs) < (*rhs); // Compare the values instead of the pointers
		}
	};

	class Room: public Resource { // The room resource class is used to handle all instance event calls and instantiation
			static std::map<int,Room*> list;
			static int next_id;

			int id; // The id of the resource
			std::string name; // An arbitrary name for the resource
			std::string path; // The path of the room's child header
			int width, height; // The width and height of the room
			bool is_isometric; // Whether the room should be drawn from an isometric perspective
			bool is_persistent; // Whether the room's instances should persist to the next room

			RGBA background_color; // The background color of the room
			bool is_background_color_enabled; // Whether the background color should be drawn
			std::vector<BackgroundData*> backgrounds; // The list of backgrounds that should be drawn
			std::vector<ViewPort*> views; // The list of views that shold be drawn

			int next_instance_id; // The id for the next created instance, always increasing
			std::map<int,Instance*> instances; // A map of all instances with their associated id
			std::map<Instance*,int,InstanceSort> instances_sorted; // A map of all instance sorted by depth, then by id
			std::vector<Instance*> created_instances; // A list of instances that should have their create event called after the room is loaded
			std::vector<Instance*> destroyed_instances; // A list of instances that should have their destroy event called after the event loop
			bool should_sort; // Whether the sorted instance list needs to be resorted after the event loop

			static const std::list<E_EVENT> event_list; // A list of the available events
			std::map<E_EVENT,std::map<Instance*,int,InstanceSort>> instances_sorted_events; // A map of all events and the instance which implement those events

			std::vector<ParticleSystem*> particle_systems; // A list of the particle systems that the room should run draw

			std::vector<LightData> lights; // A list of all the queued lights to be drawn
			std::vector<LightableData*> lightables; // A list of all the lightables which can cast shadows
			Texture* light_map; // A texture used for SDL light rendering

			PhysicsWorld* physics_world; // The world used to simulate all physics objects in the room
			std::map<const btRigidBody*,Instance*> physics_instances; // A map of the bodies in the world with their associated instance

			std::string instance_map; // The path of the instance map file to load instance from when the room starts

			ViewPort* view_current; // A pointer to the current view that is being drawn
		public:
			// See bee/resources/room.cpp for function comments
			Room();
			Room(const std::string&, const std::string&);
			~Room();

			int add_to_resources();
			static size_t get_amount();
			static Room* get(int);
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
			std::vector<BackgroundData*> get_backgrounds() const;
			std::string get_background_string() const;
			std::vector<ViewPort*> get_views() const;
			std::string get_view_string() const;
			const std::map<int,Instance*>& get_instances() const;
			std::string get_instance_string() const;
			ViewPort* get_current_view() const;
			PhysicsWorld* get_phys_world() const;
			const std::map<const btRigidBody*,Instance*>& get_phys_instances() const;

			int set_name(const std::string&);
			int set_path(const std::string&);
			int set_width(int);
			int set_height(int);
			int set_is_isometric(bool);
			int set_is_persistent(bool);
			int set_background_color(RGBA);
			int set_is_background_color_enabled(bool);
			int set_background(int, BackgroundData*);
			int add_background(Texture*, bool, bool, int, int, bool, bool, int, int, bool);
			int set_view(int, ViewPort*);
			int set_instance(int, Instance*);
			Instance* add_instance(int, Object*, double, double, double);
			int add_instance_grid(int, Object*, double, double, double);
			int remove_instance(int);
			int sort_instances();
			int request_instance_sort();
			int add_physbody(Instance*, PhysicsBody*);
			int remove_physbody(PhysicsBody*);
			int add_particle_system(ParticleSystem*);
			int add_lightable(LightableData*);
			int add_light(LightData);
			int handle_lights();
			int reset_lights();
			int clear_lights();

			int reset_properties();
			int transfer_instances(const Room*);

			int save_instance_map(const std::string&);
			int load_instance_map(const std::string&);
			int load_instance_map();
			std::string get_instance_map() const;
			int set_instance_map(const std::string&);

			int create();
			int destroy();
			int destroy(Instance*);
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
			static bool check_collision_filter(btBroadphaseProxy*, btBroadphaseProxy*);
			int draw();
			int draw_view(ViewPort*);
			int animation_end();
			int room_start();
			int room_end();
			int game_start();
			int game_end();
			int window(SDL_Event*);
			int network(const NetworkEvent&);

			virtual void init();
			virtual void start() {};
			virtual void end() {};
	};
}

#endif // BEE_ROOM_H
