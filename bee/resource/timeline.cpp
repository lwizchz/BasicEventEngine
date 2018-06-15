/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TIMELINE
#define BEE_TIMELINE 1

#include <sstream> // Include the required library headers

#include "timeline.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../messenger/messenger.hpp"

#include "../core/rooms.hpp"

#include "../resource/script.hpp"

namespace bee {
	/**
	* Construct the action from a name and callback.
	*/
	TimelineAction::TimelineAction(const std::string& _name, std::function<void (TimelineIterator*, TimelineAction*)> _func) :
		name(_name),
		func(_func)
	{}
	/**
	* Default construct the action.
	*/
	TimelineAction::TimelineAction() :
		TimelineAction("", nullptr)
	{}
	/**
	* Call the callback if it's set.
	*/
	void TimelineAction::operator()(TimelineIterator* tlit) {
		if (func != nullptr) {
			func(tlit, this);
		}
	}

	/**
	* Construct the iterator from an offset.
	* @param _tl the Timeline to iterate over
	* @param _start_offset the tick offset to start at
	* @param _is_looping whether to restart the Timeline when it finishes
	* @param _is_pausable whether the iterator should pause when the game pauses
	*/
	TimelineIterator::TimelineIterator(Timeline* _tl, Uint32 _start_offset, bool _is_looping, bool _is_pausable) :
		tl(_tl),

		start_frame(0),
		position_frame(-1),
		start_offset(_start_offset),
		pause_offset(0),

		is_looping(_is_looping),
		is_pausable(_is_pausable)
	{}
	/**
	* Default construct the iterator.
	*/
	TimelineIterator::TimelineIterator() :
		TimelineIterator(nullptr, 0, false, false)
	{}
	/**
	* Clip the start offset to the first action's frame.
	*/
	void TimelineIterator::clip_offset() {
		if ((tl == nullptr)||(tl->get_actions().empty())) {
			start_offset = 0;
			return;
		}

		start_offset = tl->get_actions().begin()->first;
	}
	/**
	* Step to the given frame.
	* @param frame the frame to step to
	*/
	int TimelineIterator::step_to(Uint32 frame) {
		return tl->step_to(this, frame);
	}

	std::map<int,Timeline*> Timeline::list;
	int Timeline::next_id = 0;

	/**
	* Default construct the Timeline.
	* @note This constructor should only be directly used for temporary timelines, the other constructor should be used for all other cases.
	*/
	Timeline::Timeline() :
		Resource(),

		id(-1),
		name(),
		path(),

		is_loaded(false),
		actions(),
		end_action(),

