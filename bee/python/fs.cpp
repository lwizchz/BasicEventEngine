/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_FS
#define BEE_PYTHON_FS 1

#include "fs.hpp"

#include "../fs/fs.hpp"
#include "../fs/python.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEEFSMethods[] = {
		{"add_filemap", fs_add_filemap, METH_VARARGS, "Add a FileMap with the given name"},

		{"exists", fs_exists, METH_VARARGS, "Return whether a file has been mapped to the given path"},
		{"get_file", fs_get_file, METH_VARARGS, "Return the primary FilePath of the mapped file"},

		{"load_level", fs_load_level, METH_VARARGS, "Load the given level and Room"},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEEFSModule = {
		PyModuleDef_HEAD_INIT, "fs", nullptr, -1, BEEFSMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_fs() {
		return PyModule_Create(&BEEFSModule);
	}

	PyObject* fs_add_filemap(PyObject* self, PyObject* args) {
		PyObject* name;
		PyObject* path;
		int root_type = static_cast<int>(E_FS_ROOT_TYPE::NOT_ROOT);

		if (!PyArg_ParseTuple(args, "UUi", &name, &path, &root_type)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));
		std::string _path (PyUnicode_AsUTF8(path));

		E_FS_ROOT_TYPE _root_type = static_cast<E_FS_ROOT_TYPE>(root_type);

		return PyLong_FromLong(fs::add_filemap(_name, _path, _root_type));
	}

	PyObject* fs_exists(PyObject* self, PyObject* args) {
		PyObject* path;

		if (!PyArg_ParseTuple(args, "U", &path)) {
			return nullptr;
		}

		std::string _path (PyUnicode_AsUTF8(path));

		return PyBool_FromLong(fs::exists(_path));
	}
	PyObject* fs_get_file(PyObject* self, PyObject* args) {
		PyObject* path;

		if (!PyArg_ParseTuple(args, "U", &path)) {
			return nullptr;
		}

		std::string _path (PyUnicode_AsUTF8(path));
		if (!fs::exists(_path)) {
			Py_RETURN_NONE;
		}

		FilePath fp = fs::get_file(_path);

		PyObject* file = PyDict_New();
		PyDict_SetItemString(file, "path", PyUnicode_FromString(fp.get_path().c_str()));
		PyDict_SetItemString(file, "mapname", PyUnicode_FromString(fp.get_mapname().c_str()));
		PyDict_SetItemString(file, "get", fs::python::internal::get_file_callable(file));

		return file;
	}

	PyObject* fs_load_level(PyObject* self, PyObject* args) {
		PyObject* name;
		PyObject* path;
		PyObject* roomname = nullptr;

		if (!PyArg_ParseTuple(args, "UUU", &name, &path, &roomname)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));
		std::string _path (PyUnicode_AsUTF8(path));
		std::string _roomname (PyUnicode_AsUTF8(roomname));

		return PyLong_FromLong(fs::load_level(_name, _path, _roomname));
	}
}}}

#endif // BEE_PYTHON_FS
