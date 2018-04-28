/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_H
#define BEE_PYTHON_H 1

#include <string>

#include <Python.h>

#include "../resource/script.hpp"

namespace bee { namespace python {
	int init();
	int close();

	int run_string(const std::string&);
	int run_file(const std::string&);

	std::string get_traceback();

	void set_displayhook(PyObject*);
	Variant get_displayhook();

	Variant pyobj_to_variant(PyObject*);
	PyObject* variant_to_pyobj(Variant);
}

	class PythonScriptInterface: public ScriptInterface {
		std::string path;
		PyObject* module;
	public:
		PythonScriptInterface(const std::string&);
		PythonScriptInterface(PyObject*);
		~PythonScriptInterface();

		int load();
		void free();
		void release();

		int run_string(const std::string&, Variant*, int);
		int run_string(const std::string&, Variant*);
		int run_file(const std::string&);
		int run_func(const std::string&, Variant*);

		int set_var(const std::string&, const Variant&);
		Variant get_var(const std::string&) const;

		std::vector<Variant> complete(const std::string&) const;
	};
}

#endif // BEE_PYTHON_H
