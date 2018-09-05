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
	public:
		FilePath(const std::string&);

		std::string get_path() const;

		bool exists() const;
		size_t size() const;
		std::string get() const;
		SDL_RWops* get_rwops() const;

		std::streamoff put(const std::string&);
	};
}

#endif // BEE_FS_FILEPATH_H
