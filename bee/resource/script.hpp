/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SCRIPT_H
#define BEE_SCRIPT_H 1

#include <string> // Include the required library headers
#include <map>

#include "resource.hpp"

namespace bee {
	class ScriptInterface {
	public:
		virtual ~ScriptInterface() {};
		virtual int load() =0;
		virtual void free() =0;
		virtual void release() =0;
		virtual int run_string(const std::string&) =0;
		virtual int run_file(const std::string&) =0;
		virtual int run_func(const std::string&) =0;
	};

	class Script: public Resource { // The script resource class is used to execute python scripts
		static std::map<int,Script*> list;
		static int next_id;

		int id; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the file to load the script from

		ScriptInterface* script; // The script object
		bool is_loaded; // Whether the script was successfully loaded into its interface
	public:
		// See bee/resources/script.cpp for function comments
		Script();
		Script(const std::string&, const std::string&);
		~Script();

		static size_t get_amount();
		static Script* get(int);
		static Script* get_by_name(const std::string&);
		static Script* add(const std::string&, const std::string&);

		int add_to_resources();
		int reset();
		int print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;

		int set_name(const std::string&);
		int set_path(const std::string&);

		int load();
		int free();
		int run_string(const std::string&);
		int run_file(const std::string&);
		int run_func(const std::string&);
	};
}

#endif // BEE_SCRIPT_H
