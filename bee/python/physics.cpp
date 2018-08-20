/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PHYSICS
#define BEE_PYTHON_PHYSICS 1

#include "physics.hpp"

#include "../physics/physics.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEEPhysicsMethods[] = {
		{"get_shape_param_amount", physics_get_shape_param_amount, METH_VARARGS, ""},
		{"get_constraint_param_amount", physics_get_constraint_param_amount, METH_VARARGS, ""},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEEPhysicsModule = {
		PyModuleDef_HEAD_INIT, "physics", nullptr, -1, BEEPhysicsMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_physics() {
		return PyModule_Create(&BEEPhysicsModule);
	}

	PyObject* physics_get_shape_param_amount(PyObject* self, PyObject* args) {
		int type;
		int p0 = 0;

		if (!PyArg_ParseTuple(args, "i|i", &type, &p0)) {
			return nullptr;
		}

		E_PHYS_SHAPE _type = static_cast<E_PHYS_SHAPE>(type);

		return Py_BuildValue("n", physics::get_shape_param_amount(_type, p0));
	}
	PyObject* physics_get_constraint_param_amount(PyObject* self, PyObject* args) {
		int type;

		if (!PyArg_ParseTuple(args, "i", &type)) {
			return nullptr;
		}

		E_PHYS_CONSTRAINT _type = static_cast<E_PHYS_CONSTRAINT>(type);

		return Py_BuildValue("n", physics::get_constraint_param_amount(_type));
	}
}}}

#endif // BEE_PYTHON_PHYSICS
