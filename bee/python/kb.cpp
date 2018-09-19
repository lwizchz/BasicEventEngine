/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_KB
#define BEE_PYTHON_KB 1

#include <functional>

#include "kb.hpp"

#include "../input/kb.hpp"
#include "../input/keybind.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEEKBMethods[] = {
		{"get_state", kb_get_state, METH_VARARGS, "Return whether the given keycode is pressed"},
		{"get_mod_state", kb_get_mod_state, METH_VARARGS, "Return whether the given modifier key is pressed"},

		{"append_input", kb_append_input, METH_VARARGS, "Append a keyboard event key to the given string"},

		{"bind", kb_bind, METH_VARARGS, ""},
		{"get_keybind", kb_get_keybind, METH_VARARGS, ""},
		{"get_keybind_by_name", kb_get_keybind_by_name, METH_VARARGS, ""},
		{"unbind", kb_unbind, METH_VARARGS, ""},
		{"unbind_by_name", kb_unbind_by_name, METH_VARARGS, ""},
		{"unbind_all", kb_unbind_all, METH_NOARGS, ""},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEEKBModule = {
		PyModuleDef_HEAD_INIT, "kb", nullptr, -1, BEEKBMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_kb() {
		return PyModule_Create(&BEEKBModule);
	}

	PyObject* kb_get_state(PyObject* self, PyObject* args) {
		PyObject* keyname;

		if (!PyArg_ParseTuple(args, "U", &keyname)) {
			return nullptr;
		}

		std::string _keyname (PyUnicode_AsUTF8(keyname));

		return PyBool_FromLong(kb::get_state(kb::keystrings_get_key(_keyname)));
	}
	PyObject* kb_get_mod_state(PyObject* self, PyObject* args) {
		PyObject* modname;

		if (!PyArg_ParseTuple(args, "U", &modname)) {
			return nullptr;
		}

		std::string _modname (PyUnicode_AsUTF8(modname));

		int mod = KMOD_NONE;
		if (_modname == "KMOD_LSHIFT") {
			mod = KMOD_LSHIFT;
		} else if (_modname == "KMOD_RSHIFT") {
			mod = KMOD_RSHIFT;
		} else if (_modname == "KMOD_LCTRL") {
			mod = KMOD_LCTRL;
		} else if (_modname == "KMOD_RCTRL") {
			mod = KMOD_RCTRL;
		} else if (_modname == "KMOD_LALT") {
			mod = KMOD_LALT;
		} else if (_modname == "KMOD_RALT") {
			mod = KMOD_RALT;
		} else if (_modname == "KMOD_LGUI") {
			mod = KMOD_LGUI;
		} else if (_modname == "KMOD_RGUI") {
			mod = KMOD_RGUI;
		} else if (_modname == "KMOD_NUM") {
			mod = KMOD_NUM;
		} else if (_modname == "KMOD_CAPS") {
			mod = KMOD_CAPS;
		} else if (_modname == "KMOD_MODE") {
			mod = KMOD_MODE;
		} else if (_modname == "KMOD_SHIFT") {
			mod = KMOD_SHIFT;
		} else if (_modname == "KMOD_CTRL") {
			mod = KMOD_CTRL;
		} else if (_modname == "KMOD_ALT") {
			mod = KMOD_ALT;
		} else if (_modname == "KMOD_GUI") {
			mod = KMOD_GUI;
		}

		return PyBool_FromLong(kb::get_mod_state(mod));
	}

	PyObject* kb_append_input(PyObject* self, PyObject* args) {
		PyObject* str;
		PyObject* keyname;

		if (!PyArg_ParseTuple(args, "UU", &str, &keyname)) {
			return nullptr;
		}

		str = PyTuple_GetItem(args, 0);
		std::string _str (PyUnicode_AsUTF8(str));

		std::string _keyname (PyUnicode_AsUTF8(keyname));
		SDL_KeyboardEvent kbe {};
		kbe.keysym.sym = kb::keystrings_get_key(_keyname);

		kb::append_input(&_str, &kbe);

		return PyUnicode_FromString(_str.c_str());
	}

	PyObject* kb_bind(PyObject* self, PyObject* args) {
		PyObject* keyname;
		PyObject* keybind;

		if (!PyArg_ParseTuple(args, "UO!", &keyname, &PyDict_Type, &keybind)) {
			return nullptr;
		}

		std::string _keyname (PyUnicode_AsUTF8(keyname));

		PyObject* kb_name = PyDict_GetItemString(keybind, "name");
		if (kb_name == nullptr) {
			return nullptr;
		}
		std::string _kb_name (PyUnicode_AsUTF8(kb_name));

		PyObject* kb_func = PyDict_GetItemString(keybind, "func");
		if (kb_func == nullptr) {
			return nullptr;
		}
		Py_INCREF(kb_func);
		std::function<void (const SDL_Event*)> _kb_func = [kb_func] (const SDL_Event* e) {
			PyObject* keysym = PyDict_New();
			PyDict_SetItemString(keysym, "scancode", PyLong_FromLong(e->key.keysym.scancode));
			PyDict_SetItemString(keysym, "sym", PyLong_FromLong(e->key.keysym.sym));
			PyDict_SetItemString(keysym, "mod", PyLong_FromLong(e->key.keysym.mod));

			PyObject* event = PyDict_New();
			PyDict_SetItemString(event, "type", PyLong_FromLong(e->key.type));
			PyDict_SetItemString(event, "timestamp", PyLong_FromLong(e->key.timestamp));
			PyDict_SetItemString(event, "windowID", PyLong_FromLong(e->key.windowID));
			PyDict_SetItemString(event, "state", PyLong_FromLong(e->key.state));
			PyDict_SetItemString(event, "repeat", PyLong_FromLong(e->key.repeat));
			PyDict_SetItemString(event, "keysym", keysym);

			PyObject* arg_tup = Py_BuildValue("(O)", event);
			if (PyEval_CallObject(kb_func, arg_tup) == nullptr) {
				PyErr_Print();
			}

			Py_DECREF(arg_tup);
		};

		KeyBind _keybind (_kb_name, _kb_func);

		PyObject* kb_key = PyDict_GetItemString(keybind, "key");
		if (kb_key != nullptr) {
			_keybind.key = kb::keystrings_get_key(PyUnicode_AsUTF8(kb_key));
		}
		PyObject* kb_rep = PyDict_GetItemString(keybind, "is_repeatable");
		if (kb_rep != nullptr) {
			_keybind.is_repeatable = PyObject_IsTrue(kb_rep);
		}

		return PyLong_FromLong(kb::bind(kb::keystrings_get_key(_keyname), _keybind));
	}
	PyObject* kb_get_keybind(PyObject* self, PyObject* args) {
		PyObject* keyname;

		if (!PyArg_ParseTuple(args, "U", &keyname)) {
			return nullptr;
		}

		std::string _keyname (PyUnicode_AsUTF8(keyname));

		KeyBind kb = kb::get_keybind(kb::keystrings_get_key(_keyname));

		PyObject* _kb = PyDict_New();
		PyDict_SetItemString(_kb, "name", PyUnicode_FromString(kb.name.c_str()));
		PyDict_SetItemString(_kb, "is_repeatable", PyBool_FromLong(kb.is_repeatable));

		return _kb;
	}
	PyObject* kb_get_keybind_by_name(PyObject* self, PyObject* args) {
		PyObject* bindname;

		if (!PyArg_ParseTuple(args, "U", &bindname)) {
			return nullptr;
		}

		std::string _bindname (PyUnicode_AsUTF8(bindname));

		KeyBind kb = kb::get_keybind(_bindname);

		PyObject* _kb = PyDict_New();
		PyDict_SetItemString(_kb, "key", PyUnicode_FromString(kb::keystrings_get_name(kb.key).c_str()));
		PyDict_SetItemString(_kb, "is_repeatable", PyBool_FromLong(kb.is_repeatable));

		return _kb;
	}
	PyObject* kb_unbind(PyObject* self, PyObject* args) {
		PyObject* keyname;

		if (!PyArg_ParseTuple(args, "U", &keyname)) {
			return nullptr;
		}

		std::string _keyname (PyUnicode_AsUTF8(keyname));

		return PyLong_FromLong(kb::unbind(kb::keystrings_get_key(_keyname)));
	}
	PyObject* kb_unbind_by_name(PyObject* self, PyObject* args) {
		PyObject* bindname;

		if (!PyArg_ParseTuple(args, "U", &bindname)) {
			return nullptr;
		}

		std::string _bindname (PyUnicode_AsUTF8(bindname));

		return PyLong_FromLong(kb::unbind(KeyBind(_bindname)));
	}
	PyObject* kb_unbind_all(PyObject* self, PyObject* args) {
		kb::unbind_all();

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_KB
