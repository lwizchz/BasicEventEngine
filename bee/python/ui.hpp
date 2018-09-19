/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_UI_H
#define BEE_PYTHON_UI_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_ui();

	PyObject* ui_load(PyObject*, PyObject*);
	PyObject* ui_free(PyObject*, PyObject*);

	PyObject* ui_destroy_parent(PyObject*, PyObject*);

	PyObject* ui_create_button(PyObject*, PyObject*);
	PyObject* ui_button_callback(PyObject*, PyObject*);

	PyObject* ui_create_handle(PyObject*, PyObject*);
	PyObject* ui_destroy_handle(PyObject*, PyObject*);

	PyObject* ui_create_text_entry(PyObject*, PyObject*);
	PyObject* ui_add_text_entry_completor(PyObject*, PyObject*);
	PyObject* ui_add_text_entry_handler(PyObject*, PyObject*);
	PyObject* ui_text_entry_callback(PyObject*, PyObject*);
	PyObject* ui_text_entry_completor(PyObject*, PyObject*);
	PyObject* ui_text_entry_handler(PyObject*, PyObject*);

	PyObject* ui_create_gauge(PyObject*, PyObject*);

	PyObject* ui_create_slider(PyObject*, PyObject*);
	PyObject* ui_slider_callback(PyObject*, PyObject*);

	PyObject* ui_create_optionbox(PyObject*, PyObject*);
	PyObject* ui_push_optionbox_option(PyObject*, PyObject*);
	PyObject* ui_pop_optionbox_option(PyObject*, PyObject*);
	PyObject* ui_reset_optionbox_options(PyObject*, PyObject*);
	PyObject* ui_optionbox_callback(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_UI_H
