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
#include "resources.hpp"
#include "rooms.hpp"

#include "../render/drawing.hpp"
#include "../render/renderer.hpp"
#include "../render/rgba.hpp"
#include "../render/viewdata.hpp"

#include "../resource/font.hpp"
#include "../resource/room.hpp"

#include "../ui/ui.hpp"
#include "../ui/elements.hpp"

namespace bee{ namespace console {
	namespace internal {
		bool is_open = false;

		std::unordered_map<std::string,std::pair<std::string,std::function<void (const MessageContents&)>>> commands;
		std::unordered_map<std::string,std::string> aliases;
		std::unordered_map<std::string,SIDP> variables;

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
}}

#include "commands.cpp"

namespace bee{ namespace console {
	/*
	* reset() - Reset the console log
	*/
	int reset() {
		internal::commands.clear();
		internal::aliases.clear();
		internal::variables.clear();

		internal::bindings.clear();

		internal::history.clear();
		internal::history_index = -1;

		internal::log.clear();
		internal::page_index = 0;

		internal::rect = {0, 0, 800, 500};
		internal::line_height = 20;

		ObjUITextEntry* obj_text_entry = dynamic_cast<ObjUITextEntry*>(internal::ui_text_entry->get_object());
		obj_text_entry->set_input(internal::ui_text_entry, "");
		obj_text_entry->reset_completion(internal::ui_text_entry);

		internal::update_ui();

		if (internal::td_log != nullptr) {
			delete internal::td_log;
			internal::td_log = nullptr;
		}

		return 0;
	}

	/*
	* internal::handle_input() - Handle the input as a console keypress
	* @e: the keyboard input event
	*/
	int internal::handle_input(SDL_Event* e) {
		// If the console is closed, handle keybindings
		if (!get_is_open()) {
			KeyBind kb = get_keybind(e->key.keysym.sym); // Fetch the command that's bound to the key
			if ((!e->key.repeat)||(kb.is_repeatable)) {
				if (!kb.command.empty()) { // If the command is set
					run(kb.command, true, 0); // Run the command without storing it in history
				}
			}

			return 1; // Return 1 on successful handling of binds
		}

		if (e->key.repeat != 0) { // On some platforms, repeat keys are sent on keyboard input
			return -1; // Return -1 on incorrect input
		}

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
				//input.pop_back(); // Remove the backquote from input
				toggle(); // Toggle the console
				break;
			}
		}

		return 0; // Return 0 on successful handling of console input
	}
	/*
	* internal::init() - Initialize the console and the default commands
	*/
	int internal::init() {
		line_height = engine->font_default->get_string_height(); // Store the default drawing line height for later console drawing operations

		// Register the console logger
		messenger::internal::register_protected("consolelog", {"engine", "console"}, true, [] (const MessageContents& msg) {
			log << msg.descr << "\n";
		});

		init_commands();

		run("exec \"config.cfg\"", true, 0); // Configure default binds

		return 0; // Return 0 on success
	}
	/*
	* internal::init_ui() - Init the console ui
	*/
	int internal::init_ui() {
		ui_handle = bee::ui::create_handle(rect.x, rect.y, rect.w, 10, nullptr);

		ui_handle->set_is_persistent(true);
		ObjUIHandle* obj_handle = dynamic_cast<ObjUIHandle*>(ui_handle->get_object());
		obj_handle->set_is_visible(ui_handle, is_open);
		obj_handle->set_color(ui_handle, {0, 0, 0, 255});

		ui_text_entry = bee::ui::create_text_entry(rect.x, rect.y+rect.h, 1, 80, [] (Instance* text_entry, const std::string& input) {
			bee::console::run(input); // Run the command

			// Reset the console state
			ObjUITextEntry* obj_text_entry = dynamic_cast<ObjUITextEntry*>(text_entry->get_object());
			obj_text_entry->set_input(text_entry, "");
			obj_text_entry->reset_completion(text_entry);
			obj_text_entry->set_focus(text_entry, true);
			page_index = 0;
		});

		ui_text_entry->set_is_persistent(true);
		ui_text_entry->set_data("input_tmp", std::string());

		ObjUITextEntry* obj_text_entry = dynamic_cast<ObjUITextEntry*>(ui_text_entry->get_object());
		obj_text_entry->set_is_visible(ui_text_entry, is_open);
		obj_text_entry->set_color(ui_text_entry, {127, 127, 127, 127});

		bee::ui::add_text_entry_completor(ui_text_entry, [] (Instance* text_entry, const std::string& input) -> std::vector<SIDP> {
			std::vector<SIDP> params = parse_parameters(input); // Parse the current parameters from the input line
			if (params.size() == 1) { // Complete the command if it's the only parameter
				return complete(text_entry, SIDP_s(params[0])); // Find new completion comands
			} else { // TODO: Complete command arguments
				return std::vector<SIDP>();
			}
		});
		bee::ui::add_text_entry_handler(ui_text_entry, [] (Instance* text_entry, const std::string& input, const SDL_Event* e) {
			switch (e->key.keysym.sym) {
				case SDLK_UP: { // The up arrow cycles through completion commands or reverse history
					std::vector<SIDP>* completions = static_cast<std::vector<SIDP>*>(SIDP_p(text_entry->get_data("completions")));
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
					std::vector<SIDP>* completions = static_cast<std::vector<SIDP>*>(SIDP_p(text_entry->get_data("completions")));
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
					ObjUITextEntry* obj_entry = dynamic_cast<ObjUITextEntry*>(text_entry->get_object());
					obj_entry->set_input(text_entry, "");
					obj_entry->reset_completion(text_entry);
					break;
				}
				default: {}
			}
		});

		return 0;
	}
	/*
	* internal::close() - Close the console and free its memory
	*/
	int internal::close() {
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
		return 0;
	}

