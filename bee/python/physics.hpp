/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PHYSICS_H
#define BEE_PYTHON_PHYSICS_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_physics();

	PyObject* physics_get_shape_param_amount(PyObject*, PyObject*);
	PyObject* physics_get_constraint_param_amount(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_PHYSICS_H
