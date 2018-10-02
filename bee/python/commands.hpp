/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_COMMANDS_H
#define BEE_PYTHON_COMMANDS_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_commands();

	PyObject* commands_quit(PyObject*, PyObject*);
	PyObject* commands_find(PyObject*, PyObject*);
	PyObject* commands_clear(PyObject*, PyObject*);

	PyObject* commands_execfile(PyObject*, PyObject*);
	PyObject* commands_load_map(PyObject*, PyObject*);
	PyObject* commands_log(PyObject*, PyObject*);

	PyObject* commands_bind(PyObject*, PyObject*);
	PyObject* commands_unbind(PyObject*, PyObject*);

	PyObject* commands_screenshot(PyObject*, PyObject*);

	PyObject* commands_verbosity(PyObject*, PyObject*);

	PyObject* commands_volume(PyObject*, PyObject*);

	PyObject* commands_info(PyObject*, PyObject*);
	PyObject* commands_restart(PyObject*, PyObject*);
	PyObject* commands_restart_room(PyObject*, PyObject*);
	PyObject* commands_pause(PyObject*, PyObject*);

	PyObject* commands_netstatus(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_COMMANDS_H
