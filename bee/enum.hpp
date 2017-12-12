/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENUM_H
#define BEE_ENUM_H 1

#include "util/windefine.hpp"

namespace bee {
	#define BIT(x) (1u << x)

	enum class E_FLAGARG {
		NONE,
		OPTIONAL,
		REQUIRED
	};

	enum class E_SOUNDEFFECT : int {
		NONE       = BIT(0),
		CHORUS     = BIT(1),
		ECHO       = BIT(2),
		FLANGER    = BIT(3),
		GARGLE     = BIT(4),
		REVERB     = BIT(5),
		COMPRESSOR = BIT(6),
		EQUALIZER  = BIT(7)
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

	enum class E_COMPUTATION : int {
		NOTHING    = BIT(0),
		STATIC     = BIT(1),
		SEMISTATIC = BIT(2),
		SEMIPLAYER = BIT(3),
		PLAYER     = BIT(4),
		DYNAMIC    = BIT(5)
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
		PLAYER_UPDATE,
		INST_UPDATE,
		DATA_UPDATE,
		OTHER
	};
	enum class E_NETSIG1 : unsigned char {
		INVALID = 255,
		CONNECT = 1,
		DISCONNECT,
		SERVER_INFO,
		CLIENT_INFO
	};
	enum class E_NETSIG2 : unsigned char {
		INVALID = 255,
		NONE = 0,
		KEEPALIVE = 0,
		NAME,
		PLAYERS,
		KEYFRAME,
		DELTA
	};

	enum class E_DATA_TYPE : unsigned int {
		CHAR = 0,
		INT,
		FLOAT,
		DOUBLE,
		STRING,
		VECTOR,
		MAP,
		SERIAL
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

	#undef BIT
}

#endif // BEE_ENUM_H
