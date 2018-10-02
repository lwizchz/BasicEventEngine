/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_FILEMAP
#define BEE_FS_FILEMAP 1

#include "filemap.hpp"

namespace bee {
	FileMap::FileMap(const std::string& _name, const std::string& _path) :
		name(_name),
		path()
	{
		set_path(_path);
	}

	std::string FileMap::get_name() const {
		return name;
	}
	std::string FileMap::get_path() const {
		return path;
	}

	void FileMap::set_name(const std::string& _name) {
		name = _name;
	}
	void FileMap::set_path(const std::string& _path) {
		if ((!_path.empty())&&(_path.front() == '/')) {
			path = _path.substr(1);
		} else {
			path = _path;
		}

		if (path.empty()) {
			path = ".";
		}
	}
}

#endif // BEE_FS_FILEMAP
