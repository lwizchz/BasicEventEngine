/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PHYSICS_WORLD
#define BEE_PYTHON_PHYSICS_WORLD 1

#include <Python.h>
#include <structmember.h>

#include "world.hpp"

#include "../python.hpp"

#include "../../physics/physics.hpp"
#include "../../physics/world.hpp"
#include "../../physics/body.hpp"

#include "body.hpp"

namespace bee { namespace python {
	PyObject* PhysicsWorld_from(std::weak_ptr<PhysicsWorld> world) {
		PyObject* py_world = internal::PhysicsWorld_new(&internal::PhysicsWorldType, nullptr, nullptr);
		internal::PhysicsWorldObject* _py_world = reinterpret_cast<internal::PhysicsWorldObject*>(py_world);

		_py_world->world = world;

		return py_world;
	}
	bool PhysicsWorld_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::PhysicsWorldType);
	}
namespace internal {
	PyMethodDef PhysicsWorldMethods[] = {
		{"get_gravity", reinterpret_cast<PyCFunction>(PhysicsWorld_get_gravity), METH_NOARGS, ""},
		{"get_scale", reinterpret_cast<PyCFunction>(PhysicsWorld_get_scale), METH_NOARGS, ""},

		{"set_gravity", reinterpret_cast<PyCFunction>(PhysicsWorld_set_gravity), METH_VARARGS, ""},
		{"set_scale", reinterpret_cast<PyCFunction>(PhysicsWorld_set_scale), METH_VARARGS, ""},

		{"add_body", reinterpret_cast<PyCFunction>(PhysicsWorld_add_body), METH_VARARGS, ""},
		{"add_constraint", reinterpret_cast<PyCFunction>(PhysicsWorld_add_constraint), METH_VARARGS, ""},

		{"remove_body", reinterpret_cast<PyCFunction>(PhysicsWorld_remove_body), METH_VARARGS, ""},

		{"step", reinterpret_cast<PyCFunction>(PhysicsWorld_step), METH_VARARGS, ""},

		{"draw_debug", reinterpret_cast<PyCFunction>(PhysicsWorld_draw_debug), METH_NOARGS, ""},

		{nullptr, nullptr, 0, nullptr}
	};

