/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_LIGHT
#define BEE_PYTHON_LIGHT 1

#include <Python.h>
#include <structmember.h>

#include "light.hpp"

#include "../python.hpp"

#include "../../resource/light.hpp"

namespace bee { namespace python {
	PyObject* Light_from(const Light* light) {
		if (light == nullptr) {
			return nullptr;
		}

		PyObject* py_light = internal::Light_new(&internal::LightType, nullptr, nullptr);
		internal::LightObject* _py_light = reinterpret_cast<internal::LightObject*>(py_light);

		if (Light_init(_py_light, Py_BuildValue("(N)", PyUnicode_FromString(light->get_name().c_str())), nullptr)) {
			return nullptr;
		}

		return py_light;
	}
	bool Light_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::LightType);
	}
namespace internal {
	PyMethodDef LightMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Light_print), METH_NOARGS, "Print all relevant information about the Light"},

		{"get_type", reinterpret_cast<PyCFunction>(Light_get_type), METH_NOARGS, "Return the lighting type"},
		{"get_position", reinterpret_cast<PyCFunction>(Light_get_position), METH_NOARGS, "Return the lighting position"},
		{"get_direction", reinterpret_cast<PyCFunction>(Light_get_direction), METH_NOARGS, "Return the lighting direction"},
		{"get_attenuation", reinterpret_cast<PyCFunction>(Light_get_attenuation), METH_NOARGS, "Return the lighting attenuations"},
		{"get_color", reinterpret_cast<PyCFunction>(Light_get_color), METH_NOARGS, "Return the lighting color"},

		{"set_type", reinterpret_cast<PyCFunction>(Light_set_type), METH_VARARGS, "Set the lighting type"},
		{"set_position", reinterpret_cast<PyCFunction>(Light_set_position), METH_VARARGS, "Set the lighting position"},
		{"set_direction", reinterpret_cast<PyCFunction>(Light_set_direction), METH_VARARGS, "Set the lighting direction"},
		{"set_attenuation", reinterpret_cast<PyCFunction>(Light_set_attenuation), METH_VARARGS, "Set the lighting attenuations"},
		{"set_color", reinterpret_cast<PyCFunction>(Light_set_color), METH_VARARGS, "Set the lighting color"},

		{"load", reinterpret_cast<PyCFunction>(Light_load), METH_NOARGS, "Load the Light from its path"},

		{"queue", reinterpret_cast<PyCFunction>(Light_queue), METH_NOARGS, "Queue the Light for drawing in the Room rendering loop"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef LightMembers[] = {
		{"name", T_OBJECT_EX, offsetof(LightObject, name), 0, "The Light name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject LightType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Light",
		sizeof(LightObject), 0,
		reinterpret_cast<destructor>(Light_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Light_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Light_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Light objects",
		0,
		0,
		0,
		0,
		0, 0,
		LightMethods,
		LightMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Light_init),
		0, Light_new,
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

	PyObject* PyInit_bee_light(PyObject* module) {
		LightType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&LightType) < 0) {
			return nullptr;
		}

		Py_INCREF(&LightType);
		PyModule_AddObject(module, "Light", reinterpret_cast<PyObject*>(&LightType));

		return reinterpret_cast<PyObject*>(&LightType);
	}

	Light* as_light(LightObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Light::get_by_name(_name);
	}
	Light* as_light(PyObject* self) {
		if (Light_check(self)) {
			return as_light(reinterpret_cast<LightObject*>(self));
		}
		return nullptr;
	}

	void Light_dealloc(LightObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Light_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		LightObject* self;

		self = reinterpret_cast<LightObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Light_init(LightObject* self, PyObject* args, PyObject* kwds) {
		PyObject* name = nullptr;

		const char* kwlist[] = {"name", nullptr};
		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", const_cast<char**>(kwlist), &name)) {
			return -1;
		}

		if (name != nullptr) {
			PyObject* tmp = self->name;
			Py_INCREF(name);
			self->name = name;
			Py_XDECREF(tmp);
		}

		if (self->name == nullptr) {
            self->name = PyUnicode_FromString("");
		}

		return 0;
	}

	PyObject* Light_repr(LightObject* self) {
		std::string s = std::string("bee.Light(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Light_str(LightObject* self) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return PyUnicode_FromString("Invalid Light name");
		}

		Variant m (lt->serialize());
		std::string s = "Light " + m.to_str(true);

		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Light_print(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		lt->print();

		Py_RETURN_NONE;
	}

	PyObject* Light_get_type(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", static_cast<int>(lt->get_type()));
	}
	PyObject* Light_get_position(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		glm::vec4 pos (lt->get_position());

		return Py_BuildValue("(dddd)", pos.x, pos.y, pos.z, pos.w);
	}
	PyObject* Light_get_direction(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		glm::vec4 dir (lt->get_direction());

		return Py_BuildValue("(dddd)", dir.x, dir.y, dir.z, dir.w);
	}
	PyObject* Light_get_attenuation(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		glm::vec4 att (lt->get_attenuation());

		return Py_BuildValue("(dddd)", att.x, att.y, att.z, att.w);
	}
	PyObject* Light_get_color(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		RGBA color (lt->get_color());

		return Py_BuildValue("(bbbb)", color.r, color.g, color.b, color.a);
	}

	PyObject* Light_set_type(LightObject* self, PyObject* args) {
		int type;

		if (!PyArg_ParseTuple(args, "i", &type)) {
			return nullptr;
		}

		E_LIGHT_TYPE _type (static_cast<E_LIGHT_TYPE>(type));

		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		lt->set_type(_type);

		Py_RETURN_NONE;
	}
	PyObject* Light_set_position(LightObject* self, PyObject* args) {
		double x, y, z, w;

		if (!PyArg_ParseTuple(args, "(dddd)", &x, &y, &z, &w)) {
			return nullptr;
		}

		glm::vec4 pos (x, y, z, w);

		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		lt->set_position(pos);

		Py_RETURN_NONE;
	}
	PyObject* Light_set_direction(LightObject* self, PyObject* args) {
		double x, y, z, w;

		if (!PyArg_ParseTuple(args, "(dddd)", &x, &y, &z, &w)) {
			return nullptr;
		}

		glm::vec4 dir (x, y, z, w);

		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		lt->set_direction(dir);

		Py_RETURN_NONE;
	}
	PyObject* Light_set_attenuation(LightObject* self, PyObject* args) {
		double x, y, z, w;

		if (!PyArg_ParseTuple(args, "(dddd)", &x, &y, &z, &w)) {
			return nullptr;
		}

		glm::vec4 att (x, y, z, w);

		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		lt->set_attenuation(att);

		Py_RETURN_NONE;
	}
	PyObject* Light_set_color(LightObject* self, PyObject* args) {
		unsigned r, g, b, a;

		if (!PyArg_ParseTuple(args, "(iiii)", &r, &g, &b, &a)) {
			return nullptr;
		}

		RGBA color (r, g, b, a);

		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		lt->set_color(color);

		Py_RETURN_NONE;
	}

	PyObject* Light_load(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", lt->load());
	}

	PyObject* Light_queue(LightObject* self, PyObject* args) {
		Light* lt = as_light(self);
		if (lt == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", lt->queue());
	}
}}}

#endif // BEE_PYTHON_LIGHT
