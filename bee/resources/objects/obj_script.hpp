/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_SCRIPT_H
#define BEE_OBJ_SCRIPT_H 1

#include "../../resource/object.hpp"

namespace bee {
	class Script;

	/// Used to run Scripts as Objects
	class ObjScript : public Object {
		std::string scriptfile; ///< The path of the Script
		Script* script; ///< The Script containing the Object event functions
		std::set<E_EVENT> events; ///< The set of fully implemented events

		bool is_loaded; ///< Whether the Object has been loaded from its Script
	public:
		ObjScript(const std::string&);
		virtual ~ObjScript();

		int load();
		int free();

		bool get_is_loaded() const;

		void update(Instance*);
		void create(Instance*);
		void destroy(Instance*);
		void alarm(Instance*, const std::string&);
		void step_begin(Instance*);
		void step_mid(Instance*);
		void step_end(Instance*);
		void keyboard_press(Instance*, SDL_Event*);
		void mouse_press(Instance*, SDL_Event*);
		void keyboard_input(Instance*, SDL_Event*);
		void mouse_input(Instance*, SDL_Event*);
		void keyboard_release(Instance*, SDL_Event*);
		void mouse_release(Instance*, SDL_Event*);
		void controller_axis(Instance*, SDL_Event*);
		void controller_press(Instance*, SDL_Event*);
		void controller_release(Instance*, SDL_Event*);
		void controller_modify(Instance*, SDL_Event*);
		void commandline_input(Instance*, const std::string&);
		void path_end(Instance*, PathFollower*);
		void outside_room(Instance*);
		void intersect_boundary(Instance*);
		void collision(Instance*, Instance*);
		bool check_collision_filter(const Instance*, const Instance*) const;
		void draw(Instance*);
		void animation_end(Instance*);
		void room_start(Instance*);
		void room_end(Instance*);
		void game_start(Instance*);
		void game_end(Instance*);
		void window(Instance*, SDL_Event*);
		void network(Instance*, const NetworkEvent&);
	};
}

#endif // BEE_OBJ_SCRIPT_H