	/*
	* internal::update_ui() - Update the UI visibility to match the console visibility
	*/
	int internal::update_ui() {
		ObjUIHandle* obj_handle = dynamic_cast<ObjUIHandle*>(ui_handle->get_object());
		obj_handle->update(ui_handle);
		obj_handle->set_is_visible(ui_handle, is_open);

		ObjUITextEntry* obj_text_entry = dynamic_cast<ObjUITextEntry*>(ui_text_entry->get_object());
		obj_text_entry->update(ui_text_entry);
		obj_text_entry->set_focus(ui_text_entry, is_open);
		obj_text_entry->set_is_visible(ui_text_entry, is_open);

		return 0;
	}

	/*
	* internal::run_internal() - Run a command in the console, silently or urgently if requested
	* @command: the command string to run, including the arguments
	* @is_urgent: whether to Immediately send the command or to wait until the end of the frame
	* @delay: the amount of milliseconds to delay command execution
	*/
	int internal::run_internal(const std::string& command, bool is_urgent, Uint32 delay) {
		std::string c = trim(command);
		std::vector<SIDP> params = parse_parameters(c); // Parse the parameters from the given command in order to get the command name as params[0]

		// Remove comments from the end of commands
		size_t compos = c.find("//");
		if (compos != std::string::npos) {
			c = c.substr(0, compos);
		}

		if (c.empty()) { // Check whether the provided command is empty
			return 1; // Return 1 on an empty command
		}

		if (commands.find(SIDP_s(params[0])) == commands.end()) { // If the command or alias does not exist, then output a warning
			if (aliases.find(SIDP_s(params[0])) == aliases.end()) {
				messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to run command \"" + SIDP_s(params[0]) + "\", the command does not exist.");
				return 2; // Return 2 on non-existent command
			} else { // Otherwise if the alias exists, run it
				return run(aliases[SIDP_s(params[0])], true, delay);
			}
		}

		if (is_urgent) { // If the command is urgent, send it immediately
			messenger::internal::send_urgent(
				{"engine", "console", SIDP_s(params[0])},
				E_MESSAGE::GENERAL,
				c,
				nullptr
			);
		} else { // Otherwise, send it normally
			messenger::send(MessageContents(
				get_ticks()+delay,
				{"engine", "console", SIDP_s(params[0])},
				E_MESSAGE::GENERAL,
				c,
				nullptr
			));
		}

