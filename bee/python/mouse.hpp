/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MOUSE_H
#define BEE_PYTHON_MOUSE_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
        PyObject* PyInit_bee_mouse();

        PyObject* mouse_get_display_pos(PyObject*, PyObject*);
        PyObject* mouse_get_pos(PyObject*, PyObject*);
        PyObject* mouse_get_relative_pos(PyObject*, PyObject*);
        PyObject* mouse_set_display_pos(PyObject*, PyObject*);
        PyObject* mouse_set_pos(PyObject*, PyObject*);

        PyObject* mouse_is_inside(PyObject*, PyObject*);

        PyObject* mouse_get_state(PyObject*, PyObject*);

        PyObject* mouse_set_cursor(PyObject*, PyObject*);
        PyObject* mouse_set_show_cursor(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_MOUSE_H