	PyTypeObject PhysicsWorldType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.PhysicsWorld",
		sizeof(PhysicsWorldObject), 0,
		reinterpret_cast<destructor>(PhysicsWorld_dealloc),
		0,
		0, 0,
		0,
		0,
		0, 0, 0,
		0,
		0,
		0,
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"PhysicsWorld objects",
		0,
		0,
		0,
		0,
		0, 0,
		PhysicsWorldMethods,
		0,
		0,
		0,
		0,
		0, 0,
		0,
		0,
		0, PhysicsWorld_new,
		0, 0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};

	PyObject* PyInit_bee_physics_world(PyObject* module) {
		PhysicsWorldType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&PhysicsWorldType) < 0) {
			return nullptr;
		}

		Py_INCREF(&PhysicsWorldType);
		PyModule_AddObject(module, "PhysicsWorld", reinterpret_cast<PyObject*>(&PhysicsWorldType));

		return reinterpret_cast<PyObject*>(&PhysicsWorldType);
	}

	std::weak_ptr<PhysicsWorld> as_physics_world(PhysicsWorldObject* self) {
		return self->world;
	}
	std::weak_ptr<PhysicsWorld> as_physics_world(PyObject* self) {
		if (PhysicsWorld_check(self)) {
			return as_physics_world(reinterpret_cast<PhysicsWorldObject*>(self));
		}
		return {};
	}

	void PhysicsWorld_dealloc(PhysicsWorldObject* self) {
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* PhysicsWorld_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		PhysicsWorldObject* self;

		self = reinterpret_cast<PhysicsWorldObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->world = {};
		}

		return reinterpret_cast<PyObject*>(self);
	}

	PyObject* PhysicsWorld_get_gravity(PhysicsWorldObject* self, PyObject* args) {
		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		btVector3 grav = world->get_gravity();

		double x = grav.x();
		double y = grav.y();
		double z = grav.z();

		return Py_BuildValue("(ddd)", x, y, z);
	}
	PyObject* PhysicsWorld_get_scale(PhysicsWorldObject* self, PyObject* args) {
		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", world->get_scale());
	}

	PyObject* PhysicsWorld_set_gravity(PhysicsWorldObject* self, PyObject* args) {
		double x, y, z;

		if (!PyArg_ParseTuple(args, "(ddd)", &x, &y, &z)) {
			return nullptr;
		}

		btVector3 grav (x, y, z);

		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		world->set_gravity(grav);

		Py_RETURN_NONE;
	}
	PyObject* PhysicsWorld_set_scale(PhysicsWorldObject* self, PyObject* args) {
		double scale;

		if (!PyArg_ParseTuple(args, "d", &scale)) {
			return nullptr;
		}

		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", world->set_scale(scale));
	}

	PyObject* PhysicsWorld_add_body(PhysicsWorldObject* self, PyObject* args) {
		PhysicsBodyObject* pbobj;

		if (!PyArg_ParseTuple(args, "O!", &PhysicsBodyType, &pbobj)) {
			return nullptr;
		}

		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		auto physbody = as_physics_body(pbobj).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", world->add_physbody(physbody));
	}
	PyObject* PhysicsWorld_add_constraint(PhysicsWorldObject* self, PyObject* args) {
		int type;
		PyObject* params;
		PhysicsBodyObject* pbobj1;
		PhysicsBodyObject* pbobj2 = nullptr;

		if (!PyArg_ParseTuple(args, "iO!O!|O!", &type, &PyTuple_Type, &params, &PhysicsBodyType, &pbobj1, &PhysicsBodyType, &pbobj2)) {
			return nullptr;
		}

		E_PHYS_CONSTRAINT _type = static_cast<E_PHYS_CONSTRAINT>(type);

		const size_t amount = physics::get_constraint_param_amount(_type);
		double* _params = new double[amount];
		for (size_t i=0; i<amount; ++i) {
			PyObject* item = PyTuple_GetItem(params, i);
			if (item == nullptr) {
				PyErr_SetString(PyExc_RuntimeError, "missing constraint parameters for this type");
				return nullptr;
			}

			_params[i] = PyFloat_AsDouble(item);
		}

		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		auto physbody1 = as_physics_body(pbobj1).lock();
		if (physbody1 == nullptr) {
			return nullptr;
		}
		auto physbody2 = as_physics_body(pbobj2).lock();

		if (physbody2 == nullptr) {
			world->add_constraint(_type, physbody1->get_body(), _params);
		} else {
			world->add_constraint(_type, physbody1->get_body(), physbody2->get_body(), _params);
		}

		Py_RETURN_NONE;
	}

	PyObject* PhysicsWorld_remove_body(PhysicsWorldObject* self, PyObject* args) {
		PhysicsBodyObject* pbobj;

		if (!PyArg_ParseTuple(args, "O!", &PhysicsBodyType, &pbobj)) {
			return nullptr;
		}

		auto physbody = as_physics_body(pbobj).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		world->remove_body(physbody->get_body());

		Py_RETURN_NONE;
	}

	PyObject* PhysicsWorld_step(PhysicsWorldObject* self, PyObject* args) {
		double step_amount;

		if (!PyArg_ParseTuple(args, "d", &step_amount)) {
			return nullptr;
		}

		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", world->step(step_amount));
	}

	PyObject* PhysicsWorld_draw_debug(PhysicsWorldObject* self, PyObject* args) {
		auto world = as_physics_world(self).lock();
		if (world == nullptr) {
			return nullptr;
		}

		world->draw_debug();

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_PHYSICS_WORLD
