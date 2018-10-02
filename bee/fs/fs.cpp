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

#include "../core/console.hpp"
#include "../core/rooms.hpp"
#include "assimp.hpp"

#include "../input/kb.hpp"

#include "../resource/texture.hpp"
#include "../resource/sound.hpp"
#include "../resource/font.hpp"
#include "../resource/path.hpp"
#include "../resource/timeline.hpp"
#include "../resource/mesh.hpp"
#include "../resource/light.hpp"
#include "../resource/script.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

#include "../resources/objects/obj_script.hpp"
#include "../resources/rooms/rm_script.hpp"

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
			// Add subdirectories
			std::vector<std::string> dir_files = util::directory_get_files(path);
			for (auto& f : dir_files) {
				if ((!root.get_path().empty())&&(root.get_path().back() == '/')) {
					if (util::directory_exists(path+"/"+f)) {
						add_filemap(mapname+"/"+f, "/"+path+"/"+f, E_FS_ROOT_TYPE::IS_ROOT);
						continue;
					}
				}

				amount += scan_files(path+"/"+f, root, mapname);
			}
		} else if (util::file_exists(path)) {
			// Add files
			if ((util::file_extname(path) == ".xz")&&(util::file_extname(util::file_plainname(path)) == ".tar")) {
				amount += scan_archive(path, root, mapname);
			} else {
				FilePath fp (path, mapname);
				if ((!root.get_path().empty())&&(root.get_path() != ".")) {
					fp = FilePath(util::string::replace(fp.get_path(), root.get_path(), ""), mapname);
				}

				std::unordered_map<std::string,std::vector<FilePath>>::iterator filevec;
				std::tie(filevec, std::ignore) = files.emplace(fp.get_path(), std::vector<FilePath>());
				filevec->second.push_back(FilePath(path, mapname));
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
			root = FilePath(path).get_parent_dir();
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
	* Remove all files mapped from the FileMap with the given name.
	* @param name the name of the FileMap to unmap
	*
	* @retval 0 success
	*/
	void remove_filemaps(const std::string& name) {
		for (auto fpv = internal::files.begin(); fpv != internal::files.end(); ) {
			fpv->second.erase(std::remove_if(fpv->second.begin(), fpv->second.end(), [&name] (const FilePath& fp) -> bool {
				const std::string mn (fp.get_mapname());
				return (mn == name) || (mn.rfind(name+"/", 0) == 0);
			}), fpv->second.end());

			if (fpv->second.empty()) {
				fpv = internal::files.erase(fpv);
			} else {
				++fpv;
			}
		}

		for (auto fm = internal::filemaps.begin(); fm != internal::filemaps.end(); ) {
			const std::string mn (fm->second.get_name());
			if ((mn == name)||(mn.rfind(name+"/", 0) == 0)) {
				fm = internal::filemaps.erase(fm);
			} else {
				++fm;
			}
		}
	}
	/**
	* Remove all mapped files.
	*
	* @retval 0 success
	* @retval >0 failed to unmap some FileMaps
	*/
	void remove_all_filemaps() {
		auto fm = internal::filemaps.begin();
		while (fm != internal::filemaps.end()) {
			remove_filemaps(fm->second.get_name());
			fm = internal::filemaps.begin();
		}
	}

	/**
	* @param path the path to check
	* @param mapname the name of the specific FileMap that mapped the path
	*
	* @returns whether a file has been mapped to the given path
	*/
	bool exists(const std::string& path, const std::string& mapname) {
		auto fp = internal::files.find(FilePath(path).get_path());
		if (fp == internal::files.end()) {
			return false;
		}

		if (mapname.empty()) {
			return fp->second.back().exists();
		}

		for (auto& f : fp->second) {
			if (f.get_mapname() == mapname) {
				return f.exists();
			}
		}

		return false;
	}
	/**
	* @param path the path to check
	*
	* @returns whether a file has been mapped to the given path
	*/
	bool exists(const std::string& path) {
		return exists(path, "");
	}
	/**
	* @param the path to get
	* @param the FileMap name to fetch from
	*
	* @returns the primary FilePath of the mapped file
	*/
	FilePath get_file(const std::string& path, const std::string& mapname) {
		auto fp = internal::files.find(FilePath(path).get_path());
		if (fp == internal::files.end()) {
			messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "FilePath \"" + path + "\" not mapped");
			return FilePath();
		}

		if (mapname.empty()) {
			return fp->second.back();
		}

		for (auto& f : fp->second) {
			if (f.get_mapname() == mapname) {
				return f;
			}
		}

		return FilePath();
	}
	FilePath get_file(const std::string& path) {
		return get_file(path, "");
	}

	/**
	* Load the given level and Room.
	* @param name the name of the level
	* @param path the path to the levelmap
	* @param room the name of the first Room to change to after loading
	* @param are_scripts_enabled whether to allow the level to load Scripts
	*
	* @retval 0 success
	* @retval 1 failed to load some Resources
	* @retval 2 failed since the first Room couldn't be changed to
	*/
	int load_level(const std::string& name, const std::string& path, const std::string& room, bool are_scripts_enabled) {
		std::string _room (room);

		add_filemap(name, path, E_FS_ROOT_TYPE::HAS_ROOTS);

		FilePath res = get_file("resources.json", name);
		if (res.exists()) {
			int amount_failed = 0;
			Variant resources (res.get(), true);
			std::map<Variant,Variant> resmap = resources.m["resources"].m;

			bool prev_new_enabled = Script::set_is_new_enabled(are_scripts_enabled);

			try {
				for (auto& t : resmap["textures"].v) {
					Texture* _t = new Texture(t.m["name"].s, t.m["path"].s);

					if (t.m.find("subimage_amount") != t.m.end()) {
						_t->set_subimage_amount(t.m["subimage_amount"].v[0].i, t.m["subimage_amount"].v[1].i);
					}

					amount_failed += (_t->load()) ? 1 : 0;
				}
				for (auto& s : resmap["sounds"].v) {
					Sound* _s = new Sound(s.m["name"].s, s.m["path"].s, s.m["is_music"].i);
					amount_failed += (_s->load()) ? 1 : 0;
				}
				for (auto& f : resmap["fonts"].v) {
					Font* _f = new Font(f.m["name"].s, f.m["path"].s, f.m["font_size"].i);
					amount_failed += (_f->load()) ? 1 : 0;
				}
				for (auto& p : resmap["paths"].v) {
					Path* _p = new Path(p.m["name"].s, p.m["path"].s);
					amount_failed += (_p->load()) ? 1 : 0;
				}
				for (auto& t : resmap["timelines"].v) {
					Timeline* _t = new Timeline(t.m["name"].s, t.m["path"].s);
					amount_failed += (_t->load()) ? 1 : 0;
				}
				for (auto& m : resmap["meshes"].v) {
					Mesh* _m = new Mesh(m.m["name"].s, m.m["path"].s);

					if (m.m.find("mesh_index") != m.m.end()) {
						amount_failed += (_m->load(m.m["mesh_index"].i)) ? 1 : 0;
					} else {
						amount_failed += (_m->load()) ? 1 : 0;
					}
				}
				for (auto& l : resmap["lights"].v) {
					Light* _l = new Light(l.m["name"].s, l.m["path"].s);
					amount_failed += (_l->load()) ? 1 : 0;
				}
				for (auto& s : resmap["scripts"].v) {
					Script* _s = new Script(s.m["name"].s, s.m["path"].s);
					amount_failed += (_s->load()) ? 1 : 0;
				}
				for (auto& o : resmap["objects"].v) {
					Object* _o = new ObjScript(o.m["path"].s);

					if (o.m.find("sprite") != o.m.end()) {
						_o->set_sprite(Texture::get_by_name(o.m["sprite"].s));
					}

					amount_failed += (_o->load()) ? 1 : 0;
				}
				for (auto& r : resmap["rooms"].v) {
					Room* _r = new RmScript(r.m["path"].s);
					amount_failed += (_r->load()) ? 1 : 0;
				}
			} catch (int e) {
				messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "Failed to init some resources for the level \"" + name + "\" at \"" + path + "\"");
				return 1;
			}

			Script::set_is_new_enabled(prev_new_enabled);

			if (amount_failed != 0) {
				messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "Failed to load some resources for the level \"" + name + "\" at \"" + path + "\"");
				return 2;
			}

			if ((_room.empty())&&(!resources.m["first_room"].s.empty())) {
				_room = resources.m["first_room"].s;
			}
		}

		if (!_room.empty()) {
			Room* rm = Room::get_by_name(_room);
			if ((rm == nullptr)||(!rm->get_is_loaded())) {
				messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "Failed to change to the first room called \"" + _room + "\" for level \"" + name + "\"");
				return 3;
			}

			kb::unbind_all();
			console::run("execfile(\"cfg/binds.py\")");

			change_room(rm, false);
		}

		return 0;
	}
	/**
	* Load the given level.
	* @param name the name of the level
	* @param path the path to the levelmap
	*
	* @retval 0 successfully loaded level
	* @retval >0 see load_level(const std::string&, const std::string&, const std::string&, bool)
	*/
	int load_level(const std::string& name, const std::string& path) {
		return load_level(name, path, "", false);
	}
	/**
	* Load the given level and Room after unloading the previous level.
	* @param name the name of the level
	* @param path the path to the levelmap
	* @param room the name of the first Room to change to after loading
	* @param are_scripts_enabled whether to allow the level to load Scripts
	*
	* @retval -2 failed to unload the previous level
	* @retval -1 restarted the current Room
	* @retval 0 successfully loaded level
	* @retval >0 see load_level(const std::string&, const std::string&, const std::string&, bool)
	*/
	int switch_level(const std::string& name, const std::string& path, const std::string& room, bool are_scripts_enabled) {
		if (
			((get_current_room() != nullptr)&&(get_current_room()->get_name() == room))
			||((room.empty())&&(exists("resources.json", name)))
		) {
			kb::unbind_all();
			console::run("execfile(\"cfg/binds.py\")");

			restart_room();

			return -1;
		}

		if (unload_level(name)) {
			return -2;
		}

		return load_level(name, path, room, are_scripts_enabled);
	}
	/**
	* Unload the level with the given name.
	* @param name the name of the level to unload
	*
	* @retval 0 success
	* @retval 1 failed to unload some Resources
	*/
	int unload_level(const std::string& name) {
		if (exists("resources.json", name)) {
			FilePath res = get_file("resources.json", name);
			Variant resources (res.get(), true);
			std::map<Variant,Variant> resmap = resources.m["resources"].m;

			try {
				for (auto& t : resmap["textures"].v) {
					Texture* _t = Texture::get_by_name(t.m["name"].s);
					delete _t;
				}
				for (auto& s : resmap["sounds"].v) {
					Sound* _s = Sound::get_by_name(s.m["name"].s);
					delete _s;
				}
				for (auto& f : resmap["fonts"].v) {
					Font* _f = Font::get_by_name(f.m["name"].s);
					delete _f;
				}
				for (auto& p : resmap["paths"].v) {
					Path* _p = Path::get_by_name(p.m["name"].s);
					delete _p;
				}
				for (auto& t : resmap["timelines"].v) {
					Timeline* _t = Timeline::get_by_name(t.m["name"].s);
					delete _t;
				}
				for (auto& m : resmap["meshes"].v) {
					Mesh* _m = Mesh::get_by_name(m.m["name"].s);
					delete _m;
				}
				for (auto& l : resmap["lights"].v) {
					Light* _l = Light::get_by_name(l.m["name"].s);
					delete _l;
				}
				for (auto& s : resmap["scripts"].v) {
					Script* _s = Script::get_by_name(s.m["name"].s);
					delete _s;
				}
				for (auto& o : resmap["objects"].v) {
					Object* _o = Object::get_by_name(Script::get_type(o.m["path"].s).second);
					delete _o;
				}
				for (auto& r : resmap["rooms"].v) {
					Room* _r = Room::get_by_name(Script::get_type(r.m["path"].s).second);
					delete _r;
				}
			} catch (int e) {
				messenger::send({"engine", "fs"}, E_MESSAGE::WARNING, "Failed to destroy some resources for the level \"" + name + "\"");
				return 1;
			}
		}

		remove_filemaps(name);

		return 0;
	}
	/**
	* Unload all levels.
	*
	* @returns how many levels failed to unload
	*/
	int unload_all_levels() {
		int r = 0;

		const auto fms (internal::filemaps);
		for (auto fm = fms.begin(); fm != fms.end(); ++fm) {
			const std::string name (fm->second.get_name());
			if (exists("resources.json", name)) {
				r += !!unload_level(name);
			}
		}

		return r;
	}

	/**
	* Print the mapped files and the name of their associated FileMap.
	*/
	void print_mappings() {
		std::map<Variant,Variant> info;
		for (auto& fp : internal::files) {
			info[Variant(fp.first)] = "<" + fp.second.back().get_mapname() + ">";
		}

		messenger::send({"engine", "fs"}, E_MESSAGE::INFO, Variant(info).to_str(true));
	}
}}

#endif // BEE_FS
