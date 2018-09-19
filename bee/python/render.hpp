/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_RENDER_H
#define BEE_PYTHON_RENDER_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_render();

	PyObject* render_set_is_lightable(PyObject*, PyObject*);

	PyObject* render_set_3d(PyObject*, PyObject*);
	PyObject* render_set_camera(PyObject*, PyObject*);
	PyObject* render_get_3d(PyObject*, PyObject*);
	PyObject* render_get_camera(PyObject*, PyObject*);

	PyObject* render_get_transition_type(PyObject*, PyObject*);
	PyObject* render_set_transition_type(PyObject*, PyObject*);
	PyObject* render_set_transition_custom(PyObject*, PyObject*);
	PyObject* render_get_transition_speed(PyObject*, PyObject*);
	PyObject* render_set_transition_speed(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_RENDER_H
