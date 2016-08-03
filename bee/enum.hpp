/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_ENUM_H
#define _BEE_ENUM_H 1

enum bee_rgba_t {
	c_cyan, c_aqua = c_cyan,
	c_black,
	c_blue,
	c_dkgray,
	c_magenta, c_fuchsia = c_magenta,
	c_gray,
	c_green,
	c_lime,
	c_ltgray, c_silver = c_ltgray,
	c_maroon,
	c_navy,
	c_olive,
	c_orange,
	c_purple,
	c_red,
	c_teal,
	c_white,
	c_yellow
};

enum bee_renderer_t {
	BEE_RENDERER_SDL,
	BEE_RENDERER_OPENGL3,
	BEE_RENDERER_OPENGL4
};

enum bee_event_t {
	BEE_EVENT_UPDATE,
	BEE_EVENT_CREATE,
	BEE_EVENT_DESTROY,
	BEE_EVENT_ALARM,
	BEE_EVENT_STEP_BEGIN,
	BEE_EVENT_STEP_MID,
	BEE_EVENT_STEP_END,
	BEE_EVENT_KEYBOARD_PRESS,
	BEE_EVENT_MOUSE_PRESS,
	BEE_EVENT_KEYBOARD_INPUT,
	BEE_EVENT_MOUSE_INPUT,
	BEE_EVENT_KEYBOARD_RELEASE,
	BEE_EVENT_MOUSE_RELEASE,
	BEE_EVENT_CONTROLLER_AXIS,
	BEE_EVENT_CONTROLLER_PRESS,
	BEE_EVENT_CONTROLLER_RELEASE,
	BEE_EVENT_CONTROLLER_MODIFY,
	BEE_EVENT_CONSOLE_INPUT,
	BEE_EVENT_PATH_END,
	BEE_EVENT_OUTSIDE_ROOM,
	BEE_EVENT_INTERSECT_BOUNDARY,
	BEE_EVENT_COLLISION,
	BEE_EVENT_CHECK_COLLISION_LIST,
	BEE_EVENT_DRAW,
	BEE_EVENT_ANIMATION_END,
	BEE_EVENT_ROOM_START,
	BEE_EVENT_ROOM_END,
	BEE_EVENT_GAME_START,
	BEE_EVENT_GAME_END,
	BEE_EVENT_WINDOW
};

enum bee_light_t {
	BEE_LIGHT_AMBIENT = 1,
	BEE_LIGHT_DIFFUSE,
	BEE_LIGHT_POINT,
	BEE_LIGHT_SPOT
};

enum bee_transition_t {
	BEE_TRANSITION_NONE,
	BEE_TRANSITION_CREATE_LEFT,
	BEE_TRANSITION_CREATE_RIGHT,
	BEE_TRANSITION_CREATE_TOP,
	BEE_TRANSITION_CREATE_BOTTOM,
	BEE_TRANSITION_CREATE_CENTER,
	BEE_TRANSITION_SHIFT_LEFT,
	BEE_TRANSITION_SHIFT_RIGHT,
	BEE_TRANSITION_SHIFT_TOP,
	BEE_TRANSITION_SHIFT_BOTTOM,
	BEE_TRANSITION_INTERLACE_LEFT,
	BEE_TRANSITION_INTERLACE_RIGHT,
	BEE_TRANSITION_INTERLACE_TOP,
	BEE_TRANSITION_INTERLACE_BOTTOM,
	BEE_TRANSITION_PUSH_LEFT,
	BEE_TRANSITION_PUSH_RIGHT,
	BEE_TRANSITION_PUSH_TOP,
	BEE_TRANSITION_PUSH_BOTTOM,
	BEE_TRANSITION_ROTATE_LEFT,
	BEE_TRANSITION_ROTATE_RIGHT,
	BEE_TRANSITION_BLEND,
	BEE_TRANSITION_FADE,
	BEE_TRANSITION_CUSTOM,
	BEE_TRANSITION_MAX
};

enum bee_message_t {
	BEE_MESSAGE_GENERAL,
	BEE_MESSAGE_START,
	BEE_MESSAGE_END,
	BEE_MESSAGE_INFO,
	BEE_MESSAGE_WARNING,
	BEE_MESSAGE_ERROR
};

#endif // _BEE_ENUM_H
