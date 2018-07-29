/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TIMELINE_H
#define BEE_TIMELINE_H 1

#include <string> // Include the required library headers
#include <map>
#include <functional>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "resource.hpp"

#include "../data/variant.hpp"

namespace bee {
	// Forward declarations
	class Timeline;
	struct TimelineIterator;
	class Script;

	/// Used to hold action callbacks
	struct TimelineAction {
		std::string name; ///< The action name
		std::function<void (TimelineIterator*, TimelineAction*)> func; ///< The action callback

		TimelineAction(const std::string&, std::function<void (TimelineIterator*, TimelineAction*)>);
		TimelineAction();

		void operator()(TimelineIterator*);
	};

	/// Used to iterate through a Timeline's actions
	struct TimelineIterator {
		Timeline* tl; ///< The associated Timeline

		Uint32 start_frame; ///< The iterator starting frame
		Uint32 position_frame; ///< The iterator position
		Uint32 start_offset; ///< The offset to start the iterator with
		Uint32 pause_offset; ///< The offset which increases during a pause

		bool is_looping; ///< Whether the iterator should loop
		bool is_pausable; ///< Whether the iterator should pause

		TimelineIterator(Timeline*, Uint32, bool, bool);
		TimelineIterator();

		void clip_offset();
		int step_to(Uint32);
	};

	/// Used to execute specific actions at a given time offset
	class Timeline: public Resource {
		static std::map<int,Timeline*> list;
		static int next_id;

		int id; ///< The unique Timeline identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the file to load the Timeline from

		bool is_loaded; ///< Whether the actions were successfully loaded into the map
		std::multimap<Uint32,TimelineAction> actions; ///< The map which holds the actions
		TimelineAction end_action; ///< An action which will be executed when the Timeline is finished

		Script* scr_actions; ///< The Script which loads the desired actions
	public:
		// See bee/resources/timeline.cpp for function comments
		Timeline();
		Timeline(const std::string&, const std::string&);
		~Timeline();

		static size_t get_amount();
		static Timeline* get(int);
		static Timeline* get_by_name(const std::string&);
		static Timeline* add(const std::string&, const std::string&);

		int add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		const std::multimap<Uint32,TimelineAction>& get_actions() const;

		void set_name(const std::string&);
		void set_path(const std::string&);

		int load();
		int free();

		void add_action(Uint32, const std::string&, std::function<void (TimelineIterator*, TimelineAction*)>);
		void add_action(Uint32, std::function<void (TimelineIterator*, TimelineAction*)>);
		int remove_actions(Uint32);
		int remove_actions_range(Uint32, Uint32);
		int remove_actions_all();
		void set_ending(TimelineAction);

		int step_to(TimelineIterator*, Uint32);
		void end(TimelineIterator*);
	};
}

#endif // BEE_TIMELINE_H
