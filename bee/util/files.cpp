/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_FILES
#define BEE_UTIL_FILES 1

// File handling functions

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>

#include "files.hpp" // Include the function definitions

#include "platform.hpp" // Include the required platform-specific file operation functions

namespace util {

/**
* @param fname the filename to check
*
* @returns whether the given filename is the location of a file
*/
bool file_exists(const std::string& fname) {
	std::fstream f (fname.c_str());
	return f.good(); // Return whether the file could be successfully opened
}
/**
* Delete the given file.
* @param fname the name of the file to delete
*
* @retval 0 success
* @retval nonzero failed to remove file
*/
int file_delete(const std::string& fname) {
	return platform::remove(fname);
}
/**
* Rename the given file.
* @param old_fname the original name of the given file
* @param new_fname the new name of the given file
*
* @retval 0 success
* @retval nonzero failed to rename file
*/
int file_rename(const std::string& old_fname, const std::string& new_fname) {
	return rename(old_fname.c_str(), new_fname.c_str());
}
/**
* Copy the given file to a new file.
* @param fname the original file to copy
* @param new_fname the file to be copied to
*
* @retval 0 success
* @retval 1 failed to open source file
* @retval 1 failed to open destination file
*/
int file_copy(const std::string& fname, const std::string& new_fname) {
	// Open the original file in binary mode
	std::ifstream source (fname, std::ios::binary);
	if (!source.is_open()) {
		return 1;
	}

	// Open the copy in binary mode
	std::ofstream dest (new_fname, std::ios::binary);
	if (!dest.is_open()) {
		return 2;
	}

	dest << source.rdbuf(); // Read the entire file from the original to the copy

	// Close both files
	source.close();
	dest.close();

	return 0;
}
/**
* @param fname the file to read
*
* @returns a string of the contents of the given file or an empty string if it could not be opened
*/
std::string file_get_contents(const std::string& fname) {
	// Open the given file for reading
	std::ifstream input (fname);
	if (!input.is_open()) {
		std::cerr << "UTIL Failed to open \"" << fname << "\" for reading\n"; // Output an error if the file could not be opened
		return "";
	}

	std::string contents, tmp;
	while (!input.eof()) { // Read each line of the file into a string
		getline(input, tmp);
		contents += tmp + "\n";
	}
	contents.pop_back(); // Remove the last new line

	input.close(); // Close the file

	return contents;
}
/**
* Write the given string to the given file.
* @param fname the file to write to
* @param contents the string to write to the file
*
* @returns the amount of bytes written
*/
std::streamoff file_put_contents(const std::string& fname, const std::string& contents) {
	// Open the given file for writing
	std::ofstream output (fname);
	if (!output.is_open()) {
		std::cerr << "UTIL Failed to open \"" << fname << "\" for writing\n"; // Output an error if the file could not be opened
		return 0;
	}

	output.write(contents.c_str(), contents.size()); // Write to the file
	std::streamoff n = output.tellp();

	output.close(); // Close the file

	return n;
}

/**
* @param fname the name of the directory to check
*
* @returns whether the given directory exists
*/
bool directory_exists(const std::string& fname) {
	return (platform::dir_exists(fname) == 1);
}
/**
* Create a directory at the given path.
* @param fname the path of the new directory
*
* @retval 0 success
* @retval -1 failed to create directory
*/
int directory_create(const std::string& fname) {
	return platform::mkdir(fname, 0755);
}
/**
* @returns the path of a temporary directory to store temporary game files in
*/
std::string directory_get_temp() {
	std::string d = platform::mkdtemp("/tmp/bee-XXXXXX"); // Call the cross-platform mkdtemp function with the format specifier
	if (!d.empty()) {
		d += "/";
	}
	return d;
}

/**
* @note For the path "/tmp/bee.log", the basename is "bee.log".
* @param path the path to evaluate
*
* @returns the basename of the given path
*/
std::string file_basename(const std::string& path) {
	return {
		std::find_if(path.rbegin(), path.rend(), // Iterating in reverse over the path,
			[] (char c) {
				return ((c == '/')||(c == '\\')); // Return the first character (from the end) that is a slash, i.e. the end of a directory
			}
		).base(),
		path.end()
	}; // Return the string from the last directory to the end
}
/**
* @note For the path "/tmp/bee.log", the dirname is "/tmp/".
* @note For a path with no slashes, the dirname is "." (the current directory).
* @param path the path to evaluate
*
* @returns the dirname of the given path
*/
std::string file_dirname(const std::string& path) {
	// Find the last slash
	size_t fslash = path.rfind("/");
	size_t bslash = path.rfind("\\");
	if ((fslash == std::string::npos)&&(bslash == std::string::npos)) {
		return ".";
	}

	// Return the string from the beginning to the last directory
	if (fslash == std::string::npos) {
		return path.substr(0, bslash);
	}
	return path.substr(0, fslash);
}
/**
* @note For the path "/tmp/bee.log", the plain name is "/tmp/bee"
* @param path the path to evaluate
*
* @returns the plain name of the given path
*/
std::string file_plainname(const std::string& path) {
	// Find the last period
	size_t period = path.rfind(".");
	if (period == std::string::npos) {
		return path;
	}

	return path.substr(0, period);
}
/**
* @note For the path "/tmp/bee.log", the extension is ".log".
* @param path the path to evaluate
*
* @returns the extension of the given path
*/
std::string file_extname(const std::string& path) {
	// Find the last period
	size_t period = path.rfind(".");
	if (period == std::string::npos) {
		return "";
	}

	return path.substr(period);
}

}

#endif // BEE_UTIL_FILES
