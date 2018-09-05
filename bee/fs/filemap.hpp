/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_FILEMAP_H
#define BEE_FS_FILEMAP_H 1

#include <string>

namespace bee {
	class FileMap {
		std::string name;
		std::string path;
	public:
		FileMap(const std::string&, const std::string&);

		std::string get_name() const;
		std::string get_path() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
	};
}

#endif // BEE_FS_FILEMAP_H
