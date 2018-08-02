/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_OBJECT_H
#define BEE_PYTHON_OBJECT_H 1

#include <Python.h>

namespace bee {
	class Object;
namespace python {
	PyObject* Object_from(Object*);
	bool Object_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} ObjectObject;

	extern PyTypeObject ObjectType;

	PyObject* PyInit_bee_object(PyObject*);

	Object* as_object(ObjectObject*);
	Object* as_object(PyObject*);

	void Object_dealloc(ObjectObject*);
	PyObject* Object_new(PyTypeObject*, PyObject*, PyObject*);
	int Object_init(ObjectObject*, PyObject*, PyObject*);

	// Object methods
	PyObject* Object_repr(ObjectObject*);
	PyObject* Object_str(ObjectObject*);
	PyObject* Object_print(ObjectObject*, PyObject*);

	PyObject* Object_get_sprite(ObjectObject*, PyObject*);
	PyObject* Object_get_is_persistent(ObjectObject*, PyObject*);
	PyObject* Object_get_depth(ObjectObject*, PyObject*);
	PyObject* Object_get_parent(ObjectObject*, PyObject*);
	PyObject* Object_get_mask_offset(ObjectObject*, PyObject*);
	PyObject* Object_get_is_pausable(ObjectObject*, PyObject*);
	PyObject* Object_get_events(ObjectObject*, PyObject*);

	PyObject* Object_set_sprite(ObjectObject*, PyObject*);
	PyObject* Object_set_is_persistent(ObjectObject*, PyObject*);
	PyObject* Object_set_depth(ObjectObject*, PyObject*);
	PyObject* Object_set_parent(ObjectObject*, PyObject*);
	PyObject* Object_set_mask_offset(ObjectObject*, PyObject*);
	PyObject* Object_set_is_pausable(ObjectObject*, PyObject*);

	PyObject* Object_get_instances(ObjectObject*, PyObject*);
	PyObject* Object_get_instance_amount(ObjectObject*, PyObject*);
	PyObject* Object_get_instance_at(ObjectObject*, PyObject*);
}}}

#endif // BEE_PYTHON_OBJECT_H
