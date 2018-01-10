/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_CONSOLE
#define BEE_CORE_CONSOLE 1

#include <sstream>
#include <algorithm>
#include <regex>

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "console.hpp"

#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/string.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "enginestate.hpp"
#include "input.hpp"
#include "keybind.hpp"
#include "rooms.hpp"

#include "../render/drawing.hpp"
#include "../render/renderer.hpp"
#include "../render/rgba.hpp"
#include "../render/viewport.hpp"

#include "../resource/font.hpp"
#include "../resource/script.hpp"
#include "../resource/room.hpp"

#include "../ui/ui.hpp"
#include "../ui/elements.hpp"

namespace bee{ namespace console {
	namespace internal {
		bool is_open = false;

		Script* scr_console = nullptr;

		std::unordered_map<SDL_Keycode,KeyBind> bindings;

		std::vector<std::string> history;
		int history_index = -1;

		std::stringstream log;
		TextData* td_log = nullptr;
		size_t page_index = 0;

		// Set the drawing sizes
		SDL_Rect rect = {0, 0, 800, 530};
		unsigned int line_height = 20;

		Instance* ui_handle = nullptr;
		Instance* ui_text_entry = nullptr;
	}

	/**
	* Reset the console log.
	*/
	void reset() {
		internal::bindings.clear();

		internal::history.clear();
		internal::history_index = -1;

		internal::log.clear();
		internal::page_index = 0;

		internal::rect = {0, 0, 800, 500};
		internal::line_height = 20;

		ObjUITextEntry* obj_text_entry = static_cast<ObjUITextEntry*>(internal::ui_text_entry->get_object());
		obj_text_entry->set_input(internal::ui_text_entry, "");
		obj_text_entry->reset_completion(internal::ui_text_entry);

		internal::update_ui();

		if (internal::td_log != nullptr) {
			delete internal::td_log;
			internal::td_log = nullptr;
		}
	}

