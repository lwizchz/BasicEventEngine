/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

/*
* file_exists() - Return whether the given filename is the location of a file
* @fname: the filename to check
*/
bool file_exists(const std::string& fname) {
	std::fstream f (fname.c_str());
	return f.good(); // Return whether the file could be successfully opened
}
/*
* file_delete() - Delete the given file and return the status
* @fname: the name of the file to delete
*/
int file_delete(const std::string& fname) {
	return bee_remove(fname);
}
/*
* file_rename() - Rename the given file and return the status
* @old_fname: the original name of the given file
* @new_fname: the new name of the given file
*/
int file_rename(const std::string& old_fname, const std::string& new_fname) {
	return rename(old_fname.c_str(), new_fname.c_str());
}
/*
* file_copy() - Copy the given file to a new file
* @fname: the original file to copy
* @new_fname: the file to be copied to
*/
int file_copy(const std::string& fname, const std::string& new_fname) {
	std::ifstream source (fname, std::ios::binary); // Open the original file in binary mode
	std::ofstream dest (new_fname, std::ios::binary); // Open the copy in binary mode

	dest << source.rdbuf(); // Read the entire file from the original to the copy

	source.close(); // Close both files
	dest.close();

	return 0; // Return 0 on success
}
/*
* file_get_contents() - Return a string of the contents of the given file
* @fname: the file to read
*/
std::string file_get_contents(const std::string& fname) {
	std::ifstream input (fname); // Open the given file for reading
	if (input.is_open()) { // If the file was successfully opened
		std::string s, tmp;
		while (!input.eof()) { // Read each line of the file into a string
			getline(input, tmp);
			s += tmp + "\n";
		}
		input.close(); // Close the file
		s.erase(s.end()-1); // Remove the last new line
		return s; // Return the contents of the file on success
	}
	std::cerr << "UTIL Failed to open \"" << fname << "\"\n"; // Output error if the file could not be opened
	return ""; // Return an empty string if the file could not be opened
}
/*
* file_put_contents() - Write the given string to the given file and return the bytes written
* @fname: the file to write
* @contents: the string to write to the file
*/
std::streamoff file_put_contents(const std::string& fname, const std::string& contents) {
	std::ofstream output (fname); // Open the given file for writing
	if (output.is_open()) { // If the file was successfully opened
		output.write(contents.c_str(), contents.size()); // Write to the file
		std::streamoff n = output.tellp();
		output.close(); // Close the file
		return n; // Return the number of bytes written on success
	}
	std::cerr << "UTIL Failed to open \"" << fname << "\"\n"; // Output error if the file could not be opened
	return 0;
}

/*
* directory_exists() - Return whether the given directory exists
* @fname: the name of the directory to check
*/
bool directory_exists(const std::string& fname) {
	return bee_dir_exists(fname); // Call the cross-platform dir_exists function
}
/*
* directory_create() - Create a directory at the given path
* @fname: the path of the new directory
*/
int directory_create(const std::string& fname) {
	return bee_mkdir(fname, 0755); // Call the cross-platform mkdir function with default unix permissions
}
/*
* directory_get_temp() - Get a temporary directory to store temporary game files in
*/
std::string directory_get_temp() {
	std::string d = bee_mkdtemp("/tmp/bee-XXXXXX"); // Call the cross-platform mkdtemp function with the format specifier
	if (!d.empty()) { // If the call was successful, then append a slash to the path
		d += "/";
	}
	return d; // Return the path on success
}

/*
* file_basename() - Return the basename of the given path
* ! For the path "/tmp/bee.log", the basename is "bee.log"
* @path: the path to evaluate
*/
std::string file_basename(const std::string& path) {
	return {
		std::find_if(path.rbegin(), path.rend(), // Iterating in reverse over the path,
			[] (char c) {
				return c == '/' || c == '\\'; // Return the first character that is a slash, i.e. the end of a directory
			}
		).base(),
		path.end()
	}; // Return the string from the last directory to the end
}
/*
* file_dirname() - Return the dirname of the given path
* ! For the path "/tmp/bee.log", the dirname is "/tmp/"
* @path: the path to evaluate
*/
std::string file_dirname(const std::string& path) {
	return {
		path.begin(),
		std::find_if(path.rbegin(), path.rend(), // Iterating in reverse over the path,
			[] (char c) {
				return c == '/' || c == '\\'; // Return the first character that is a slash, i.e. the end of a directory
			}
		).base()
	}; // Return the string from the beginning to the last directory
}
/*
* file_plainname() - Return the plain name of the given path
* ! For the path "/tmp/bee.log", the plain name is "/tmp/bee"
* @path: the path to evaluate
*/
std::string file_plainname(const std::string& path) {
	return {
		path.begin(),
		std::find_if(path.rbegin(), path.rend(), // Iterating in reverse over the path,
			[] (char c) {
				return c == '.'; // Return the first character that is a period, i.e. the start of an extension
			}
		).base()-1 // Don't include the period in the plain name
	}; // Return the string from the beginning to the extension
}
/*
* file_extname() - Return the extension of the given path
* ! For the path "/tmp/bee.log", the extension is ".log"
* @path: the path to evaluate
*/
std::string file_extname(const std::string& path) {
	return {
		std::find_if(path.rbegin(), path.rend(), // Iterating in reverse over the path,
			[] (char c) {
				return c == '.'; // Return the first character that is a period, i.e. the start of an extension
			}
		).base()-1, // Include the period in the extension
		path.end()
	}; // Return the string from the extension to the end
}

#endif // BEE_UTIL_FILES
