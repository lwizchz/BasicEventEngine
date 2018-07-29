/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_PLATFORM
#define BEE_UTIL_PLATFORM 1

// Platform compatibility functions

#include <sys/stat.h>

#include "platform.hpp" // Include the function definitions

#ifdef __linux__

#include <iostream>
#include <string.h>

#include <sys/time.h> // Include the required functions for non-blocking commandline input
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h> // Include the required network functions
#include <linux/limits.h> // Include the required PATH_MAX

namespace util { namespace platform {

/**
* @retval 0 linux
* @retval 1 windows
* @retval 2 macos
* @retval 3 unknown
*/
int get_platform() {
	return 0;
}

/**
* @returns the path of the executable
*/
std::string get_path() {
	char buffer[PATH_MAX];
	int len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
	if (len != -1) {
		buffer[len] = '\0';
		return std::string(buffer);
	}
	return std::string();
}

/**
* Delete the given file.
* @param fname the name of the file to delete
*
* @retval 0 success
* @retval nonzero failed to remove file
*/
int remove(const std::string& fname) {
	return ::remove(fname.c_str());
}
/**
* @param fname the name of the directory to check
*
* @returns whether the given directory exists
*/
int dir_exists(const std::string& fname) {
	struct stat st;
	if (stat(fname.c_str(), &st) == -1) { // Get the status of the given file
		return -1;
	}
	return (S_ISDIR(st.st_mode) == 0) ? 0 : 1;
}
/**
* Attempt to create a directory with the given path and permissions.
* @param path the path of the new directory
* @param mode the permissions of the directory
*
* @retval 0 success
* @retval -1 failed to create directory
*/
int mkdir(const std::string& path, mode_t mode) {
	return ::mkdir(path.c_str(), mode);
}
/**
* @see http://linux.die.net/man/3/mkdtemp for details
* @param t the template of the temporary directory in the format "*XXXXXX"
*
* @returns a path for a temporary directory with the given template
*/
std::string mkdtemp(const std::string& temp) {
	char* t = new char[temp.length()+1](); // Get the modifiable c-string version of the template
	strcpy(t, temp.c_str());

	char* tpath = ::mkdtemp(t); // Fetch the directory path
	std::string path;
	if (tpath != nullptr) { // If the directory was successfully created then set the path
		path.assign(t);
	}
	delete[] t;

	return path;
}

/**
* @see http://linux.die.net/man/3/inet_ntop for details
* @param src the address data in Network Byte Order
*
* @returns a IPv4 address string from the given data in Network Byte Order
*/
std::string inet_ntop(const void* src) {
	char dest[INET_ADDRSTRLEN]; // Declare a char array to store the address string
	::inet_ntop(AF_INET, src, dest, INET_ADDRSTRLEN); // Fetch the address string into dest
	return std::string(dest);
}

/**
* @returns whether there is input in the commandline without blocking
*/
bool has_commandline_input() {
	struct timeval tv = {0, 0}; // Wait 0 seconds and 0 microseconds for input
	fd_set rfds; // Declare a new set of input streams
	FD_ZERO(&rfds); // Clear the set
	FD_SET(0, &rfds); // Add standard input to the set

	return (select(1, &rfds, 0, 0, &tv) > 0); // Return true when the given input is waiting to be read
}
/**
* Change the color that commandline output will appear in.
* @see http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html for information on the color codes
* @param color the new color to use defined according to the Linux terminal color codes
*/
void commandline_color(std::ostream* o, int color) {
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
			commandline_color_reset(o);
		}
	}
	*o << code;
}
/**
* Reset the commandline color to the default value.
*/
void commandline_color_reset(std::ostream* o) {
	*o << "\033[0m";
}
/**
* Clear the commandline.
*/
void commandline_clear() {
	printf("\033[2J\033[1;1H\n");
}

}}

#elif _WIN32

#define NOMINMAX

#include <ws2tcpip.h> // Include the required Windows headers
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#include <conio.h> // Include the required functions for non-blocking commandline input
#include <sstream>

