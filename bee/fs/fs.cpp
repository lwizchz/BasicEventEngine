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
#include "../util/string.hpp"

#include "../messenger/messenger.hpp"

#include "../core/rooms.hpp"
#include "assimp.hpp"

#include "../resource/room.hpp"

namespace bee { namespace fs {
	namespace internal {
		std::unordered_map<std::string,FileMap> filemaps;
		std::unordered_map<std::string,std::vector<FilePath>> files;
	}

	/**
	* Scan the given path for files to map.
	* @param path the real path to map
	* @param root the root relation of the FileMap
	* @param mapname the name of the current FileMap being scanned
	*
	* @returns the number of mapped files
	*/
	int internal::scan_files(const std::string& path, FilePath root, const std::string& mapname) {
		int amount = 0;
		if ((path != ".")&&(util::directory_exists(path))) {
			std::vector<std::string> dir_files = util::directory_get_files(path);
			for (auto& f : dir_files) {
				FilePath r = root;
				if ((!r.get_path().empty())&&(r.get_path().back() == '/')) {
					add_filemap(path+"/"+f, "/"+path+"/"+f, E_FS_ROOT_TYPE::IS_ROOT);
					continue;
				}

				amount += scan_files(path+"/"+f, r, mapname);
			}
		} else if (util::file_exists(path)) {
			if ((util::file_extname(path) == ".xz")&&(util::file_extname(util::file_plainname(path)) == ".tar")) {
				amount += scan_archive(path, root, mapname);
			} else {
				FilePath fp (path, mapname);
				if ((!root.get_path().empty())&&(root.get_path() != ".")) {
					fp = FilePath(util::string::replace(fp.get_path(), root.get_path(), ""), mapname);
				}

				std::unordered_map<std::string,std::vector<FilePath>>::iterator filevec;
				std::tie(filevec, std::ignore) = files.emplace(fp.get_path(), std::vector<FilePath>());
				filevec->second.push_back(fp);
				++amount;
			}
		}

		return amount;
	}
	/**
	* Scan the given archive for files to map.
	* @param path the archive path
	* @param root the root relation of the FileMap
	* @param mapname the name of the current FileMap being scanned
	*
	* @returns the number of mapped files
	*/
	int internal::scan_archive(const std::string& path, FilePath root, const std::string& mapname) {
		return 0;
	}

	/**
	* Initialize the filesystem with default mappings.
	*/
	int init() {
		fs::add_filemap("__default_bee", "/bee");
		fs::add_filemap("__default_cfg", "/cfg");
		//fs::add_filemap("__default_maps", "/maps", E_FS_ROOT_TYPE::HAS_ROOTS);
		fs::add_filemap("__default_resources", "/resources");

		assimp::init();

		return 0;
	}

	/**
	* Add a FileMap with the given name.
	* @param name the name to use for the FileMap
	* @param path the FileMap path
	* @param root_type the root relation of the FileMap
	*
	* @retval 0 success
	* @retval 1 success but a FileMap with the same name was overwritten
	*/
	int add_filemap(const std::string& name, const std::string& path, E_FS_ROOT_TYPE root_type) {
		FilePath root ("/");
		if (root_type == E_FS_ROOT_TYPE::HAS_ROOTS) {
			root = FilePath(path+"/");
		} else if (root_type == E_FS_ROOT_TYPE::IS_ROOT) {
			root = FilePath(path);
		}

		auto fm = internal::filemaps.find(name);
		if (fm != internal::filemaps.end()) {
			fm->second.set_path(path);
			internal::scan_files(fm->second.get_path(), root, name);
			return 1;
		}

		std::tie(fm, std::ignore) = internal::filemaps.emplace(name, FileMap(name, path));
		internal::scan_files(fm->second.get_path(), root, name);

		return 0;
	}
	/**
	* Add a FileMap with the given name.
	* @param name the name to use for the FileMap
	* @param path the FileMap path
	*
	* @retval 0 success
	* @retval 1 success but a FileMap with the same name was overwritten
	*/
	int add_filemap(const std::string& name, const std::string& path) {
		return add_filemap(name, path, E_FS_ROOT_TYPE::NOT_ROOT);
	}

	/**
	* @param path the path to check
	*
	* @returns whether a file has been mapped to the given path
	*/
	bool exists(const std::string& path) {
		auto fp = internal::files.find(FilePath(path).get_path());
		if (fp == internal::files.end()) {
			return false;
		}
		return fp->second.at(0).exists();
	}
	/**
	* @param the path to get
	*
	* @returns the primary FilePath of the mapped file
	*/
	FilePath get_file(const std::string& path) {
		auto fp = internal::files.find(FilePath(path).get_path());
		if (fp == internal::files.end()) {
			messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "FilePath \"" + path + "\" not mapped");
			return FilePath();
		}
		return fp->second.at(0);
	}

	/**
	* Load the given level and Room.
	* @param name the name of the level
	* @param path the path to the levelmap
	* @param room the name of the first Room to change to after loading
	*
	* @retval 0 success
	*/
	int load_level(const std::string& name, const std::string& path, const std::string& room) {
		add_filemap(name, path, E_FS_ROOT_TYPE::HAS_ROOTS);

		// TODO: load resources.json

		if (!room.empty()) {
			change_room(Room::get_by_name(room), false);
		}

		return 0;
	}
	/**
	* Load the given level.
	* @param name the name of the level
	* @param path the path to the levelmap
	*
	* @retval 0 success
	*/
	int load_level(const std::string& name, const std::string& path) {
		return load_level(name, path, "");
	}
}}

#endif // BEE_FS
