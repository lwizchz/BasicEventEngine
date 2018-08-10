/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
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
#include "../core/instance.hpp"

#include "../messenger/messenger.hpp"

#include "beemodule.hpp"

namespace bee { namespace python {
	namespace internal {
		wchar_t* program = nullptr;
		std::vector<wchar_t*> argv;

		Variant dh;
	}

	/**
	* Initialize the Python scripting system.
	* @retval 0 success
	* @retval 1 failed to decode argv[0] for the program name
	*/
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

		run_string("import bee");

		return 0;
	}
	/**
	* Close the Python scripting system.
	* @retval 0 success
	* @retval 1 failed to finalize and flush memory buffers
	*/
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

	/**
	* Run the given string in the __main__ module
	* @param code the code string to run
	*
	* @retval 0 success
	* @retval 1 a Python exception was raised during execution
	*/
	int run_string(const std::string& code) {
		if (PyRun_SimpleString(code.c_str()) < 0) {
			return 1;
		}
		return 0;
	}
	/**
	* Run the given file in the __main__ module
	* @param filename the file to run
	*
	* @retval 0 success
	* @retval 1 failed to find file
	* @retval 2 failed to open file
	* @retval 3 a Python exception was raised during execution
	*/
	int run_file(const std::string& filename) {
		if (!util::file_exists(filename)) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to run file \"" + filename + "\": file does not exist");
			return 1;
		}

		PyObject* fileobj = Py_BuildValue("s", filename.c_str());
		FILE* file = _Py_fopen_obj(fileobj, "r");
		if (file == nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to open file \"" + filename + "\"");
			return 2;
		}

		if (PyRun_SimpleFileEx(file, filename.c_str(), 1) < 0) {
			return 3;
		}

		return 0;
	}

	/**
	* @returns a string of the last Python traceback
	*/
	std::string get_traceback() {
		PyObject* sys = PyImport_AddModule("sys");

		PyObject* dict = PyModule_GetDict(sys);

		PyObject* type = PyDict_GetItemString(dict, "last_type");
		PyObject* value = PyDict_GetItemString(dict, "last_value");
		PyObject* traceback = PyDict_GetItemString(dict, "last_traceback");

		std::string _traceback;
		if (traceback != Py_None) {
			_traceback += "Traceback (most recent call last):\n";
			while (traceback != Py_None) {
				PyObject* frame = PyObject_GetAttrString(traceback, "tb_frame");
				PyObject* code = PyObject_GetAttrString(frame, "f_code");

				_traceback += "File \"";
				_traceback += PyUnicode_AsUTF8(PyObject_Str(
					PyObject_GetAttrString(code, "co_filename")
				));
				_traceback += "\", line ";
				_traceback += PyUnicode_AsUTF8(PyObject_Str(
					PyObject_GetAttrString(frame, "f_lineno")
				));
				_traceback += ", in ";
				_traceback += PyUnicode_AsUTF8(PyObject_Str(
					PyObject_GetAttrString(code, "co_name")
				));
				_traceback += "\n";

				if (PyObject_GetAttrString(traceback, "tb_next") == Py_None) {
					break;
				}
				traceback = PyObject_GetAttrString(traceback, "tb_next");
			}

			_traceback += PyUnicode_AsUTF8(PyObject_GetAttrString(type, "__name__"));
			_traceback += ": ";
			_traceback += PyUnicode_AsUTF8(PyObject_Str(value));
		} else {
			_traceback += "File \"";
			_traceback += PyUnicode_AsUTF8(PyObject_GetAttrString(value, "filename"));
			_traceback += "\", line ";
			_traceback += PyUnicode_AsUTF8(PyObject_Str(PyObject_GetAttrString(value, "lineno")));
			_traceback += "\n  ";
			_traceback += PyUnicode_AsUTF8(PyObject_GetAttrString(value, "text"));
			_traceback += "\n";
			long i = 2 + PyLong_AsLong(PyObject_GetAttrString(value, "offset"));
			_traceback += util::string::repeat(std::max(i-1l, 0l), " ");
			_traceback += "^\n";

			_traceback += PyUnicode_AsUTF8(PyObject_GetAttrString(type, "__name__"));
			_traceback += ": ";
			_traceback += PyUnicode_AsUTF8(PyObject_GetAttrString(value, "msg"));
		}

		return _traceback;
	}

	/**
	* Store the last evaluated Python object in the internal displayhook Variant.
	* @param obj the object to store
	*/
	void set_displayhook(PyObject* obj) {
		internal::dh = pyobj_to_variant(obj);
	}
	/**
	* @returns the internal displayhook Variant
	*/
	Variant get_displayhook() {
		return internal::dh;
	}

	/**
	* Convert a PyObject to a Variant
	* @param obj the PyObject to convert
	*
	* @returns the converted value
	*/
	Variant pyobj_to_variant(PyObject* obj) {
		Variant var;

		if (obj == Py_None) {
			var = Variant(E_DATA_TYPE::NONE);
		} else if (PyLong_Check(obj)) {
			var = static_cast<int>(PyLong_AsLong(obj));
		} else if (PyBool_Check(obj)) {
			var = (obj == Py_True);
		} else if (PyFloat_Check(obj)) {
			var = PyFloat_AsDouble(obj);
		} else if (PyUnicode_Check(obj)) {
			var = PyUnicode_AsUTF8(obj);
		} else if (PyTuple_Check(obj)) {
			var = Variant(E_DATA_TYPE::VECTOR);

			Py_ssize_t size = PyTuple_Size(obj);
			for (Py_ssize_t i=0; i<size; ++i) {
				var.v.push_back(pyobj_to_variant(PyTuple_GetItem(obj, i)));
			}
		} else if (PyList_Check(obj)) {
			var = Variant(E_DATA_TYPE::VECTOR);

			Py_ssize_t size = PyList_Size(obj);
			for (Py_ssize_t i=0; i<size; ++i) {
				var.v.push_back(pyobj_to_variant(PyList_GetItem(obj, i)));
			}
		} else if (PyDict_Check(obj)) {
			var = Variant(E_DATA_TYPE::MAP);

			PyObject *key, *value;
			Py_ssize_t pos = 0;
			while (PyDict_Next(obj, &pos, &key, &value)) {
				var.m.emplace(pyobj_to_variant(key), pyobj_to_variant(value));
			}
		} else if (Texture_check(obj)) {
			var = internal::as_texture(obj);
		} else if (Sound_check(obj)) {
			var = internal::as_sound(obj);
		} else if (Font_check(obj)) {
			var = internal::as_font(obj);
		} else if (Path_check(obj)) {
			var = internal::as_path(obj);
		} else if (Timeline_check(obj)) {
			var = internal::as_timeline(obj);
		} else if (Mesh_check(obj)) {
			var = internal::as_mesh(obj);
		} else if (Light_check(obj)) {
			var = internal::as_light(obj);
		} else if (Script_check(obj)) {
			var = internal::as_script(obj);
		} else if (Object_check(obj)) {
			var = internal::as_object(obj);
		} else if (Room_check(obj)) {
			var = internal::as_room(obj);
		} else if (Instance_check(obj)) {
			var = internal::as_instance(obj);
		} else {
			PyObject* str = PyObject_Str(obj);
			if (str == nullptr) {
				str = PyObject_Repr(obj);
				if (str == nullptr) {
					messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to convert PyObject to Variant");
					var = Variant();
				} else {
					var = Variant(PyUnicode_AsUTF8(str));
				}
			} else {
				var = Variant(PyUnicode_AsUTF8(str));
			}
		}

		return var;
	}
	/**
	* Convert a Variant to a PyObject
	* @param var the Variant to convert
	*
	* @returns the converted value
	*/
	PyObject* variant_to_pyobj(Variant var) {
		switch (var.get_type()) {
			case E_DATA_TYPE::NONE: {
				if (var.p != nullptr) {
					if (var.get_ptype() == std::type_index(typeid(Texture*))) {
						Texture* p = static_cast<Texture*>(var.p);
						return Texture_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Sound*))) {
						Sound* p = static_cast<Sound*>(var.p);
						return Sound_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Font*))) {
						Font* p = static_cast<Font*>(var.p);
						return Font_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Path*))) {
						Path* p = static_cast<Path*>(var.p);
						return Path_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Timeline*))) {
						Timeline* p = static_cast<Timeline*>(var.p);
						return Timeline_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Mesh*))) {
						Mesh* p = static_cast<Mesh*>(var.p);
						return Mesh_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Light*))) {
						Light* p = static_cast<Light*>(var.p);
						return Light_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Script*))) {
						Script* p = static_cast<Script*>(var.p);
						return Script_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Object*))) {
						Object* p = static_cast<Object*>(var.p);
						return Object_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Room*))) {
						Room* p = static_cast<Room*>(var.p);
						return Room_from(p);
					} else if (var.get_ptype() == std::type_index(typeid(Instance*))) {
						Instance* p = static_cast<Instance*>(var.p);
						return Instance_from(p);
					}
				}

				Py_RETURN_NONE;
			}
			case E_DATA_TYPE::CHAR: {
				return PyUnicode_FromString(std::string(1, var.c).c_str());
			}
			case E_DATA_TYPE::INT: {
				return PyLong_FromLong(var.i);
			}
			case E_DATA_TYPE::FLOAT: {
				return PyFloat_FromDouble(var.f);
			}
			case E_DATA_TYPE::DOUBLE: {
				return PyFloat_FromDouble(var.d);
			}
			case E_DATA_TYPE::STRING: {
				return PyUnicode_FromString(var.s.c_str());
			}
			case E_DATA_TYPE::VECTOR: {
				PyObject* obj = PyList_New(var.v.size());
				Py_ssize_t i = 0;
				for (auto& e : var.v) {
					PyList_SetItem(obj, i++, variant_to_pyobj(e));
				}
				return obj;
			}
			case E_DATA_TYPE::MAP: {
				PyObject* obj = PyDict_New();
				for (auto& e : var.m) {
					PyDict_SetItem(obj, variant_to_pyobj(e.first), variant_to_pyobj(e.second));
				}
				return obj;
			}
			case E_DATA_TYPE::SERIAL: {
				std::vector<Uint8> sdv (var.sd.get());
				PyObject* obj = PyList_New(sdv.size());
				Py_ssize_t i = 0;
				for (auto& e : sdv) {
					PyList_SetItem(obj, i++, PyLong_FromLong(e));
				}
				return obj;
			}
		}
		return nullptr;
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
		this->free();
	}
	/**
	* Load the module from the path.
	* @retval 0 success
	* @retval 1 failed since it is already loaded
	* @retval 2 failed since the path does not exist
	* @retval 3 failed to import, see the Python exception for more info
	*/
	int PythonScriptInterface::load() {
		if (module != nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to load python script \"" + path + "\": script is already loaded");
			return 1;
		}

		if (path == ".py") { // An "empty" path loads the main module
			module = PyImport_AddModule("__main__");
			return 0;
		}

		if (!util::file_exists(path)) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to load python script \"" + path + "\": file does not exist");
			return 2;
		}

		std::string fname = path;
		const std::string prefix ("bee/resources/");
		if (fname.rfind(prefix, 0) == 0) {
			fname = fname.substr(prefix.length());
		}
		fname = util::string::replace(fname, ".py", "");
		fname = util::string::replace(fname, "/", ".");

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
	/**
	* DecRef the module and remove it from this interface.
	*/
	void PythonScriptInterface::free() {
		if (module == nullptr) {
			return;
		}

		if (module != PyImport_AddModule("__main__")) {
			Py_DECREF(module);
		}
		module = nullptr;
	}
	/**
	* Remove the module from this interface without DecRefing it.
	*/
	void PythonScriptInterface::release() {
		path.clear();
		module = nullptr;
	}
	/**
	* Run the given string in the loaded module.
	* @param code the code string to run
	* @param retval the pointer to store the code return value in
	* @param start the start point of the Python interpreter, either Py_file_input or Py_single_input
	*
	* @retval 0 success
	* @retval 1 failed since the module is not loaded
	* @retval 2 failed to compile the code string, see the Python exception for more info
	* @retval 3 failed to evaluate the code, see the Python exception for more info
	*/
	int PythonScriptInterface::run_string(const std::string& code, Variant* retval, int start) {
		if (module == nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to run python string \"" + path + "\": script is not loaded");
			return 1;
		}

		if (start != Py_file_input) {
			std::vector<std::string> _code (util::splitv(code, '\n', true));
			if (_code.size() > 1) {
				// Check whether the code should be ran as a file
				for (auto& l : _code) {
					if ((l[0] == '\t')||(l[0] == ' ')) {
						start = Py_file_input;
						break;
					}
				}

				if (start != Py_file_input) {
					for (auto& l : _code) {
						int r = run_string(l, retval);
						if (r != 0) {
							if (retval != nullptr) {
								*retval = Variant();
							}
							return r;
						}
					}
					return 0;
				}
			}
		}

		PyObject* codeobj = Py_CompileString(code.c_str(), "<string>", start);
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

			return 3;
		}

		if (retval != nullptr) {
			if (value == Py_None) {
				*retval = python::get_displayhook();
			} else {
				*retval = python::pyobj_to_variant(value);
			}
		}
		Py_DECREF(value);

		Py_DECREF(codeobj);

		return 0;
	}
	/**
	* Run the given string as a single input.
	* @param code the code string to run
	* @param retval the pointer to store the code return value in
	*
	* @see run_string(const std::string&, Variant*, int) for return values
	*/
	int PythonScriptInterface::run_string(const std::string& code, Variant* retval) {
		return run_string(code, retval, Py_single_input);
	}
	/**
	* Run the given file in the loaded module.
	* @param filename the file to run
	*
	* @see run_string(const std::string&, Variant*, int) for return values
	*/
	int PythonScriptInterface::run_file(const std::string& filename) {
		return run_string(util::file_get_contents(filename), nullptr, Py_file_input);
	}
	/**
	* Run the given function in the loaded module.
	* @param funcname the function to run
	* @param args the function arguments
	* @param retval the pointer to store the function return value in
	*
	* @retval 0 success
	* @retval 1 failed since the module is not loaded
	* @retval 2 failed to find a callable object with the given function name
	* @retval 3 failed to convert function arguments
	* @retval 4 failed to call the function, see the Python exception for more info
	*/
	int PythonScriptInterface::run_func(const std::string& funcname, const Variant& args, Variant* retval) {
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

		PyObject* _args = nullptr;
		if (args.get_type() != E_DATA_TYPE::VECTOR) {
			if (args.get_type() != E_DATA_TYPE::NONE) {
				messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to convert function arguments for \"" + funcname + "\" in \"" + path + "\": arguments must be a vector");
				return 3;
			}
		} else {
			const std::vector<Variant>& vargs = args.v;
			_args = PyTuple_New(vargs.size());

			size_t i = 0;
			for (auto& a : vargs) {
				PyTuple_SetItem(_args, i++, python::variant_to_pyobj(a));
			}
		}

		PyObject* value = PyObject_CallObject(func, _args);
		if (value == nullptr) {
			Py_XDECREF(_args);
			Py_DECREF(func);

			PyErr_Print();
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Python script function \"" + funcname + "\" failed for \"" + path + "\"");

			return 4;
		}

		if (retval != nullptr) {
			*retval = python::pyobj_to_variant(value);
		}
		Py_DECREF(value);

		Py_XDECREF(_args);
		Py_DECREF(func);

		return 0;
	}

	/**
	* Set the given Python name to the given value.
	* @param name the name to set
	* @param value the value to set the name to
	*
	* @retval 0 success
	* @retval 1 failed since the module is not loaded
	* @retval 2 failed to set the attribute
	*/
	int PythonScriptInterface::set_var(const std::string& name, const Variant& value) {
		if (module == nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to set python variable \"" + name + "\": script \"" + path + "\" is not loaded");
			return 1;
		}

		if (PyObject_SetAttrString(module, name.c_str(), python::variant_to_pyobj(value)) < 0) {
			PyErr_Print();
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to set python variable \"" + name + "\" for \"" + path + "\"");

			return 2;
		}

		return 0;
	}
	/**
	* @param name the name of the variable to check for
	*
	* @returns whether a variable with the given name exists or false when not loaded
	*/
	bool PythonScriptInterface::has_var(const std::string& name) const {
		if (module == nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to get python variable \"" + name + "\": script \"" + path + "\" is not loaded");
			return false;
		}

		return PyObject_HasAttrString(module, name.c_str());
	}
	/**
	* @param name the name of the variable to get
	*
	* @returns the variable with the given name or an empty variable when the name does not exist
	*/
	Variant PythonScriptInterface::get_var(const std::string& name) const {
		if (module == nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to get python variable \"" + name + "\": script \"" + path + "\" is not loaded");
			return Variant();
		}

		PyObject* var = PyObject_GetAttrString(module, name.c_str());
		if (var == nullptr) {
			PyErr_Print();
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to get python variable \"" + name + "\" from \"" + path + "\"");

			return Variant();
		}

		Variant v (python::pyobj_to_variant(var));

		Py_DECREF(var);

		return v;
	}

	/**
	* Find all attributes which match the given string.
	* @param input the string to match against
	*
	* @returns a vector of possible matches
	*/
	std::vector<Variant> PythonScriptInterface::complete(const std::string& input) const {
		std::vector<Variant> vec;

		if (module == nullptr) {
			messenger::send({"engine", "python"}, E_MESSAGE::ERROR, "Failed to complete python names: script \"" + path + "\" is not loaded");
			return vec;
		}

		PyObject* dict = PyModule_GetDict(module);

		PyObject *key, *value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(dict, &pos, &key, &value)) {
			std::string _key (PyUnicode_AsUTF8(key));

			if (_key.rfind(input, 0) == 0) {
				vec.push_back(Variant(_key));
			}
		}

		return vec;
	}
}

#endif // BEE_PYTHON
