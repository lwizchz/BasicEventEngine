/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_LOADER_H
#define BEE_PYTHON_LOADER_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_loader();

	PyObject* loader_queue(PyObject*, PyObject*);
	PyObject* loader_clear(PyObject*, PyObject*);

	PyObject* loader_load(PyObject*, PyObject*);
	PyObject* loader_load_lazy(PyObject*, PyObject*);

	PyObject* loader_get_amount_loaded(PyObject*, PyObject*);
	PyObject* loader_get_total(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_LOADER_H
