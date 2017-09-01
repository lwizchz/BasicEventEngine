/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

namespace bee {
	std::map<int,Timeline*> Timeline::list;
	int Timeline::next_id = 0;

	/*
	* Timeline::Timeline() - Default construct the timeline
	* ! This constructor should only be directly used for temporary timelines, the other constructor should be used for all other cases
	*/
	Timeline::Timeline() :
		Resource(),

		id(-1),
		name(),
		path(),
		action_list(),
		next_action(),
		end_action(nullptr),

		start_frame(0xffffffff),
		position_frame(0xffffffff),
		start_offset(0),
		pause_offset(0),
		is_looping(false),
		is_paused(false)
	{}
	/*
	* Timeline::Timeline() - Construct the timeline, add it to the timeline resource list, and set the new name and path
	* @new_name: the name of the timeline to use
	* @new_path: the path of the timeline config file
	*/
	Timeline::Timeline(const std::string& new_name, const std::string& new_path) :
		Timeline()
	{
		add_to_resources(); // Add the timeline to the appropriate resource list
		if (id < 0) { // If the timeline could not be added to the resource list, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add timeline resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the timeline name
		set_path(new_path); // Set the timeline path
	}
	/*
	* Timeline::~Timeline() - Remove the timeline from the resource list
	*/
	Timeline::~Timeline() {
		if (list.find(id) != list.end()) { // Remove the timeline from the resource list
			list.erase(id);
		}
	}

	/*
	* Timeline::add_to_resources() - Add the timeline to the appropriate resource list
	*/
	int Timeline::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Timeline::get_amount() - Return the amount of timeline resources
	*/
	size_t Timeline::get_amount() {
		return list.size();
	}
	/*
	* Timeline::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Timeline* Timeline::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Timeline::reset() - Reset all resource variables for reinitialization
	*/
	int Timeline::reset() {
		// Reset all properties
		name = "";
		path = "";
		action_list.clear();
		next_action = action_list.end();

		start_frame = 0xffffffff;
		position_frame = 0xffffffff;
		start_offset = 0;
		is_looping = false;

		return 0; // Return 0 on success
	}
	/*
	* Timeline::print() - Print all relevant information about the resource
	*/
	int Timeline::print() const {
		std::string action_string = get_action_string(); // Get the list of actions in string form

		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Timeline { "
		"\n	id             " << id <<
		"\n	name           " << name <<
		"\n	path           " << path <<
		"\n	start_frame    " << start_frame <<
		"\n	position_frame " << position_frame <<
		"\n	is_looping     " << is_looping <<
		"\n	action_list\n" << debug_indent(action_string, 2) <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Timeline::get_*() - Return the requested resource information
	*/
	int Timeline::get_id() const {
		return id;
	}
	std::string Timeline::get_name() const {
		return name;
	}
	std::string Timeline::get_path() const {
		return path;
	}
	timeline_list_t Timeline::get_action_list() const {
		return action_list;
	}
	std::string Timeline::get_action_string() const {
		if (action_list.empty()) { // If there are no actions in the list, return a none-string
			return "none\n";
		}

		std::vector<std::vector<std::string>> table; // Declare a table to hold the actions
		table.push_back({"(frame", "func_name)"}); // Append the table header

		for (auto& a : action_list) { // Iterate over the actions and add each one of them to the table
			table.push_back({bee_itos(a.first), a.second.first});
		}

		return string_tabulate(table); // Return the table as a properly spaced string
	}
	bool Timeline::get_is_running() const {
		if (is_paused) {
			return false; // Return false if the timeline is currently paused
		}

		if (start_frame == 0xffffffff) {
			return false; // Return false if the start frame is the maximum value, i.e. not currently running
		}

		return true; // Return true if the timeline is neither paused nor stopped at the max value
	}
	bool Timeline::get_is_looping() const {
		return is_looping;
	}

	/*
	* Timeline::set_*() - Set the requested resource data
	*/
	int Timeline::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Timeline::set_path(const std::string& new_path) {
		path = "resources/timelines/"+new_path; // Append the path to the timeline directory
		return 0;
	}
	int Timeline::add_action(Uint32 frame_number, const std::string& func_name, std::function<void()> callback) {
		if (get_is_running()) { // If the timeline is already running, output a warning
			messenger::send({"engine", "timeline"}, E_MESSAGE::WARNING, "Failed to add action to timeline \"" + name + "\" because it is currently running");
			return 1; // Return 1 when the timeline is already running
		}

		action_list.emplace(frame_number, std::make_pair(func_name, callback)); // Insert the action in the list

		return 0; // Return 0 on success
	}
	int Timeline::add_action(Uint32 frame_number, std::function<void()> callback) {
		return add_action(frame_number, "anonymous", callback); // Return the attempt to add the anonymous action
	}
	int Timeline::remove_actions(Uint32 frame_number) {
		int amount_removed = 0;

		while (action_list.find(frame_number) != action_list.end()) { // Continue removing actions until no more exist at the given frame
			action_list.erase(action_list.find(frame_number)); // Remove the action
			++amount_removed; // Increment the counter
		}

		return amount_removed; // Return the amount of actions that were removed
	}
	int Timeline::remove_actions_range(Uint32 frame_start, Uint32 frame_end) {
		int amount_removed = 0;

		for (size_t i=0; i<=frame_end-frame_start; ++i) { // Iterate over the frames
			amount_removed += remove_actions(frame_start+i); // Remove the actions from each frame and increment the counter
		}

		return amount_removed; // Return the amount of actions that were removed
	}
	int Timeline::remove_actions_all() {
		int amount_removed = action_list.size(); // Store the amount of actions in the list

		action_list.clear(); // Clear the action list

		return amount_removed; // Return the amount of actions that were removed
	}
	/*
	* Timeline::set_offset() - Set the amount of frames to skip at the beginning of timeline execution
	* @new_offset: the new offset to use
	*/
	int Timeline::set_offset(Uint32 new_offset) {
		start_offset = new_offset;
		return 0;
	}
	/*
	* Timeline::clip_offset() - Offset the list such that the first action is executed on timeline start
	*/
	int Timeline::clip_offset() {
		if (action_list.empty()) { // If the list is empty, remove the offset
			start_offset = 0;
			return 1; // Return 1 when the offset could not be clipped
		}

		start_offset = action_list.begin()->first; // Set the offset to the first executable frame

		return 0; // Return 0 on success
	}
	int Timeline::set_is_looping(bool new_is_looping) {
		is_looping = new_is_looping;
		return 0;
	}
	int Timeline::set_ending(std::function<void()> callback) {
		end_action = callback;
		return 0;
	}
	int Timeline::set_pause(bool new_is_paused) {
		if (is_paused == new_is_paused) {
			return 1; // Return 1 when the requested pause state is already in effect
		}

		pause_offset = get_frame() - pause_offset; // Modify the pause offset according to when the timeline was paused

		is_paused = new_is_paused; // Set the timeline's pause state

		return 0; // Return 0 on success
	}

	/*
	* Timeline::start() - Enable timeline execution on the first action
	*/
	int Timeline::start() {
		start_frame = get_frame() - start_offset; // Set the start frame with respect to the start offset
		next_action = action_list.begin(); // Set the next action to be executed
		return 0; // Return 0 on success
	}
	/*
	* Timeline::step_to() - Execute all actions up to the given frame
	* @new_frame: the frame to execute up to
	*/
	int Timeline::step_to(Uint32 new_frame) {
		if (!get_is_running()) { // If the timeline isn't running, ignore any step_to() calls
			return 1; // Return 1 when the timeline is not running
		}

		if (next_action == action_list.end()) {
			return 2;  // Return 2 when the timeline has no more actions to execute
		}

		const Uint32 last_frame = new_frame - start_frame - pause_offset; // Calculate the frame to step to with respect to offsets

		position_frame = next_action->first; // Set the position frame to the frame of the next action
		while (position_frame < last_frame) { // Iterate over the action list until the last step frame is reached
			next_action->second.second(); // Call the action's callback
			++next_action; // Increment the iterator

			if (next_action == action_list.end()) { // If the timeline has no more actions left
				start_frame = 0xffffffff; // Reset the start frame in preparation for the timeline end
				return 2; // Return 2 when there's nothing left to execute
			}

			position_frame = next_action->first; // Set the position frame to the frame of the next action
		}

		return 0; // Return 0 on success
	}
	/*
	* Timeline::end() - End timeline execution whether it finished or not
	*/
	int Timeline::end() {
		pause_offset = 0; // Reset the pause state
		is_paused = false;

		if (end_action == nullptr) {
			return 1; // Return 1 when there is no end action to call
		}

		end_action(); // Call the end action callback if necessary

		return 0; // Return 0 on success
	}
}

#endif // BEE_TIMELINE
