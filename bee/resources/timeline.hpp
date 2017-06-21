/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TIMELINE_H
#define BEE_TIMELINE_H 1

#include <map> // Include the required library headers
#include <functional>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "../resources.hpp"

namespace bee {
	typedef std::multimap<Uint32, std::pair<std::string,std::function<void()>>> timeline_list_t;

	class Timeline: public Resource { // The timeline resource class is used to execute specific actions at a given time offset
			int id; // The id of resource
			std::string name; // An arbitrary name for the resource
			std::string path; // The path of the file to load the timeline from
			timeline_list_t action_list; // The map which holds the timeline actions
			timeline_list_t::iterator next_action; // An iterator pointing to the next action to be executed
			std::function<void()> end_action; // An action which will be executed when the timeline is finished

			Uint32 start_frame; // The frame that the timeline begins executing
			Uint32 position_frame; // The current frame the the timeline is executing
			Uint32 start_offset; // The amount of frames to skip upon starting
			Uint32 pause_offset; // The amount of frames that were skipped while paused
			bool is_looping; // Whether the timeline should loop
			bool is_paused; // Whether the timeline is currently paused
		public:
			// See bee/resources/timeline.cpp for function comments
			Timeline();
			Timeline(const std::string&, const std::string&);
			~Timeline();
			int add_to_resources();
			int reset();
			int print() const;

			int get_id() const;
			std::string get_name() const;
			std::string get_path() const;
			timeline_list_t get_action_list() const;
			std::string get_action_string() const;
			bool get_is_running() const;
			bool get_is_looping() const;

			int set_name(const std::string&);
			int set_path(const std::string&);
			int add_action(Uint32, const std::string&, std::function<void()>);
			int add_action(Uint32, std::function<void()>);
			int remove_actions(Uint32);
			int remove_actions_range(Uint32, Uint32);
			int remove_actions_all();
			int set_offset(Uint32);
			int clip_offset();
			int set_is_looping(bool);
			int set_ending(std::function<void()>);
			int set_pause(bool);

			int start();
			int step_to(Uint32);
			int end();
	};
}

#endif // BEE_TIMELINE_H
