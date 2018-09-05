/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_H
#define BEE_FS_H 1

#include <string>

#include "filemap.hpp"
#include "filepath.hpp"

namespace bee { namespace fs {
	namespace internal {
		int scan_files(const std::string&);
	}

	int init();

	int add_filemap(const std::string&, const std::string&);

	bool exists(const std::string&);
	FilePath get_file(const std::string&);
}}

#endif // BEE_FS_H
