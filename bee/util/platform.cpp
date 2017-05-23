/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_PLATFORM
#define _BEE_UTIL_PLATFORM 1

// Platform compatibility functions

#include "platform.hpp" // Include the function definitions

#ifdef __linux__

/*
* bee_get_platform() - Return the platform id
* ! For Linux the platform id is 0
*/
int bee_get_platform() {
	return 0;
}

/*
* bee_get_path() - Return the path of the executable
*/
std::string bee_get_path() {
	char buffer[PATH_MAX];
	int len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
	if (len != -1) {
		buffer[len] = '\0';
		return std::string(buffer);
	}
	return std::string();
}

/*
* bee_itos() - Convert an integer to a string
* @i: the integer to convert
*/
std::string bee_itos(int i) {
	return std::to_string(i);
}
/*
* bee_stoi() - Convert a string to an integer
* @s: the string to convert
*/
int bee_stoi(const std::string& s) {
	return std::stoi(s);
}

/*
* bee_inc_dst() - Increment the time by 1 hour, given in seconds, in order to handle daylight savings changes
* ! On non-Windows systems this is a no-op
* @t: the time to increment
*/
time_t bee_inc_dst(time_t t) {
	return t;
}

/*
* bee_has_commandline_input() - Return whether there is input in the commandline without blocking
*/
bool bee_has_commandline_input() {
	struct timeval tv = {0, 0}; // Wait 0 seconds and 0 microseconds for input
	fd_set rfds; // Declare a new set of input streams
	FD_ZERO(&rfds); // Clear the set
	FD_SET(0, &rfds); // Add standard input to the set

	return (select(1, &rfds, 0, 0, &tv) > 0); // Return true when the given input is waiting to be read
}

/*
* bee_remove() - Delete the given file and return the status
* @fname: the name of the file to delete
*/
int bee_remove(const std::string& fname) {
	return remove(fname.c_str());
}
/*
* bee_dir_exists() - Return whether the given directory exists
* @fname: the name of the directory to check
*/
bool bee_dir_exists(const std::string& fname) {
	struct stat st;
	stat(fname.c_str(), &st); // Get the status of the given file
	return S_ISDIR(st.st_mode); // Return whether it is a directory or not
}
/*
* bee_mkdir() - Attempt to create a directory with the given path and permissions and return 0 on success
* @path: the path of the new directory
* @mode: the permissions of the directory
*/
int bee_mkdir(const char* path, mode_t mode) {
	return mkdir(path, mode); // Return whether the directory was successfully created or not
}
/*
* bee_mkdtemp() - Return a path for a temporary directory with the given template
* ! See http://linux.die.net/man/3/mkdtemp for details
* @t: the template of the temporary directory in the format "*XXXXXX"
*/
std::string bee_mkdtemp(const std::string& temp) {
	char* t = new char[temp.length()+1](); // Get the modifiable c-string version of the template
	strcpy(t, temp.c_str());

	char* tpath = mkdtemp(t); // Fetch the directory path
	std::string path;
	if (tpath != nullptr) { // If the directory was successfully created then set the path
		path.assign(t);
	}
	delete[] t;

	return path; // Return the path on success
}

/*
* bee_inet_ntop() - Return a IPv4 address string from the given data in Network Byte Order
* ! See http://linux.die.net/man/3/inet_ntop for details
* @src: the address data in Network Byte Order
*/
std::string bee_inet_ntop(const void* src) {
	char dest[INET_ADDRSTRLEN]; // Declare a char array to store the address string
	inet_ntop(AF_INET, src, dest, INET_ADDRSTRLEN); // Fetch the address string into dest
	return std::string(dest); // Return a the address as a string on success
}

/*
* bee_commandline_color() - Change the color that commandline output will appear in
* ! See http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html for information on the color codes
* @color: the new color to use defined according to the Linux terminal color codes
*/
int bee_commandline_color(int color) {
	std::string code = "";
	switch (color) {
		case 0: { // Black
			code = "\033[0;30m";
			break;
		}
		case 1: { // Red
			code = "\033[0;31m";
			break;
		}
		case 2: { // Green
			code = "\033[0;32m";
			break;
		}
		case 3: { // Yellow
			code = "\033[0;33m";
			break;
		}
		case 4: { // Blue
			code = "\033[0;34m";
			break;
		}
		case 5: { // Magenta
			code = "\033[0;35m";
			break;
		}
		case 6: { // Cyan
			code = "\033[0;36m";
			break;
		}
		case 7: { // White
			code = "\033[0;37m";
			break;
		}
		case 8: { // Bold Black
			code = "\033[1;30m";
			break;
		}
		case 9: { // Bold Red
			code = "\033[1;31m";
			break;
		}
		case 10: { // Bold Green
			code = "\033[1;32m";
			break;
		}
		case 11: { // Bold Yellow
			code = "\033[1;33m";
			break;
		}
		case 12: { // Bold Blue
			code = "\033[1;34m";
			break;
		}
		case 13: { // Bold Magenta
			code = "\033[1;35m";
			break;
		}
		case 14: { // Bold Cyan
			code = "\033[1;36m";
			break;
		}
		case 15: { // Bold White
			code = "\033[1;37m";
			break;
		}
		default: {
			bee_commandline_color_reset();
		}
	}
	std::cout << code;
	return 0;
}
/*
* bee_commandline_color_reset() - Reset the commandline color to the default value
*/
int bee_commandline_color_reset() {
	std::cout << "\033[0m";
	return 0;
}

#elif _WIN32

/*
* bee_get_platform() - Return the platform id
* ! For Windows the platform id is 1
*/
int bee_get_platform() {
	return 1;
}

