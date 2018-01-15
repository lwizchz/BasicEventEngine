/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SCRIPT
#define BEE_SCRIPT 1

#include <sstream> // Include the required library headers

#include "script.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../messenger/messenger.hpp"

#include "../python/python.hpp"

namespace bee {
	std::map<int,Script*> Script::list;
	int Script::next_id = 0;

	/*
	* Script::Script() - Default construct the script
	* ! This constructor should only be directly used for temporary scripts, the other constructor should be used for all other cases
	*/
	Script::Script() :
		Resource(),

		id(-1),
		name(),
		path(),

		script(nullptr),
		is_loaded(false)
	{}
	/*
	* Script::Script() - Construct the script, add it to the script resource list, and set the new name and path
	*/
	Script::Script(const std::string& new_name, const std::string& new_path) :
		Script() // Default initialize all variables
	{
		add_to_resources(); // Add the script to the appropriate resource list
		if (id < 0) { // If the script could not be added to the resource list, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add script resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the script name
		set_path(new_path); // Set the script path
	}
	/*
	* Script::~Script() - Remove the script from the resouce list
	*/
	Script::~Script() {
		list.erase(id); // Remove the script from the resource list
	}

	/*
	* Script::get_amount() - Return the amount of script resources
	*/
	size_t Script::get_amount() {
		return list.size();
	}
	/*
	* Script::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Script* Script::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Script::get_by_name() - Return the script resource with the given name
	* @name: the name of the desired script
	*/
	Script* Script::get_by_name(const std::string& name) {
		for (auto& script : list) { // Iterate over the scripts in order to find the first one with the given name
			Script* s = script.second;
			if (s != nullptr) {
				if (s->get_name() == name) {
					return s; // Return the desired script on success
				}
			}
		}
		return nullptr; // Return nullptr on failure
	}
	/*
	* Script::add() - Initiliaze, load, and return a newly created script resource
	* @name: the name to initialize the script with
	* @path: the path to initialize the script with
	*/
	Script* Script::add(const std::string& name, const std::string& path) {
		Script* new_script = new Script(name, path);
		new_script->load();
		return new_script;
	}

	/*
	* Script::add_to_resources() - Add the sprite to the appropriate resource list
	*/
	int Script::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Script::reset() - Reset all resource variables for reinitialization
	*/
	int Script::reset() {
		// Reset all properties
		name = "";
		path = "";

		script = nullptr;
		is_loaded = false;

		return 0; // Return 0 on success
	}
	/*
	* Script::print() - Print all relevant information about the resource
	*/
	int Script::print() const {
		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Script { "
		"\n	id          " << id <<
		"\n	name        " << name <<
		"\n	path        " << path <<
		"\n	is_loaded   " << is_loaded <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Script::get_*() - Return the requested resource information
	*/
	int Script::get_id() const {
		return id;
	}
	std::string Script::get_name() const {
		return name;
	}
	std::string Script::get_path() const {
		return path;
	}
	ScriptInterface* Script::get_interface() const {
		return script;
	}

	/*
	* Script::set_*() - Set the requested resource data
	*/
	int Script::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Script::set_path(const std::string& new_path) {
		if (new_path == ".py") { // If the path is "empty," use the script type as the path
			path = new_path;
		} else {
			if (new_path.front() == '/') {
				path = new_path.substr(1);
			} else {
				path = "resources/scripts/"+new_path; // Append the path to the script directory if no root
			}
		}
		return 0;
	}

	int Script::load() {
		if (is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to load script \"" + name + "\" because it has already been loaded");
			return 1;
		}

		if (path.substr(path.length()-3, 3) == ".py") {
			script = new PythonScriptInterface(path);
			if (script->load()) {
				return 3;
			}
		} else {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to load script \"" + name + "\": unknown script extension \"" + path.substr(path.length()-3, 3) + "\"");
			return 2;
		}

		is_loaded = true;

		return 0;
	}
	int Script::free() {
		if (!is_loaded) {
			return 0;
		}

		if (script != nullptr) {
			delete script;
			script = nullptr;
		}

		return 0;
	}
	int Script::run_string(const std::string& code) {
		if (!is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to run script \"" + name + "\" because it is not loaded");
			return 1;
		}

		return script->run_string(code);
	}
	int Script::run_file(const std::string& filename) {
		if (!is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to run script \"" + name + "\" because it is not loaded");
			return 1;
		}

		return script->run_file(filename);
	}
	int Script::run_func(const std::string& funcname) {
		if (!is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to run script \"" + name + "\" because it is not loaded");
			return 1;
		}

		return script->run_func(funcname);
	}
}

#endif // BEE_SCRIPT
