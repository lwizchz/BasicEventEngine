/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_KB_H
#define BEE_PYTHON_KB_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
        PyObject* PyInit_bee_kb();

        PyObject* kb_get_state(PyObject*, PyObject*);
        PyObject* kb_get_mod_state(PyObject*, PyObject*);

        PyObject* kb_append_input(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_KB_H
