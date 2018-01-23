/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_BEEMODULE
#define BEE_PYTHON_BEEMODULE 1

#include <string>
#include <vector>

#include "beemodule.hpp"

#include "python.hpp"
#include "messenger.hpp"
#include "console.hpp"
#include "mouse.hpp"
#include "kb.hpp"

#include "../engine.hpp"

#include "../init/gameoptions.hpp"
#include "../init/programflags.hpp"

#include "../core/display.hpp"
#include "../core/rooms.hpp"
#include "../core/window.hpp"

#include "../resource/room.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef BEEMethods[] = {
                {"get_ticks", get_ticks, METH_NOARGS, "Return the millisecond ticks elapsed since initialization"},
                {"get_seconds", get_seconds, METH_NOARGS, "Return the seconds elapsed since initialization"},
                {"get_frame", get_frame, METH_NOARGS, "Return the frames elapsed since initialization"},
                {"get_delta", get_delta, METH_NOARGS, "Return the seconds elapsed since last frame"},
                {"get_tick_delta", get_tick_delta, METH_NOARGS, "Return the millisecond ticks elapsed since last frame"},
                {"get_fps_goal", get_fps_goal, METH_NOARGS, "Return the goal frames per second"},

                {"restart_game", restart_game, METH_NOARGS, "Restart the game without reinitializaing"},
                {"end_game", end_game, METH_NOARGS, "End the game"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyMethodDef BEEInitMethods[] = {
                {"add_flag", init_add_flag, METH_VARARGS, "Add a program flag for post-init parsing"},

                {"get_option", init_get_option, METH_VARARGS, "Return the option value"},
                {"set_option", init_set_option, METH_VARARGS, "Assign a value and setter callback to the given option"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyMethodDef BEECoreMethods[] = {
                {"get_display", core_get_display, METH_NOARGS, "Return the SDL display mode"},
                {"get_display_size", core_get_display_size, METH_NOARGS, "Return the size of the display"},
                {"get_display_refresh_rate", core_get_display_refresh_rate, METH_NOARGS, "Return the refresh rate of the display"},
                {"set_display", core_set_display, METH_VARARGS, "Set the SDL display mode's width, height, and refresh rate"},
                {"set_display_size", core_set_display_size, METH_VARARGS, "Set the width and height of the display"},
                {"set_display_refresh_rate", core_set_display_refresh_rate, METH_VARARGS, "Set the refresh rate of the display"},

                {"restart_room", core_restart_room, METH_NOARGS, "Change to the current room again"},
                {"change_room", core_change_room, METH_VARARGS, "Handle room transitions and resource changes between rooms"},
                {"get_current_room", core_get_current_room, METH_NOARGS, "Return the current room resource"},
                {"get_room_size", core_get_room_size, METH_NOARGS, "Return the size of the current room"},
                {"is_on_screen", core_is_on_screen, METH_VARARGS, "Return whether the given rectangle will appear on screen"},
                {"set_is_paused", core_set_is_paused, METH_VARARGS, "Set the pause state of the engine"},
                {"get_is_paused", core_get_is_paused, METH_NOARGS, "Return the pause state of the engine"},

                {"get_window_title", core_get_window_title, METH_NOARGS, "Return the current window title string"},
                {"get_window", core_get_window, METH_NOARGS, "Return the window coordinates and dimensions"},
                {"get_window_pos", core_get_window_pos, METH_NOARGS, "Return the x- and y-coordinates of the game window"},
                {"get_window_size", core_get_window_size, METH_NOARGS, "Return the size of the game window"},
                {"set_window_title", core_set_window_title, METH_VARARGS, "Set the title string of the current window"},
                {"set_window_pos", core_set_window_pos, METH_VARARGS, "Set the game window position"},
                {"set_window_center", core_set_window_center, METH_NOARGS, "Center the game window on the screen"},
                {"set_window_size", core_set_window_size, METH_VARARGS, "Set the size of the game window"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyModuleDef BEEModule = {
                PyModuleDef_HEAD_INIT, "bee", nullptr, -1, BEEMethods,
                nullptr, nullptr, nullptr, nullptr
        };

        PyObject* make_enum(const std::vector<std::string>& enums, long start=0) {
                PyObject* _enum = PyDict_New();

                long i = start;
                for (auto& e : enums) {
                        if (e.front() == '=') {
                                PyDict_SetItemString(_enum, e.substr(1).c_str(), PyLong_FromLong(i));
                        } else {
                                PyDict_SetItemString(_enum, e.c_str(), PyLong_FromLong(i++));
                        }
                }

                return _enum;
        }
        PyObject* make_enum_bits(const std::vector<std::string>& enums, long start=0) {
                PyObject* _enum = PyDict_New();

                long i = start;
                for (auto& e : enums) {
                        if (e.front() == '=') {
                                PyDict_SetItemString(_enum, e.substr(1).c_str(), PyLong_FromLong(1u << i));
                        } else {
                                PyDict_SetItemString(_enum, e.c_str(), PyLong_FromLong(1u << i++));
                        }
                }

                return _enum;
        }

        int init_module() {
                PyImport_AppendInittab("bee", &PyInit_bee);
                return 0;
        }
        PyObject* PyInit_bee() {
                PyObject* module = PyModule_Create(&BEEModule);

                // Add submodules
                PyModule_AddFunctions(module, BEEInitMethods);
                PyModule_AddFunctions(module, BEECoreMethods);
                PyModule_AddObject(module, "messenger", PyInit_bee_messenger());
                PyModule_AddObject(module, "console", PyInit_bee_console());
                PyModule_AddObject(module, "mouse", PyInit_bee_mouse());
                PyModule_AddObject(module, "kb", PyInit_bee_kb());
                //PyModule_AddObject(module, "loader", PyInit_bee_loader());
                //PyModule_AddObject(module, "network", PyInit_bee_network());
                //PyModule_AddObject(module, "physics", PyInit_bee_physics());
                //PyModule_AddObject(module, "render", PyInit_bee_render());
                //PyModule_AddObject(module, "resource", PyInit_bee_resource());
                //PyModule_AddObject(module, "ui", PyInit_bee_ui());
                //PyModule_AddObject(module, "util", PyInit_bee_util());

                // Add enums
                PyModule_AddObject(module, "E_FLAGARG", make_enum({
                        "NONE",
                        "OPTIONAL",
                        "REQUIRED"
                }));
                PyModule_AddObject(module, "E_SOUNDEFFECT", make_enum_bits({
                        "NONE",
                        "CHORUS",
                        "ECHO",
                        "FLANGER",
                        "GARGLE",
                        "REVERB",
                        "COMPRESSOR",
                        "EQUALIZER"
                }));
                PyModule_AddObject(module, "E_RGB", make_enum({
                        "=CYAN", "AQUA",
                        "BLACK",
                        "BLUE",
                        "DKGRAY",
                        "=MAGENTA", "FUCHSIA",
                        "GRAY",
                        "GREEN",
                        "LIME",
                        "=LTGRAY", "SILVER",
                        "MAROON",
                        "NAVY",
                        "OLIVE",
                        "ORANGE",
                        "PURPLE",
                        "RED",
                        "TEAL",
                        "WHITE",
                        "YELLOW"
                }));
                PyModule_AddObject(module, "E_RENDERER", make_enum({
                        "OPENGL3",
                        "OPENGL4"
                }));
                PyModule_AddObject(module, "E_EVENT", make_enum({
                        "UPDATE",
        		"CREATE",
        		"DESTROY",
        		"ALARM",
        		"STEP_BEGIN",
        		"STEP_MID",
        		"STEP_END",
        		"KEYBOARD_PRESS",
        		"MOUSE_PRESS",
        		"KEYBOARD_INPUT",
        		"MOUSE_INPUT",
        		"KEYBOARD_RELEASE",
        		"MOUSE_RELEASE",
        		"CONTROLLER_AXIS",
        		"CONTROLLER_PRESS",
        		"CONTROLLER_RELEASE",
        		"CONTROLLER_MODIFY",
        		"COMMANDLINE_INPUT",
        		"PATH_END",
        		"OUTSIDE_ROOM",
        		"INTERSECT_BOUNDARY",
        		"COLLISION",
        		"CHECK_COLLISION_LIST",
        		"DRAW",
        		"ANIMATION_END",
        		"ROOM_START",
        		"ROOM_END",
        		"GAME_START",
        		"GAME_END",
        		"WINDOW",
        		"NETWORK"
                }));
                PyModule_AddObject(module, "E_LIGHT", make_enum({
                        "AMBIENT",
                        "DIFFUSE",
                        "POINT",
                        "SPOT"
                }, 1));
                PyModule_AddObject(module, "E_TRANSITION", make_enum({
                        "NONE",
        		"CREATE_LEFT",
        		"CREATE_RIGHT",
        		"CREATE_TOP",
        		"CREATE_BOTTOM",
        		"CREATE_CENTER",
        		"SHIFT_LEFT",
        		"SHIFT_RIGHT",
        		"SHIFT_TOP",
        		"SHIFT_BOTTOM",
        		"INTERLACE_LEFT",
        		"INTERLACE_RIGHT",
        		"INTERLACE_TOP",
        		"INTERLACE_BOTTOM",
        		"PUSH_LEFT",
        		"PUSH_RIGHT",
        		"PUSH_TOP",
        		"PUSH_BOTTOM",
        		"ROTATE_LEFT",
        		"ROTATE_RIGHT",
        		"BLEND",
        		"FADE",
        		"CUSTOM"
                }));
                PyModule_AddObject(module, "E_MESSAGE", make_enum({
                        "GENERAL",
        		"START",
        		"END",
        		"INFO",
        		"WARNING",
        		"ERROR",
        		"INTERNAL"
                }));
                PyModule_AddObject(module, "E_OUTPUT", make_enum({
                        "NONE",
                        "QUIET",
                        "NORMAL",
                        "VERBOSE"
                }));
                PyModule_AddObject(module, "E_PHYS_SHAPE", make_enum({
                        "NONE",
                        "SPHERE",
                        "BOX",
                        "CYLINDER",
                        "CAPSULE",
                        "CONE",
                        "MULTISPHERE",
                        "CONVEX_HULL"
                }));
                PyModule_AddObject(module, "E_PHYS_CONSTRAINT", make_enum({
                        "NONE",
                        "POINT",
                        "HINGE",
                        "SLIDER",
                        "CONE",
                        "SIXDOF",
                        "FIXED",
                        "FLAT",
                        "TILE"
                }));
                PyModule_AddObject(module, "E_COMPUTATION", make_enum_bits({
                        "NOTHING",
                        "STATIC",
                        "SEMISTATIC",
                        "SEMIPLAYER",
                        "PLAYER",
                        "DYNAMIC"
                }));
                PyModule_AddObject(module, "E_PATH_END", make_enum({
                        "STOP",
                        "RESTART",
                        "CONTINUE",
                        "REVERSE"
                }));
                PyModule_AddObject(module, "E_NETEVENT", make_enum({
                        "NONE",
                        "CONNECT",
                        "DISCONNECT",
                        "KEYFRAME",
                        "PLAYER_UPDATE",
                        "INST_UPDATE",
                        "DATA_UPDATE",
                        "OTHER"
                }));
                PyObject* netsig1 (make_enum({
                        "CONNECT",
                        "DISCONNECT",
                        "SERVER_INFO",
                        "CLIENT_INFO",
                }, 1));
                PyDict_SetItemString(netsig1, "INVALID", PyLong_FromLong(255));
                PyModule_AddObject(module, "E_NETSIG1", netsig1);
                PyObject* netsig2 (make_enum({
                        "=NONE",
                        "KEEPALIVE",
                        "NAME",
                        "PLAYERS",
                        "KEYFRAME",
                        "DELTA",
                }));
                PyDict_SetItemString(netsig2, "INVALID", PyLong_FromLong(255));
                PyModule_AddObject(module, "E_NETSIG2", netsig2);
                PyModule_AddObject(module, "E_DATA_TYPE", make_enum({
                        "NONE",
                        "CHAR",
                        "INT",
                        "FLOAT",
                        "DOUBLE",
                        "STRING",
                        "VECTOR",
                        "MAP",
                        "SERIAL"
                }));
                PyModule_AddObject(module, "E_PT_SHAPE", make_enum({
                        "PIXEL",
        		"DISK",
        		"SQUARE",
        		"LINE",
        		"STAR",
        		"CIRCLE",
        		"RING",
        		"SPHERE",
        		"FLARE",
        		"SPARK",
        		"EXPLOSION",
        		"CLOUD",
        		"SMOKE",
        		"SNOW"
                }));
                PyModule_AddObject(module, "E_PS_SHAPE", make_enum({
                        "RECTANGLE",
                        "LINE",
                        "CIRCLE"
                }));
                PyModule_AddObject(module, "E_PS_DISTR", make_enum({
                        "LINEAR",
                        "GAUSSIAN",
                        "INVGAUSSIAN"
                }));
                PyModule_AddObject(module, "E_PS_FORCE", make_enum({
                        "CONSTANT",
                        "LINEAR",
                        "QUADRATIC"
                }));
                PyModule_AddObject(module, "E_PS_CHANGE", make_enum({
                        "MOTION",
                        "LOOK",
                        "ALL"
                }));

                return module;
        }

        PyObject* get_ticks(PyObject* self, PyObject* args) {
                return Py_BuildValue("k", bee::get_ticks());
        }
        PyObject* get_seconds(PyObject* self, PyObject* args) {
                return Py_BuildValue("k", bee::get_seconds());
        }
        PyObject* get_frame(PyObject* self, PyObject* args) {
                return Py_BuildValue("k", bee::get_frame());
        }
        PyObject* get_delta(PyObject* self, PyObject* args) {
                return Py_BuildValue("d", bee::get_delta());
        }
        PyObject* get_tick_delta(PyObject* self, PyObject* args) {
                return Py_BuildValue("k", bee::get_tick_delta());
        }
        PyObject* get_fps_goal(PyObject* self, PyObject* args) {
                return Py_BuildValue("k", bee::get_fps_goal());
        }

        PyObject* restart_game(PyObject* self, PyObject* args) {
                bee::restart_game();

                Py_RETURN_NONE;
        }
        PyObject* end_game(PyObject* self, PyObject* args) {
                bee::end_game();

                Py_RETURN_NONE;
        }

        PyObject* init_add_flag(PyObject* self, PyObject* args) {
                PyObject* longopt;
                int shortopt;
                unsigned long arg_type;
                PyObject* callback;

                if (!PyArg_ParseTuple(args, "UCkO", &longopt, &shortopt, &arg_type, &callback)) {
                        return nullptr;
                }

                longopt = PyTuple_GetItem(args, 0);
                std::string _longopt (PyUnicode_AsUTF8(longopt));

                char _shortopt = shortopt;

                E_FLAGARG _arg_type = static_cast<E_FLAGARG>(arg_type);

                if (!PyCallable_Check(callback)) {
                        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                        return nullptr;
                }
                Py_INCREF(callback);

                add_flag(new ProgramFlag(_longopt, _shortopt, false, _arg_type, [callback] (const std::string& arg) {
                        PyObject* arg_tup = Py_BuildValue("(N)", PyUnicode_FromString(arg.c_str()));
                        if (PyEval_CallObject(callback, arg_tup) == nullptr) {
                                PyErr_Print();
                        }
                        Py_DECREF(arg_tup);
                }));

                Py_RETURN_NONE;
        }

        PyObject* init_get_option(PyObject* self, PyObject* args) {
                PyObject* name;

                if (!PyArg_ParseTuple(args, "U", &name)) {
                        return nullptr;
                }

                std::string _name (PyUnicode_AsUTF8(name));

                return Py_BuildValue("N", variant_to_pyobj(get_option(_name)));
        }
        PyObject* init_set_option(PyObject* self, PyObject* args) {
                PyObject* name;
                PyObject* value;
                PyObject* setter = nullptr;

                if (!PyArg_ParseTuple(args, "UO|O", &name, &value, &setter)) {
                        return nullptr;
                }

                std::string _name (PyUnicode_AsUTF8(name));

                Variant _value (pyobj_to_variant(value));

                if (setter == nullptr) {
                        return Py_BuildValue("i", set_option(_name, _value));
                }

                if (!PyCallable_Check(setter)) {
                        PyErr_SetString(PyExc_TypeError, "parameter must be callable");
                        return nullptr;
                }
                Py_INCREF(setter);

                return Py_BuildValue("i", set_option(_name, _value, [setter] (GameOption* option, Variant new_value) {
                        PyObject* arg_tup = Py_BuildValue("(N)", variant_to_pyobj(new_value));
                        PyObject* ret = PyEval_CallObject(setter, arg_tup);

                        if (ret == nullptr) {
                                PyErr_Print();
                                Py_DECREF(arg_tup);
                                return 2;
                        } else if (ret == Py_None) {
                                Py_DECREF(arg_tup);
                                return 1;
                        } else {
                                option->value = pyobj_to_variant(ret);
                                Py_DECREF(arg_tup);
                                return 0;
                        }
                }));
        }

        PyObject* core_get_display(PyObject* self, PyObject* args) {
                SDL_DisplayMode dm (get_display());
                return Py_BuildValue("(kkkk)", dm.format, dm.w, dm.h, dm.refresh_rate);
        }
        PyObject* core_get_display_size(PyObject* self, PyObject* args) {
                std::pair<int,int> size (get_display_size());
                return Py_BuildValue("(kk)", size.first, size.second);
        }
        PyObject* core_get_display_refresh_rate(PyObject* self, PyObject* args) {
                return Py_BuildValue("k", get_display().refresh_rate);
        }
        PyObject* core_set_display(PyObject* self, PyObject* args) {
                unsigned long w, h, hz;

                if (!PyArg_ParseTuple(args, "kkk", &w, &h, &hz)) {
                        return nullptr;
                }

                set_display(w, h, hz);

                Py_RETURN_NONE;
        }
        PyObject* core_set_display_size(PyObject* self, PyObject* args) {
                unsigned long w, h;

                if (!PyArg_ParseTuple(args, "kk", &w, &h)) {
                        return nullptr;
                }

                set_display_size(w, h);

                Py_RETURN_NONE;
        }
        PyObject* core_set_display_refresh_rate(PyObject* self, PyObject* args) {
                unsigned long hz;

                if (!PyArg_ParseTuple(args, "k", &hz)) {
                        return nullptr;
                }

                set_display_refresh_rate(hz);

                Py_RETURN_NONE;
        }

        PyObject* core_restart_room(PyObject* self, PyObject* args) {
                restart_room();

                Py_RETURN_NONE;
        }
        PyObject* core_change_room(PyObject* self, PyObject* args) {
                PyObject* roomname;

                if (!PyArg_ParseTuple(args, "U", &roomname)) {
                        return nullptr;
                }

                std::string _roomname (PyUnicode_AsUTF8(roomname));

                Room* rm = Room::get_by_name(_roomname);
                if (rm == nullptr) {
                        PyErr_SetString(PyExc_ValueError, "parameter must be the name of an existing room");
                        return nullptr;
                }

                change_room(rm);

                Py_RETURN_NONE;
        }
        PyObject* core_get_current_room(PyObject* self, PyObject* args) {
                if (get_current_room() == nullptr) {
                        PyErr_SetString(PyExc_RuntimeError, "null room");
                        return nullptr;
                }

                std::string roomname (get_current_room()->get_name());
                return Py_BuildValue("N", PyUnicode_FromString(roomname.c_str())); // TODO: return a room struct instead of the name
        }
        PyObject* core_get_room_size(PyObject* self, PyObject* args) {
                if (get_current_room() == nullptr) {
                        PyErr_SetString(PyExc_RuntimeError, "null room");
                        return nullptr;
                }

                std::pair<int,int> size (get_room_size());
                return Py_BuildValue("(kk)", static_cast<unsigned long>(size.first), static_cast<unsigned long>(size.second));
        }
        PyObject* core_is_on_screen(PyObject* self, PyObject* args) {
                SDL_Rect rect;

                if (!PyArg_ParseTuple(args, "(iiii)", &rect.x, &rect.y, &rect.w, &rect.h)) {
                        return nullptr;
                }

                return Py_BuildValue("O", is_on_screen(rect) ? Py_True : Py_False);
        }
        PyObject* core_set_is_paused(PyObject* self, PyObject* args) {
                bool is_paused;

                if (!PyArg_ParseTuple(args, "p", &is_paused)) {
                        return nullptr;
                }

                set_is_paused(is_paused);

                Py_RETURN_NONE;
        }
        PyObject* core_get_is_paused(PyObject* self, PyObject* args) {
                return Py_BuildValue("O", get_is_paused() ? Py_True : Py_False);
        }

        PyObject* core_get_window_title(PyObject* self, PyObject* args) {
                std::string title (get_window_title());
                return Py_BuildValue("N", PyUnicode_FromString(title.c_str()));
        }
        PyObject* core_get_window(PyObject* self, PyObject* args) {
                SDL_Rect win (get_window());
                return Py_BuildValue("(iiii)", win.x, win.y, win.w, win.h);
        }
        PyObject* core_get_window_pos(PyObject* self, PyObject* args) {
                std::pair<int,int> pos (get_window_pos());
                return Py_BuildValue("(ii)", pos.first, pos.second);
        }
        PyObject* core_get_window_size(PyObject* self, PyObject* args) {
                std::pair<int,int> size (get_window_size());
                return Py_BuildValue("(ii)", size.first, size.second);
        }
        PyObject* core_set_window_title(PyObject* self, PyObject* args) {
                PyObject* title;

                if (!PyArg_ParseTuple(args, "U", &title)) {
                        return nullptr;
                }

                std::string _title (PyUnicode_AsUTF8(title));

                set_window_title(_title);

                Py_RETURN_NONE;
        }
        PyObject* core_set_window_pos(PyObject* self, PyObject* args) {
                int x, y;

                if (!PyArg_ParseTuple(args, "ii", &x, &y)) {
                        return nullptr;
                }

                set_window_position(x, y);

                Py_RETURN_NONE;
        }
        PyObject* core_set_window_center(PyObject* self, PyObject* args) {
                set_window_center();

                Py_RETURN_NONE;
        }
        PyObject* core_set_window_size(PyObject* self, PyObject* args) {
                int w, h;

                if (!PyArg_ParseTuple(args, "ii", &w, &h)) {
                        return nullptr;
                }

                set_window_size(w, h);

                Py_RETURN_NONE;
        }
}}}

#endif // BEE_PYTHON_BEEMODULE
