/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENUM_H
#define BEE_ENUM_H 1

#include "defines.hpp"

#include "util/windefine.hpp"

namespace bee {
	/**
	* The type of argument that a ProgramFlag accepts.
	*/
	enum class E_FLAGARG {
		NONE,     ///< No argument.
		OPTIONAL, ///< A single optional argument.
		REQUIRED  ///< A single required argument.
	};

	/**
	* The type of sound effect that a Sound can have.
	*/
	enum class E_SOUNDEFFECT {
		CHORUS,
		ECHO,
		FLANGER,
		GARGLE,
		REVERB,
		COMPRESSOR,
		EQUALIZER
	};

	/**
	* The predefined RGB colors.
	* @note The below colors are in {Red, Green, Blue} format.
	*/
	enum class E_RGB {
		CYAN,              ///< {0, 255, 255}
		BLACK,             ///< {0, 0, 0}
		BLUE,              ///< {0, 0, 255}
		DKGRAY,            ///< {64, 64, 64}
		MAGENTA,           ///< {255, 0, 255}
		GRAY,              ///< {128, 128, 128}
		GREEN,             ///< {0, 128, 0}
		LIME,              ///< {0, 255, 0}
		LTGRAY,            ///< {192, 192, 192}
		MAROON,            ///< {128, 0, 0}
		NAVY,              ///< {0, 0, 128}
		OLIVE,             ///< {128, 128, 0}
		ORANGE,            ///< {255, 128, 0}
		PURPLE,            ///< {128, 0, 255}
		RED,               ///< {255, 0, 0}
		TEAL,              ///< {0, 128, 128}
		WHITE,             ///< {255, 255, 255}
		YELLOW             ///< {255, 255, 0}
	};

	/**
	* The renderer to use.
	*/
	enum class E_RENDERER : int {
		OPENGL3, ///< OpenGL 3.3.
		OPENGL4 ///< OpenGL 4.1.
	};

	/**
	* The Object event types.
	*/
	enum class E_EVENT {
		UPDATE,                 ///< Used to update the Object's Instance data pointer.
		CREATE,                 ///< Used to initialize the Instance.
		DESTROY,                ///< Used to free the Instance's extra memory.
		ALARM,                  ///<
		STEP_BEGIN,             ///<
		STEP_MID,               ///<
		STEP_END,               ///<
		KEYBOARD_PRESS,         ///<
		MOUSE_PRESS,            ///<
		KEYBOARD_INPUT,         ///<
		MOUSE_INPUT,            ///<
		KEYBOARD_RELEASE,       ///<
		MOUSE_RELEASE,          ///<
		CONTROLLER_AXIS,        ///<
		CONTROLLER_PRESS,       ///<
		CONTROLLER_RELEASE,     ///<
		CONTROLLER_MODIFY,      ///<
		COMMANDLINE_INPUT,      ///<
		PATH_END,               ///<
		OUTSIDE_ROOM,           ///<
		INTERSECT_BOUNDARY,     ///<
		COLLISION,              ///<
		CHECK_COLLISION_FILTER, ///< Used to check whether two Instances should collide.
		DRAW,                   ///< Used to render the Instance.
		ANIMATION_END,          ///<
		ROOM_START,             ///<
		ROOM_END,               ///<
		GAME_START,             ///<
		GAME_END,               ///<
		WINDOW,                 ///<
		NETWORK                 ///<
	};

	/**
	* The Light rendering types.
	*/
	enum class E_LIGHT_TYPE : int {
		AMBIENT = 1, ///< Evenly lights the entire viewport.
		DIFFUSE,     ///< Light the scene from a specific angle.
		POINT,       ///< Emit light from a point.
		SPOT         ///< Emit light from a point in a specific angle.
	};

	/**
	* The Room transition animation types.
	*/
	enum class E_TRANSITION {
		NONE,             ///< No transition.
		CREATE_LEFT,      ///< Create the screen from the left.
		CREATE_RIGHT,     ///<
		CREATE_TOP,       ///<
		CREATE_BOTTOM,    ///<
		CREATE_CENTER,    ///<
		SHIFT_LEFT,       ///< Shift the screen from the left.
		SHIFT_RIGHT,      ///<
		SHIFT_TOP,        ///<
		SHIFT_BOTTOM,     ///<
		INTERLACE_LEFT,   ///< Interlace the screen from the left.
		INTERLACE_RIGHT,  ///<
		INTERLACE_TOP,    ///<
		INTERLACE_BOTTOM, ///<
		PUSH_LEFT,        ///< Push the screen from the left.
		PUSH_RIGHT,       ///<
		PUSH_TOP,         ///<
		PUSH_BOTTOM,      ///<
		ROTATE_LEFT,      ///< Rotate the screen to the left.
		ROTATE_RIGHT,     ///<
		BLEND,            ///< Blend between screens.
		FADE,             ///< Fade between screens.
		CUSTOM            ///< Use a custom callback to draw the transition animation.
	};

