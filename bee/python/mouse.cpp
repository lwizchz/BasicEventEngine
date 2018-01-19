/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_MOUSE
#define BEE_PYTHON_MOUSE 1

#include "mouse.hpp"

#include "../util/string.hpp"

#include "../input/mouse.hpp"

#include "../resource/texture.hpp"
#include "../resource/object.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef BEEMouseMethods[] = {
                {"get_display_pos", mouse_get_display_pos, METH_NOARGS, "Return the mouse coordinates relative to the display"},
                {"get_pos", mouse_get_pos, METH_NOARGS, "Return the mouse coordinates relative to the window"},
                {"get_relative_pos", mouse_get_relative_pos, METH_NOARGS, "Return the mouse coordinates relative to the viewport"},
                {"set_display_pos", mouse_set_display_pos, METH_VARARGS, "Set the mouse coordinates relative to the display"},
                {"set_pos", mouse_set_pos, METH_VARARGS, "Set the mouse coordinates relative to the window"},

                {"is_inside", mouse_is_inside, METH_VARARGS, "Return whether the mouse collides with the instance's bounding box"},

                {"get_state", mouse_get_state, METH_VARARGS, "Return whether the given mouse button is pressed"},

                {"set_cursor", mouse_set_cursor, METH_VARARGS, "Change the window cursor"},
                {"set_show_cursor", mouse_set_show_cursor, METH_VARARGS, "Set whether to show the cursor or not"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyModuleDef BEEMouseModule = {
                PyModuleDef_HEAD_INIT, "mouse", nullptr, -1, BEEMouseMethods,
                nullptr, nullptr, nullptr, nullptr
        };

        PyObject* PyInit_bee_mouse() {
                return PyModule_Create(&BEEMouseModule);
        }

        PyObject* mouse_get_display_pos(PyObject* self, PyObject* args) {
                std::pair<int,int> pos (mouse::get_display_pos());
                return Py_BuildValue("(ii)", pos.first, pos.second);
        }
        PyObject* mouse_get_pos(PyObject* self, PyObject* args) {
                std::pair<int,int> pos (mouse::get_pos());
                return Py_BuildValue("(ii)", pos.first, pos.second);
        }
        PyObject* mouse_get_relative_pos(PyObject* self, PyObject* args) {
                std::pair<int,int> pos (mouse::get_relative_pos());
                return Py_BuildValue("(ii)", pos.first, pos.second);
        }
        PyObject* mouse_set_display_pos(PyObject* self, PyObject* args) {
                int x, y;

                if (!PyArg_ParseTuple(args, "ii", &x, &y)) {
                        return nullptr;
                }

                return Py_BuildValue("i", mouse::set_display_pos(x, y));
        }
        PyObject* mouse_set_pos(PyObject* self, PyObject* args) {
                int x, y;

                if (!PyArg_ParseTuple(args, "ii", &x, &y)) {
                        return nullptr;
                }

                mouse::set_pos(x, y);

                Py_RETURN_NONE;
        }

        PyObject* mouse_is_inside(PyObject* self, PyObject* args) {
                PyObject* obj_name;
                int inst_id;

                if (!PyArg_ParseTuple(args, "(Ui)", &obj_name, &inst_id)) {
                        return nullptr;
                }

                std::string _obj_name (PyUnicode_AsUTF8(obj_name));
                const Object* object (Object::get_by_name(_obj_name));

                const Instance* instance (object->get_instance(inst_id));

                return Py_BuildValue("O", mouse::is_inside(instance) ? Py_True : Py_False);
        }

        PyObject* mouse_get_state(PyObject* self, PyObject* args) {
                unsigned char button;

                if (!PyArg_ParseTuple(args, "b", &button)) {
                        return nullptr;
                }

                return Py_BuildValue("O", mouse::get_state(button) ? Py_True : Py_False);
        }

        PyObject* mouse_set_cursor(PyObject* self, PyObject* args) {
                if (PyTuple_Size(args) <= 1) {
                        PyObject* sys_cursor;

                        if (!PyArg_ParseTuple(args, "U", &sys_cursor)) {
                                return nullptr;
                        }

                        std::string _sys_cursor (PyUnicode_AsUTF8(sys_cursor));
                        _sys_cursor = util::string::lower(_sys_cursor);

                        SDL_SystemCursor cursor = SDL_SYSTEM_CURSOR_ARROW;
                        if (_sys_cursor == "ibeam") {
                                cursor = SDL_SYSTEM_CURSOR_IBEAM;
                        } else if (_sys_cursor == "wait") {
                                cursor = SDL_SYSTEM_CURSOR_WAIT;
                        } else if (_sys_cursor == "crosshair") {
                                cursor = SDL_SYSTEM_CURSOR_CROSSHAIR;
                        } else if (_sys_cursor == "waitarrow") {
                                cursor = SDL_SYSTEM_CURSOR_WAITARROW;
                        } else if (_sys_cursor == "sizenwse") {
                                cursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                        } else if (_sys_cursor == "sizenesw") {
                                cursor = SDL_SYSTEM_CURSOR_SIZENESW;
                        } else if (_sys_cursor == "sizeall") {
                                cursor = SDL_SYSTEM_CURSOR_SIZEALL;
                        } else if (_sys_cursor == "no") {
                                cursor = SDL_SYSTEM_CURSOR_NO;
                        } else if (_sys_cursor == "hand") {
                                cursor = SDL_SYSTEM_CURSOR_HAND;
                        }

                        return Py_BuildValue("i", mouse::set_cursor(cursor));
                } else {
                        PyObject* tex_name;
                        int x, y;

                        if (!PyArg_ParseTuple(args, "Uii", &tex_name, &x, &y)) {
                                return nullptr;
                        }

                        tex_name = PyTuple_GetItem(args, 0);
                        std::string _tex_name (PyUnicode_AsUTF8(tex_name));

                        return Py_BuildValue("i", mouse::set_cursor(Texture::get_by_name(_tex_name), x, y));
                }
        }
        PyObject* mouse_set_show_cursor(PyObject* self, PyObject* args) {
                bool show_cursor;

                if (!PyArg_ParseTuple(args, "p", &show_cursor)) {
                        return nullptr;
                }

                return Py_BuildValue("i", mouse::set_show_cursor(show_cursor));
        }
}}}

#endif // BEE_PYTHON_MOUSE
