/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_ROOM_H
#define BEE_CORE_ROOM_H 1

namespace bee {
	// Forward declarations
	class Room;
	class ViewData;

	void restart_room();
	int change_room(Room*, bool);
	int change_room(Room*);
	int room_goto(int);
	int room_goto_previous();
	int room_goto_next();

	Room* get_current_room();
	bool get_is_ready();
	int get_room_width();
	int get_room_height();
	bool is_on_screen(const SDL_Rect&);

	int set_viewport(ViewData*);

	bool set_is_paused(bool);
	bool get_is_paused();
}

#endif // BEE_CORE_ROOM_H
