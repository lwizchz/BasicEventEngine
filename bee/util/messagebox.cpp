/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_MESSAGEBOX
#define BEE_UTIL_MESSAGEBOX 1

// Message box functions

#include <string> // Include the required library headers
#include <iostream>

#include <SDL2/SDL.h> // Include the SDL2 headers for the message box functions

#include "messagebox.hpp" // Include the function definitions

#include "debug.hpp"

/*
* show_message() - Display a modal message box with the given information and buttons
* ! This is an entirely separate function from the previous show_message() because it is much more complicated
* @str: the string to display
* @button1: the text on the first button
* @button2: the text on the second button
* @button3: the text on the third button
*/
int show_message(const std::string& str, const std::string& button1, const std::string& button2, const std::string& button3) {
	SDL_MessageBoxButtonData buttons[3]; // Initialize a new array of SDL message box buttons
	int numbuttons = 0; // Set the initial button amount to 0
	if (button3.empty()) { // If there is no third button
		if (button2.empty()) { // If there is no second button then only fill in the first button info
			buttons[0] = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, button1.c_str()};
			numbuttons = 1;
		} else { // If there are two buttons then fill in the info for both
			buttons[0] = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, button1.c_str()};
			buttons[1] = {0, 1, button2.c_str()};
			numbuttons = 2;
		}
	} else { // If there are three buttons then fill in all info
		buttons[0] = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, button1.c_str()};
		buttons[1] = {0, 1, button2.c_str()};
		buttons[2] = {0, 2, button3.c_str()};
		numbuttons = 3;
	}

	SDL_MessageBoxData data = { // Construct the message box structure
		SDL_MESSAGEBOX_INFORMATION, // Message type
		nullptr, // Window
		"Message", // Title
		str.c_str(), // Message
		numbuttons, // Number of buttons
		buttons, // List of buttons
		nullptr // Color scheme
	};

	int brid = -1; // The index of the selected button
	if (SDL_ShowMessageBox(&data, &brid) < 0) { // If the message box does not display correctly
		std::cerr << "Failed to display message box: " << get_sdl_error() << "\n"; // Output the error
		return -2; // Return -2 on display failure
	}
	return brid; // Return the pressed button value on success
}
/*
* show_question() - Display a modal messagebox with the given yes/no question
* @str: the question to display
*/
bool show_question(const std::string& str) {
	return (show_message(str, "Yes", "No", "") == 0) ? true : false; // Return whether the user selected the yes button or not
}

/*
* show_message() - Display a modal message box with the given information
* ! When the function is called without the button text, simply provide it with a single "OK" button
* @str: the string to display
*/
int show_message(const std::string& str) {
	std::cout << "UTIL Messagebox: " << str << "\n"; // Output the message
	return show_message(str, "OK", "", "");
}
/*
* show_warning() - Display a modal message box with the given warning
* @str: the string to display
*/
int show_warning(const std::string& str) {
	std::cerr << "UTIL Messagebox: " << str << "\n"; // Output the message
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", str.c_str(), nullptr); // Return the status of the message box
}
/*
* show_error() - Display a modal message box with the given error
* @str: the string to display
* @is_fatal: whether to abort the program after displaying the message
*/
int show_error(const std::string& str, bool is_fatal) {
	std::cerr << str << "\n"; // Output the message
	int r = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", str.c_str(), nullptr); // Display the message box

	if (is_fatal) { // If the error should be considered fatal, throw an exception
		throw 1;
	}

	return r; // Otherwise return the status of the message box
}
/*
* show_error() - Display a modal message box with the given error
* ! When the function is called without the fatality, simply treat it as non-fatal
* @str: the string to display
*/
int show_error(const std::string& str) {
	return show_error(str, false);
}

#endif // BEE_UTIL_MESSAGEBOX
