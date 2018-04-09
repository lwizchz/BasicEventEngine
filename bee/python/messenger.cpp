/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MESSENGER
#define BEE_PYTHON_MESSENGER 1

#include "messenger.hpp"

#include "../messenger/messenger.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef BEEMessengerMethods[] = {
                {"register_recipient", messenger_register_recipient, METH_VARARGS, "Register the given recipient within the messaging system"},
                {"unregister", messenger_unregister, METH_VARARGS, "Unregister the recipient with the given name within the messaging system"},

                {"send", messenger_send, METH_VARARGS, "Queue the given message in the messaging system"},
                {"log", messenger_log, METH_VARARGS, "Queue the given string as a simple log message"},

                {"add_filter", messenger_add_filter, METH_VARARGS, "Add a tag to the filter list"},
                {"set_filter_blacklist", messenger_set_filter_blacklist, METH_VARARGS, "Set the filter type"},
                {"reset_filter", messenger_reset_filter, METH_NOARGS, "Remove all filters"},

                {"add_log", messenger_add_log, METH_VARARGS, "Add a filename as a log file"},
                {"remove_log", messenger_remove_log, METH_VARARGS, "Remove the given filename from being a log file"},
                {"clear_logs", messenger_clear_logs, METH_VARARGS, "Clear all log files except stdout"},

                {"set_level", messenger_set_level, METH_VARARGS, "Set the output level when printing message descriptions"},
                {"get_level", messenger_get_level, METH_NOARGS, "Return the output level when printing message descriptions"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyModuleDef BEEMessengerModule = {
                PyModuleDef_HEAD_INIT, "messenger", nullptr, -1, BEEMessengerMethods,
                nullptr, nullptr, nullptr, nullptr
        };

        PyObject* PyInit_bee_messenger() {
                return PyModule_Create(&BEEMessengerModule);
        }

        PyObject* messenger_register_recipient(PyObject* self, PyObject* args) {
                PyObject* name;
                PyObject* tag_list;
                bool is_strict;
                PyObject* callback;

                if (!PyArg_ParseTuple(args, "UO!pO", &name, &PyList_Type, &tag_list, &is_strict, &callback)) {
                        return nullptr;
                }

                name = PyTuple_GetItem(args, 0); // For some reason, PyArg_ParseTuple() doesn't set the first arg correctly
                std::string _name (PyUnicode_AsUTF8(name));

                std::vector<std::string> tags;
                Py_ssize_t size = PyList_Size(tag_list);
                for (Py_ssize_t i=0; i<size; ++i) {
                        PyObject* item = PyList_GetItem(tag_list, i);
                        if (!PyUnicode_Check(item)) {
                                PyErr_SetString(PyExc_TypeError, "list items must be strings");
                                return nullptr;
                        }

                        tags.emplace_back(PyUnicode_AsUTF8(item));
                }

                if (!PyCallable_Check(callback)) {
                        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                        return nullptr;
                }

                Py_INCREF(callback);
                messenger::register_recipient(_name, tags, is_strict, [callback] (const MessageContents& msg) {
                        PyObject* msg_tags = PyList_New(0);
                        for (auto& t : msg.tags) {
                                PyList_Append(msg_tags, PyUnicode_FromString(t.c_str()));
                        }

                        PyObject* arg_tup = Py_BuildValue("(kNbN)", msg.tickstamp, msg_tags, msg.type, PyUnicode_FromString(msg.descr.c_str()));
                        if (PyEval_CallObject(callback, arg_tup) == nullptr) {
                                PyErr_Print();
                        }

                        Py_DECREF(arg_tup);
                        Py_DECREF(msg_tags);
                });

                Py_RETURN_NONE;
        }
        PyObject* messenger_unregister(PyObject* self, PyObject* args) {
                PyObject* recv;

                if (!PyArg_ParseTuple(args, "U", &recv)) {
                        return nullptr;
                }

                std::string _recv (PyUnicode_AsUTF8(recv));

                messenger::unregister(_recv);

                Py_RETURN_NONE;
        }

        PyObject* messenger_send(PyObject* self, PyObject* args) {
                PyObject* tag_list;
                unsigned long type;
                PyObject* descr;

                if (!PyArg_ParseTuple(args, "O!kU", &PyList_Type, &tag_list, &type, &descr)) {
                        return nullptr;
                }

                tag_list = PyTuple_GetItem(args, 0); // For some reason, PyArg_ParseTuple() doesn't set the first arg correctly
                std::vector<std::string> tags;
                Py_ssize_t size = PyList_Size(tag_list);
                for (Py_ssize_t i=0; i<size; ++i) {
                        PyObject* item = PyList_GetItem(tag_list, i);
                        if (!PyUnicode_Check(item)) {
                                PyErr_SetString(PyExc_TypeError, "list items must be strings");
                                return nullptr;
                        }

                        tags.emplace_back(PyUnicode_AsUTF8(item));
                }

                E_MESSAGE _type (static_cast<E_MESSAGE>(type));

                std::string _descr (PyUnicode_AsUTF8(descr));

                messenger::send(tags, _type, _descr);

                Py_RETURN_NONE;
        }
        PyObject* messenger_log(PyObject* self, PyObject* args) {
                PyObject* msg;

                if (!PyArg_ParseTuple(args, "U", &msg)) {
                        return nullptr;
                }

                std::string _msg (PyUnicode_AsUTF8(msg));

                messenger::log(_msg);

                Py_RETURN_NONE;
        }

        PyObject* messenger_add_filter(PyObject* self, PyObject* args) {
                PyObject* filter;

                if (!PyArg_ParseTuple(args, "U", &filter)) {
                        return nullptr;
                }

                std::string _filter (PyUnicode_AsUTF8(filter));

                messenger::add_filter(_filter);

                Py_RETURN_NONE;
        }
        PyObject* messenger_set_filter_blacklist(PyObject* self, PyObject* args) {
                bool is_blacklist;

                if (!PyArg_ParseTuple(args, "p", &is_blacklist)) {
                        return nullptr;
                }

                messenger::set_filter_blacklist(is_blacklist);

                Py_RETURN_NONE;
        }
        PyObject* messenger_reset_filter(PyObject* self, PyObject* args) {
                messenger::reset_filter();

                Py_RETURN_NONE;
        }

        PyObject* messenger_add_log(PyObject* self, PyObject* args) {
                PyObject* filename;
                unsigned long level;

                if (!PyArg_ParseTuple(args, "Uk", &filename, &level)) {
                        return nullptr;
                }

                filename = PyTuple_GetItem(args, 0); // For some reason, PyArg_ParseTuple() doesn't set the first arg correctly
                std::string _filename (PyUnicode_AsUTF8(filename));

                E_OUTPUT _level (static_cast<E_OUTPUT>(level));

                messenger::add_log(_filename, _level);

                Py_RETURN_NONE;
        }
        PyObject* messenger_remove_log(PyObject* self, PyObject* args) {
                PyObject* filename;
                bool should_delete;

                if (!PyArg_ParseTuple(args, "Up", &filename, &should_delete)) {
                        return nullptr;
                }

                filename = PyTuple_GetItem(args, 0); // For some reason, PyArg_ParseTuple() doesn't set the first arg correctly
                std::string _filename (PyUnicode_AsUTF8(filename));

                messenger::remove_log(_filename, should_delete);

                Py_RETURN_NONE;
        }
        PyObject* messenger_clear_logs(PyObject* self, PyObject* args) {
                bool should_delete;

                if (!PyArg_ParseTuple(args, "p", &should_delete)) {
                        return nullptr;
                }

                messenger::clear_logs(should_delete);

                Py_RETURN_NONE;
        }

        PyObject* messenger_set_level(PyObject* self, PyObject* args) {
                unsigned long level;

                if (!PyArg_ParseTuple(args, "k", &level)) {
                        return nullptr;
                }

                E_OUTPUT _level (static_cast<E_OUTPUT>(level));

                messenger::set_level(_level);

                Py_RETURN_NONE;
        }
        PyObject* messenger_get_level(PyObject* self, PyObject* args) {
                return Py_BuildValue("i", messenger::get_level());
        }
}}}

#endif // BEE_PYTHON_MESSENGER
