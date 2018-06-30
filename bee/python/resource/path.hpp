/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PATH_H
#define BEE_PYTHON_PATH_H 1

#include <string>

#include <Python.h>

namespace bee { namespace python { namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} PathObject;

	extern PyTypeObject PathType;

	PyObject* PyInit_bee_path(PyObject*);

	void Path_dealloc(PathObject*);
	PyObject* Path_new(PyTypeObject*, PyObject*, PyObject*);
	int Path_init(PathObject*, PyObject*, PyObject*);

	// Path methods
	PyObject* Path_repr(PathObject*);
	PyObject* Path_str(PathObject*);
	PyObject* Path_print(PathObject*, PyObject*);

	PyObject* Path_get_nodes(PathObject*, PyObject*);

	PyObject* Path_add_node(PathObject*, PyObject*);
	PyObject* Path_add_control_point(PathObject*, PyObject*);
	PyObject* Path_remove_node(PathObject*, PyObject*);
	PyObject* Path_remove_control_point(PathObject*, PyObject*);

	PyObject* Path_load(PathObject*, PyObject*);

	PyObject* Path_draw(PathObject*, PyObject*);
}}}

#endif // BEE_PYTHON_PATH_H
