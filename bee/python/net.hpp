/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_NET_H
#define BEE_PYTHON_NET_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_net();

	PyObject* net_init(PyObject*, PyObject*);
	PyObject* net_get_is_initialized(PyObject*, PyObject*);
	PyObject* net_get_time(PyObject*, PyObject*);
	PyObject* net_close(PyObject*, PyObject*);

	PyObject* net_handle_events(PyObject*, PyObject*);

	PyObject* net_session_start(PyObject*, PyObject*);
	PyObject* net_session_find(PyObject*, PyObject*);
	PyObject* net_session_join(PyObject*, PyObject*);
	PyObject* net_get_is_connected(PyObject*, PyObject*);
	PyObject* net_session_end(PyObject*, PyObject*);

	PyObject* net_session_sync_data(PyObject*, PyObject*);
	PyObject* net_session_sync_instance(PyObject*, PyObject*);
	PyObject* net_session_sync_player(PyObject*, PyObject*);

	PyObject* net_get_print(PyObject*, PyObject*);
	PyObject* net_get_players(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_NET_H
