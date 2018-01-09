/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_ROOMS_H
#define BEE_CORE_ROOMS_H 1

#include <utility>

namespace bee {
	// Forward declarations
	class Room;

	void restart_room();
	int change_room(Room*, bool);
	int change_room(Room*);

	Room* get_current_room();
	bool get_is_ready();
	std::pair<int,int> get_room_size();
	bool is_on_screen(const SDL_Rect&);

	bool set_is_paused(bool);
	bool get_is_paused();
}

#endif // BEE_CORE_ROOMS_H
