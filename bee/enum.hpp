/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENUM_H
#define BEE_ENUM_H 1

namespace bee {
	enum class E_SOUNDEFFECT : int {
		NONE       = (1u << 0),
		CHORUS     = (1u << 1),
		ECHO       = (1u << 2),
		FLANGER    = (1u << 3),
		GARGLE     = (1u << 4),
		REVERB     = (1u << 5),
		COMPRESSOR = (1u << 6),
		EQUALIZER  = (1u << 7)
	};

	enum class E_RGB {
		CYAN, AQUA = CYAN,
		BLACK,
		BLUE,
		DKGRAY,
		MAGENTA, FUCHSIA = MAGENTA,
		GRAY,
		GREEN,
		LIME,
		LTGRAY, SILVER = LTGRAY,
		MAROON,
		NAVY,
		OLIVE,
		ORANGE,
		PURPLE,
		RED,
		TEAL,
		WHITE,
		YELLOW
	};

	enum class E_RENDERER {
		SDL,
		OPENGL3,
		OPENGL4
	};

	enum class E_EVENT {
		UPDATE,
		CREATE,
		DESTROY,
		ALARM,
		STEP_BEGIN,
		STEP_MID,
		STEP_END,
		KEYBOARD_PRESS,
		MOUSE_PRESS,
		KEYBOARD_INPUT,
		MOUSE_INPUT,
		KEYBOARD_RELEASE,
		MOUSE_RELEASE,
		CONTROLLER_AXIS,
		CONTROLLER_PRESS,
		CONTROLLER_RELEASE,
		CONTROLLER_MODIFY,
		COMMANDLINE_INPUT,
		PATH_END,
		OUTSIDE_ROOM,
		INTERSECT_BOUNDARY,
		COLLISION,
		CHECK_COLLISION_LIST,
		DRAW,
		ANIMATION_END,
		ROOM_START,
		ROOM_END,
		GAME_START,
		GAME_END,
		WINDOW,
		NETWORK
	};

	enum class E_LIGHT : int {
		AMBIENT = 1,
		DIFFUSE,
		POINT,
		SPOT
	};

	enum class E_TRANSITION {
		NONE,
		CREATE_LEFT,
		CREATE_RIGHT,
		CREATE_TOP,
		CREATE_BOTTOM,
		CREATE_CENTER,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		SHIFT_TOP,
		SHIFT_BOTTOM,
		INTERLACE_LEFT,
		INTERLACE_RIGHT,
		INTERLACE_TOP,
		INTERLACE_BOTTOM,
		PUSH_LEFT,
		PUSH_RIGHT,
		PUSH_TOP,
		PUSH_BOTTOM,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		BLEND,
		FADE,
		CUSTOM
	};

	enum class E_MESSAGE {
		GENERAL,
		START,
		END,
		INFO,
		WARNING,
		ERROR,
		INTERNAL
	};
	enum class E_OUTPUT {
		NONE,
		QUIET,
		NORMAL,
		VERBOSE
	};

	enum class E_PHYS_SHAPE {
		NONE,
		SPHERE,
		BOX,
		CYLINDER,
		CAPSULE,
		CONE,
		MULTISPHERE,
		CONVEX_HULL
	};
	enum class E_PHYS_CONSTRAINT {
		NONE,
		POINT,
		HINGE,
		SLIDER,
		CONE,
		SIXDOF,
		FIXED,
		FLAT,
		TILE
	};

	enum class E_COMPUTATION {
		STATIC,
		SEMISTATIC,
		DYNAMIC
	};

	enum class E_PATH_END {
		STOP,
		RESTART,
		CONTINUE,
		REVERSE
	};

	enum class E_NETEVENT {
		NONE,
		CONNECT,
		DISCONNECT,
		KEYFRAME,
		INST_UPDATE,
		DATA_UPDATE
	};

	// Particles
	enum class E_PT_SHAPE {
		PIXEL,
		DISK,
		SQUARE,
		LINE,
		STAR,
		CIRCLE,
		RING,
		SPHERE,
		FLARE,
		SPARK,
		EXPLOSION,
		CLOUD,
		SMOKE,
		SNOW
	};
	enum class E_PS_SHAPE {
		RECTANGLE,
		LINE,
		CIRCLE
	};
	enum class E_PS_DISTR {
		LINEAR,
		GAUSSIAN,
		INVGAUSSIAN
	};
	enum class E_PS_FORCE {
		CONSTANT,
		LINEAR,
		QUADRATIC
	};
	enum class E_PS_CHANGE {
		MOTION,
		LOOK,
		ALL
	};
}

#endif // BEE_ENUM_H
