/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_KB
#define BEE_PYTHON_KB 1

#include "kb.hpp"

#include "../input/kb.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef BEEKBMethods[] = {
                {"get_state", kb_get_state, METH_VARARGS, "Return whether the given keycode is pressed"},
                {"get_mod_state", kb_get_mod_state, METH_VARARGS, "Return whether the given modifier key is pressed"},

                {"append_input", kb_append_input, METH_VARARGS, "Append a keyboard event key to the given string"},

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

                return Py_BuildValue("O", kb::get_state(kb::keystrings_get_key(_keyname)) ? Py_True : Py_False);
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

                return Py_BuildValue("O", kb::get_mod_state(mod) ? Py_True : Py_False);
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

                return Py_BuildValue("N", PyUnicode_FromString(_str.c_str()));
        }
}}}

#endif // BEE_PYTHON_KB
