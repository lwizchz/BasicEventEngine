/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FS_ASSIMP_H
#define BEE_FS_ASSIMP_H 1

#include <string>

#include <assimp/scene.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

#include "fs.hpp"

namespace bee { namespace fs { namespace assimp {
	struct IOSystem: public Assimp::IOSystem {
		bool Exists(const char*) const;
		char getOsSeparator() const;
		Assimp::IOStream* Open(const char*, const char*);
		void Close(Assimp::IOStream*);
	};
	class IOStream: public Assimp::IOStream {
		size_t pos;
	public:
		FilePath fp;

		IOStream(FilePath);

		size_t FileSize() const;

		size_t Read(void*, size_t, size_t);
		size_t Write(const void*, size_t, size_t);
		void Flush();

		aiReturn Seek(size_t, aiOrigin);
		size_t Tell() const;
	};

	int init();

	aiScene* import(const std::string&, unsigned int);
	const char* get_error_string();
}}}

#endif // BEE_FS_ASSIMP_H
