/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_LIGHT_H
#define BEE_PYTHON_LIGHT_H 1

#include <string>

#include <Python.h>

namespace bee {
	class Light;
namespace python {
	PyObject* Light_from(Light*);
	bool Light_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} LightObject;

	extern PyTypeObject LightType;

	PyObject* PyInit_bee_light(PyObject*);

	Light* as_light(LightObject*);
	Light* as_light(PyObject*);

	void Light_dealloc(LightObject*);
	PyObject* Light_new(PyTypeObject*, PyObject*, PyObject*);
	int Light_init(LightObject*, PyObject*, PyObject*);

	// Light methods
	PyObject* Light_repr(LightObject*);
	PyObject* Light_str(LightObject*);
	PyObject* Light_print(LightObject*, PyObject*);

	PyObject* Light_get_type(LightObject*, PyObject*);
	PyObject* Light_get_position(LightObject*, PyObject*);
	PyObject* Light_get_direction(LightObject*, PyObject*);
	PyObject* Light_get_attenuation(LightObject*, PyObject*);
	PyObject* Light_get_color(LightObject*, PyObject*);

	PyObject* Light_set_type(LightObject*, PyObject*);
	PyObject* Light_set_position(LightObject*, PyObject*);
	PyObject* Light_set_direction(LightObject*, PyObject*);
	PyObject* Light_set_attenuation(LightObject*, PyObject*);
	PyObject* Light_set_color(LightObject*, PyObject*);

	PyObject* Light_load(LightObject*, PyObject*);

	PyObject* Light_queue(LightObject*, PyObject*);
}}}

#endif // BEE_PYTHON_LIGHT_H
