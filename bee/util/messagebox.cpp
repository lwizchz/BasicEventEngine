/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
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

namespace util {

/**
* Display a modal message box with the given information and buttons.
* @see https://wiki.libsdl.org/SDL_ShowMessageBox for details
* @param str the string to display
* @param button1 the text on the first button
* @param button2 the text on the second button
* @param button3 the text on the third button
*
* @retval 0 button1 was selected
* @retval 1 button2 was selected
* @retval 2 button3 was selected
* @retval -1 no button was selected
* @retval -2 failed to display the message box
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
		std::cerr << "Failed to display message box: " << util::get_sdl_error() << "\n"; // Output the error
		return -2; // Return -2 on display failure
	}
	return brid; // Return the pressed button value on success
}
/**
* Display a modal messagebox with a yes/no question.
* @param str the question to display
*
* @returns whether "Yes" was selected or not
*/
bool show_question(const std::string& str) {
	return (show_message(str, "Yes", "No", "") == 0) ? true : false;
}

/**
* Display a modal message box with the given information.
* @note If the function is called with no button text, then an "OK" button will be used.
* @str: the string to display
*
* @retval 0 the "OK" button was selected
* @retval -1 no button was selected
* @retval -2 failed to display the message box
*/
int show_message(const std::string& str) {
	return show_message(str, "OK", "", "");
}
/**
* Display a modal message box with the given warning.
* @param str the string to display
*
* @retval 0 success
* @retval <0 failed to display the message box
*/
int show_warning(const std::string& str) {
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", str.c_str(), nullptr);
}
/**
* Display a modal message box with the given error.
* @param str the string to display
*
* @retval 0 success
* @retval <0 failed to display the message box
*/
int show_error(const std::string& str) {
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", str.c_str(), nullptr);
}

}

#endif // BEE_UTIL_MESSAGEBOX
