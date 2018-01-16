/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_COMMANDS
#define BEE_PYTHON_COMMANDS 1

#include <iostream>

#include "commands.hpp"

#include "python.hpp"

#include "../engine.hpp"

#include "../util/debug.hpp"
#include "../util/platform.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/console.hpp"
#include "../core/rooms.hpp"

#include "../input/kb.hpp"
#include "../input/keybind.hpp"

#include "../network/network.hpp"

#include "../render/drawing.hpp"

#include "../resource/sound.hpp"
#include "../resource/room.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef BEECommandsMethods[] = {
                {"quit", commands_quit, METH_NOARGS, "End the game"},
                {"find", commands_find, METH_VARARGS, "Output all commands which match a certain string"},
                {"clear", commands_clear, METH_NOARGS, "Clear the console log"},

                {"execfile", commands_execfile, METH_VARARGS, "Execute the specified config file"},
                {"log", commands_log, METH_VARARGS, "Log a message to the console"},

                {"bind", commands_bind, METH_VARARGS, "Bind a key to a command"},
                {"unbind", commands_unbind, METH_VARARGS, "Unbind a key from a command"},

                {"screenshot", commands_screenshot, METH_VARARGS, "Save a screenshot to the given file"},

                {"verbosity", commands_verbosity, METH_VARARGS, "Set the verbosity level of the messenger"},

                {"volume", commands_volume, METH_VARARGS, "Set the global sound volume from 0.0 to 1.0"},

                {"info", commands_info, METH_NOARGS, "Output information about the current room"},
                {"restart", commands_restart, METH_NOARGS, "Restart the game"},
                {"restart_room", commands_restart_room, METH_NOARGS, "Restart the current room"},
                {"pause", commands_pause, METH_NOARGS, "Toggle the pause state of the game"},

                {"netstatus", commands_netstatus, METH_NOARGS, "Output information about the network session"},

                {nullptr, nullptr, 0, nullptr}
        };
        PyModuleDef BEECommandsModule = {
                PyModuleDef_HEAD_INIT, "commands", nullptr, -1, BEECommandsMethods,
                nullptr, nullptr, nullptr, nullptr
        };

        PyObject* PyInit_bee_commands() {
                return PyModule_Create(&BEECommandsModule);
        }

        PyObject* commands_quit(PyObject* self, PyObject* args) {
                kb::get_keybind("Quit").call(nullptr);

                Py_RETURN_NONE;
        }
        PyObject* commands_find(PyObject* self, PyObject* args) {
                PyObject* object;

                if (!PyArg_ParseTuple(args, "U", &object)) {
                        return nullptr;
                }

                std::string _object (PyUnicode_AsUTF8(object));

                PyObject* dict = PyModule_GetDict(self);
                PyObject *key, *value;
                Py_ssize_t pos = 0;
                while (PyDict_Next(dict, &pos, &key, &value)) {
                        std::string _key (PyUnicode_AsUTF8(key));
                        if (_key.find(_object) != std::string::npos) {
                                std::cout << _key << "\n";
                                if (PyObject_HasAttrString(value, "__doc__")) {
                                        PyObject* doc (PyObject_GetAttrString(value, "__doc__"));
                                        std::string _doc (PyUnicode_AsUTF8(doc));
                                        std::cout << debug_indent(_doc, 1);
                                }
                        }
                }

                Py_RETURN_NONE;
        }
        PyObject* commands_clear(PyObject* self, PyObject* args) {
                console::clear();

                if (get_options().is_headless) {
                        bee_commandline_clear();
                }

                Py_RETURN_NONE;
        }

        PyObject* commands_execfile(PyObject* self, PyObject* args) {
                PyObject* filename;

                if (!PyArg_ParseTuple(args, "U", &filename)) {
                        return nullptr;
                }

                std::string _filename (PyUnicode_AsUTF8(filename));

                Script::get_by_name("scr_console")->run_file(_filename);

                Py_RETURN_NONE;
        }
        PyObject* commands_log(PyObject* self, PyObject* args) {
                PyObject* str;
                unsigned long type = 3;

                if (!PyArg_ParseTuple(args, "U|k", &str, &type)) {
                        return nullptr;
                }

                std::string _str (PyUnicode_AsUTF8(str));

                E_MESSAGE _type (static_cast<E_MESSAGE>(type));

                console::log(_type, _str);

                Py_RETURN_NONE;
        }

        PyObject* commands_bind(PyObject* self, PyObject* args) {
                PyObject* keystring;
                PyObject* command = nullptr;

                if (!PyArg_ParseTuple(args, "U|U", &keystring, &command)) {
                        return nullptr;
                }

                keystring = PyTuple_GetItem(args, 0);
                std::string _keystring (PyUnicode_AsUTF8(keystring));
                SDL_Keycode k (kb::keystrings_get_key(_keystring));

                if (command == nullptr) {
                        std::string _name (kb::get_keybind(k).name);
                        return Py_BuildValue("N", PyUnicode_FromString(_name.c_str()));
                } else {
                        std::string _command (PyUnicode_AsUTF8(command));

                        KeyBind kb;
                        if (_command.front() == '$') { // Bind to an existing KeyBind with the given name
                                kb = kb::get_keybind(_command.substr(1));
                        } else { // Otherwise bind to the console string
                                kb = KeyBind(_command, k, false, [_command] (const SDL_Event* e) {
                                        console::internal::run(_command, false);
                                });
                        }
                        kb::bind(k, kb);
                }

                Py_RETURN_NONE;
        }
        PyObject* commands_unbind(PyObject* self, PyObject* args) {
                PyObject* keystring;

                if (!PyArg_ParseTuple(args, "U", &keystring)) {
                        return nullptr;
                }

                std::string _keystring (PyUnicode_AsUTF8(keystring));

                if (_keystring == "all") {
                        kb::unbind_all();
                } else if (_keystring.rfind("SDLK_", 0) == 0) {
                        kb::unbind(kb::keystrings_get_key(_keystring));
                } else {
                        kb::unbind(KeyBind(_keystring));
                }

                Py_RETURN_NONE;
        }

        PyObject* commands_screenshot(PyObject* self, PyObject* args) {
                PyObject* filename = nullptr;

                if (!PyArg_ParseTuple(args, "|U", &filename)) {
                        return nullptr;
                }

                if (filename == nullptr) {
                        save_screenshot("screenshot.bmp");
                } else {
                        std::string _filename (PyUnicode_AsUTF8(filename));
                        save_screenshot(_filename);
                }

                Py_RETURN_NONE;
        }

        PyObject* commands_verbosity(PyObject* self, PyObject* args) {
                int level = -1;

                if (!PyArg_ParseTuple(args, "|i", &level)) {
                        return nullptr;
                }

                if (level < 0) {
                        messenger::send({"engine", "console"}, E_MESSAGE::INFO, std::to_string(static_cast<int>(messenger::get_level())));
                } else {
                        level = std::min(level, static_cast<int>(E_OUTPUT::VERBOSE));
                        messenger::set_level(static_cast<E_OUTPUT>(level));
                }

                Py_RETURN_NONE;
        }

        PyObject* commands_volume(PyObject* self, PyObject* args) {
                double volume = -1.0;

                if (!PyArg_ParseTuple(args, "|d", &volume)) {
                        return nullptr;
                }

                if (volume < 0.0) {
                        messenger::send({"engine", "console"}, E_MESSAGE::INFO, std::to_string(Sound::get_master_volume()));
                } else {
                        volume = std::min(volume, 1.0);
                        Sound::set_master_volume(volume);
                }

                Py_RETURN_NONE;
        }

        PyObject* commands_info(PyObject* self, PyObject* args) {
                messenger::send({"engine", "console"}, E_MESSAGE::INFO, get_current_room()->get_print());

                Py_RETURN_NONE;
        }
        PyObject* commands_restart(PyObject* self, PyObject* args) {
                restart_game();

                Py_RETURN_NONE;
        }
        PyObject* commands_restart_room(PyObject* self, PyObject* args) {
                restart_room();

                Py_RETURN_NONE;
        }
        PyObject* commands_pause(PyObject* self, PyObject* args) {
                set_is_paused(!get_is_paused());

                Py_RETURN_NONE;
        }

        PyObject* commands_netstatus(PyObject* self, PyObject* args) {
                messenger::send({"engine", "console"}, E_MESSAGE::INFO, net::get_print());

                Py_RETURN_NONE;
        }
}}}

#endif // BEE_PYTHON_COMMANDS
