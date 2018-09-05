/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_PYTHON_H
#define BEE_FS_PYTHON_H 1

#include <string>

#include <Python.h>

namespace bee { namespace fs { namespace python {
	namespace internal {
		PyObject* spec_from_file(const std::string&, const std::string&);
	}

	PyObject* import(const std::string&);
}}}

#endif // BEE_FS_PYTHON_H
