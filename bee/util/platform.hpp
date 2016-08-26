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

#ifdef __linux__

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
* bee_get_path() - Return the path of the executable
*/
std::string bee_get_path() {
        char buffer[1024];
        readlink("/proc/self/exe", buffer, 1024);
        return std::string(buffer);
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

        mkdtemp(t); // Fetch the directory path
        std::string path;
        if (t != nullptr) { // If the directory was successfully created then set the path
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
* bee_console_color() - Change the color that console output will appear in
* ! See http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html for information on the color codes
* @color: the new color to use defined according to the Linux terminal color codes
*/
int bee_console_color(int color) {
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
                        bee_console_color_reset();
                }
        }
        std::cout << code;
        return 0;
}
/*
* bee_console_color_reset() - Reset the console color to the default value
*/
int bee_console_color_reset() {
        std::cout << "\033[0m";
        return 0;
}

#elif _WIN32

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
* bee_get_path() - Return the path of the executable
*/
std::string bee_get_path() {
        char buffer[1024];
        GetModuleFileName(nullptr, buffer, 1024);
        return std::string(buffer);
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

/*
* bee_console_color() - Change the color that console output will appear in
* ! See http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html for information on the color codes
* @color: the new color to use defined according to the Linux terminal color codes
*/
int bee_console_color(int color) {
        switch (color) {
                case 0: { // Black
                        system("color 00");
                        break;
                }
                case 1: { // Red
                        system("color 04");
                        break;
                }
                case 2: { // Green
                        system("color 02");
                        break;
                }
                case 3: { // Yellow
                        system("color 06");
                        break;
                }
                case 4: { // Blue
                        system("color 01");
                        break;
                }
                case 5: { // Magenta
                        system("color 05");
                        break;
                }
                case 6: { // Cyan
                        system("color 03");
                        break;
                }
                case 7: { // White
                        system("color 07");
                        break;
                }
                case 8: { // Bold Black
                        system("color 08");
                        break;
                }
                case 9: { // Bold Red
                        system("color 0C");
                        break;
                }
                case 10: { // Bold Green
                        system("color 0A");
                        break;
                }
                case 11: { // Bold Yellow
                        system("color 0E");
                        break;
                }
                case 12: { // Bold Blue
                        system("color 09");
                        break;
                }
                case 13: { // Bold Magenta
                        system("color 0D");
                        break;
                }
                case 14: { // Bold Cyan
                        system("color 0B");
                        break;
                }
                case 15: { // Bold White
                        system("color 0F");
                        break;
                }
                default: {
                        bee_console_color_reset();
                }
        }
        return 0;
}
/*
* bee_console_color_reset() - Reset the console color to the default value
*/
int bee_console_color_reset() {
        system("color 07");
        return 0;
}

#endif

#endif // _BEE_UTIL_PLATFORM_H
