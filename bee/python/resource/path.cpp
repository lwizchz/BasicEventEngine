/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PATH
#define BEE_PYTHON_PATH 1

#include <Python.h>
#include <structmember.h>

#include "path.hpp"

#include "../python.hpp"

#include "../../resource/path.hpp"

namespace bee { namespace python {
	PyObject* Path_from(Path* path) {
		PyObject* py_path = internal::Path_new(&internal::PathType, nullptr, nullptr);
		internal::PathObject* _py_path = reinterpret_cast<internal::PathObject*>(py_path);
		_py_path->name = PyUnicode_FromString(path->get_name().c_str());
		return py_path;
	}
	bool Path_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::PathType);
	}
namespace internal {
	PyMethodDef PathMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Path_print), METH_NOARGS, "Print all relevant information about the Path"},

		{"get_nodes", reinterpret_cast<PyCFunction>(Path_get_nodes), METH_NOARGS, "Return the node list"},

		{"add_node", reinterpret_cast<PyCFunction>(Path_add_node), METH_VARARGS, "Add a node to the node list"},
		{"add_control_point", reinterpret_cast<PyCFunction>(Path_add_control_point), METH_VARARGS, "Add a Bezier control point to the list"},
		{"remove_node", reinterpret_cast<PyCFunction>(Path_remove_node), METH_VARARGS, "Remove the node with the given index"},
		{"remove_control_point", reinterpret_cast<PyCFunction>(Path_remove_control_point), METH_VARARGS, "Remove the control point associated with the given node index"},

		{"load", reinterpret_cast<PyCFunction>(Path_load), METH_VARARGS, "Load the Path from a file path"},

		{"draw", reinterpret_cast<PyCFunction>(Path_draw), METH_VARARGS, "Draw the Path for debugging purposes"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef PathMembers[] = {
		{"name", T_OBJECT_EX, offsetof(PathObject, name), 0, "The Path name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject PathType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Path",
		sizeof(PathObject), 0,
		reinterpret_cast<destructor>(Path_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Path_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Path_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Path objects",
		0,
		0,
		0,
		0,
		0, 0,
		PathMethods,
		PathMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Path_init),
		0, Path_new,
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

	PyObject* PyInit_bee_path(PyObject* module) {
		PathType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&PathType) < 0) {
			return nullptr;
		}

		Py_INCREF(&PathType);
		PyModule_AddObject(module, "Path", reinterpret_cast<PyObject*>(&PathType));

		return reinterpret_cast<PyObject*>(&PathType);
	}

	Path* as_path(PathObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Path::get_by_name(_name);
	}
	Path* as_path(PyObject* self) {
		if (Path_check(self)) {
			return as_path(reinterpret_cast<PathObject*>(self));
		}
		return nullptr;
	}

	void Path_dealloc(PathObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Path_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		PathObject* self;

		self = reinterpret_cast<PathObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Path_init(PathObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Path_repr(PathObject* self) {
		std::string s = std::string("bee.Path(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Path_str(PathObject* self) {
		Path* path = as_path(self);
		if (path == nullptr) {
			return Py_BuildValue("N", PyUnicode_FromString("Invalid Path name"));
		}

		Variant m (path->serialize());
		std::string s = "Path " + m.to_str(true);

		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Path_print(PathObject* self, PyObject* args) {
		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		path->print();

		Py_RETURN_NONE;
	}

	PyObject* Path_get_nodes(PathObject* self, PyObject* args) {
		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		const std::vector<PathNode>& nodes (path->get_nodes());

		PyObject* _nodes = PyList_New(nodes.size());
		size_t i = 0;
		for (auto& n : nodes) {
			PyList_SetItem(_nodes, i++, Py_BuildValue("(ddd)", n.pos.x(), n.pos.y(), n.pos.z()));
		}

		return _nodes;
	}

	PyObject* Path_add_node(PathObject* self, PyObject* args) {
		double x, y, z;
		double speed;

		if (!PyArg_ParseTuple(args, "(ddd)d", &x, &y, &z, &speed)) {
			return nullptr;
		}

		btVector3 pos (x, y, z);

		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		path->add_node(PathNode(pos, speed));

		Py_RETURN_NONE;
	}
	PyObject* Path_add_control_point(PathObject* self, PyObject* args) {
		unsigned int node;
		double x, y, z;

		if (!PyArg_ParseTuple(args, "I(ddd)", &node, &x, &y, &z)) {
			return nullptr;
		}

		btVector3 pos (x, y, z);

		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		path->add_control_point(node, pos);

		Py_RETURN_NONE;
	}
	PyObject* Path_remove_node(PathObject* self, PyObject* args) {
		unsigned int node;

		if (!PyArg_ParseTuple(args, "I", &node)) {
			return nullptr;
		}

		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", path->remove_node(node));
	}
	PyObject* Path_remove_control_point(PathObject* self, PyObject* args) {
		unsigned int node;

		if (!PyArg_ParseTuple(args, "I", &node)) {
			return nullptr;
		}

		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", path->remove_control_point(node));
	}

	PyObject* Path_load(PathObject* self, PyObject* args) {
		PyObject* filepath;

		if (!PyArg_ParseTuple(args, "U", &filepath)) {
			return nullptr;
		}

		std::string _filepath (PyUnicode_AsUTF8(filepath));

		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		path->set_path(_filepath);

		return Py_BuildValue("i", path->load());
	}

	PyObject* Path_draw(PathObject* self, PyObject* args) {
		double x, y, z;

		if (!PyArg_ParseTuple(args, "(ddd)", &x, &y, &z)) {
			return nullptr;
		}

		btVector3 pos (x, y, z);

		Path* path = as_path(self);
		if (path == nullptr) {
			return nullptr;
		}

		path->draw(pos);

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_PATH
