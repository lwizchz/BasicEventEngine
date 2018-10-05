/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ROOM_SCRIPT
#define BEE_ROOM_SCRIPT 1

#include "../../util.hpp"
#include "../../all.hpp"

#include "rm_script.hpp"

namespace bee {
	RmScript::RmScript(const std::string& _scriptfile) :
		Room("__rm_script", _scriptfile),

		scriptfile(_scriptfile),
		script(nullptr),

		is_loaded(false)
	{
		set_name(util::file_plainname(util::file_basename(scriptfile)));
	}
	RmScript::~RmScript() {
		this->free();
	}

	/**
	* Load the Room from its scriptfile.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load the Script
	*/
	int RmScript::load() {
		if (is_loaded) { // If the Room has already been loaded, output a warning
			messenger::send({"engine", "room", "rm_script"}, E_MESSAGE::WARNING, "Failed to load Room \"" + scriptfile + "\" because it has already been loaded");
			return 1;
		}

		std::string filename (scriptfile);
		if ((!filename.empty())&&(filename.front() == '$')) {
			filename = "resources/rooms"+filename.substr(1);
		}

		script = Script::add("__scr_rm_script:" + scriptfile, filename);
		if ((script == nullptr)||(!script->get_is_loaded())) {
			return 2;
		}

		is_loaded = true;

		return 0;
	}
	/**
	* Free the Room and its internal Script.
	*
	* @retval 0 success
	*/
	int RmScript::free() {
		if (!is_loaded) {
			return 0;
		}

		if (script != nullptr) {
			delete script;
			script = nullptr;
		}

		is_loaded = false;

		return 0;
	}

	bool RmScript::get_is_loaded() const {
		return is_loaded;
	}

	void RmScript::init() {
		Room::init();

		if ((script->get_is_loaded())&&(script->get_interface()->has_var("init"))) {
			Variant args (std::vector<Variant>{
				Variant(reinterpret_cast<Room*>(this))
			});
			script->run_func("init", args, nullptr);
		}
	}
	void RmScript::start() {
		Room::start();

		if ((script->get_is_loaded())&&(script->get_interface()->has_var("start"))) {
			Variant args (std::vector<Variant>{
				Variant(reinterpret_cast<Room*>(this))
			});
			script->run_func("start", args, nullptr);
		}
	}
	void RmScript::end() {
		if ((script->get_is_loaded())&&(script->get_interface()->has_var("end"))) {
			Variant args (std::vector<Variant>{
				Variant(reinterpret_cast<Room*>(this))
			});
			script->run_func("end", args, nullptr);
		}

		Room::end();
	}
}

#endif // BEE_ROOM_SCRIPT
