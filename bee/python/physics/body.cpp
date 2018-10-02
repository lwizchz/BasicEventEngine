/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PHYSICS_BODY
#define BEE_PYTHON_PHYSICS_BODY 1

#include <Python.h>
#include <structmember.h>

#include "body.hpp"

#include "../python.hpp"

#include "../../core/instance.hpp"

#include "../../physics/physics.hpp"
#include "../../physics/body.hpp"

#include "../instance.hpp"
#include "world.hpp"

namespace bee { namespace python {
	PyObject* PhysicsBody_from(std::weak_ptr<PhysicsBody> body) {
		PyObject* py_pb = internal::PhysicsBody_new(&internal::PhysicsBodyType, nullptr, nullptr);
		internal::PhysicsBodyObject* _py_pb = reinterpret_cast<internal::PhysicsBodyObject*>(py_pb);

		_py_pb->body = body;

		return py_pb;
	}
	bool PhysicsBody_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::PhysicsBodyType);
	}
namespace internal {
	PyMethodDef PhysicsBodyMethods[] = {
		{"attach", reinterpret_cast<PyCFunction>(PhysicsBody_attach), METH_VARARGS, ""},
		{"remove", reinterpret_cast<PyCFunction>(PhysicsBody_remove), METH_NOARGS, ""},

		{"get_shape_type", reinterpret_cast<PyCFunction>(PhysicsBody_get_shape_type), METH_NOARGS, ""},
		{"get_mass", reinterpret_cast<PyCFunction>(PhysicsBody_get_mass), METH_NOARGS, ""},
		{"get_scale", reinterpret_cast<PyCFunction>(PhysicsBody_get_scale), METH_NOARGS, ""},
		{"get_inertia", reinterpret_cast<PyCFunction>(PhysicsBody_get_inertia), METH_NOARGS, ""},
		{"get_world", reinterpret_cast<PyCFunction>(PhysicsBody_get_world), METH_NOARGS, ""},
		{"get_instance", reinterpret_cast<PyCFunction>(PhysicsBody_get_instance), METH_NOARGS, ""},
		{"get_constraints", reinterpret_cast<PyCFunction>(PhysicsBody_get_constraints), METH_NOARGS, ""},

		{"get_pos", reinterpret_cast<PyCFunction>(PhysicsBody_get_pos), METH_NOARGS, ""},
		{"get_rotation", reinterpret_cast<PyCFunction>(PhysicsBody_get_rotation), METH_NOARGS, ""},

		{"set_shape", reinterpret_cast<PyCFunction>(PhysicsBody_set_shape), METH_VARARGS, ""},
		{"set_mass", reinterpret_cast<PyCFunction>(PhysicsBody_set_mass), METH_VARARGS, ""},
		{"set_friction", reinterpret_cast<PyCFunction>(PhysicsBody_set_friction), METH_VARARGS, ""},

		{"add_constraint", reinterpret_cast<PyCFunction>(PhysicsBody_add_constraint), METH_VARARGS, ""},
		{"remove_constraints", reinterpret_cast<PyCFunction>(PhysicsBody_remove_constraints), METH_NOARGS, ""},

		{"update_state", reinterpret_cast<PyCFunction>(PhysicsBody_update_state), METH_NOARGS, ""},

		{nullptr, nullptr, 0, nullptr}
	};

	PyTypeObject PhysicsBodyType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.PhysicsBody",
		sizeof(PhysicsBodyObject), 0,
		reinterpret_cast<destructor>(PhysicsBody_dealloc),
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
		"PhysicsBody objects",
		0,
		0,
		0,
		0,
		0, 0,
		PhysicsBodyMethods,
		0,
		0,
		0,
		0,
		0, 0,
		0,
		0,
		0, PhysicsBody_new,
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

