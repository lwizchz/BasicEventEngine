/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON
#define BEE_PYTHON 1

#include <cstdlib>
#include <vector>

#include "python.hpp"

#include "../util/files.hpp"
#include "../util/string.hpp"

#include "../core/enginestate.hpp"

#include "../messenger/messenger.hpp"

#include "beemodule.hpp"

namespace bee { namespace python {
        namespace internal {
                wchar_t* program = nullptr;
                std::vector<wchar_t*> argv;
        }

        int init() {
                if (internal::program != nullptr) {
                        return 0; // Return 0 if Python is already initialized
                }

                internal::program = Py_DecodeLocale(engine->argv[0], nullptr);
                if (internal::program == nullptr) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to decode program argv[0]");
                        return 1;
                }

                wchar_t a0[1] = {0};
                internal::argv.push_back(a0);
                for (int i=1; i<engine->argc; ++i) {
                        wchar_t* arg = Py_DecodeLocale(engine->argv[i], nullptr);
                        if (arg == nullptr) {
                                continue;
                        }
                        internal::argv.push_back(arg);
                }

                internal::init_module();

                setenv("PYTHONPATH", "lib/cpython/build/lib.linux-x86_64-3.7:lib/cpython/Lib:bee/resources", 1);
                setenv("PYTHONHOME", "lib/cpython", 1);

                Py_SetProgramName(internal::program);
                Py_Initialize();

                PySys_SetArgv(internal::argv.size(), internal::argv.data());

                return 0;
        }
        int close() {
                if (internal::program == nullptr) {
                        return 0; // Return 0 if Python is already closed
                }

                if (Py_FinalizeEx() < 0) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to close python scripting");
                        return 1;
                }

                PyMem_RawFree(internal::program);
                internal::program = nullptr;

                while (internal::argv.size() > 1) {
                        PyMem_RawFree(internal::argv.back());
                        internal::argv.pop_back();
                }
                internal::argv.clear();

                return 0;
        }

        int run_string(const std::string& code) {
                PyRun_SimpleString(code.c_str());
                return 0;
        }
        int run_file(const std::string& filename) {
                if (!file_exists(filename)) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to run file \"" + filename + "\": file does not exist");
                        return 1;
                }

                PyObject* fileobj = Py_BuildValue("s", filename.c_str());
                FILE* file = _Py_fopen_obj(fileobj, "r");
                if (file == nullptr) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to open file \"" + filename + "\"");
                        return 2;
                }

                PyRun_SimpleFileEx(file, filename.c_str(), 1);

                return 0;
        }
}

        PythonScriptInterface::PythonScriptInterface(const std::string& _path) :
                path(_path),
                module(nullptr)
        {}
        PythonScriptInterface::PythonScriptInterface(PyObject* _module) :
                path(),
                module(_module)
        {}
        PythonScriptInterface::~PythonScriptInterface() {
                if (module != nullptr) {
                        this->free();
                }
        }
        int PythonScriptInterface::load() {
                if (module != nullptr) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to load python script \"" + path + "\": script is already loaded");
                        return 1;
                }

                if (path == ".py") { // An "empty" path loads the main module
                        module = PyImport_AddModule("__main__");
                        return 0;
                }

                if (!file_exists(path)) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to load python script \"" + path + "\": file does not exist");
                        return 2;
                }

                std::string fname = path;
                const std::string prefix ("bee/resources/");
                if (fname.rfind(prefix, 0) == 0) {
                        fname = fname.substr(prefix.length());
                }
                fname = string_replace(fname, ".py", "");
                fname = string_replace(fname, "/", ".");

                PyObject* scr_name = PyUnicode_DecodeFSDefault(fname.c_str());
                PyObject* _module = PyImport_Import(scr_name);
                Py_DECREF(scr_name);

                if (_module == nullptr) {
                        PyErr_Print();
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to import python script \"" + path + "\"");

                        return 3;
                }

                module = _module;

                return 0;
        }
        void PythonScriptInterface::free() {
                if (module == nullptr) {
                        return;
                }

                if (module != PyImport_AddModule("__main__")) {
                        Py_DECREF(module);
                }
                module = nullptr;
        }
        void PythonScriptInterface::release() {
                path.clear();
                module = nullptr;
        }
        int PythonScriptInterface::run_string(const std::string& code) {
                if (module == nullptr) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to run python string \"" + path + "\": script is not loaded");
                        return 1;
                }

                PyObject* codeobj = Py_CompileString(code.c_str(), "<string>", Py_file_input);
                if (codeobj == nullptr) {
                        PyErr_Print();
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Python script codestring \"" + code + "\" compile failed for \"" + path + "\"");

                        return 2;
                }

                PyObject* global_dict = PyModule_GetDict(module);
                PyObject* value = PyEval_EvalCode(codeobj, global_dict, global_dict);
                if (value == nullptr) {
                        Py_DECREF(codeobj);

                        PyErr_Print();
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Python script codestring \"" + code + "\" failed for \"" + path + "\"");

                        return 2;
                }

                Py_DECREF(codeobj);

                return 0;
        }
        int PythonScriptInterface::run_file(const std::string& filename) {
                if (module == nullptr) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to run python script \"" + filename + "\": module is not loaded");
                        return 1;
                }

                PyObject* codeobj = Py_CompileString(file_get_contents(filename).c_str(), filename.c_str(), Py_file_input);
                if (codeobj == nullptr) {
                        PyErr_Print();
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Python script compile failed for \"" + filename + "\" in module \"" + std::string(PyModule_GetName(module)) + "\"");

                        return 2;
                }

                PyObject* global_dict = PyModule_GetDict(module);
                PyObject* value = PyEval_EvalCode(codeobj, global_dict, global_dict);
                if (value == nullptr) {
                        Py_DECREF(codeobj);

                        PyErr_Print();
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Python script failed for \"" + filename + "\" in module \"" + std::string(PyModule_GetName(module)) + "\"");

                        return 2;
                }

                Py_DECREF(codeobj);

                return 0;
        }
        int PythonScriptInterface::run_func(const std::string& funcname) {
                if (module == nullptr) {
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to run python function \"" + path + "\": script is not loaded");
                        return 1;
                }

                PyObject* func = PyObject_GetAttrString(module, funcname.c_str());
                if ((func == nullptr)||(!PyCallable_Check(func))) {
                        if (PyErr_Occurred()) {
                                PyErr_Print();
                        }
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to find python script function \"" + funcname + "\" for \"" + path + "\"");

                        return 2;
                }

                PyObject* value = PyObject_CallObject(func, nullptr);
                if (value == nullptr) {
                        Py_DECREF(func);

                        PyErr_Print();
                        messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Python script function \"" + funcname + "\" failed for \"" + path + "\"");

                        return 3;
                }
                Py_DECREF(value);

                Py_DECREF(func);

                return 0;
        }
}

#endif // BEE_PYTHON
