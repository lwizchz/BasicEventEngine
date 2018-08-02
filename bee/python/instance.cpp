/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_INSTANCE
#define BEE_PYTHON_INSTANCE 1

#include <Python.h>
#include <structmember.h>

#include "instance.hpp"

#include "python.hpp"

#include "../core/instance.hpp"

#include "../resource/texture.hpp"
#include "../resource/object.hpp"

namespace bee { namespace python {
	PyObject* Instance_from(Instance* inst) {
		PyObject* py_inst = internal::Instance_new(&internal::InstanceType, nullptr, nullptr);
		internal::InstanceObject* _py_inst = reinterpret_cast<internal::InstanceObject*>(py_inst);
		_py_inst->name = PyUnicode_FromString(inst->get_object()->get_name().c_str());
		_py_inst->num = inst->id;
		return py_inst;
	}
	bool Instance_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::InstanceType);
	}
namespace internal {
	PyMethodDef InstanceMethods[] = {
		{"at", reinterpret_cast<PyCFunction>(Instance_at), METH_VARARGS, "Return the nth Instance of the Object"},

		{"print", reinterpret_cast<PyCFunction>(Instance_print), METH_NOARGS, "Print all relevant information about the Instance"},

		{"set_alarm", reinterpret_cast<PyCFunction>(Instance_set_alarm), METH_VARARGS, "Set the alarm with the given name"},

		{"set_sprite", reinterpret_cast<PyCFunction>(Instance_set_sprite), METH_VARARGS, "Change the sprite"},
		{"set_computation_type", reinterpret_cast<PyCFunction>(Instance_set_computation_type), METH_VARARGS, "Change the computation type and collision mask"},
		{"set_is_persistent", reinterpret_cast<PyCFunction>(Instance_set_is_persistent), METH_VARARGS, "Change whether the Instance will persist between Rooms"},

		{"get_data", reinterpret_cast<PyCFunction>(Instance_get_data), METH_VARARGS, "Return the requested data field from the data map"},
		{"set_data", reinterpret_cast<PyCFunction>(Instance_set_data), METH_VARARGS, "Set the requested data field"},

		{"get_pos", reinterpret_cast<PyCFunction>(Instance_get_pos), METH_NOARGS, "Return the 3D position of the attached PhysicsBody"},
		{"get_aabb", reinterpret_cast<PyCFunction>(Instance_get_aabb), METH_NOARGS, "Return the AABB for the sprite mask"},
		{"get_corner", reinterpret_cast<PyCFunction>(Instance_get_corner), METH_NOARGS, "Return the top-left corner of the 2D AABB"},
		{"get_start", reinterpret_cast<PyCFunction>(Instance_get_start), METH_NOARGS, "Return the starting position"},

		{"get_sprite", reinterpret_cast<PyCFunction>(Instance_get_sprite), METH_NOARGS, "Return the sprite"},
		{"get_mass", reinterpret_cast<PyCFunction>(Instance_get_mass), METH_NOARGS, "Return the mass of the attached PhysicsBody"},
		{"get_computation_type", reinterpret_cast<PyCFunction>(Instance_get_computation_type), METH_NOARGS, "Return the computation type"},
		{"get_is_persistent", reinterpret_cast<PyCFunction>(Instance_get_is_persistent), METH_NOARGS, "Return whether the Instance will persist between Rooms"},

		{"set_pos", reinterpret_cast<PyCFunction>(Instance_set_pos), METH_VARARGS, "Set the center position of the attached PhysicsBody"},
		{"set_to_start", reinterpret_cast<PyCFunction>(Instance_set_to_start), METH_NOARGS, "Set the position back to the starting position"},
		{"set_corner", reinterpret_cast<PyCFunction>(Instance_set_corner), METH_VARARGS, "Set the corner position of the AABB"},
		{"set_mass", reinterpret_cast<PyCFunction>(Instance_set_mass), METH_VARARGS, "Set the mass of the attached PhysicsBody"},
		{"move", reinterpret_cast<PyCFunction>(Instance_move), METH_VARARGS, "Move with the given impulse"},
		{"set_friction", reinterpret_cast<PyCFunction>(Instance_set_friction), METH_VARARGS, "Set the friction of the attached PhysicsBody"},
		{"set_gravity", reinterpret_cast<PyCFunction>(Instance_set_gravity), METH_VARARGS, "Set the gravity vector of the attached PhysicsBody"},
		{"set_velocity", reinterpret_cast<PyCFunction>(Instance_set_velocity), METH_VARARGS, "Set the velocity of the attached PhysicsBody"},
		{"add_velocity", reinterpret_cast<PyCFunction>(Instance_add_velocity), METH_VARARGS, "Add the given velocityy to the velocity of the attached PhysicsBody"},
		// Do some terrible casting to get rid of an incompatible function type warning
		{"limit_velocity", reinterpret_cast<PyCFunction>(reinterpret_cast<void (*)(void)>(Instance_limit_velocity)), METH_VARARGS | METH_KEYWORDS, "Limit the velocity of the attached PhysicsBody along the given axes"},
		//{"limit_velocity", reinterpret_cast<PyCFunction>(Instance_limit_velocity), METH_VARARGS | METH_KEYWORDS, "Limit the velocity of the attached PhysicsBody along the given axes"},

		{"get_speed", reinterpret_cast<PyCFunction>(Instance_get_speed), METH_NOARGS, "Return the speed of the attached PhysicsBody"},
		{"get_velocity", reinterpret_cast<PyCFunction>(Instance_get_velocity), METH_NOARGS, "Return the velocity of the attached PhysicsBody"},
		{"get_velocity_ang", reinterpret_cast<PyCFunction>(Instance_get_velocity_ang), METH_NOARGS, "Return the angular velocity of the attached PhysicsBody"},
		{"get_friction", reinterpret_cast<PyCFunction>(Instance_get_friction), METH_NOARGS, "Return the friction of the attached PhysicsBody"},
		{"get_gravity", reinterpret_cast<PyCFunction>(Instance_get_gravity), METH_NOARGS, "Return the gravity vector of the attached PhysicsBody"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef InstanceMembers[] = {
		{"name", T_OBJECT_EX, offsetof(InstanceObject, name), 0, "The Object name"},
		{"num", T_INT, offsetof(InstanceObject, num), 0, "The Instance number"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject InstanceType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Instance",
		sizeof(InstanceObject), 0,
		reinterpret_cast<destructor>(Instance_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Instance_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Instance_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Instance objects",
		0,
		0,
		0,
		0,
		0, 0,
		InstanceMethods,
		InstanceMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Instance_init),
		0, Instance_new,
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

	PyObject* PyInit_bee_instance(PyObject* module) {
		InstanceType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&InstanceType) < 0) {
			return nullptr;
		}

		Py_INCREF(&InstanceType);
		PyModule_AddObject(module, "Instance", reinterpret_cast<PyObject*>(&InstanceType));

		return reinterpret_cast<PyObject*>(&InstanceType);
	}

	Instance* as_instance(InstanceObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		Object* obj = Object::get_by_name(_name);
		if (obj == nullptr) {
			PyErr_SetString(PyExc_ValueError, "the provided Object name is not valid");
			return nullptr;
		}

		const std::map<size_t,Instance*>& instances (obj->get_instances());
		std::map<size_t,Instance*>::const_iterator inst (instances.find(self->num));
		if (inst == instances.end()) {
			PyErr_SetString(PyExc_ValueError, "the provided Instance id is not valid");
			return nullptr;
		}

		return inst->second;
	}
	Instance* as_instance(PyObject* self) {
		if (Instance_check(self)) {
			return as_instance(reinterpret_cast<InstanceObject*>(self));
		}
		return nullptr;
	}

	void Instance_dealloc(InstanceObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Instance_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		InstanceObject* self;

		self = reinterpret_cast<InstanceObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}

			self->num = 0;
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Instance_init(InstanceObject* self, PyObject* args, PyObject* kwds) {
		PyObject* name = nullptr;

		const char* kwlist[] = {"name", "num", nullptr};
		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oi", const_cast<char**>(kwlist), &name, &self->num)) {
			return -1;
		}

		if (name != nullptr) {
			PyObject* tmp = self->name;
			Py_INCREF(name);
			self->name = name;
			Py_XDECREF(tmp);
		}

		return 0;
	}

	PyObject* Instance_at(InstanceObject* self, PyObject* args) {
		int index;
		if (!PyArg_ParseTuple(args, "i", &index)) {
			return nullptr;
		}

		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		Object* obj = Object::get_by_name(_name);
		if (obj == nullptr) {
			PyErr_SetString(PyExc_ValueError, "the provided Object name is not valid");
			return nullptr;
		}

		Instance* inst = obj->get_instance_at(index);
		if (inst == nullptr) {
			Py_RETURN_NONE;
		}

		self->num = inst->id;

		return Py_BuildValue("O", self);
	}

	PyObject* Instance_repr(InstanceObject* self) {
		std::string s = std::string("bee.Instance(\"") + PyUnicode_AsUTF8(self->name) + "\", " + std::to_string(self->num) + ")";
		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Instance_str(InstanceObject* self) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		Variant m (inst->serialize());
		std::string s = "Instance " + m.to_str(true);

		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Instance_print(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->print();

		Py_RETURN_NONE;
	}

	PyObject* Instance_set_alarm(InstanceObject* self, PyObject* args) {
		PyObject* name;
		int ticks;
		if (!PyArg_ParseTuple(args, "Ui", &name, &ticks)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_alarm(_name, ticks);

		Py_RETURN_NONE;
	}

	PyObject* Instance_set_sprite(InstanceObject* self, PyObject* args) {
		PyObject* texture;
		if (!PyArg_ParseTuple(args, "U", &texture)) {
			return nullptr;
		}

		std::string _texture (PyUnicode_AsUTF8(texture));

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_sprite(Texture::get_by_name(_texture));

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_computation_type(InstanceObject* self, PyObject* args) {
		unsigned long comptype;
		if (!PyArg_ParseTuple(args, "k", &comptype)) {
			return nullptr;
		}

		E_COMPUTATION _comptype (static_cast<E_COMPUTATION>(comptype));

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_computation_type(_comptype);

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_is_persistent(InstanceObject* self, PyObject* args) {
		int is_persistent;
		if (!PyArg_ParseTuple(args, "p", &is_persistent)) {
			return nullptr;
		}

		bool _is_persistent = is_persistent;

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_is_persistent(_is_persistent);

		Py_RETURN_NONE;
	}

	PyObject* Instance_get_data(InstanceObject* self, PyObject* args) {
		PyObject* field;
		if (!PyArg_ParseTuple(args, "U", &field)) {
			return nullptr;
		}

		std::string _field (PyUnicode_AsUTF8(field));

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		Variant data (inst->get_data(_field));

		return Py_BuildValue("N", variant_to_pyobj(data));
	}
	PyObject* Instance_set_data(InstanceObject* self, PyObject* args) {
		PyObject* field;
		PyObject* data;
		if (!PyArg_ParseTuple(args, "UO", &field, &data)) {
			return nullptr;
		}

		std::string _field (PyUnicode_AsUTF8(field));

		Variant _data (pyobj_to_variant(data));

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_data(_field, _data);

		Py_RETURN_NONE;
	}

	PyObject* Instance_get_pos(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		btVector3 pos (inst->get_pos());

		return Py_BuildValue("(ddd)", pos.x(), pos.y(), pos.z());
	}
	PyObject* Instance_get_aabb(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		SDL_Rect aabb (inst->get_aabb());

		return Py_BuildValue("(iiii)", aabb.x, aabb.y, aabb.w, aabb.h);
	}
	PyObject* Instance_get_corner(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		double cx, cy;
		std::tie(cx, cy) = inst->get_corner();

		return Py_BuildValue("(dd)", cx, cy);
	}
	PyObject* Instance_get_start(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		btVector3 start (inst->get_start());

		return Py_BuildValue("(ddd)", start.x(), start.y(), start.z());
	}

	PyObject* Instance_get_sprite(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("N", PyUnicode_FromString(inst->get_sprite()->get_name().c_str()));
	}
	PyObject* Instance_get_mass(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", inst->get_mass());
	}
	PyObject* Instance_get_computation_type(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("k", inst->get_computation_type());
	}
	PyObject* Instance_get_is_persistent(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("p", inst->get_is_persistent());
	}

	PyObject* Instance_set_pos(InstanceObject* self, PyObject* args) {
		double x, y, z;
		if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
			return nullptr;
		}

		btScalar _x (x), _y (y), _z (z);

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_pos({_x, _y, _z});

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_to_start(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_to_start();

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_corner(InstanceObject* self, PyObject* args) {
		double cx, cy;
		if (!PyArg_ParseTuple(args, "dd", &cx, &cy)) {
			return nullptr;
		}

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_corner(cx, cy);

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_mass(InstanceObject* self, PyObject* args) {
		double mass;
		if (!PyArg_ParseTuple(args, "d", &mass)) {
			return nullptr;
		}

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_mass(mass);

		Py_RETURN_NONE;
	}
	PyObject* Instance_move(InstanceObject* self, PyObject* args) {
		double x, y, z;
		if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
			return nullptr;
		}

		btScalar _x (x), _y (y), _z (z);

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->move({_x, _y, _z});

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_friction(InstanceObject* self, PyObject* args) {
		double friction;
		if (!PyArg_ParseTuple(args, "d", &friction)) {
			return nullptr;
		}

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_friction(friction);

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_gravity(InstanceObject* self, PyObject* args) {
		double x, y, z;
		if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
			return nullptr;
		}

		btScalar _x (x), _y (y), _z (z);

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_gravity({_x, _y, _z});

		Py_RETURN_NONE;
	}
	PyObject* Instance_set_velocity(InstanceObject* self, PyObject* args) {
		double x, y, z;
		if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
			return nullptr;
		}

		btScalar _x (x), _y (y), _z (z);

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->set_velocity({_x, _y, _z});

		Py_RETURN_NONE;
	}
	PyObject* Instance_add_velocity(InstanceObject* self, PyObject* args) {
		double x, y, z;
		if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
			return nullptr;
		}

		btScalar _x (x), _y (y), _z (z);

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		inst->add_velocity({_x, _y, _z});

		Py_RETURN_NONE;
	}
	PyObject* Instance_limit_velocity(InstanceObject* self, PyObject* args, PyObject* kwds) {
		const char* kwlist[] = {"", "x", "y", "z", nullptr};

		double vel = nan(""), x = nan(""), y = nan(""), z = nan("");
		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|d$ddd", const_cast<char**>(kwlist), &vel, &x, &y, &z)) {
			return nullptr;
		}

		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		if (!isnan(vel)) {
			inst->limit_velocity(vel);
		} else {
			bool has_param = false;
			btVector3 limit (-1.0, -1.0, -1.0);
			if (!isnan(x)) {
				has_param = true;
				limit.setX(x);
			}
			if (!isnan(y)) {
				has_param = true;
				limit.setY(y);
			}
			if (!isnan(z)) {
				has_param = true;
				limit.setZ(z);
			}

			if (limit != btVector3(-1.0, -1.0, -1.0)) {
				inst->limit_velocity(limit);
			} else if (!has_param) {
				PyErr_SetString(PyExc_RuntimeError, "a parameter is required");
				return nullptr;
			}
		}

		Py_RETURN_NONE;
	}

	PyObject* Instance_get_speed(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", inst->get_speed());
	}
	PyObject* Instance_get_velocity(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		btVector3 v (inst->get_velocity());

		return Py_BuildValue("(ddd)", v.x(), v.y(), v.z());
	}
	PyObject* Instance_get_velocity_ang(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		btVector3 v (inst->get_velocity_ang());

		return Py_BuildValue("(ddd)", v.x(), v.y(), v.z());
	}
	PyObject* Instance_get_friction(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", inst->get_friction());
	}
	PyObject* Instance_get_gravity(InstanceObject* self, PyObject* args) {
		Instance* inst = as_instance(self);
		if (inst == nullptr) {
			return nullptr;
		}

		btVector3 g (inst->get_gravity());

		return Py_BuildValue("(ddd)", g.x(), g.y(), g.z());
	}
}}}

#endif // BEE_PYTHON_INSTANCE
