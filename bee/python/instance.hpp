/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_INSTANCE_H
#define BEE_PYTHON_INSTANCE_H 1

#include <string>

#include <Python.h>

namespace bee { namespace python { namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
		int num;
	} InstanceObject;

	PyObject* PyInit_bee_instance(PyObject*);

	void Instance_dealloc(InstanceObject*);
	PyObject* Instance_new(PyTypeObject*, PyObject*, PyObject*);
	int Instance_init(InstanceObject*, PyObject*, PyObject*);

	// Instance methods
	PyObject* Instance_repr(InstanceObject*);
	PyObject* Instance_str(InstanceObject*);
	PyObject* Instance_print(InstanceObject*, PyObject*);

	PyObject* Instance_set_alarm(InstanceObject*, PyObject*);

	PyObject* Instance_set_sprite(InstanceObject*, PyObject*);
	PyObject* Instance_set_computation_type(InstanceObject*, PyObject*);
	PyObject* Instance_set_is_persistent(InstanceObject*, PyObject*);

	PyObject* Instance_get_data(InstanceObject*, PyObject*);
	PyObject* Instance_set_data(InstanceObject*, PyObject*);

	PyObject* Instance_get_pos(InstanceObject*, PyObject*);
	PyObject* Instance_get_aabb(InstanceObject*, PyObject*);
	PyObject* Instance_get_corner(InstanceObject*, PyObject*);
	PyObject* Instance_get_start(InstanceObject*, PyObject*);

	PyObject* Instance_get_sprite(InstanceObject*, PyObject*);
	PyObject* Instance_get_mass(InstanceObject*, PyObject*);
	PyObject* Instance_get_computation_type(InstanceObject*, PyObject*);
	PyObject* Instance_get_is_persistent(InstanceObject*, PyObject*);

	PyObject* Instance_set_pos(InstanceObject*, PyObject*);
	PyObject* Instance_set_to_start(InstanceObject*, PyObject*);
	PyObject* Instance_set_corner(InstanceObject*, PyObject*);
	PyObject* Instance_set_mass(InstanceObject*, PyObject*);
	PyObject* Instance_move(InstanceObject*, PyObject*);
	PyObject* Instance_set_friction(InstanceObject*, PyObject*);
	PyObject* Instance_set_gravity(InstanceObject*, PyObject*);
	PyObject* Instance_set_velocity(InstanceObject*, PyObject*);
	PyObject* Instance_add_velocity(InstanceObject*, PyObject*);
	PyObject* Instance_limit_velocity(InstanceObject*, PyObject*, PyObject*);

	PyObject* Instance_get_speed(InstanceObject*, PyObject*);
	PyObject* Instance_get_velocity(InstanceObject*, PyObject*);
	PyObject* Instance_get_velocity_ang(InstanceObject*, PyObject*);
	PyObject* Instance_get_friction(InstanceObject*, PyObject*);
	PyObject* Instance_get_gravity(InstanceObject*, PyObject*);
}}}

#endif // BEE_PYTHON_INSTANCE_H
