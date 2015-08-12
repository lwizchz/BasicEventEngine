/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_MESSAGEBOX_H
#define _BEE_UTIL_MESSAGEBOX_H 1

// Message box functions

#include <iostream>
#include <SDL2/SDL.h>

int show_message(std::string str) {
	std::cout << str << "\n";
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", str.c_str(), NULL);
}
int show_warning(std::string str) {
	std::cerr << str << "\n";
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", str.c_str(), NULL);
}
int show_error(std::string str, bool is_fatal) {
	std::cerr << str << "\n";
	int r = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", str.c_str(), NULL);

	if (is_fatal) {
		throw 1;
	}

	return r;
}
int show_error(std::string str) {
	return show_error(str, false);
}

int show_message(std::string str, std::string button1, std::string button2, std::string button3) {
	SDL_MessageBoxButtonData buttons[3];
	int numbuttons = 0;
	if (button3.empty()) {
		if (button2.empty()) {
			buttons[0] = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, button1.c_str()};
			numbuttons = 1;
		} else {
			buttons[0] = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, button1.c_str()};
			buttons[1] = {0, 1, button2.c_str()};
			numbuttons = 2;
		}
	} else {
		buttons[0] = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, button1.c_str()};
		buttons[1] = {0, 1, button2.c_str()};
		buttons[2] = {0, 2, button3.c_str()};
		numbuttons = 3;
	}

	SDL_MessageBoxData data = {
		SDL_MESSAGEBOX_INFORMATION, // Message type
		NULL, // Window
		"Message", // Title
		str.c_str(), // Message
		numbuttons, // Number of buttons
		buttons, // List of buttons
		NULL // Color scheme
	};

	int brid = 0;
	if (SDL_ShowMessageBox(&data, &brid) < 0) {
		std::cerr << "Failed to display message box: " << SDL_GetError() << "\n";
		return -2;
	}
	return brid;
}
bool show_question(std::string str) {
	return (show_message(str, "Yes", "No", "") == 0) ? true : false;
}

#endif // _BEE_UTIL_MESSAGEBOX_H
