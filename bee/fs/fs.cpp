/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS
#define BEE_FS 1

#include <unordered_map>

#include "fs.hpp"

#include "../util/files.hpp"

#include "../messenger/messenger.hpp"

#include "assimp.hpp"

namespace bee { namespace fs {
	namespace internal {
		std::unordered_map<std::string,FileMap> filemaps;
		std::unordered_map<std::string,FilePath> files;
	}

	int internal::scan_files(const std::string& path) {
		int amount = 0;
		if (util::directory_exists(path)) {
			std::vector<std::string> files = util::directory_get_files(path);
			for (auto& f : files) {
				amount += scan_files(path+"/"+f);
			}
		} else if (util::file_exists(path)) {
			FilePath fp (path);
			files.emplace(fp.get_path(), fp);
			++amount;
		}

		return amount;
	}

	int init() {
		fs::add_filemap("__default_bee", "/bee");
		fs::add_filemap("__default_cfg", "/cfg");
		fs::add_filemap("__default_maps", "/maps");
		fs::add_filemap("__default_resources", "/resources");

		assimp::init();

		return 0;
	}

	int add_filemap(const std::string& name, const std::string& path) {
		auto fm = internal::filemaps.find(name);
		if (fm != internal::filemaps.end()) {
			fm->second.set_path(path);
			internal::scan_files(fm->second.get_path());
			return 1;
		}

		std::tie(fm, std::ignore) = internal::filemaps.emplace(name, FileMap(name, path));
		internal::scan_files(fm->second.get_path());

		return 0;
	}

	bool exists(const std::string& path) {
		auto fp = internal::files.find(FilePath(path).get_path());
		if (fp == internal::files.end()) {
			return false;
		}
		return fp->second.exists();
	}
	FilePath get_file(const std::string& path) {
		auto fp = internal::files.find(FilePath(path).get_path());
		if (fp == internal::files.end()) {
			messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "FilePath \"" + path + "\" not mapped");
			return FilePath("");
		}
		return fp->second;
	}
}}

#endif // BEE_FS
