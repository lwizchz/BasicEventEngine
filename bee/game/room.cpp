/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_ROOM
#define _BEE_GAME_ROOM 1

#include "../game.hpp"

int BEE::restart_room() {
	throw 3;
	return 0;
}
int BEE::change_room(Room* new_room) {
	if (quit) {
		return 1;
	}

	bool is_game_start = false;
	if (current_room != NULL) {
		if (transition_type != 0) {
			set_render_target(texture_before);
			render_clear();
			current_room->draw();
			render();
		}
		current_room->room_end();
		current_room->reset_properties();
	} else {
		if (transition_type != 0) {
			set_render_target(texture_before);
			render_clear();
			render();
		}
		is_game_start = true;
		first_room = new_room;
	}

	sound_stop_all();
	free_media();

	current_room = new_room;
	is_ready = false;
	current_room->reset_properties();
	current_room->init();

	if (load_media()) {
		std::cerr << "Couldn't load room media for " << current_room->get_name() << "\n";
		return 1;
	}

	SDL_SetWindowTitle(window, current_room->get_name().c_str());
	std::cout << current_room->get_instance_string();

	if (transition_type != 0) {
		set_render_target(texture_after);
		render_clear();
	}

	is_ready = true;
	current_room->create();
	if (is_game_start) {
		current_room->game_start();
	}
	current_room->room_start();
	current_room->draw();

	if (transition_type != 0) {
		render();
		set_render_target(NULL);
		draw_transition();
	}

	return 0;
}
int BEE::room_goto(int index) {
	return change_room(get_room(index));
}
int BEE::room_goto_previous() {
	return room_goto(get_current_room()->get_id()-1);
}
int BEE::room_goto_next() {
	return room_goto(get_current_room()->get_id()+1);
}

BEE::Room* BEE::get_current_room() {
	return current_room;
}
bool BEE::get_is_ready() {
	return is_ready;
}
int BEE::get_room_width() {
	if (current_room != NULL) {
		return current_room->get_width();
	}
	return -1;
}
int BEE::get_room_height() {
	if (current_room != NULL) {
		return current_room->get_height();
	}
	return -1;
}

#endif // _BEE_GAME_ROOM
