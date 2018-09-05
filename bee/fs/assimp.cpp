/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_ASSIMP
#define BEE_FS_ASSIMP 1

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "assimp.hpp"

#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

namespace bee { namespace fs { namespace assimp {
	namespace internal {
		IOSystem* system;
		Assimp::Importer importer;
	}

	bool IOSystem::Exists(const char* fname) const {
		return fs::exists(fname);
	}
	char IOSystem::getOsSeparator() const {
		return util::platform::get_path_separator();
	}
	Assimp::IOStream* IOSystem::Open(const char* fname, const char* mode) {
		return new IOStream(fs::get_file(fname));
	}
	void IOSystem::Close(Assimp::IOStream* stream) {
		delete stream;
	}

	IOStream::IOStream(FilePath _fp) :
		pos(0),
		fp(_fp)
	{}
	size_t IOStream::FileSize() const {
		return fp.size();
	}
	size_t IOStream::Read(void* buffer, size_t size, size_t count) {
		std::string contents (fp.get());

		size_t c = std::min(contents.size()-pos, count);
		memcpy(static_cast<char*>(buffer), contents.c_str(), c);
		pos += c;

		return c;
	}
	size_t IOStream::Write(const void* buffer, size_t size, size_t count) {
		return fp.put(std::string(static_cast<const char*>(buffer), size));
	}
	void IOStream::Flush() {}
	aiReturn IOStream::Seek(size_t offset, aiOrigin origin) {
		switch (origin) {
			case aiOrigin_SET: {
				pos = offset;
				break;
			}
			case aiOrigin_CUR: {
				pos += offset;
				break;
			}
			case aiOrigin_END: {
				pos = FileSize() + offset;
				break;
			}
			default: {
				return AI_FAILURE;
			}
		}
		return AI_SUCCESS;
	}
	size_t IOStream::Tell() const {
		return pos;
	}

	int init() {
		//Assimp::DefaultLogger::create();
		//Assimp::DefaultLogger::get()->attachStream(Assimp::LogStream::createDefaultStream(aiDefaultLogStream_STDERR), 0xf);

		internal::system = new IOSystem();
		internal::importer.SetIOHandler(internal::system);
		return 0;
	}

	aiScene* import(const std::string& fname, unsigned int flags) {
		if (internal::importer.ReadFile(fname, flags) == nullptr) {
			return nullptr;
		}
		return internal::importer.GetOrphanedScene();
	}
	const char* get_error_string() {
		return internal::importer.GetErrorString();
	}
}}}

#endif // BEE_FS_ASSIMP
