/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_FS_H
#define BEE_PYTHON_FS_H 1

#include <Python.h>

namespace bee { namespace python { namespace internal {
	PyObject* PyInit_bee_fs();

	PyObject* fs_add_filemap(PyObject*, PyObject*);

	PyObject* fs_exists(PyObject*, PyObject*);
	PyObject* fs_get_file(PyObject*, PyObject*);

	PyObject* fs_load_level(PyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_FS_H
