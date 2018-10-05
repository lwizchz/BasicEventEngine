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

namespace bee {
	FilePath::FilePath(const std::string& _path, const std::string& _mapname) :
		path(_path),
		mapname(_mapname)
	{}
	FilePath::FilePath(const std::string& _path) :
		FilePath(_path, "")
	{}
	FilePath::FilePath() :
		FilePath("", "")
	{}

	std::string FilePath::get_path() const {
		return path;
	}
	std::string FilePath::get_mapname() const {
		return mapname;
	}
	FilePath FilePath::get_parent_dir() const {
		return FilePath(util::file_dirname(get_path()), get_mapname());
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
	std::pair<SDL_RWops*,std::string*> FilePath::get_rwops() const {
		std::string* contents = new std::string(get());
		return std::make_pair(SDL_RWFromConstMem(contents->c_str(), contents->size()), contents);
	}

	std::streamoff FilePath::put(const std::string& contents) {
		return util::file_put_contents(path, contents);
	}
}

#endif // BEE_FS_FILEPATH
