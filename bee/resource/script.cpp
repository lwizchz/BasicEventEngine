/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SCRIPT
#define BEE_SCRIPT 1

#include "script.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../messenger/messenger.hpp"

#include "../python/python.hpp"

namespace bee {
	std::map<int,Script*> Script::list;
	int Script::next_id = 0;

	/**
	* Default construct the Script.
	* @note This constructor should only be directly used for temporary Scripts, the other constructor should be used for all other cases.
	*/
	Script::Script() :
		Resource(),

		id(-1),
		name(),
		path(),

		script(nullptr),
		is_loaded(false)
	{}
	/**
	* Construct the Script, add it to the Script resource list, and set the new name and path.
	*
	* @throws int(-1) Failed to initialize Resource
	*/
	Script::Script(const std::string& _name, const std::string& _path) :
		Script() // Default initialize all variables
	{
		if (add_to_resources() < 0) { // Attempt to add the Script to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add script resource: \"" + _name + "\" from " + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Remove the Script from the resource list.
	*/
	Script::~Script() {
		this->free();
		list.erase(id);
	}

	/**
	* @returns the number of Script resources
	*/
	size_t Script::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Script* Script::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Script
	*
	* @returns the Script resource with the given name or nullptr if not found
	*/
	Script* Script::get_by_name(const std::string& name) {
		for (auto& script : list) { // Iterate over the Scripts in order to find the first one with the given name
			Script* s = script.second;
			if (s != nullptr) {
				if (s->get_name() == name) {
					return s; // Return the desired Script on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze, load, and return a newly created Script resource.
	* @param name the name to initialize the Script with
	* @param path the path to initialize the Script with
	*
	* @returns the newly loaded Script
	*/
	Script* Script::add(const std::string& name, const std::string& path) {
		Script* new_script = new Script(name, path);
		new_script->load();
		return new_script;
	}

	/**
	* @param path the path to check
	*
	* @returns the type of script that the given path represents
	*/
	E_SCRIPT_TYPE Script::get_type(const std::string& path) {
		if (path.substr(path.length()-3, 3) == ".py") {
			return E_SCRIPT_TYPE::PYTHON;
		}
		return E_SCRIPT_TYPE::INVALID;
	}

	/**
	* Add the Script to the appropriate resource list.
	*
	* @returns the Script id
	*/
	int Script::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Script::reset() {
		// Reset all properties
		name = "";
		path = "";

		script = nullptr;
		is_loaded = false;

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Script
	*/
	std::map<Variant,Variant> Script::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["script"] = script;
		info["is_loaded"] = is_loaded;

		return info;
	}
	/**
	* Restore the Script from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the script file
	*/
	int Script::deserialize(std::map<Variant,Variant>& m) {
		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		script = nullptr;
		is_loaded = false;

		if ((m["is_loaded"].i)&&(load())) {
			return 1;
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Script::print() const {
		Variant m (serialize());
		messenger::send({"engine", "script"}, E_MESSAGE::INFO, "Script " + m.to_str(true));
	}

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

	void Script::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Script resource directory.
	*/
	void Script::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path == ".py") { // If the path is "empty," use the Script type as the path
			path = _path;
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else {
			path = "resources/scripts/"+_path; // Append the path to the Script directory if no root
		}
	}

	/**
	* Load the Script from its path.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load since the script file is an invalid Script type
	* @retval 3 failed to load the Script interface
	*/
	int Script::load() {
		if (is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to load script \"" + name + "\" because it has already been loaded");
			return 1;
		}

		switch (Script::get_type(path)) {
			case E_SCRIPT_TYPE::PYTHON: {
				script = new PythonScriptInterface(path);
				if (script->load()) {
					return 3;
				}
				break;
			}
			case E_SCRIPT_TYPE::INVALID:
			default: {
				messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to load script \"" + name + "\" from \"" + path + "\": unknown script extension \"" + path.substr(path.length()-3, 3) + "\"");
				return 2;
			}
		}

		is_loaded = true;

		return 0;
	}
	/**
	* Free the Script.
	*
	* @retval 0 success
	*/
	int Script::free() {
		if (!is_loaded) { // Do not attempt to free the data if the Script has not been loaded
			return 0;
		}

		// Delete the Script interface
		if (script != nullptr) {
			delete script;
			script = nullptr;
		}

		// Set the loaded boolean
		is_loaded = false;

		return 0;
	}

	/**
	* Run the given code string in the interface.
	* @param code the code string to run
	* @param retval the pointer to store the code return value in
	*
	* @retval 0 success
	* @retval -1 failed since the Script isn't loaded
	* @returns other error codes based on the interface
	* @see ScriptInterface and PythonScriptInterface::run_string(const std::string&, Variant*, int)
	*/
	int Script::run_string(const std::string& code, Variant* retval) {
		if (!is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to run script \"" + name + "\" because it is not loaded");
			return -1;
		}

		return script->run_string(code, retval);
	}
	/**
	* Run the given file in the interface.
	* @param filename the file to run
	*
	* @retval 0 success
	* @retval -1 failed since the Script isn't loaded
	* @returns other error codes based on the interface
	* @see ScriptInterface and PythonScriptInterface::run_file(const string&)
	*/
	int Script::run_file(const std::string& filename) {
		if (!is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to run script \"" + name + "\" because it is not loaded");
			return -1;
		}

		return script->run_file(filename);
	}
	/**
	* Run the given function in the interface.
	* @param funcname the function to run
	* @param retval the pointer to store the function return value in
	*
	* @retval 0 success
	* @retval -1 failed since the Script isn't loaded
	* @returns other error codes based on the interface
	* @see ScriptInterface and PythonScriptInterface::run_func(const string&, Variant*)
	*/
	int Script::run_func(const std::string& funcname, Variant* retval) {
		if (!is_loaded) {
			messenger::send({"engine", "script"}, E_MESSAGE::WARNING, "Failed to run script \"" + name + "\" because it is not loaded");
			return -1;
		}

		return script->run_func(funcname, retval);
	}
}

#endif // BEE_SCRIPT
