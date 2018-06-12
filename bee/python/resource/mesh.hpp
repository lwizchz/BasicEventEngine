/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MESH_H
#define BEE_PYTHON_MESH_H 1

#include <string>

#include <Python.h>

namespace bee { namespace python { namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} MeshObject;

	PyObject* PyInit_bee_mesh(PyObject*);

	void Mesh_dealloc(MeshObject*);
	PyObject* Mesh_new(PyTypeObject*, PyObject*, PyObject*);
	int Mesh_init(MeshObject*, PyObject*, PyObject*);

	// Mesh methods
	PyObject* Mesh_repr(MeshObject*);
	PyObject* Mesh_str(MeshObject*);
	PyObject* Mesh_print(MeshObject*, PyObject*);

	PyObject* Mesh_has_animation(MeshObject*, PyObject*);

	PyObject* Mesh_load(MeshObject*, PyObject*);
	PyObject* Mesh_free(MeshObject*, PyObject*);

	PyObject* Mesh_draw(MeshObject*, PyObject*);
}}}

#endif // BEE_PYTHON_MESH_H
