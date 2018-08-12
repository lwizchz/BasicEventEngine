/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ROOM_SCRIPT_H
#define BEE_ROOM_SCRIPT_H 1

#include "../../resource/room.hpp"

namespace bee {
	class Script;

	/// Used to run Scripts as Rooms
	class RmScript : public Room {
		std::string scriptfile; ///< The path of the Script
		Script* script; ///< The Script containing the Room functions

		bool is_loaded; ///< Whether the Room has been loaded from its Script
	public:
		RmScript(const std::string&);
		virtual ~RmScript();

		int load();
		int free();

		bool get_is_loaded() const;

		virtual void init();
		virtual void start();
		virtual void end();
	};
}

#endif // BEE_RM_SCRIPT_H