	/**
	* The message types.
	*/
	enum class E_MESSAGE {
		GENERAL, ///< A general output message.
		START,   ///< A message indicating the start of a given state.
		END,     ///< A message indicating the end of a given state.
		INFO,    ///< An informational message.
		WARNING, ///< A warning message.
		ERROR,   ///< An error message.
		INTERNAL ///< An internal data message.
	};
	/**
	* The messenger output types.
	*/
	enum class E_OUTPUT {
		NONE,   ///< Do not output any messages.
		QUIET,  ///< Only output warnings and errors.
		NORMAL, ///< Output all message types except internal messages.
		VERBOSE ///< Output all messages.
	};

	/**
	* The physics body shapes.
	*/
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
	/**
	* The physics constraint types.
	*/
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

	/**
	* How the Instance should be computed for physics, networking, etc.
	*/
	enum class E_COMPUTATION : int {
		NOTHING    = BIT(0), ///< Do not compute.
		STATIC     = BIT(1), ///< Only compute once.
		SEMISTATIC = BIT(2), ///< Only compute when moved via scripting (props).
		SEMIPLAYER = BIT(3), ///< Only compute when moved via scripting (AI).
		PLAYER     = BIT(4), ///< Only compute when moved by the player.
		DYNAMIC    = BIT(5)  ///< Compute continuously.
	};

	/**
	* How an Instance should behave when it reaches a Path end.
	*/
	enum class E_PATH_END {
		STOP,     ///< Stop following the path.
		RESTART,  ///< Restart the path from the first node.
		CONTINUE, ///< Continue the path by moving to the first node.
		REVERSE   ///< Reverse the path.
	};

	/**
	* The network event types.
	*/
	enum class E_NETEVENT {
		NONE,          ///< No event.
		CONNECT,       ///< Connected to server / Player connection requested.
		DISCONNECT,    ///< Disconnected by server / Player disconnected.
		KEYFRAME,      ///< A keyframe data update.
		PLAYER_UPDATE, ///< A player data update.
		INST_UPDATE,   ///< An Instance update.
		DATA_UPDATE,   ///< A server data update.
		OTHER          ///< Miscellaneous.
	};
	/**
	* The major network signal types.
	*/
	enum class E_NETSIG1 : unsigned char {
		INVALID = 255,
		CONNECT = 1,
		DISCONNECT,
		SERVER_INFO,
		CLIENT_INFO
	};
	/**
	* The minor network signal types.
	*/
	enum class E_NETSIG2 : unsigned char {
		INVALID = 255,
		NONE = 0,
		KEEPALIVE = 0,
		NAME,
		PLAYERS,
		KEYFRAME,
		DELTA
	};

	/**
	* The Variant data types.
	*/
	enum class E_DATA_TYPE : unsigned char {
		NONE,
		CHAR,
		INT,
		FLOAT,
		DOUBLE,
		STRING,
		VECTOR,
		MAP,
		SERIAL
	};

	/**
	* The Particle shape types.
	*/
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
	/**
	* The ParticleSystem shape types.
	*/
	enum class E_PS_SHAPE {
		RECTANGLE,
		LINE,
		CIRCLE
	};
	/**
	* The ParticleEmitter distribution types.
	*/
	enum class E_PS_DISTR {
		LINEAR,
		GAUSSIAN,
		INVGAUSSIAN
	};
	/**
	* The ParticleAttractor force types.
	*/
	enum class E_PS_FORCE {
		CONSTANT,
		LINEAR,
		QUADRATIC
	};
	/**
	* The ParticleChanger change types.
	*/
	enum class E_PS_CHANGE {
		MOTION,
		LOOK,
		ALL
	};

	/**
	* The Font style types corresponding to the TTF_STYLE_ types
	*/
	enum class E_FONT_STYLE : int {
		NORMAL        = 0,
		BOLD          = BIT(0),
		ITALIC        = BIT(1),
		UNDERLINE     = BIT(2),
		STRIKETHROUGH = BIT(3)
	};

	/**
	* The Script types.
	*/
	enum class E_SCRIPT_TYPE {
		INVALID,
		PYTHON
	};
}

#endif // BEE_ENUM_H