		return 0; // Return 0 on success
	}
	/*
	* internal::run() - Run a command in the console
	* @command: the command string to run, including the arguments
	* @is_silent: whether to append the command to history and display it in console
	* @delay: the amount of milliseconds to delay command execution
	*/
	int internal::run(const std::string& command, bool is_silent, Uint32 delay) {
		if (!is_silent) { // If the command is not silent, then log it
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

		std::map<int,std::string> cmultiple = split(command, ';', true); // Split the command on any semicolons
		if (cmultiple.size() > 1) { // If there are multiple commands in the input
			bool has_failed = false;
			for (auto& c : cmultiple) { // Iterate over each command
				int r = run_internal(trim(c.second), false, delay); // Run the command
				if (r == -1) {
					break;
				} else if (r) {
					has_failed = true; // Store whether the command fails to run
				}
			}
			return (has_failed) ? -2 : 0; // Return -2 on command run failure, return 0 on success
		} else if (cmultiple.size() == 1) { // If there is only one command, run it
			return run_internal(command, false, delay); // Return the command run status
		} else { // If there are no commands to run
			return -1; // Return -1 on command parse failure
		}
	}
	/*
	* internal::complete() - Complete console commands
	* @command: the command to complete
	*/
	std::vector<SIDP> internal::complete(Instance* textentry, const std::string& command) {
		std::vector<SIDP> completions;

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

		return completions; // Return the vector on success
	}
	/*
	* internal::parse_parameters() - Convert the command to a parameter list
	* @command: the full command string
	*/
	std::vector<SIDP> internal::parse_parameters(const std::string& command) {
		std::map<int,std::string> params = split(command, ' ', true); // Split the command parameters by spaces, respecting quotes

		std::vector<SIDP> param_list; // Create a vector to store each parameter instead of the map from split()
		for (auto& p : params) { // Iterate over each parameter and store it as a SIDP interpreted type
			param_list.push_back(SIDP(p.second));
		}

		return param_list; // Return the vector of parameters on success
	}
	/*
	* internal::draw() - Draw the console and its output
	*/
	int internal::draw() {
		// Get the view offset
		int cx = static_cast<int>(ui_handle->get_corner_x());
		int cy = static_cast<int>(ui_handle->get_corner_y());

		ui_text_entry->set_corner_x(cx);
		ui_text_entry->set_corner_y(cy+rect.h);

		if (get_current_room()->get_current_view() != nullptr) {
			cx -= get_current_room()->get_current_view()->view_x;
			cy -= get_current_room()->get_current_view()->view_y;
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
		RGBA c_text = get_enum_color(E_RGB::BLACK);

		// Draw the console page number
		std::string p = std::to_string(page_index+1) + "/" + std::to_string(total_lines/line_amount+1);
		engine->font_default->draw_fast(cx + rect.w - 10 * p.length(), cy + rect.h - line_height, p, c_text);

		return 0; // Return 0 on success
	}

	/*
	* open() - Open the console
	*/
	int open() {
		internal::is_open = true;

		internal::update_ui();

		return 0;
	}
	/*
	* close() - Close the console
	*/
	int close() {
		internal::is_open = false;

		internal::update_ui();

		return 0;
	}
	/*
	* toggle() - Toggle the open/close state of the console
	*/
	int toggle() {
		internal::is_open = !internal::is_open;

		internal::update_ui();

		return 0;
	}
	/*
	* get_is_open() - Return whether the console is open or not
	*/
	bool get_is_open() {
		return internal::is_open;
	}

	/*
	* add_command() - Add a function which will handle a certain command
	* @command: the command name to handle
	* @descr: the command description that will be displayed when the user runs `help command_name` in this console subsystem
	* @func: the function to call that will handle the command
	*/
	int add_command(const std::string& command, const std::string& descr, std::function<void (const MessageContents&)> func) {
		if (internal::commands.find(command) != internal::commands.end()) { // If the command already exists, then output a warning
			messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to register new command \"" + command + "\", the command name is already in use.");
			return 1; // Return 1 on command existence
		}

		internal::commands.emplace(command, std::make_pair(descr, func)); // Add the command to the console command map

		messenger::internal::register_protected(command, {"engine", "console", command}, true, func); // Register the command with the messaging system

		return 0; // Return 0 on success
	}
	/*
	* add_command() - Add a function which will handle a certain command
	* ! When the function is called without a description, simply call it with an empty description
	* @command: the command name to handle
	* @func: the function to call that will handle the command
	*/
	int add_command(const std::string& command, std::function<void (const MessageContents&)> func) {
		return add_command(command, "", func);
	}

	/*
	* bind() - Bind a key to a console command
	* ! Console commands should generally be in snakecase but rebindable commands should be in camelcase
	* @key: the keycode to bind to
	* @keybind: the keybind to bind to
	*/
	int bind(SDL_Keycode key, KeyBind keybind) {
		if (internal::bindings.find(key) != internal::bindings.end()) { // If the key has already been bound, output a warning
			messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to bind key \"" + keystrings_get_string(key) + "\", the key is already in bound.");
			return 1;
		}

		keybind.key = key;

		internal::bindings.emplace(key, keybind);

		return 0;
	}
	/*
	* add_keybind() - Add a keybind's command and bind it to the given key
	* @key: the key to bind to
	* @keybind: the keybind to add
	* @func: the function to add as a command
	*/
	int add_keybind(SDL_Keycode key, KeyBind keybind, std::function<void (const MessageContents&)> func) {
		int r = add_command(keybind.command, func);
		r += bind(key, keybind);
		return r;
	}
	/*
	* bind() - Return the command that is bound to the given key
	* @key: the keycode to bind to
	*/
	KeyBind get_keybind(SDL_Keycode key) {
		if (internal::bindings.find(key) != internal::bindings.end()) { // If the key has been bound, then return its command
			return internal::bindings.at(key); // Return the command on success when no command argument is provided
		}
		return KeyBind(); // Return an empty string when the provided command is empty and the key has not been bound
	}
	/*
	* unbind() - Unbind a key from a console command
	* @key: the keycode to unbind
	*/
	int unbind(SDL_Keycode key) {
		if (internal::bindings.find(key) != internal::bindings.end()) { // If the key has been bound, then unbind it
			internal::bindings.erase(key);
		}
		return 0; // Return 0 on success
	}
	/*
	* unbind() - Unbind a keybind from a console command
	* @keybind: the keybind to unbind
	*/
	int unbind(KeyBind keybind, bool should_remove_command) {
		for (auto& bind : internal::bindings) {
			if (bind.second.command == keybind.command) {
				internal::bindings.erase(bind.first);

				if (should_remove_command) {
					internal::commands.erase(keybind.command);
				}

				return 0;
			}
		}

		//messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to unbind keybinding for \"" + keybind.command + "\": no key found");

		return 1;
	}
	/*
	* unbind_all() - Unbind all keys from the console
	*/
	int unbind_all() {
		internal::bindings.clear();
		return 0;
	}

	/*
	* alias() - Add a console alias to multiple commands
	* @alias: the alias to use
	* @commands: the commands to run when the alias is called
	*/
	int alias(const std::string& alias, const std::string& commands) {
		if (internal::commands.find(alias) != internal::commands.end()) { // If a command already exists, then return a warning
			messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to add alias \"" + alias + "\", a command with the same name exists.");
			return 1;
		}

		internal::aliases[alias] = commands; // Set the alias

		return 0; // Return 0 on success
	}
	/*
	* get_aliases() - Return the map of all the current aliases
	*/
	const std::unordered_map<std::string,std::string>& get_aliases() {
		return internal::aliases;
	}

	/*
	* set_var() - Set a console variable
	* @name: the name of the variable
	* @value: the value to set
	*/
	int set_var(const std::string& name, SIDP value) {
		if (internal::commands.find(name) != internal::commands.end()) { // If a command already exists, then return a warning
			messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to set variable \"" + name + "\", a command with the same name exists.");
			return 1;
		}

		internal::variables[name] = value;

		return 0;
	}
	/*
	* get_var() - Return the value of a console variable
	* @name: the name of the variable
	*/
	SIDP get_var(const std::string& name) {
		if (internal::variables.find(name) != internal::variables.end()) {
			return internal::variables[name];
		}
		return SIDP();
	}


	/*
	* run() - Run a command in the console
	* @command: the command string to run, including the arguments
	*/
	int run(const std::string& command) {
		return internal::run(command, false, 0); // Return the status of running a non-silent command with no requested delay
	}
	/*
	* get_help() - Return the description string of the given command
	* @command: the command to get the description for
	*/
	std::string get_help(const std::string& command) {
		if (internal::commands.find(command) == internal::commands.end()) { // If the command does not exist, then return a warning
			return "No available help for non-existent \"" + command + "\"";
		}

		std::string h = internal::commands[command].first; // Get the command description
		if (h.empty()) { // If the description is empty, then return a diagnostic message
			return "No available help for \"" + command + "\"";
		}

		return h; // Return the command's description on success
	}
}}

#endif // BEE_CORE_CONSOLE
