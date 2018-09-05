/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_FILEPATH
#define BEE_FS_FILEPATH 1

#include "filepath.hpp"

#include "../util/files.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	FilePath::FilePath(const std::string& _path) :
		path(_path)
	{
		if ((!path.empty())&&(path.front() == '/')) {
			path = path.substr(1);
		}
	}

	std::string FilePath::get_path() const {
		return path;
	}

	bool FilePath::exists() const {
		return util::file_exists(path);
	}
	size_t FilePath::size() const {
		return util::file_size(path);
	}
	std::string FilePath::get() const {
		return util::file_get_contents(path);
	}
	SDL_RWops* FilePath::get_rwops() const {
		std::string* contents = new std::string(get());
		return SDL_RWFromConstMem(contents->c_str(), contents->size());
	}

	std::streamoff FilePath::put(const std::string& contents) {
		return util::file_put_contents(path, contents);
	}
}

#endif // BEE_FS_FILEPATH
