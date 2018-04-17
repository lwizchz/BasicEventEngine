/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_FONT_H
#define BEE_PYTHON_FONT_H 1

#include <string>

#include <Python.h>

namespace bee { namespace python { namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} FontObject;

	PyObject* PyInit_bee_font(PyObject*);

	void Font_dealloc(FontObject*);
	PyObject* Font_new(PyTypeObject*, PyObject*, PyObject*);
	int Font_init(FontObject*, PyObject*, PyObject*);

	// Font methods
	PyObject* Font_repr(FontObject*, PyObject*);
	PyObject* Font_str(FontObject*, PyObject*);
	PyObject* Font_print(FontObject*, PyObject*);

	PyObject* Font_get_font_size(FontObject*, PyObject*);
	PyObject* Font_get_style(FontObject*, PyObject*);
	PyObject* Font_get_lineskip(FontObject*, PyObject*);
	PyObject* Font_get_lineskip_default(FontObject*, PyObject*);
	PyObject* Font_get_fontname(FontObject*, PyObject*);

	PyObject* Font_set_font_size(FontObject*, PyObject*);
	PyObject* Font_set_style(FontObject*, PyObject*);
	PyObject* Font_set_lineskip(FontObject*, PyObject*);

	PyObject* Font_load(FontObject*, PyObject*);
	PyObject* Font_free(FontObject*, PyObject*);

	PyObject* Font_draw(FontObject*, PyObject*);
	PyObject* Font_draw_fast(FontObject*, PyObject*);

	PyObject* Font_get_string_width(FontObject*, PyObject*);
	PyObject* Font_get_string_height(FontObject*, PyObject*);
}}}

#endif // BEE_PYTHON_FONT_H
