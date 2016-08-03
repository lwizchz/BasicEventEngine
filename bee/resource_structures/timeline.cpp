/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_TIMELINE
#define _BEE_TIMELINE 1

#include "timeline.hpp"

BEE::Timeline::Timeline () {
	if (BEE::resource_list->timelines.game != nullptr) {
		game = BEE::resource_list->timelines.game;
	}

	reset();
}
BEE::Timeline::Timeline (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/timelines/"+path);
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add timeline resource: " + path);
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Timeline::~Timeline() {
	BEE::resource_list->timelines.remove_resource(id);
}
int BEE::Timeline::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == timeline_path) {
			return 1;
		}
		BEE::resource_list->timelines.remove_resource(id);
		id = -1;
	}

	id = BEE::resource_list->timelines.add_resource(this);
	BEE::resource_list->timelines.set_resource(id, this);

	if (BEE::resource_list->timelines.game != nullptr) {
		game = BEE::resource_list->timelines.game;
	}

	return 0;
}
int BEE::Timeline::reset() {
	name = "";
	timeline_path = "";
	action_list.clear();
	next_action = action_list.end();

	start_frame = 0xffffffff;
	position_frame = 0xffffffff;
	start_offset = 0;
	is_looping = false;

	return 0;
}
int BEE::Timeline::print() {
	std::string action_string = get_action_string();

	std::stringstream s;
	s <<
	"Timeline { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	timeline_path	" << timeline_path <<
	"\n	start_frame	" << start_frame <<
	"\n	position_frame	" << position_frame <<
	"\n	is_looping	" << is_looping <<
	"\n	action_list \n" << debug_indent(action_string, 2) <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}

int BEE::Timeline::get_id() {
	return id;
}
std::string BEE::Timeline::get_name() {
	return name;
}
std::string BEE::Timeline::get_path() {
	return timeline_path;
}
bee_timeline_list BEE::Timeline::get_action_list() {
	return action_list;
}
std::string BEE::Timeline::get_action_string() {
	if (action_list.size() > 0) {
		std::ostringstream action_string;
		action_string << "(frame	func_name)\n";
		for (auto& a : action_list) {
			action_string << a.first << "	" << a.second.first << "\n";
		}

		return action_string.str();
	}
	return "none\n";
}
bool BEE::Timeline::get_is_running() {
	if (is_paused) {
		return false;
	}

	if (start_frame == 0xffffffff) {
		return false;
	} else {
		return true;
	}
}
bool BEE::Timeline::get_is_looping() {
	return is_looping;
}

int BEE::Timeline::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Timeline::set_path(std::string new_path) {
	timeline_path = new_path;
	return 0;
}
int BEE::Timeline::add_action(Uint32 frame_number, std::string func_name, std::function<void()> callback) {
	if (get_is_running()) {
		return 1;
	}

	action_list.insert(std::make_pair(frame_number, std::make_pair(func_name, callback)));

	return 0;
}
int BEE::Timeline::add_action(Uint32 frame_number, std::function<void()> callback) {
	return add_action(frame_number, "anonymous", callback);
}
int BEE::Timeline::remove_action(Uint32 frame_number) {
	if (action_list.find(frame_number) != action_list.end()) {
		action_list.erase(action_list.find(frame_number));
		return 0;
	}
	return 1;
}
int BEE::Timeline::set_offset(Uint32 new_offset) {
	start_offset = new_offset;
	return 0;
}
int BEE::Timeline::clip_offset() {
	if (!action_list.empty()) {
		start_offset = action_list.begin()->first;
	}
	return 0;
}
int BEE::Timeline::set_is_looping(bool new_is_looping) {
	is_looping = new_is_looping;
	return 0;
}
int BEE::Timeline::add_ending(std::function<void()> callback) {
	end_action = callback;
	return 0;
}
int BEE::Timeline::set_pause(bool new_is_paused) {
	if (is_paused == new_is_paused) {
		return 1;
	}

	pause_offset = game->get_frame() - pause_offset;

	is_paused = new_is_paused;

	return 0;
}

int BEE::Timeline::start() {
	start_frame = game->get_frame() - start_offset;
	next_action = action_list.begin();
	return 0;
}
int BEE::Timeline::step(Uint32 new_frame) {
	if (!get_is_running()) {
		return 1;
	}

	position_frame = new_frame - start_frame - pause_offset;
	if (next_action == action_list.end()) {
		return 1;
	}

	while (next_action->first < position_frame) {
		if (step(next_action->first + start_frame)) {
			break;
		}
	}

	if (action_list.find(position_frame) != action_list.end()) {
		std::pair <bee_timeline_list::iterator,bee_timeline_list::iterator> range;
		range = action_list.equal_range(position_frame);
		for (bee_timeline_list::iterator it=range.first; it!=range.second; ++it) {
			it->second.second();
			++next_action;
		}

		if ((next_action == action_list.end())||(position_frame >= (*action_list.rbegin()).first)) {
			start_frame = 0xffffffff;
			return 2;
		}
	}

	return 0;
}
int BEE::Timeline::end() {
	pause_offset = 0;
	is_paused = false;

	if (end_action == nullptr) {
		return 1;
	}

	end_action();

	return 0;
}

#endif // _BEE_TIMELINE
