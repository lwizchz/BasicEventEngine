/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_BEEMODULE_H
#define BEE_PYTHON_BEEMODULE_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
		int init_module();
		PyObject* PyInit_bee();

		PyObject* get_ticks(PyObject*, PyObject*);
		PyObject* get_seconds(PyObject*, PyObject*);
		PyObject* get_frame(PyObject*, PyObject*);
		PyObject* get_delta(PyObject*, PyObject*);
		PyObject* get_tick_delta(PyObject*, PyObject*);
		PyObject* get_fps_goal(PyObject*, PyObject*);

		PyObject* restart_game(PyObject*, PyObject*);
		PyObject* end_game(PyObject*, PyObject*);

		PyObject* core_get_display(PyObject*, PyObject*);
		PyObject* core_get_display_size(PyObject*, PyObject*);
		PyObject* core_get_display_refresh_rate(PyObject*, PyObject*);
		PyObject* core_set_display(PyObject*, PyObject*);
		PyObject* core_set_display_size(PyObject*, PyObject*);
		PyObject* core_set_display_refresh_rate(PyObject*, PyObject*);

		PyObject* core_restart_room(PyObject*, PyObject*);
		PyObject* core_change_room(PyObject*, PyObject*);
		PyObject* core_get_current_room(PyObject*, PyObject*);
		PyObject* core_get_room_size(PyObject*, PyObject*);
		PyObject* core_is_on_screen(PyObject*, PyObject*);
		PyObject* core_set_is_paused(PyObject*, PyObject*);
		PyObject* core_get_is_paused(PyObject*, PyObject*);

		PyObject* core_get_window_title(PyObject*, PyObject*);
		PyObject* core_get_window(PyObject*, PyObject*);
		PyObject* core_get_window_pos(PyObject*, PyObject*);
		PyObject* core_get_window_size(PyObject*, PyObject*);
		PyObject* core_set_window_title(PyObject*, PyObject*);
		PyObject* core_set_show_cursor(PyObject*, PyObject*);
		PyObject* core_set_window_pos(PyObject*, PyObject*);
		PyObject* core_set_window_center(PyObject*, PyObject*);
		PyObject* core_set_window_size(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_BEEMODULE_H