	PyObject* PyInit_bee_physics_body(PyObject* module) {
		PhysicsBodyType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&PhysicsBodyType) < 0) {
			return nullptr;
		}

		Py_INCREF(&PhysicsBodyType);
		PyModule_AddObject(module, "PhysicsBody", reinterpret_cast<PyObject*>(&PhysicsBodyType));

		return reinterpret_cast<PyObject*>(&PhysicsBodyType);
	}

	std::weak_ptr<PhysicsBody> as_physics_body(PhysicsBodyObject* self) {
		return self->body;
	}
	std::weak_ptr<PhysicsBody> as_physics_body(PyObject* self) {
		if (PhysicsBody_check(self)) {
			return as_physics_body(reinterpret_cast<PhysicsBodyObject*>(self));
		}
		return {};
	}

	void PhysicsBody_dealloc(PhysicsBodyObject* self) {
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* PhysicsBody_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		PhysicsBodyObject* self;

		self = reinterpret_cast<PhysicsBodyObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->body = {};
		}

		return reinterpret_cast<PyObject*>(self);
	}

	PyObject* PhysicsBody_attach(PhysicsBodyObject* self, PyObject* args) {
		PhysicsWorldObject* world;

		if (!PyArg_ParseTuple(args, "O!", &PhysicsWorldType, &world)) {
			return nullptr;
		}

		auto _world = as_physics_world(world).lock();
		if (_world == nullptr) {
			return nullptr;
		}

		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		physbody->attach(_world);

		Py_RETURN_NONE;
	}
	PyObject* PhysicsBody_remove(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		physbody->remove();

		Py_RETURN_NONE;
	}

	PyObject* PhysicsBody_get_shape_type(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		E_PHYS_SHAPE type = physbody->get_shape_type();

		return Py_BuildValue("i", static_cast<int>(type));
	}
	PyObject* PhysicsBody_get_mass(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", physbody->get_mass());
	}
	PyObject* PhysicsBody_get_scale(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", physbody->get_scale());
	}
	PyObject* PhysicsBody_get_inertia(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		btVector3 inertia = physbody->get_inertia();

		double x = inertia.x();
		double y = inertia.y();
		double z = inertia.z();

		return Py_BuildValue("(ddd)", x, y, z);
	}
	PyObject* PhysicsBody_get_world(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return PhysicsWorld_from(physbody->get_world());
	}
	PyObject* PhysicsBody_get_instance(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Instance_from(physbody->get_instance());
	}
	PyObject* PhysicsBody_get_constraints(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		auto& constraints = physbody->get_constraints();
		PyObject* _constraints = PyList_New(constraints.size());
		size_t i = 0;
		for (auto& c : constraints) {
			E_PHYS_CONSTRAINT type = std::get<0>(c);

			const double* params = std::get<1>(c);
			const size_t param_amount = physics::get_constraint_param_amount(type);
			PyObject* _params = PyTuple_New(param_amount);
			for (size_t j=0; j<param_amount; ++j) {
				PyTuple_SetItem(_params, j, PyFloat_FromDouble(params[j]));
			}

			PyList_SetItem(_constraints, i++, Py_BuildValue("(iN)", static_cast<int>(type), _params));
		}

		return _constraints;
	}

	PyObject* PhysicsBody_get_pos(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		btVector3 pos = physbody->get_pos();

		double x = pos.x();
		double y = pos.y();
		double z = pos.z();

		return Py_BuildValue("(ddd)", x, y, z);
	}
	PyObject* PhysicsBody_get_rotation(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("(ddd)", physbody->get_rotation_x(), physbody->get_rotation_y(), physbody->get_rotation_z());
	}

	PyObject* PhysicsBody_set_shape(PhysicsBodyObject* self, PyObject* args) {
		int type;
		PyObject* params;

		if (!PyArg_ParseTuple(args, "iO!", &type, &PyTuple_Type, &params)) {
			return nullptr;
		}

		E_PHYS_SHAPE _type = static_cast<E_PHYS_SHAPE>(type);

		const size_t amount = physics::get_shape_param_amount(_type);
		double* _params = new double[amount];
		for (size_t i=0; i<amount; ++i) {
			PyObject* item = PyTuple_GetItem(params, i);
			if (item == nullptr) {
				PyErr_SetString(PyExc_RuntimeError, "missing shape parameters for this type");
				return nullptr;
			}

			_params[i] = PyFloat_AsDouble(item);
		}

		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", physbody->set_shape(_type, _params));
	}
	PyObject* PhysicsBody_set_mass(PhysicsBodyObject* self, PyObject* args) {
		double mass;

		if (!PyArg_ParseTuple(args, "d", &mass)) {
			return nullptr;
		}

		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		physbody->set_mass(mass);

		Py_RETURN_NONE;
	}
	PyObject* PhysicsBody_set_friction(PhysicsBodyObject* self, PyObject* args) {
		double friction;

		if (!PyArg_ParseTuple(args, "d", &friction)) {
			return nullptr;
		}

		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		physbody->set_friction(friction);

		Py_RETURN_NONE;
	}

	PyObject* PhysicsBody_add_constraint(PhysicsBodyObject* self, PyObject* args) {
		int type;
		PyObject* params;

		if (!PyArg_ParseTuple(args, "iO!", &type, &PyTuple_Type, &params)) {
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

		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", physbody->add_constraint(_type, _params));
	}
	PyObject* PhysicsBody_remove_constraints(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		physbody->remove_constraints();

		Py_RETURN_NONE;
	}

	PyObject* PhysicsBody_update_state(PhysicsBodyObject* self, PyObject* args) {
		auto physbody = as_physics_body(self).lock();
		if (physbody == nullptr) {
			return nullptr;
		}

		physbody->update_state();

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_PHYSICS_BODY
