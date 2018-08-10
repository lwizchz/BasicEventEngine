/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_SCRIPT_H
#define BEE_PYTHON_SCRIPT_H 1

#include <string>

#include <Python.h>

namespace bee {
	class Script;
namespace python {
	PyObject* Script_from(const Script*);
	bool Script_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} ScriptObject;

	extern PyTypeObject ScriptType;

	PyObject* PyInit_bee_script(PyObject*);

	Script* as_script(ScriptObject*);
	Script* as_script(PyObject*);

	void Script_dealloc(ScriptObject*);
	PyObject* Script_new(PyTypeObject*, PyObject*, PyObject*);
	int Script_init(ScriptObject*, PyObject*, PyObject*);

	// Script methods
	PyObject* Script_repr(ScriptObject*);
	PyObject* Script_str(ScriptObject*);
	PyObject* Script_print(ScriptObject*, PyObject*);

	PyObject* Script_load(ScriptObject*, PyObject*);
	PyObject* Script_free(ScriptObject*, PyObject*);

	PyObject* Script_run_string(ScriptObject*, PyObject*);
	PyObject* Script_run_file(ScriptObject*, PyObject*);
	PyObject* Script_run_func(ScriptObject*, PyObject*);

	PyObject* Script_set_var(ScriptObject*, PyObject*);
	PyObject* Script_get_var(ScriptObject*, PyObject*);
}}}

#endif // BEE_PYTHON_SCRIPT_H