		scr_actions(new Script())
	{}
	/**
	* Construct the Timeline, add it to the Timeline resource list, and set the new name and path.
	* @param _name the name of the Timeline to use
	* @param _path the path of the Timeline file
	*
	* @throws int(-1) Failed to initialize Resource
	*/
	Timeline::Timeline(const std::string& _name, const std::string& _path) :
		Timeline()
	{
		if (add_to_resources() < 0) { // Attempt to add the Timeline to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add timeline resource: \"" + _name + "\" from " + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Remove the Timeline from the resource list.
	*/
	Timeline::~Timeline() {
		delete scr_actions;

		list.erase(id);
	}

	/**
	* @returns the number of Timeline resources
	*/
	size_t Timeline::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Timeline* Timeline::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Timeline
	*
	* @returns the Timeline resource with the given name or nullptr if not found
	*/
	Timeline* Timeline::get_by_name(const std::string& name) {
		for (auto& tl : list) { // Iterate over the timelines in order to find the first one with the given name
			Timeline* t = tl.second;
			if (t != nullptr) {
				if (t->get_name() == name) {
					return t; // Return the desired timeline on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze and return a newly created Timeline resource.
	* @param name the name to initialize the Timeline with
	* @param path the path to initialize the Timeline with
	*
	* @returns the newly created Timeline
	*/
	Timeline* Timeline::add(const std::string& name, const std::string& path) {
		Timeline* new_timeline = new Timeline(name, path);
		return new_timeline;
	}

	/**
	* Add the Timeline to the appropriate resource list.
	*
	* @returns the Timeline id
	*/
	int Timeline::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with it's new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Timeline::reset() {
		// Reset all properties
		name = "";
		path = "";

		actions.clear();
		end_action = TimelineAction();

		scr_actions->free();

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Timeline
	*/
	std::map<Variant,Variant> Timeline::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["is_loaded"] = is_loaded;

		std::vector<Variant> _actions;
		for (auto& a : actions) {
			_actions.emplace_back(a.second.name);
		}
		info["actions"] = _actions;

		if (end_action.func != nullptr) {
			info["end_action"] = end_action.name;
		} else {
			info["end_action"] = Variant();
		}

		return info;
	}
	/**
	* Restore the Timeline from the serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	*/
	int Timeline::deserialize(std::map<Variant,Variant>& m) {
		this->free();

		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		is_loaded = false;
		actions.clear();
		end_action = TimelineAction();

		scr_actions->free();

		if ((m["is_loaded"].i)&&(load())) {
			return 1;
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Timeline::print() const {
		Variant m (serialize());
		messenger::send({"engine", "timeline"}, E_MESSAGE::INFO, "Timeline " + m.to_str(true));
	}

	int Timeline::get_id() const {
		return id;
	}
	std::string Timeline::get_name() const {
		return name;
	}
	std::string Timeline::get_path() const {
		return path;
	}
	const std::multimap<Uint32,TimelineAction>& Timeline::get_actions() const {
		return actions;
	}

	void Timeline::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Timelines resource directory.
	*/
	void Timeline::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Timelines directory if not root
			path = "resources/timelines/"+_path;
		}
	}

	/**
	* Load the Timeline script from its path.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load since it's not a script
	*/
	int Timeline::load() {
		if (is_loaded) {
			messenger::send({"engine", "timeline"}, E_MESSAGE::WARNING, "Failed to load Timeline \"" + name + "\" because it has already been loaded");
			return 1;
		}

		if (Script::is_script(path)) {
			messenger::send({"engine", "timeline"}, E_MESSAGE::WARNING, "Failed to load Timeline \"" + name + "\" from \"" + path + "\" because it's not a script");
			return 2;
		}

		scr_actions->set_path("/"+path);
		scr_actions->load();

		// Set the loaded booleans
		is_loaded = true;

		return 0;
	}
	int Timeline::free() {
		if (!is_loaded) {
			return 0;
		}

		// Remove all actions
		actions.clear();
		end_action = TimelineAction();

		scr_actions->free();

		// Set the loaded boolean
		is_loaded = false;

		return 0;
	}

	/**
	* Add the given callback to the action list.
	* @param frame the frame at which to execute the action
	* @param action_name the name of the action
	* @param callback the callback to use for the action
	*/
	void Timeline::add_action(Uint32 frame, const std::string& action_name, std::function<void (TimelineIterator*, TimelineAction*)> callback) {
		is_loaded = true;
		actions.emplace(frame, TimelineAction(action_name, callback));
	}
	/**
	* Add the given function to the action list.
	* @param frame the frame at which to execute the action
	* @param callback the callback to use for the action
	*/
	void Timeline::add_action(Uint32 frame, std::function<void (TimelineIterator*, TimelineAction*)> callback) {
		add_action(frame, "anonymous_callback", callback);
	}
	/**
	* Remove all actions at the given frame.
	* @param frame the frame to remove from the action list
	*
	* @returns the number of actions that were removed
	*/
	int Timeline::remove_actions(Uint32 frame) {
		int amount_removed = 0;

		while (actions.find(frame) != actions.end()) { // Continue removing actions until no more exist at the given frame
			actions.erase(actions.find(frame)); // Remove the action
			++amount_removed; // Increment the counter
		}

		return amount_removed;
	}
	/**
	* Remove all actions in a given frame range.
	* @param frame_start the frame at which to begin removing actions
	* @param frame_end the frame at which to stop removing actions
	*
	* @returns the total number of actions that were removed
	*/
	int Timeline::remove_actions_range(Uint32 frame_start, Uint32 frame_end) {
		int amount_removed = 0;

		for (size_t i=frame_start; i<=frame_end; ++i) {
			amount_removed += remove_actions(i);
		}

		return amount_removed;
	}
	/**
	* Remove all actions.
	*
	* @returns the total number of actions that were removed
	*/
	int Timeline::remove_actions_all() {
		int amount_removed = actions.size();

		actions.clear();

		return amount_removed;
	}
	/**
	* Set the end action.
	* @param action the action to use upon ending the Timeline
	*/
	void Timeline::set_ending(TimelineAction action) {
		end_action = action;
	}

	/**
	* Execute all actions up to the given frame.
	* @param tlit the iterator to use
	* @param frame the frame to execute
	*
	* @retval 0 success
	* @retval 1 failed to advance the iterator since it's finished
	* @retval 2 the iterator has finished
	*/
	int Timeline::step_to(TimelineIterator* tlit, Uint32 frame) {
		if (tlit->start_frame == static_cast<Uint32>(-1)) {
			return 1;
		}

		if ((tlit->is_pausable)&&(get_is_paused())) {
			tlit->pause_offset = get_frame() - tlit->pause_offset;
			return 0;
		}

		if (tlit->position_frame == static_cast<Uint32>(-1)) {
			tlit->start_frame = frame - tlit->start_offset;
			tlit->position_frame = 0;
		}

		const Uint32 last_frame = frame - tlit->start_frame - tlit->pause_offset; // Calculate the frame to step to with respect to offsets

		while (tlit->position_frame <= last_frame) {
			std::multimap<Uint32,TimelineAction>::iterator start, end;
			std::tie(start, end) = actions.equal_range(tlit->position_frame);

			for (auto it=start; it!=end; ++it) {
				it->second(tlit);
			}

			++tlit->position_frame; // Set the position frame to the frame of the next action
		}

		if (actions.lower_bound(tlit->position_frame) == actions.end()) { // If the timeline has no more actions left
			end(tlit);
			return 2;
		}

		return 0;
	}
	/**
	* End execution whether the iterator's finished or not.
	* @param tlit the iterator to use
	*/
	void Timeline::end(TimelineIterator* tlit) {
		tlit->start_frame = -1;
		tlit->pause_offset = 0; // Reset the pause state

		if (end_action.func != nullptr) { // Call the end action callback if necessary
			end_action(tlit);
		}

		if (tlit->is_looping) {
			*tlit = TimelineIterator(tlit->tl, tlit->start_offset, tlit->is_looping, tlit->is_pausable);
		}
	}
}

#endif // BEE_TIMELINE
