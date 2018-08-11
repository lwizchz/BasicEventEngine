/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SCRIPT_H
#define BEE_SCRIPT_H 1

#include <string> // Include the required library headers
#include <map>
#include <functional>

#include "resource.hpp"

#include "../data/variant.hpp"

namespace bee {
	/// Used to interface with different scripting systems
	class ScriptInterface {
	public:
		virtual ~ScriptInterface() {};

		virtual int load() =0;
		virtual void free() =0;
		virtual void release() =0;

		virtual int run_string(const std::string&, Variant*) =0;
		virtual int run_file(const std::string&) =0;
		virtual int run_func(const std::string&, const Variant&, Variant*) =0;

		virtual int set_var(const std::string&, const Variant&) =0;
		virtual bool has_var(const std::string&) const =0;
		virtual Variant get_var(const std::string&) const =0;
	};

	/// Used to execute Python scripts
	class Script: public Resource {
		static std::map<size_t,Script*> list;
		static size_t next_id;

		size_t id; ///< The unique Script identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the script file

		ScriptInterface* script; ///< The scripting interface
		bool is_loaded; ///< Whether the script was successfully loaded into its interface
	public:
		// See bee/resource/script.cpp for function comments
		Script();
		Script(const std::string&, const std::string&);
		~Script();

		static size_t get_amount();
		static Script* get(size_t);
		static Script* get_by_name(const std::string&);
		static Script* add(const std::string&, const std::string&);

		static E_SCRIPT_TYPE get_type(const std::string&);

		size_t add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		size_t get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		ScriptInterface* get_interface() const;
		bool get_is_loaded() const;

		void set_name(const std::string&);
		void set_path(const std::string&);

		int load();
		int free();

		int run_string(const std::string&, Variant*);
		int run_file(const std::string&);
		int run_func(const std::string&, const Variant&, Variant*);
	};
}

#endif // BEE_SCRIPT_H
