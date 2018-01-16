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

namespace bee { namespace python { namespace internal {
        PyMethodDef BEEConsoleMethods[] = {
                {"open", console_open, METH_NOARGS, "Open the console"},
                {"close", console_close, METH_NOARGS, "Close the console"},
                {"toggle", console_toggle, METH_NOARGS, "Toggle the open/close state of the console"},
                {"get_is_open", console_get_is_open, METH_NOARGS, "Return whether the console is open or not"},

                {"add_command", console_add_command, METH_VARARGS, ""},

                {"set_var", console_set_var, METH_VARARGS, ""},
                {"get_var", console_get_var, METH_VARARGS, ""},

                {"run", console_run, METH_VARARGS, ""},
                {"get_help", console_get_help, METH_VARARGS, ""},
                {"log", console_log, METH_VARARGS, ""},

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

        PyObject* console_add_command(PyObject* self, PyObject* args) {
                /*PyObject* command;
                PyObject* descr;
                PyObject* callback;

                if (!PyArg_ParseTuple(args, "UUO", &command, &descr, &callback)) {
                        return nullptr;
                }

                command = PyTuple_GetItem(args, 0); // For some reason, PyArg_ParseTuple() doesn't set the first arg correctly
                std::string _command (PyUnicode_AsUTF8(command));

                std::string _descr (PyUnicode_AsUTF8(descr));

                if (!PyCallable_Check(callback)) {
                        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                        return nullptr;
                }
                Py_INCREF(callback);

                console::add_command(_command, _descr, [callback] (const MessageContents& msg) {
                        PyObject* msg_tags = PyList_New(0);
                        for (auto& t : msg.tags) {
                                PyList_Append(msg_tags, PyUnicode_FromString(t.c_str()));
                        }

                        PyObject* arg_tup = Py_BuildValue("(kNbN)", msg.tickstamp, msg_tags, msg.type, PyUnicode_FromString(msg.descr.c_str()));
                        if (PyEval_CallObject(callback, arg_tup) == nullptr) {
                                if (PyErr_Occurred()) {
                                        PyErr_Print();
                                }
                        }

                        Py_DECREF(arg_tup);
                        Py_DECREF(msg_tags);
                });*/

                Py_RETURN_NONE;
        }

        PyObject* console_set_var(PyObject* self, PyObject* args) {
                Py_RETURN_NONE;
        }
        PyObject* console_get_var(PyObject* self, PyObject* args) {
                Py_RETURN_NONE;
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
        PyObject* console_get_help(PyObject* self, PyObject* args) {
                PyObject* obj;

                if (!PyArg_ParseTuple(args, "O", &obj)) {
                        return nullptr;
                }

                if (PyObject_HasAttrString(obj, "__doc__")) {
                        return PyObject_GetAttrString(obj, "__doc__");
                }

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
