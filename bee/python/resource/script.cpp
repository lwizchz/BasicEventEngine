/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_SCRIPT
#define BEE_PYTHON_SCRIPT 1

#include <Python.h>
#include <structmember.h>

#include "script.hpp"

#include "../python.hpp"

#include "../../resource/script.hpp"

namespace bee { namespace python {
	PyObject* Script_from(const Script* script) {
		PyObject* py_script = internal::Script_new(&internal::ScriptType, nullptr, nullptr);
		internal::ScriptObject* _py_script = reinterpret_cast<internal::ScriptObject*>(py_script);

		if (Script_init(_py_script, Py_BuildValue("(N)", PyUnicode_FromString(script->get_name().c_str())), nullptr)) {
			return nullptr;
		}

		return py_script;
	}
	bool Script_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::ScriptType);
	}
namespace internal {
	PyMethodDef ScriptMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Script_print), METH_NOARGS, "Print all relevant information about the Script"},

		{"load", reinterpret_cast<PyCFunction>(Script_load), METH_NOARGS, "Load the Script from its path"},
		{"free", reinterpret_cast<PyCFunction>(Script_free), METH_NOARGS, "Free the Script interface"},

		{"run_string", reinterpret_cast<PyCFunction>(Script_run_string), METH_VARARGS, "Run the given code string in the interface"},
		{"run_file", reinterpret_cast<PyCFunction>(Script_run_file), METH_VARARGS, "Run the given file in the interface"},
		{"run_func", reinterpret_cast<PyCFunction>(Script_run_func), METH_VARARGS, "Run the given function in the interface"},

		{"set_var", reinterpret_cast<PyCFunction>(Script_set_var), METH_VARARGS, "Set the given variable to the given value"},
		{"get_var", reinterpret_cast<PyCFunction>(Script_get_var), METH_VARARGS, "Return the variable with the given name"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef ScriptMembers[] = {
		{"name", T_OBJECT_EX, offsetof(ScriptObject, name), 0, "The Script name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject ScriptType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Script",
		sizeof(ScriptObject), 0,
		reinterpret_cast<destructor>(Script_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Script_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Script_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Script objects",
		0,
		0,
		0,
		0,
		0, 0,
		ScriptMethods,
		ScriptMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Script_init),
		0, Script_new,
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

	PyObject* PyInit_bee_script(PyObject* module) {
		ScriptType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&ScriptType) < 0) {
			return nullptr;
		}

		Py_INCREF(&ScriptType);
		PyModule_AddObject(module, "Script", reinterpret_cast<PyObject*>(&ScriptType));

		return reinterpret_cast<PyObject*>(&ScriptType);
	}

	Script* as_script(ScriptObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Script::get_by_name(_name);
	}
	Script* as_script(PyObject* self) {
		if (Script_check(self)) {
			return as_script(reinterpret_cast<ScriptObject*>(self));
		}
		return nullptr;
	}

	void Script_dealloc(ScriptObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Script_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		ScriptObject* self;

		self = reinterpret_cast<ScriptObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Script_init(ScriptObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Script_repr(ScriptObject* self) {
		std::string s = std::string("bee.Script(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Script_str(ScriptObject* self) {
		Script* scr = as_script(self);
		if (scr == nullptr) {
			return PyUnicode_FromString("Invalid Script name");
		}

		Variant m (scr->serialize());
		std::string s = "Script " + m.to_str(true);

		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Script_print(ScriptObject* self, PyObject* args) {
		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		scr->print();

		Py_RETURN_NONE;
	}

	PyObject* Script_load(ScriptObject* self, PyObject* args) {
		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", scr->load());
	}
	PyObject* Script_free(ScriptObject* self, PyObject* args) {
		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", scr->free());
	}

	PyObject* Script_run_string(ScriptObject* self, PyObject* args) {
		PyObject* code;

		if (!PyArg_ParseTuple(args, "U", &code)) {
			return nullptr;
		}

		std::string _code (PyUnicode_AsUTF8(code));

		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		Variant retval;
		int r = scr->run_string(_code, &retval);
		if (r) { // If the script failed to run the string
			return Py_BuildValue("(iO)", r, Py_None);
		} else {
			return Py_BuildValue("(iN)", r, variant_to_pyobj(retval));
		}
	}
	PyObject* Script_run_file(ScriptObject* self, PyObject* args) {
		PyObject* filename;

		if (!PyArg_ParseTuple(args, "U", &filename)) {
			return nullptr;
		}

		std::string _filename (PyUnicode_AsUTF8(filename));

		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", scr->run_file(_filename));
	}
	PyObject* Script_run_func(ScriptObject* self, PyObject* args) {
		PyObject* func;
		PyObject* fargs;

		if (!PyArg_ParseTuple(args, "UO!", &func, &PyList_Type, &fargs)) {
			return nullptr;
		}

		std::string _func (PyUnicode_AsUTF8(func));

		Variant _fargs = pyobj_to_variant(fargs);

		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		Variant retval;
		int r = scr->run_func(_func, _fargs, &retval);
		if (r) { // If the script failed to run the string
			return Py_BuildValue("(iO)", r, Py_None);
		} else {
			return Py_BuildValue("(iN)", r, variant_to_pyobj(retval));
		}
	}

	PyObject* Script_set_var(ScriptObject* self, PyObject* args) {
		PyObject* varname;
		PyObject* value;

		if (!PyArg_ParseTuple(args, "UO", &varname, &value)) {
			return nullptr;
		}

		std::string _varname (PyUnicode_AsUTF8(varname));

		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		ScriptInterface* interface = scr->get_interface();
		if (interface == nullptr) {
			return Py_BuildValue("i", -1);
		}

		return Py_BuildValue("i", interface->set_var(_varname, pyobj_to_variant(value)));
	}
	PyObject* Script_get_var(ScriptObject* self, PyObject* args) {
		PyObject* varname;

		if (!PyArg_ParseTuple(args, "U", &varname)) {
			return nullptr;
		}

		std::string _varname (PyUnicode_AsUTF8(varname));

		Script* scr = as_script(self);
		if (scr == nullptr) {
			return nullptr;
		}

		ScriptInterface* interface = scr->get_interface();
		if (interface == nullptr) {
			return Py_BuildValue("i", -1);
		}

		return variant_to_pyobj(interface->get_var(_varname));
	}
}}}

#endif // BEE_PYTHON_SCRIPT
