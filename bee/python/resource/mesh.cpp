/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MESH
#define BEE_PYTHON_MESH 1

#include <Python.h>
#include <structmember.h>

#include "mesh.hpp"

#include "../python.hpp"

#include "../../resource/mesh.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef MeshMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Mesh_print), METH_NOARGS, "Print all relevant information about the Mesh"},

		{"load", reinterpret_cast<PyCFunction>(Mesh_load), METH_VARARGS, "Load the desired Mesh from its given filename"},
		{"free", reinterpret_cast<PyCFunction>(Mesh_free), METH_NOARGS, "Free the Mesh buffers"},

		{"draw", reinterpret_cast<PyCFunction>(Mesh_draw), METH_VARARGS, "Draw the Mesh with the given attributes"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef MeshMembers[] = {
		{"name", T_OBJECT_EX, offsetof(MeshObject, name), 0, "The Mesh name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject MeshType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Mesh",
		sizeof(MeshObject), 0,
		reinterpret_cast<destructor>(Mesh_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Mesh_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Mesh_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Mesh objects",
		0,
		0,
		0,
		0,
		0, 0,
		MeshMethods,
		MeshMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Mesh_init),
		0, Mesh_new,
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

	PyObject* PyInit_bee_mesh(PyObject* module) {
		MeshType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&MeshType) < 0) {
			return nullptr;
		}

		Py_INCREF(&MeshType);
		PyModule_AddObject(module, "Mesh", reinterpret_cast<PyObject*>(&MeshType));

		return reinterpret_cast<PyObject*>(&MeshType);
	}

	Mesh* as_mesh(MeshObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Mesh::get_by_name(_name);
	}

	void Mesh_dealloc(MeshObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Mesh_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		MeshObject* self;

		self = reinterpret_cast<MeshObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Mesh_init(MeshObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Mesh_repr(MeshObject* self) {
		std::string s = std::string("bee.Mesh(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Mesh_str(MeshObject* self) {
		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return Py_BuildValue("N", PyUnicode_FromString("Invalid Mesh name"));
		}

		Variant m (mesh->serialize());
		std::string s = "Mesh " + m.to_str(true);

		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Mesh_print(MeshObject* self, PyObject* args) {
		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		mesh->print();

		Py_RETURN_NONE;
	}

	PyObject* Mesh_load(MeshObject* self, PyObject* args) {
		int index = 0;

		if (!PyArg_ParseTuple(args, "|i", &index)) {
			return nullptr;
		}

		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", mesh->load(index));
	}
	PyObject* Mesh_free(MeshObject* self, PyObject* args) {
		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", mesh->free());
	}

	PyObject* Mesh_draw(MeshObject* self, PyObject* args) {
		glm::vec3 pos (0.0f);
		glm::vec3 scale (1.0f);
		glm::vec3 rotate (0.0f);
		RGBA color (255, 255, 255, 255);
		int is_wireframe = false;

		if (!PyArg_ParseTuple(
			args, "(fff)|(fff)(fff)(bbbb)p",
			&pos.x, &pos.y, &pos.z,
			&scale.x, &scale.y, &scale.z,
			&rotate.x, &rotate.y, &rotate.z,
			&color.r, &color.g, &color.b, &color.a,
			&is_wireframe
		)) {
			return nullptr;
		}

		bool _is_wireframe = is_wireframe;

		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", mesh->draw(pos, scale, rotate, color, _is_wireframe));
	}
}}}

#endif // BEE_PYTHON_MESH