/*
* bee_get_path() - Return the path of the executable
*/
std::string bee_get_path() {
	char buffer[PATH_MAX];
	int len = GetModuleFileName(nullptr, buffer, sizeof(buffer)-1);
	if (len > 0) {
		buffer[len] = '\0';
		return std::string(buffer);
	}
	return std::string();
}

/*
* bee_itos() - Convert an integer to a string
* @i: the integer to convert
*/
std::string bee_itos(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}
/*
* bee_stoi() - Convert a string to an integer
* @s: the string to convert
*/
int bee_stoi(const std::string& s) {
	std::stringstream ss (s);
	int i;
	ss >> i;
	return i;
}

/*
* bee_inc_dst() - Increment the time by 1 hour, given in seconds, in order to handle daylight savings changes
* @t: the time to increment
*/
time_t bee_inc_dst(time_t t) {
	return t + 3600;
}


/*
* bee_has_commandline_input() - Return whether there is input in the commandline without blocking
*/
bool bee_has_commandline_input() {
	return _kbhit();
}

/*
* bee_remove() - Delete the given file and return the status
* @fname: the name of the file to delete
*/
int bee_remove(const std::string& fname) {
	DWORD dwAttr = GetFileAttributes(fname.c_str()); // Get the file attributes
	if (dwAttr == 0xffffffff) { // If the file does not exist, return false
        return -1; // Return -1 when the file does not exist
	}
    if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) { // If the file is a directory, delete it appropriately
        return (RemoveDirectory(fname.c_str())) ? 0 : 1; // Return whether the directory could be deleted
    } else { // Otherwise delete it normally
        return remove(fname.c_str()); // Return whether the file could be deleted
    }
}
/*
* bee_dir_exists() - Return whether the given directory exists
* @fname: the name of the directory to check
*/
bool bee_dir_exists(const std::string& fname) {
	DWORD dwAttr = GetFileAttributes(fname.c_str()); // Get the file attributes
	if (dwAttr == 0xffffffff) { // If the file does not exist, return false
        return false;
	}
	return (dwAttr & FILE_ATTRIBUTE_DIRECTORY); // Return whether the file is a directory or not
}
/*
* bee_mkdir() - Attempt to create a directory with the given path
* ! This function accepts a second argument for file permissions but it is unused on Windows
* @path: the path of the new directory
* @mode: the permissions of the directory (unused on Windows)
*/
int bee_mkdir(const char* path, mode_t mode) {
	return _mkdir(path); // Return whether the directory was successfully created or not
}
/*
* bee_mkdtemp() - Return a path for a temporary directory
* ! This functions accepts a second argument for a name template but it is unused on Windows
* ! The constant MAX_PATH is defined by the Windows API
* ! See https://msdn.microsoft.com/en-us/library/windows/desktop/aa364992%28v=vs.85%29.aspx for details
* @t: the template of the temporary directory in the format "*XXXXXX" (unused on Windows)
*/
std::string bee_mkdtemp(const std::string& t) {
	/*static char path[MAX_PATH]; // Declare the path as static so that subsequent calls will return the same directory
	if (path[0] == '\0') { // Check whether the path has been requested yet
		GetTempPath(MAX_PATH, path); // Request a unique temporary directory path

	}
	bee_mkdir(path, 0755);
	return path; // Return the path*/
	static std::string path; // Declare the path as static so that subsequent calls will return the same directory
	if (path[0] == '\0') { // Check whether the path has been requested yet
        char p[MAX_PATH];
		GetTempPath(MAX_PATH, p); // Request a unique temporary directory path
        path = p;
        path += "bee-" + std::to_string(GetCurrentProcessId());
	}
	bee_mkdir(path.c_str(), 0755);
	return path; // Return the path
}

/*
* bee_inet_ntop() - Return a IPv4 address string from the given data in Network Byte Order
* @src: the address data in Network Byte Order
*/
std::string bee_inet_ntop(const void* src) {
    //InetNtop(AF_INET, src, dest, INET_ADDRSTRLEN); // FIXME: it won't let me include the Winsock2 library, ws2_32.lib, so I rewrote the below functionality

    const unsigned char* addr = (unsigned char*) src; // Cast the address data into unsigned chars
	char dest[INET_ADDRSTRLEN]; // Declare a char array to put the address into

    sprintf(dest, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]); // Convert each byte into an unsigned integer, separated by '.'

	return std::string(dest); // Return the address as a string
}

/*
* bee_commandline_color() - Change the color that commandline output will appear in
* ! See http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html for information on the color codes
* @color: the new color to use defined according to the Linux terminal color codes
*/
int bee_commandline_color(int color) {
	switch (color) {
		case 0: { // Black
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 16*7);
			break;
		}
		case 1: { // Red
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
			break;
		}
		case 2: { // Green
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
			break;
		}
		case 3: { // Yellow
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
			break;
		}
		case 4: { // Blue
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
			break;
		}
		case 5: { // Magenta
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
			break;
		}
		case 6: { // Cyan
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			break;
		}
		case 7: { // White
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			break;
		}
		case 8: { // Bold Black
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
			break;
		}
		case 9: { // Bold Red
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			break;
		}
		case 10: { // Bold Green
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			break;
		}
		case 11: { // Bold Yellow
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			break;
		}
		case 12: { // Bold Blue
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
			break;
		}
		case 13: { // Bold Magenta
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			break;
		}
		case 14: { // Bold Cyan
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			break;
		}
		case 15: { // Bold White
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			break;
		}
		default: {
			bee_commandline_color_reset();
		}
	}
	return 0;
}
/*
* bee_commandline_color_reset() - Reset the commandline color to the default value
*/
int bee_commandline_color_reset() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	return 0;
}

#endif // _WIN32

#endif // _BEE_UTIL_PLATFORM
