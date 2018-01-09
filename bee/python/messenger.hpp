/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MESSENGER_H
#define BEE_PYTHON_MESSENGER_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
        PyObject* PyInit_bee_messenger();

        PyObject* messenger_register_recipient(PyObject*, PyObject*);
        PyObject* messenger_unregister(PyObject*, PyObject*);

        PyObject* messenger_send(PyObject*, PyObject*);
        PyObject* messenger_log(PyObject*, PyObject*);

        PyObject* messenger_add_filter(PyObject*, PyObject*);
        PyObject* messenger_set_filter_blacklist(PyObject*, PyObject*);
        PyObject* messenger_reset_filter(PyObject*, PyObject*);

        PyObject* messenger_add_log(PyObject*, PyObject*);
        PyObject* messenger_remove_log(PyObject*, PyObject*);
        PyObject* messenger_clear_logs(PyObject*, PyObject*);

        PyObject* messenger_set_level(PyObject*, PyObject*);
        PyObject* messenger_get_level(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_MESSENGER_H
