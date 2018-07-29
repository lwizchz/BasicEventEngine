/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_OBJECT
#define BEE_PYTHON_OBJECT 1

#include <Python.h>
#include <structmember.h>

#include "object.hpp"

#include "../python.hpp"

#include "../../resource/object.hpp"

#include "texture.hpp"
#include "../instance.hpp"

#include "../../resource/texture.hpp"
#include "../../core/instance.hpp"

namespace bee { namespace python {
	PyObject* Object_from(Object* object) {
		PyObject* py_object = internal::Object_new(&internal::ObjectType, nullptr, nullptr);
		internal::ObjectObject* _py_object = reinterpret_cast<internal::ObjectObject*>(py_object);
		_py_object->name = PyUnicode_FromString(object->get_name().c_str());
		return py_object;
	}
	bool Object_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::ObjectType);
	}
namespace internal {
	PyMethodDef ObjectMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Object_print), METH_NOARGS, "Print all relevant information about the Object"},

		{"get_sprite", reinterpret_cast<PyCFunction>(Object_get_sprite), METH_NOARGS, "Return the Object's sprite Texture"},
		{"get_is_persistent", reinterpret_cast<PyCFunction>(Object_get_is_persistent), METH_NOARGS, "Return whether the Object's Instances should persist between Rooms"},
		{"get_depth", reinterpret_cast<PyCFunction>(Object_get_depth), METH_NOARGS, "Return the sorting depth of the Object"},
		{"get_parent", reinterpret_cast<PyCFunction>(Object_get_parent), METH_NOARGS, "Return the parent of the Object"},
		{"get_mask_offset", reinterpret_cast<PyCFunction>(Object_get_mask_offset), METH_NOARGS, "Return how far the sprite should be offset from the Instance position"},
		{"get_is_pausable", reinterpret_cast<PyCFunction>(Object_get_is_pausable), METH_NOARGS, "Return whether the Object's events are pausable or not"},
		{"get_events", reinterpret_cast<PyCFunction>(Object_get_events), METH_NOARGS, "Return a list of all the events that the Object implements"},

		{"set_sprite", reinterpret_cast<PyCFunction>(Object_set_sprite), METH_VARARGS, "Set the Object's sprite Texture"},
		{"set_is_persistent", reinterpret_cast<PyCFunction>(Object_set_is_persistent), METH_VARARGS, "Set whether the Object's Instances should persist between Rooms"},
		{"set_depth", reinterpret_cast<PyCFunction>(Object_set_depth), METH_VARARGS, "Set the sorting depth of the Object"},
		{"set_parent", reinterpret_cast<PyCFunction>(Object_set_parent), METH_VARARGS, "Set the parent of the Object"},
		{"set_mask_offset", reinterpret_cast<PyCFunction>(Object_set_mask_offset), METH_VARARGS, "Set how far the sprite should be offset from the Instance position"},
		{"set_is_pausable", reinterpret_cast<PyCFunction>(Object_set_is_pausable), METH_VARARGS, "Set whether the Object's events are pausable or not"},

		{"get_instances", reinterpret_cast<PyCFunction>(Object_get_instances), METH_NOARGS, "Return a list of all the Object's Instances"},
		{"get_instance_amount", reinterpret_cast<PyCFunction>(Object_get_instance_amount), METH_NOARGS, "Return the number of Instances"},
		{"get_instance_at", reinterpret_cast<PyCFunction>(Object_get_instance_at), METH_VARARGS, "Returns the nth Instance of the Object"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef ObjectMembers[] = {
		{"name", T_OBJECT_EX, offsetof(ObjectObject, name), 0, "The Object name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject ObjectType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Object",
		sizeof(ObjectObject), 0,
		reinterpret_cast<destructor>(Object_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Object_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Object_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Object objects",
		0,
		0,
		0,
		0,
		0, 0,
		ObjectMethods,
		ObjectMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Object_init),
		0, Object_new,
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

	PyObject* PyInit_bee_object(PyObject* module) {
		ObjectType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&ObjectType) < 0) {
			return nullptr;
		}

		Py_INCREF(&ObjectType);
		PyModule_AddObject(module, "Object", reinterpret_cast<PyObject*>(&ObjectType));

		return reinterpret_cast<PyObject*>(&ObjectType);
	}

	Object* as_object(ObjectObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Object::get_by_name(_name);
	}
	Object* as_object(PyObject* self) {
		if (Object_check(self)) {
			return as_object(reinterpret_cast<ObjectObject*>(self));
		}
		return nullptr;
	}

	void Object_dealloc(ObjectObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Object_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		ObjectObject* self;

		self = reinterpret_cast<ObjectObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Object_init(ObjectObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Object_repr(ObjectObject* self) {
		std::string s = std::string("bee.Object(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Object_str(ObjectObject* self) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return Py_BuildValue("N", PyUnicode_FromString("Invalid Object name"));
		}

		Variant m (obj->serialize());
		std::string s = "Object " + m.to_str(true);

		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Object_print(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->print();

		Py_RETURN_NONE;
	}

	PyObject* Object_get_sprite(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		const Texture* spr = obj->get_sprite();
		if (spr != nullptr) {
			TextureObject* tex = reinterpret_cast<TextureObject*>(
				Texture_new(&TextureType, nullptr, nullptr)
			);

			if (Texture_init(tex, Py_BuildValue("(N)", PyUnicode_FromString(spr->get_name().c_str())), nullptr)) {
				Py_RETURN_NONE;
			}

			return Py_BuildValue("N", tex);
		} else {
			Py_RETURN_NONE;
		}
	}
	PyObject* Object_get_is_persistent(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("O", obj->get_is_persistent() ? Py_True : Py_False);
	}
	PyObject* Object_get_depth(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", obj->get_depth());
	}
	PyObject* Object_get_parent(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		const Object* parent = obj->get_parent();
		if (parent != nullptr) {
			ObjectObject* _parent = reinterpret_cast<ObjectObject*>(
				Object_new(&ObjectType, nullptr, nullptr)
			);
			_parent->name = PyUnicode_FromString(parent->get_name().c_str());
			return Py_BuildValue("N", _parent);
		} else {
			Py_RETURN_NONE;
		}
	}
	PyObject* Object_get_mask_offset(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		std::pair<int,int> offset = obj->get_mask_offset();

		return Py_BuildValue("(ii)", offset.first, offset.second);
	}
	PyObject* Object_get_is_pausable(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("O", obj->get_is_pausable() ? Py_True : Py_False);
	}
	PyObject* Object_get_events(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		Variant events (E_DATA_TYPE::VECTOR);
		for (auto& e : obj->get_events()) {
			events.v.emplace_back(static_cast<int>(e));
		}

		return Py_BuildValue("N", variant_to_pyobj(events));
	}

	PyObject* Object_set_sprite(ObjectObject* self, PyObject* args) {
		PyObject* tex;

		if (!PyArg_ParseTuple(args, "O!", &TextureType, &tex)) {
			return nullptr;
		}

		Texture* _tex = as_texture(tex);

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->set_sprite(_tex);

		Py_RETURN_NONE;
	}
	PyObject* Object_set_is_persistent(ObjectObject* self, PyObject* args) {
		int is_persistent;

		if (!PyArg_ParseTuple(args, "p", &is_persistent)) {
			return nullptr;
		}

		bool _is_persistent = is_persistent;

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->set_is_persistent(_is_persistent);

		Py_RETURN_NONE;
	}
	PyObject* Object_set_depth(ObjectObject* self, PyObject* args) {
		int depth;

		if (!PyArg_ParseTuple(args, "i", &depth)) {
			return nullptr;
		}

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->set_depth(depth);

		Py_RETURN_NONE;
	}
	PyObject* Object_set_parent(ObjectObject* self, PyObject* args) {
		PyObject* parent;

		if (!PyArg_ParseTuple(args, "O!", ObjectType, &parent)) {
			return nullptr;
		}

		Object* _parent = as_object(reinterpret_cast<ObjectObject*>(parent));

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->set_parent(_parent);

		Py_RETURN_NONE;
	}
	PyObject* Object_set_mask_offset(ObjectObject* self, PyObject* args) {
		std::pair<int,int> offset;

		if (!PyArg_ParseTuple(args, "(ii)", &offset.first, &offset.second)) {
			return nullptr;
		}

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->set_mask_offset(offset);

		Py_RETURN_NONE;
	}
	PyObject* Object_set_is_pausable(ObjectObject* self, PyObject* args) {
		int is_pausable;

		if (!PyArg_ParseTuple(args, "p", &is_pausable)) {
			return nullptr;
		}

		bool _is_pausable = is_pausable;

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		obj->set_is_pausable(_is_pausable);

		Py_RETURN_NONE;
	}

	PyObject* Object_get_instances(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		const std::map<int,Instance*>& _instances = obj->get_instances();

		PyObject* instances = PyList_New(_instances.size());
		size_t i = 0;
		for (auto& _inst : _instances) {
			InstanceObject* inst = reinterpret_cast<InstanceObject*>(
				Instance_new(&InstanceType, nullptr, nullptr)
			);

			if (Instance_init(inst, Py_BuildValue("(Ni)", PyUnicode_FromString(obj->get_name().c_str()), _inst.first), nullptr)) {
				Py_DECREF(instances);
				return nullptr;
			}

			PyList_SetItem(instances, i++, reinterpret_cast<PyObject*>(inst));
		}

		return instances;
	}
	PyObject* Object_get_instance_amount(ObjectObject* self, PyObject* args) {
		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", obj->get_instance_amount());
	}
	PyObject* Object_get_instance_at(ObjectObject* self, PyObject* args) {
		int index;

		if (!PyArg_ParseTuple(args, "i", &index)) {
			return nullptr;
		}

		Object* obj = as_object(self);
		if (obj == nullptr) {
			return nullptr;
		}

		InstanceObject* inst = reinterpret_cast<InstanceObject*>(
			Instance_new(&InstanceType, nullptr, nullptr)
		);

		if (Instance_init(inst, Py_BuildValue("(Ni)", PyUnicode_FromString(obj->get_name().c_str()), index), nullptr)) {
			Py_RETURN_NONE;
		}

		return Py_BuildValue("N", inst);
	}
}}}

#endif // BEE_PYTHON_OBJECT