	/**
	* Initialize the console and the default commands.
	* @retval 0 success
	* @retval 1 failed to load console Script
	*/
	int internal::init() {
		scr_console = Script::add("scr_console", "/bee/resources/scripts/console.py");
		if (scr_console == nullptr) {
			return 1;
		}

		line_height = engine->font_default->get_string_height(); // Store the default drawing line height for later console drawing operations

		// Register the console logger
		messenger::internal::register_protected("consolelog", {"engine", "console"}, true, [] (const MessageContents& msg) {
			log << msg.descr << "\n";
		});

		return 0;
	}
	/**
	* Init the console ui.
	* @retval 0 success
	* @retval 1 failed to create a UI element
	*/
	int internal::init_ui() {
		ui_handle = bee::ui::create_handle(rect.x, rect.y, rect.w, 10, nullptr);
		if (ui_handle == nullptr) {
			return 1;
		}

		ui_handle->set_is_persistent(true);
		ObjUIHandle* obj_handle = static_cast<ObjUIHandle*>(ui_handle->get_object());
		obj_handle->set_is_visible(ui_handle, is_open);
		obj_handle->set_color(ui_handle, {0, 0, 0, 255});

		ui_text_entry = bee::ui::create_text_entry(rect.x, rect.y+rect.h, 1, 80, [] (Instance* text_entry, const std::string& input) {
			bee::console::run(input); // Run the command

			// Reset the console state
			ObjUITextEntry* obj_text_entry = static_cast<ObjUITextEntry*>(text_entry->get_object());
			obj_text_entry->set_input(text_entry, "");
			obj_text_entry->reset_completion(text_entry);
			obj_text_entry->set_focus(text_entry, true);
			page_index = 0;
		});
		if (ui_text_entry == nullptr) {
			return 1;
		}

		ui_text_entry->set_is_persistent(true);
		ui_text_entry->set_data("input_tmp", std::string());

		ObjUITextEntry* obj_text_entry = static_cast<ObjUITextEntry*>(ui_text_entry->get_object());
		obj_text_entry->set_is_visible(ui_text_entry, is_open);
		obj_text_entry->set_color(ui_text_entry, {127, 127, 127, 127});

		/*bee::ui::add_text_entry_completor(ui_text_entry, [] (Instance* text_entry, const std::string& input) -> std::vector<Variant> {
			return complete(text_entry, input);
		});*/
		bee::ui::add_text_entry_handler(ui_text_entry, [] (Instance* text_entry, const std::string& input, const SDL_Event* e) {
			switch (e->key.keysym.sym) {
				case SDLK_UP: { // The up arrow cycles through completion commands or reverse history
					SIDP c = text_entry->get_data("completions"); // Store the copy returned by get_data()
					std::vector<SIDP>* completions = static_cast<std::vector<SIDP>*>(SIDP_p(c));
					if (completions->size() > 1) { // If a command is being completed
						int completion_index = SIDP_i(text_entry->get_data("completion_index"));
						if (completion_index > 0) { // If a completion command is already selected, lower the index and set the input line to the given command
							completion_index = fit_bounds(completion_index-1, 0, static_cast<int>(completions->size())-1);
							text_entry->set_data("input", *(completions->begin()+completion_index));
							text_entry->set_data("completion_index", completion_index);
						} else { // If the first completion command is selected, reset the input line to the previous user input
							text_entry->set_data("completion_index", -1);
							text_entry->set_data("input", text_entry->get_data("input_tmp"));
						}
					} else { // If a command is not being completed, cycle through history
						if (history.size() > 0) { // If there is a history to look up
							history_index = fit_bounds(history_index+1, 0, static_cast<int>(history.size())-1); // Prevent the index from going past the end
							// Replace the command line with the history item
							text_entry->set_data("input", *(history.rbegin()+history_index));
						}
					}
					break;
				}
				case SDLK_DOWN: { // The down arrow cycles through the forward history
					SIDP c = text_entry->get_data("completions"); // Store the copy returned by get_data()
					std::vector<SIDP>* completions = static_cast<std::vector<SIDP>*>(SIDP_p(c));
					if (completions->size() > 1) { // If a command is being completed
						int completion_index = SIDP_i(text_entry->get_data("completion_index"));
						if (completion_index == -1) { // If not completion command has been selected, store the previous user input
							text_entry->set_data("input_tmp", input);
						}
						// Raise the index and set the input line to the given command
						completion_index = fit_bounds(completion_index+1, 0, static_cast<int>(completions->size())-1);
						text_entry->set_data("input", *(completions->begin()+completion_index));
						text_entry->set_data("completion_index", completion_index);
					} else { // If a command is not being completed, cycle through history
						if (history_index > 0) { // If the index is in previous history
							history_index = fit_bounds(history_index-1, 0, static_cast<int>(history.size())-1); // Prevent the index from going past the front
							// Replace the command line with the history item
							text_entry->set_data("input", *(history.rbegin()+history_index));
						} else { // If the index is new history
							text_entry->set_data("input", std::string());
							history_index = -1;
						}
					}
					break;
				}
				case SDLK_ESCAPE: {
					ObjUITextEntry* obj_entry = static_cast<ObjUITextEntry*>(text_entry->get_object());
					obj_entry->set_input(text_entry, "");
					obj_entry->reset_completion(text_entry);
					break;
				}
				default: {}
			}
		});

		return 0;
	}
	/**
	* Close the console and free its memory.
	*/
	void internal::close() {
		if (scr_console == nullptr) {
			delete scr_console;
			scr_console = nullptr;
		}

		Room* room = get_current_room();
		if (room != nullptr) {
			ui_handle->set_is_persistent(false);
			ui_text_entry->set_is_persistent(false);
			room->destroy(ui_text_entry);
			room->destroy();
			ui_handle = nullptr;
			ui_text_entry = nullptr;
		} else {
			ui_handle = nullptr;
			ui_text_entry = nullptr;
		}
	}

