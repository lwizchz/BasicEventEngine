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

#include "../enum.hpp"

#include "filemap.hpp"
#include "filepath.hpp"

namespace bee { namespace fs {
	namespace internal {
		int scan_files(const std::string&, FilePath, const std::string&);
		int scan_archive(const std::string&, FilePath, const std::string&);
	}

	int init();

	int add_filemap(const std::string&, const std::string&, E_FS_ROOT_TYPE);
	int add_filemap(const std::string&, const std::string&);
	void remove_filemaps(const std::string&);
	void remove_all_filemaps();

	bool exists(const std::string&, const std::string&);
	bool exists(const std::string&);
	FilePath get_file(const std::string&, const std::string&);
	FilePath get_file(const std::string&);

	int load_level(const std::string&, const std::string&, const std::string&, bool);
	int load_level(const std::string&, const std::string&);
	int switch_level(const std::string&, const std::string&, const std::string&, bool);
	int unload_level(const std::string&);
	int unload_all_levels();

	void print_mappings();
}}

#endif // BEE_FS_H
