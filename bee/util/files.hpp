/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_FILES_H
#define _BEE_UTIL_FILES_H 1

// File handling functions

#include <fstream>
#include <sys/stat.h>

bool file_exists(std::string fname) {
        std::fstream f (fname.c_str());
        return f.good();
}
int file_delete(std::string fname) {
        return remove(fname.c_str());
}
int file_rename(std::string old_fname, std::string new_fname) {
        return rename(old_fname.c_str(), new_fname.c_str());
}
int file_copy(std::string fname, std::string new_fname) {
        std::ifstream source (fname, std::ios::binary);
        std::ofstream dest (new_fname, std::ios::binary);

        dest << source.rdbuf();

        source.close();
        dest.close();

        return 0;
}
std::string file_get_contents(std::string fname) {
        std::ifstream input (fname);
        if (input.is_open()) {
                std::string s, tmp;
                while (!input.eof()) {
                        getline(input, tmp);
                        s += tmp + "\n";
                }
                input.close();
                return s;
        }
        return "";
}

bool directory_exists(std::string fname) {
        struct stat st;
        stat(fname.c_str(), &st);
        return S_ISDIR(st.st_mode);
}
int directory_create(std::string fname) {
        return mkdir(fname.c_str(), 0755);
}
std::string directory_get_temp() {
        char t[] = "/tmp/bee-XXXXXX";
        static std::string fname (mkdtemp(t));
        return fname;
}

#endif // _BEE_UTIL_FILES_H