	/**
	* Update the UI visibility to match the console visibility.
	*/
	void internal::update_ui() {
		ObjUIHandle* obj_handle = static_cast<ObjUIHandle*>(ui_handle->get_object());
		obj_handle->update(ui_handle);
		obj_handle->set_is_visible(ui_handle, is_open);

		ObjUITextEntry* obj_text_entry = static_cast<ObjUITextEntry*>(ui_text_entry->get_object());
		obj_text_entry->update(ui_text_entry);
		obj_text_entry->set_focus(ui_text_entry, is_open);
		obj_text_entry->set_is_visible(ui_text_entry, is_open);
	}

	/**
	* Handle the input as a console keypress.
	* @param e the keyboard input event
	*/
	void internal::handle_input(SDL_Event* e) {
		if (!get_is_open()) { // If the console is closed, handle keybindings
			KeyBind kb = get_keybind(e->key.keysym.sym); // Fetch the command that's bound to the key
			if ((!e->key.repeat)||(kb.is_repeatable)) {
				if (!kb.command.empty()) { // If the command is set
					run(kb.command, true); // Run the command without storing it in history
				}
			}
		} else if (e->key.repeat == 0) { // Avoid repeat keys sent on some platforms
			// Handle certain key presses in order to manipulate history or the command line
			switch (e->key.keysym.sym) {
				case SDLK_PAGEUP: { // The pageup key scrolls backward through the console log
					if (handle_newlines(log.str()).size()/((rect.h-30)/line_height + 1) > page_index) { // If the page index is lower than the full amount of pages in the log, increment the index
						++page_index;
					}
					break;
				}
				case SDLK_PAGEDOWN: { // The pagedown key scrolls forward through the console log
					if (page_index > 0) { // Limit the page index to greater than 0
						--page_index;
					}
					break;
				}

				case SDLK_BACKQUOTE: { // The tilde key toggles the console open state
					// TODO: allow different keys to open the console
					toggle(); // Toggle the console
					break;
				}
			}
		}
	}
	/**
	* Run a command in the console
	* @param command the command string to run, including the arguments
	* @param is_silent whether to append the command to history and display it in console
	*
	* @returns the script's return value
	* @see Script::run_string() and PythonScriptInterface::run_string()
	*/
	int internal::run(const std::string& command, bool is_silent) {
		// If the command is not silent, then log it
		if (!is_silent) {
			if (command[0] != ' ') { // If the first character is not a space, then add it to the console history
				// Fetch the last command if one exists
				std::string last_command = "";
				if (!history.empty()) {
					last_command = history.back();
				}

				if (command != last_command) { // If the current command is not identical to the previous command, then add it to the console history
					history.push_back(command);
				}
			}

			if (!get_options().is_headless) {
				messenger::send({"engine", "console"}, E_MESSAGE::INFO, "> " + trim(command)); // Output the command to the messenger log
			}
		}
		history_index = -1; // Reset the history index

		// Run the command
		return scr_console->run_string(command);
	}
	/*
	* Complete console commands.
	* @param command the command to complete
	*
	* @returns the vector of possible command completions
	*/
	/*std::vector<Variant> internal::complete(Instance* textentry, const std::string& command) {
		std::vector<Variant> completions;

		// Find any command matches
		for (auto& c : commands) { // Iterate over the possible commands
			if (c.first.find(command) == 0) { // If the given command begins with the completion string
				completions.push_back(c.first); // Add the given command to the completion list
			}
		}
		if (completions.size() <= 1) { // If there was one or none matches, return the vector
			return completions;
		}

		std::sort(completions.begin(), completions.end()); // Sort the completion commands alphabetically

		return completions;
	}*/
	/**
	* Draw the console and its output.
	*/
	void internal::draw() {
		// Get the view offset
		int cx = static_cast<int>(ui_handle->get_corner_x());
		int cy = static_cast<int>(ui_handle->get_corner_y());

		ui_text_entry->set_corner_x(cx);
		ui_text_entry->set_corner_y(cy+rect.h);

		if (get_current_room()->get_current_view() != nullptr) {
			cx -= get_current_room()->get_current_view()->view.x;
			cy -= get_current_room()->get_current_view()->view.y;
		}

		// Set the drawing sizes
		unsigned int input_line_y = rect.h - 30;

		// Draw console rectangle and input line
		draw_rectangle(cx, cy, rect.w, rect.h, -1, {127, 127, 127, 225});

		// Remove the top of the console log if it doesn't fit
		size_t line_amount = input_line_y/line_height+1; // Calculate the total lines that can be stored in the console window
		std::vector<std::string> lines = splitv(log.str(), '\n', false); // Separate the console log by each newline
		size_t total_lines = lines.size(); // Store the total line number for the below page number calculation

		// Split lines if they are wider than the console window
		for (auto it=lines.begin(); it!=lines.end(); ++it) {
			if (engine->font_default->get_string_width(*it) > rect.w) {
				int c = static_cast<int>(
					1.5 * rect.w / engine->font_default->get_font_size()
				) - 1;

				lines.emplace(it+1, (*it).substr(c));
				*it = (*it).substr(0, c);

				++total_lines;
			}
		}

		if (total_lines > line_amount) { // If there are more lines than can fit in the console, then remove the ones which shouldn't be rendered
			if (lines.size() > line_amount + page_index*line_amount + 1) { // If the console page has non-rendered lines before it, then remove them
				const size_t size_goal = lines.size() - line_amount - page_index*line_amount - 1;

				std::vector<std::string>::iterator it = lines.begin();
				for (size_t i=0; i<size_goal; ++i) {
					++it;
				}

				lines.erase(lines.begin(), it);
			}

			if (page_index > 0) { // If the console page has non-rendered lines after it, then remove them
				const size_t size_goal = lines.size() - line_amount - (page_index-1)*line_amount - 1;

				std::vector<std::string>::iterator it = lines.begin();
				for (size_t i=0; i<size_goal; ++i) {
					++it;
				}

				lines.erase(it, lines.end());
			}
		}

		std::string short_log = joinv(lines, '\n'); // Create a shorter log from the cropped log
		if ((page_index > 0)&&(lines.size() < line_amount)) { // If the console page has less lines than are renderable (e.g. it's the first page), then prepend extra newlines
			short_log = string_repeat(line_amount - lines.size() + 1, "\n") + short_log;
		}
		td_log = engine->font_default->draw(td_log, cx, cy, short_log, {0, 0, 0, 255}); // Draw the console log

		// Define several drawing colors
		RGBA c_text (E_RGB::BLACK);

		// Draw the console page number
		std::string p = std::to_string(page_index+1) + "/" + std::to_string(total_lines/line_amount+1);
		engine->font_default->draw_fast(cx + rect.w - 10 * p.length(), cy + rect.h - line_height, p, c_text);
	}

