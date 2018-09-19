/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MESH
#define BEE_PYTHON_MESH 1

#include <Python.h>
#include <structmember.h>

#include "mesh.hpp"

#include "../python.hpp"

#include "../../resource/mesh.hpp"

namespace bee { namespace python {
	PyObject* Mesh_from(const Mesh* mesh) {
		if (mesh == nullptr) {
			return nullptr;
		}

		PyObject* py_mesh = internal::Mesh_new(&internal::MeshType, nullptr, nullptr);
		internal::MeshObject* _py_mesh = reinterpret_cast<internal::MeshObject*>(py_mesh);

		if (Mesh_init(_py_mesh, Py_BuildValue("(N)", PyUnicode_FromString(mesh->get_name().c_str())), nullptr)) {
			return nullptr;
		}

		return py_mesh;
	}
	bool Mesh_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::MeshType);
	}
namespace internal {
	PyMethodDef MeshMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Mesh_print), METH_NOARGS, "Print all relevant information about the Mesh"},

		{"has_animation", reinterpret_cast<PyCFunction>(Mesh_has_animation), METH_VARARGS, "Return whether an animation with the given name exists"},

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
	Mesh* as_mesh(PyObject* self) {
		if (Mesh_check(self)) {
			return as_mesh(reinterpret_cast<MeshObject*>(self));
		}
		return nullptr;
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
		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Mesh_str(MeshObject* self) {
		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return PyUnicode_FromString("Invalid Mesh name");
		}

		Variant m (mesh->serialize());
		std::string s = "Mesh " + m.to_str(true);

		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Mesh_print(MeshObject* self, PyObject* args) {
		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		mesh->print();

		Py_RETURN_NONE;
	}

	PyObject* Mesh_has_animation(MeshObject* self, PyObject* args) {
		PyObject* anim_name;

		if (!PyArg_ParseTuple(args, "U", &anim_name)) {
			return nullptr;
		}

		std::string _anim_name (PyUnicode_AsUTF8(anim_name));

		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(mesh->has_animation(_anim_name));
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
		double px = 0.0, py = 0.0, pz = 0.0;
		double sx = 1.0, sy = 1.0, sz = 1.0;
		double rx = 0.0, ry = 0.0, rz = 0.0;
		RGBA color (255, 255, 255, 255);
		int is_wireframe = false;
		PyObject* anim_name = nullptr;
		Uint32 animation_time = 0;

		if (!PyArg_ParseTuple(
			args, "(ddd)|(ddd)(ddd)(bbbb)pUI",
			&px, &py, &pz,
			&sx, &sy, &sz,
			&rx, &ry, &rz,
			&color.r, &color.g, &color.b, &color.a,
			&is_wireframe,
			&anim_name, &animation_time
		)) {
			return nullptr;
		}

		glm::vec3 pos (px, py, pz);
		glm::vec3 scale (sx, sy, sz);
		glm::vec3 rotate (rx, ry, rz);

		bool _is_wireframe = is_wireframe;

		std::string _anim_name;
		if (anim_name != nullptr) {
			_anim_name = PyUnicode_AsUTF8(anim_name);
		}

		Mesh* mesh = as_mesh(self);
		if (mesh == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", mesh->draw(_anim_name, animation_time, pos, scale, rotate, color, _is_wireframe));
	}
}}}

#endif // BEE_PYTHON_MESH
