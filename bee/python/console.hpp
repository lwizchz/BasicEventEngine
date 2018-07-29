/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_CONSOLE_H
#define BEE_PYTHON_CONSOLE_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_console();

	PyObject* console_open(PyObject*, PyObject*);
	PyObject* console_close(PyObject*, PyObject*);
	PyObject* console_toggle(PyObject*, PyObject*);
	PyObject* console_get_is_open(PyObject*, PyObject*);

	PyObject* console_set_var(PyObject*, PyObject*);
	PyObject* console_get_var(PyObject*, PyObject*);

	PyObject* console_run(PyObject*, PyObject*);
	PyObject* console_log(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_CONSOLE_H