	/**
	* Open the console.
	*/
	void open() {
		internal::is_open = true;

		internal::update_ui();
	}
	/**
	* Close the console.
	*/
	void close() {
		internal::is_open = false;

		internal::update_ui();
	}
	/**
	* Toggle the open/close state of the console.
	*/
	void toggle() {
		internal::is_open = !internal::is_open;

		internal::update_ui();
	}
	/**
	* Return whether the console is open or not.
	*/
	bool get_is_open() {
		return internal::is_open;
	}
	/**
	* Clear the console log and reset the page index.
	*/
	void clear() {
		internal::log.str(std::string());
                internal::log.clear();
                internal::page_index = 0;
	}

	/**
	* Add a function which will handle a certain command.
	* @param command the command name to handle
	* @param descr the command description that will be displayed when the user runs `help command_name` in this console subsystem
	* @param func the function to call that will handle the command
	*
	* @retval 0 success
	*/
	int add_command(const std::string& command, const std::string& descr, std::function<void (const MessageContents&)> func) {
		// TODO: add python functions

		//messenger::internal::register_protected(command, {"engine", "console", command}, true, func); // Register the command with the messaging system

		return 0; // Return 0 on success
	}

	/**
	* Bind a key to a console command.
	* @param key the keycode to bind to
	* @param keybind the keybind to bind to
	*
	* @retval 0 success
	* @retval 1 failed since key is already bound
	*/
	int bind(SDL_Keycode key, KeyBind keybind) {
		if (internal::bindings.find(key) != internal::bindings.end()) { // If the key has already been bound, output a warning
			messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to bind key \"" + keystrings_get_string(key) + "\", the key is already bound.");
			return 1;
		}

