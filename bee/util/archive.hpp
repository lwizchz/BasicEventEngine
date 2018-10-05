/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_ARCHIVE_H
#define BEE_UTIL_ARCHIVE_H 1

#include <string>
#include <vector>

namespace util { namespace archive {
	std::string tar_extract_temp(const std::string&);

	std::string xz_decompress_temp(const std::string&);
}}

#endif // BEE_UTIL_ARCHIVE_H
