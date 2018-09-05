/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_FILES_H
#define BEE_UTIL_FILES_H 1

#include <string>
#include <vector>

namespace util {

bool file_exists(const std::string&);
size_t file_size(const std::string&);
int file_delete(const std::string&);
int file_rename(const std::string&, const std::string&);
int file_copy(const std::string&, const std::string&);
std::string file_get_contents(const std::string&);
std::streamoff file_put_contents(const std::string&, const std::string&);

bool directory_exists(const std::string&);
int directory_create(const std::string&);
std::string directory_get_temp();
std::vector<std::string> directory_get_files(const std::string&);

std::string file_basename(const std::string&);
std::string file_dirname(const std::string&);
std::string file_plainname(const std::string&);
std::string file_extname(const std::string&);

}

#endif // BEE_UTIL_FILES_H
