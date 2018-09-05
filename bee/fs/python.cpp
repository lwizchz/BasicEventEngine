/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_PYTHON
#define BEE_FS_PYTHON 1

#include "python.hpp"

#include "../util/string.hpp"

#include "../messenger/messenger.hpp"

#include "fs.hpp"

namespace bee { namespace fs { namespace python {
	namespace internal {
		std::map<std::string,PyObject*> modules;
	}

	PyObject* import(const std::string& fname) {
		if (!fs::exists(fname)) {
			PyErr_SetString(PyExc_FileNotFoundError, (std::string("no file named \"") + fname + "\"").c_str());
			return nullptr;
		}

		std::string modname = fname;
		modname = util::string::replace(modname, ".py", "");
		modname = util::string::replace(modname, "/", ".");

		auto m = internal::modules.find(modname);
		if (m != internal::modules.end()) {
			return m->second;
		}

		PyObject* code = Py_CompileString(fs::get_file(fname).get().c_str(), fname.c_str(), Py_file_input);
		PyObject* module = PyImport_ExecCodeModule(modname.c_str(), code);

		internal::modules.emplace(modname, module);

		return module;
	}
}}}

#endif // BEE_FS_PYTHON