namespace util { namespace platform {

int get_platform() {
	return 1;
}

std::string get_path() {
	char buffer[MAX_PATH];
	int len = GetModuleFileName(nullptr, buffer, sizeof(buffer)-1);
	if (len > 0) {
		buffer[len] = '\0';
		return std::string(buffer);
	}
	return std::string();
}

int remove(const std::string& fname) {
	DWORD dwAttr = GetFileAttributes(fname.c_str()); // Get the file attributes
	if (dwAttr == 0xffffffff) { // If the file does not exist, return false
        	return -1;
	}

	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) { // If the file is a directory, delete it appropriately
		return (RemoveDirectory(fname.c_str())) ? 0 : 1; // Return whether the directory could be deleted
	} else { // Otherwise delete it normally
		return ::remove(fname.c_str()); // Return whether the file could be deleted
	}
}
int dir_exists(const std::string& fname) {
	DWORD dwAttr = GetFileAttributes(fname.c_str()); // Get the file attributes
	if (dwAttr == 0xffffffff) { // If the file does not exist, return false
        	return 0;
	}
	return (dwAttr & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0; // Return whether the file is a directory or not
}
int mkdir(const std::string& path, mode_t mode) {
	return _mkdir(path.c_str());
}
/*
* @note The constant MAX_PATH is defined by the Windows API.
* @see https://msdn.microsoft.com/en-us/library/windows/desktop/aa364992%28v=vs.85%29.aspx for details
* @param t the template of the temporary directory in the format "*XXXXXX" (unused on Windows)
*
* @returns a path for a temporary directory
*/
std::string mkdtemp(const std::string& t) {
	static std::string path; // Declare the path as static so that subsequent calls will return the same directory
	if (path.empty()) { // Check whether the path has been requested yet
        	char p[MAX_PATH];
		GetTempPath(MAX_PATH, p); // Request a unique temporary directory path
        	path = p;
        	path += "bee-" + std::to_string(GetCurrentProcessId());
	}

	platform::mkdir(path, 0755); // Recreate the directory if necessary

	return path; // Return the path
}

std::string inet_ntop(const void* src) {
	//InetNtop(AF_INET, src, dest, INET_ADDRSTRLEN); // FIXME: it won't let me include the Winsock2 library, ws2_32.lib, so I rewrote the below functionality

	const unsigned char* addr = static_cast<const unsigned char*>(src); // Cast the address data into unsigned chars
	char dest[INET_ADDRSTRLEN+1]; // Declare a char array to put the address into

	sprintf_s(dest, INET_ADDRSTRLEN, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]); // Convert each byte into an unsigned integer, separated by '.'

	return std::string(dest);
}

bool has_commandline_input() {
	return _kbhit();
}
/*
* Change the color that commandline output will appear in.
* @note The given ostream is ignored on Windows.
* @param color the new color to use defined according to the Linux terminal color codes
*/
void commandline_color(std::ostream* o, int color) {
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
			commandline_color_reset();
		}
	}
}
/*
* Reset the commandline color to the default value.
* @note The given ostream is ignored on Windows.
*/
void commandline_color_reset(std::ostream* o) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
void commandline_clear() {
	HANDLE h_stdout;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cell_count;
	COORD homecoords = {0, 0};

	h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (h_stdout == INVALID_HANDLE_VALUE) {
		return;
	}

	if (!GetConsoleScreenBufferInfo(h_stdout, &csbi)) {
		return;
	}

	cell_count = csbi.dwSize.X * csbi.dwSize.Y;

	if (!FillConsoleOutputCharacter(
		h_stdout,
		static_cast<TCHAR>(' '),
		cell_count,
		homecoords,
		&count
	)) {
		return;
	}

	if (!FillConsoleOutputCharacter(
		h_stdout,
		static_cast<TCHAR>(csbi.wAttributes),
		cell_count,
		homecoords,
		&count
	)) {
		return;
	}

	SetConsoleCursorPosition(h_stdout, homecoords);

	return;
}

}}

// Undefine windows.h macros
#include "windefine.hpp"

#elif __APPLE__

#include <iostream>
#include <string.h>

#include <sys/time.h> // Include the required functions for non-blocking commandline input
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h> // Include the required network functions

namespace util { namespace platform {

int get_platform() {
	return 2;
}

std::string get_path() {
	char buffer[PATH_MAX];
	int len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
	if (len != -1) {
		buffer[len] = '\0';
		return std::string(buffer);
	}
	return std::string();
}

int remove(const std::string& fname) {
	return ::remove(fname.c_str());
}
int dir_exists(const std::string& fname) {
	struct stat st;
	if (stat(fname.c_str(), &st) == -1) { // Get the status of the given file
		return -1;
	}
	return (S_ISDIR(st.st_mode) == 0) ? 0 : 1; // Return whether it is a directory or not
}
int mkdir(const std::string& path, mode_t mode) {
	return ::mkdir(path.c_str(), mode);
}
std::string mkdtemp(const std::string& temp) {
	char* t = new char[temp.length()+1](); // Get the modifiable c-string version of the template
	strcpy(t, temp.c_str());

	char* tpath = ::mkdtemp(t); // Fetch the directory path
	std::string path;
	if (tpath != nullptr) { // If the directory was successfully created then set the path
		path.assign(t);
	}
	delete[] t;

	return path;
}

std::string inet_ntop(const void* src) {
	char dest[INET_ADDRSTRLEN]; // Declare a char array to store the address string
	::inet_ntop(AF_INET, src, dest, INET_ADDRSTRLEN); // Fetch the address string into dest
	return std::string(dest); // Return a the address as a string on success
}

bool has_commandline_input() {
	struct timeval tv = {0, 0}; // Wait 0 seconds and 0 microseconds for input
	fd_set rfds; // Declare a new set of input streams
	FD_ZERO(&rfds); // Clear the set
	FD_SET(0, &rfds); // Add standard input to the set

	return (select(1, &rfds, 0, 0, &tv) > 0); // Return true when the given input is waiting to be read
}
void commandline_color(std::ostream* o, int color) {
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
			commandline_color_reset(o);
		}
	}
	*o << code;
}
void commandline_color_reset(std::ostream* o) {
	*o << "\033[0m";
}
void commandline_clear() {
	printf("\033[2J\033[1;1H\n");
}

}}

#else

#include <iostream>
#include <string>

// Most functions for unknown platforms are just dummies

namespace util { namespace platform {

int get_platform() {
	return 3;
}

std::string get_path() {
	return std::string();
}

int remove(const std::string& fname) {
	return 1;
}
int dir_exists(const std::string& fname) {
	return 0;
}
int mkdir(const std::string& path, mode_t mode) {
	return 1;
}
std::string mkdtemp(const std::string& temp) {
	return std::string();
}

std::string inet_ntop(const void* src) {
	const unsigned char* addr = (unsigned char*) src; // Cast the address data into unsigned chars
	char dest[INET_ADDRSTRLEN]; // Declare a char array to put the address into

	sprintf(dest, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]); // Convert each byte into an unsigned integer, separated by '.'

	return std::string(dest); // Return the address as a string
}

bool has_commandline_input() {
	return false;
}
void commandline_color(std::ostream* o, int color) {}
void commandline_color_reset(std::ostream* o) {}
void commandline_clear() {}

}}

#endif // platform

#endif // BEE_UTIL_PLATFORM
