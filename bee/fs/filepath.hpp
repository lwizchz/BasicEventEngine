/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_FILEPATH_H
#define BEE_FS_FILEPATH_H 1

#include <string>

#include <SDL2/SDL.h>

namespace bee {
	class FilePath {
		std::string path;
		std::string mapname;
	public:
		FilePath(const std::string&, const std::string&);
		FilePath(const std::string&);
		FilePath();

		std::string get_path() const;
		std::string get_mapname() const;
		FilePath get_parent_dir() const;

		bool exists() const;
		size_t size() const;
		std::string get() const;
		std::pair<SDL_RWops*,std::string*> get_rwops() const;

		std::streamoff put(const std::string&);
	};
}

#endif // BEE_FS_FILEPATH_H
