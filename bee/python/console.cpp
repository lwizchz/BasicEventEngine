/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_CONSOLE
#define BEE_PYTHON_CONSOLE 1

#include "console.hpp"

#include "commands.hpp"

#include "../core/console.hpp"

#include "python.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef BEEConsoleMethods[] = {
                {"open", console_open, METH_NOARGS, "Open the console"},
                {"close", console_close, METH_NOARGS, "Close the console"},
                {"toggle", console_toggle, METH_NOARGS, "Toggle the open/close state of the console"},
                {"get_is_open", console_get_is_open, METH_NOARGS, "Return whether the console is open or not"},

                {"set_var", console_set_var, METH_VARARGS, "Set a console variable"},
                {"get_var", console_get_var, METH_VARARGS, "Return the value of a console variable"},

                {"run", console_run, METH_VARARGS, "Run a command in the console"},
                {"log", console_log, METH_VARARGS, "Log a message to the console"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyModuleDef BEEConsoleModule = {
                PyModuleDef_HEAD_INIT, "console", nullptr, -1, BEEConsoleMethods,
                nullptr, nullptr, nullptr, nullptr
        };

        PyObject* PyInit_bee_console() {
                PyObject* module = PyModule_Create(&BEEConsoleModule);

                // Add submodules
                PyModule_AddObject(module, "commands", PyInit_bee_commands());

                return module;
        }

        PyObject* console_open(PyObject* self, PyObject* args) {
                console::open();

                Py_RETURN_NONE;
        }
        PyObject* console_close(PyObject* self, PyObject* args) {
                console::close();

                Py_RETURN_NONE;
        }
        PyObject* console_toggle(PyObject* self, PyObject* args) {
                console::toggle();

                Py_RETURN_NONE;
        }
        PyObject* console_get_is_open(PyObject* self, PyObject* args) {
                return Py_BuildValue("O", console::get_is_open() ? Py_True : Py_False);
        }

        PyObject* console_set_var(PyObject* self, PyObject* args) {
                PyObject* name;
                PyObject* value;

                if (!PyArg_ParseTuple(args, "UO", &name, &value)) {
                        return nullptr;
                }

                std::string _name (PyUnicode_AsUTF8(name));

                Variant _value (pyobj_to_variant(value));

                console::set_var(_name, _value);

                Py_RETURN_NONE;
        }
        PyObject* console_get_var(PyObject* self, PyObject* args) {
                PyObject* name;

                if (!PyArg_ParseTuple(args, "U", &name)) {
                        return nullptr;
                }

                std::string _name (PyUnicode_AsUTF8(name));

                return Py_BuildValue("N", variant_to_pyobj(console::get_var(_name)));
        }

        PyObject* console_run(PyObject* self, PyObject* args) {
                PyObject* command;

                if (!PyArg_ParseTuple(args, "U", &command)) {
                        return nullptr;
                }

                std::string _command (PyUnicode_AsUTF8(command));

                console::run(_command);

                Py_RETURN_NONE;
        }
        PyObject* console_log(PyObject* self, PyObject* args) {
                unsigned long type;
                PyObject* str;

                if (!PyArg_ParseTuple(args, "kU", &type, &str)) {
                        return nullptr;
                }

                E_MESSAGE _type (static_cast<E_MESSAGE>(type));

                std::string _str (PyUnicode_AsUTF8(str));

                console::log(_type, _str);

                Py_RETURN_NONE;
        }
}}}

#endif // BEE_PYTHON_CONSOLE