		keybind.key = key;

		internal::bindings.emplace(key, keybind);

		return 0;
	}
	/**
	* Add a keybind's command and bind it to the given key.
	* @param key the key to bind to
	* @param keybind the keybind to add
	* @param func the function to add as a command
	*
	* @returns the sum of add_command() and bind()
	*/
	int add_keybind(SDL_Keycode key, KeyBind keybind, std::function<void (const MessageContents&)> func) {
		int r = add_command(keybind.command, "", func);
		r += bind(key, keybind);
		return r;
	}
	/**
	* Return the command that is bound to the given key.
	* @param key the keycode to find the bind of
	*
	* @returns the bound keybind or an empty bind if none was found
	*/
	KeyBind get_keybind(SDL_Keycode key) {
		if (internal::bindings.find(key) == internal::bindings.end()) { // If the key has not been bound, then return an empty bind
			return KeyBind();
		}
		return internal::bindings.at(key);
	}
	/**
	* Return the keycode bound to the keybind with the given name.
	* @param keybind the name of the keybind to find the key of
	*
	* @returns the bound keycode or SDLK_UNKNOWN if none was found
	*/
	SDL_Keycode get_keycode(const std::string& keybind) {
		for (auto& kb : internal::bindings) {
			if (kb.second.command == keybind) {
				return kb.second.key;;
			}
		}
		return SDLK_UNKNOWN;
	}
	/**
	* Unbind a key from a command.
	* @param key the keycode to unbind
	*/
	void unbind(SDL_Keycode key) {
		internal::bindings.erase(key);
	}
	/**
	* Unbind a keybind command from a key.
	* @param keybind the keybind to unbind
	*
	* @retval 0 success
	* @retval 1 failed to unbind since no key was bound
	*/
	int unbind(KeyBind keybind) {
		for (auto& bind : internal::bindings) {
			if (bind.second.command == keybind.command) {
				internal::bindings.erase(bind.first);
				return 0;
			}
		}

		return 1;
	}
	/**
	* Unbind all keys from their commands.
	*/
	void unbind_all() {
		internal::bindings.clear();
	}

	/*
	* Set a console variable.
	* @param name the name of the variable
	* @param value the value to set it to
	*/
	/*int set_var(const std::string& name, const SIDP& value) {
		if (internal::commands.find(name) != internal::commands.end()) { // If a command already exists, then return a warning
			messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to set variable \"" + name + "\", a command with the same name exists.");
			return 1;
		}

		internal::variables[name] = value;

		return 0;
	}*/
	/*
	* Return the value of a console variable.
	* @param name the name of the variable
	*
	* @returns the variable value
	*/
	/*Variant get_var(const std::string& name) {
		if (internal::variables.find(name) != internal::variables.end()) {
			return internal::variables[name];
		}

		return internal::replace_vars(name);
	}*/

	/**
	* Run a command in the console.
	* @param command the command string to run including the arguments
	*
	* @returns the script's return value
	* @see internal::run()
	*/
	int run(const std::string& command) {
		return internal::run(command, false); // Run the command in non-silent mode
	}
	/**
	* @param command the command to get the description for
	*
	* @returns the description string of the given command
	*/
	std::string get_help(const std::string& command) {
		/*if (internal::commands.find(command) == internal::commands.end()) { // If the command does not exist, then return a warning
			return "No available help for non-existent \"" + command + "\"";
		}

		std::string h = internal::commands[command].first; // Get the command description
		if (h.empty()) { // If the description is empty, then return a diagnostic message
			return "No available help for \"" + command + "\"";
		}

		return h; // Return the command's description on success*/
		return "";
	}
	/**
	* Log a message to the console.
	* @param type the message type
	* @param str the message to log
	*/
	void log(E_MESSAGE type, const std::string& str) {
		messenger::send({"engine", "console"}, type, str);
	}
}}

#endif // BEE_CORE_CONSOLE
