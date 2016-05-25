/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_PLATFORM_H
#define _BEE_UTIL_PLATFORM_H 1

// Platform compatibility functions

#include <sys/stat.h> // Include the required file functions

#ifdef _WINDOWS

#include <ws2tcpip.h> // Include the required Windows headers
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#include <conio.h> // Include the required functions for non-blocking console input

/*
* bee_get_platform() - Return the platform id
* ! For Windows the platform id is 1
*/
int bee_get_platform() {
        return 1;
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
* bee_has_console_input() - Return whether there is input in the console without blocking
*/
bool bee_has_console_input() {
        return _kbhit();
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
        static char path[MAX_PATH]; // Declare the path as static so that subsequent calls will return the same directory
        if (path[0] == '\0') { // Check whether the path has been requested yet
                GetTempPath(MAX_PATH, path); // Request a unique temporary directory path
        }
        return path; // Return the path
}

/*
* bee_inet_ntop() - Return a IPv4 address string from the given data in Network Byte Order
* ! This function is broken on Windows, see the below FIXME for details
* ! See https://msdn.microsoft.com/en-us/library/windows/desktop/cc805843%28v=vs.85%29.aspx for details
* @src: the address data in Network Byte Order
*/
std::string bee_inet_ntop(const void* src) {
        char dest[INET_ADDRSTRLEN]; // Declare a char array to put the address into
        //InetNtop(AF_INET, src, dest, INET_ADDRSTRLEN); // FIXME: it won't let me include the Winsock2 library, ws2_32.lib
        return std::string(dest); // Return the address as a string
}

#else // _WINDOWS

#include <sys/time.h> // Include the required functions for non-blocking console input
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h> // Include the required network functions

/*
* bee_get_platform() - Return the platform id
* ! For Linux the platform id is 0
*/
int bee_get_platform() {
        return 0;
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
* bee_has_console_input() - Return whether there is input in the console without blocking
*/
bool bee_has_console_input() {
        struct timeval tv = {0, 0}; // Wait 0 seconds and 0 microseconds for input
        fd_set rfds; // Declare a new set of input streams
        FD_ZERO(&rfds); // Clear the set
        FD_SET(0, &rfds); // Add standard input to the set

        return (select(1, &rfds, 0, 0, &tv) > 0); // Return true when the given input is waiting to be read
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

        std::string path (mkdtemp(t)); // Fetch the directory path into a new string variable
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

#endif // _WINDOWS else

#endif // _BEE_UTIL_PLATFORM_H
