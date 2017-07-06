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
#include "../util/files.hpp"

#include "../init/gameoptions.hpp"

#include "enginestate.hpp"
#include "input.hpp"
#include "resources.hpp"
#include "room.hpp"
#include "messenger/messenger.hpp"

#include "../render/drawing.hpp"
#include "../render/renderer.hpp"
#include "../render/rgba.hpp"
#include "../render/transition.hpp"
#include "../render/viewdata.hpp"

#include "../resource/font.hpp"
#include "../resource/room.hpp"

namespace bee{
	/*
	* Console::Console() - Initialize the console
	*/
	Console::Console() :
		is_open(false),

		commands(),
		aliases(),
		variables(),
		bindings(),

		input(),
		history(),
		history_index(-1),

		log(),
		page_index(0),

		completion_commands(),
		completion_index(-1),
		input_tmp(),

		x(0),
		y(0),
		w(800),
		h(530),
		line_height(20),

		td_log(nullptr)
	{}
	/*
	* Console::~Console() - Free the console log TextData
	*/
	Console::~Console() {
		if (td_log != nullptr){
			delete td_log;
			td_log = nullptr;
		}
	}

	/*
	* internal::console_handle_input() - Handle the input as a console keypress
	* @e: the keyboard input event
	*/
	int internal::console_handle_input(SDL_Event* e) {
		// If the console is closed, handle keybindings
		if (!console_get_is_open()) {
			if (e->key.repeat == 0) { // If the key is not repeating
				std::string command = console_bind(e->key.keysym.sym); // Fetch the command that's bound to the key
				if (!command.empty()) { // If the command is set
					console_run(command, true, 0); // Run the command without storing it in history
				}
			}

			return 1; // Return 1 on successful handling of binds
		}

		if (e->key.repeat != 0) { // On some platforms, repeat keys are sent on keyboard input
			return -1; // Return -1 on incorrect input
		}

		append_input(&engine->console->input, &e->key); // Append the input to the console input line

		// Handle certain key presses in order to manipulate history or the command line
		switch (e->key.keysym.sym) {
			case SDLK_UP: { // The up arrow cycles through completion commands or reverse history
				if (engine->console->completion_commands.size() > 1) { // If a command is being completed
					if (engine->console->completion_index > 0) { // If a completion command is already selected, lower the index and set the input line to the given command
						engine->console->completion_index = fit_bounds(engine->console->completion_index-1, 0, (int)engine->console->completion_commands.size()-1);
						engine->console->input = (*(engine->console->completion_commands.begin()+engine->console->completion_index));
					} else { // If the first completion command is selected, reset the input line to the previous user input
						engine->console->completion_index = -1;
						engine->console->input = engine->console->input_tmp;
					}
				} else { // If a command is not being completed, cycle through history
					if (engine->console->history.size() > 0) { // If there is a history to look up
						engine->console->history_index = fit_bounds(engine->console->history_index+1, 0, (int)engine->console->history.size()-1); // Prevent the index from going past the end
						// Replace the command line with the history item
						engine->console->input.clear();
						engine->console->input = (*(engine->console->history.rbegin()+engine->console->history_index));
					}
				}
				break;
			}
			case SDLK_DOWN: { // The down arrow cycles through the forward history
				if (engine->console->completion_commands.size() > 1) { // If a command is being completed
					if (engine->console->completion_index == -1) { // If not completion command has been selected, store the previous user input
						engine->console->input_tmp = engine->console->input;
					}
					// Raise the index and set the input line to the given command
					engine->console->completion_index = fit_bounds(engine->console->completion_index+1, 0, (int)engine->console->completion_commands.size()-1);
					engine->console->input = (*(engine->console->completion_commands.begin()+engine->console->completion_index));
				} else { // If a commandis not being completed, cycle through history
					if (engine->console->history_index > 0) { // If the index is in previous history
						engine->console->history_index = fit_bounds(engine->console->history_index-1, 0, (int)engine->console->history.size()-1); // Prevent the index from going past the front
						// Replace the command line with the history item
						engine->console->input.clear();
						engine->console->input = (*(engine->console->history.rbegin()+engine->console->history_index));
					} else { // If the index is new history
						engine->console->history_index = -1; // Set the index appropriately
						engine->console->input.clear(); // Clear the command line
					}
				}
				break;
			}

			case SDLK_PAGEUP: { // The pageup key scrolls backward through the console log
				if (handle_newlines(engine->console->log.str()).size()/((engine->console->h-30)/engine->console->line_height + 1) > engine->console->page_index) { // If the page index is lower than the full amount of pages in the log, increment the index
					++engine->console->page_index;
				}
				break;
			}
			case SDLK_PAGEDOWN: { // The pagedown key scrolls forward through the console log
				if (engine->console->page_index > 0) { // Limit the page index to greater than 0
					--engine->console->page_index;
				}
				break;
			}

			case SDLK_TAB: { // The tab key attempts to complete commands from user input
				engine->console->input.pop_back(); // Remove the tab character from the command line

				std::vector<SIDP> params = console_parse_parameters(engine->console->input); // Parse the current parameters from the input line
				if (params.size() == 1) { // Complete the command if it's the only parameter
					engine->console->completion_commands.clear(); // Clear the old completion commands
					console_complete(params[0].s()); // Find new completion comands
				} else { // TODO: Complete command arguments

				}

				break;
			}
			case SDLK_RETURN: { // The enter key runs the command currently in the input line
				bee::console_run(engine->console->input); // Run the command

				// Reset the console state
				engine->console->input.clear();
				engine->console->input_tmp.clear();
				engine->console->completion_commands.clear();
				engine->console->completion_index = -1;
				engine->console->page_index = 0;

				break;
			}
			case SDLK_ESCAPE: { // The escape key clears the input line
				// Clear the input and completion states
				engine->console->input.clear();
				engine->console->input_tmp.clear();
				engine->console->completion_commands.clear();
				engine->console->completion_index = -1;
				break;
			}

			case SDLK_BACKQUOTE: { // The tilde key toggles the console open state
				// TODO: allow different keys to open the console
				engine->console->input.pop_back(); // Remove the backquote from input
				console_toggle(); // Toggle the console
				break;
			}

			default: { // Handle all other keys
				if (engine->console->input.empty()) { // If the input line is now empty (i.e. backspace), clear the completion commands
					engine->console->completion_commands.clear();
				}

				if (engine->console->completion_index > -1) { // Clear the completion commands when additional input is received
					engine->console->completion_index = -1;
					engine->console->input_tmp.clear();
					engine->console->completion_commands.clear();
				}

				std::vector<SIDP> params = console_parse_parameters(engine->console->input); // Parse the current parameters from the input line
				if (params.size() == 1) { // If the command is the only parameter
					if (engine->console->completion_commands.size() > 1) { // If a command is being completed, update the completion list for the new input
						engine->console->completion_commands.clear();
						console_complete(params[0].s());
					}
				}
			}
		}

		return 0; // Return 0 on successful handling of console input
	}
	/*
	* internal::console_init_commands() - Initialize the default console commands
	*/
	int internal::console_init_commands() {
		engine->console->line_height = engine->font_default->get_string_height(); // Store the default drawing line height for later console drawing operations

		// Register the console logger
		internal::messenger_register_protected("consolelog", {"engine", "console"}, true, [] (std::shared_ptr<MessageContents> msg) {
			engine->console->log << msg->descr << "\n";
		});

		// Below comments will use [optional] to specify certain argument rules
		/*
		* console_quit - End the game
		*/
		console_add_command(
			"quit",
			"End the game",
			[] (std::shared_ptr<MessageContents> msg) {
				messenger_send({"engine", "console"}, E_MESSAGE::INFO, "Quitting...");
				set_transition_type(E_TRANSITION::NONE);
				end_game();
			}
		);
		/*
		* console_help [command] - Show help text for certain commands
		* ! The description of the command is used as the help text
		* ! When called without a command specified, only the first line of each command's help text will be displayed
		* @command: the command to show help for
		*/
		console_add_command(
			"help",
			"Show help text for specified commands",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a specific command was specified, output its entire help text
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, "Command help for \"" + params[1].s() + "\":\n" + console_get_help(params[1].s()));
				} else { // If the command was called without any arguments, output the first line of help text for every registered command
					std::string help = "Available commands are:\n"; // Initialize a help string to be used as output

					// Create a usage string for each engine command
					std::list<std::string> commands = {
						"quit",
						"help [\"command\"]",
						"clear",
						"echo \"string\"",
						"console_toggle",
						"alias [\"alias\"] [\"commands\"]",
						"bind \"key\" [\"command\"]",
						"unbind \"key\"",
						"exec \"filename.cfg\"",
						"wait delay \"command\"",
						"unwait",
						"set \"name\" \"value\"",
						"get \"name\"",
						"screenshot [\"filename.bmp\"]",
						"debug [\"mode\"]",
						"volume [\"level\"]",
						"restart",
						"restart_room"
					};
					for (auto& c : commands) { // Iterate over the commands
						std::vector<SIDP> p = console_parse_parameters(c); // Get the list of command parameters in order to remove the usage string parts
						std::string h = console_get_help(p[0].s()); // Get the command help text

						if (!h.empty()) { // If the help text exists, output the command and its text
							help += c + "\n\t" + handle_newlines(h)[0] + "\n";
						}
					}
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, help); // Send the output to the console log
				}
			}
		);
		/*
		* console_find "search" - Output all commands which match a certain string
		* @"search": the string to search for
		*/
		console_add_command(
			"find",
			"Output all commands which match a certain string",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				std::string commands = "";
				std::string search = params[1].s();
				for (auto& c : engine->console->commands) {
					std::string h = console_get_help(c.first);
					if (c.first.find(search) != std::string::npos) {
						commands += c.first + "\n\t" + handle_newlines(h)[0] + "\n";
					} else if (h.find(search) != std::string::npos) {
						if (handle_newlines(h)[0].find(search) == std::string::npos) {
							commands += c.first + "\n\t" + h + "\n";
						} else {
							commands += c.first + "\n\t" + handle_newlines(h)[0] + "\n";
						}
					}
				}

				messenger_send({"engine", "console"}, E_MESSAGE::INFO, commands); // Send the output to the console log
			}
		);
		/*
		* console_clear - Clear the console log
		*/
		console_add_command(
			"clear",
			"Clear the console log",
			[] (std::shared_ptr<MessageContents> msg) {
				// Clear the console log and reset the page index
				engine->console->log.str(std::string());
				engine->console->log.clear();
				engine->console->page_index = 0;
			}
		);
		/*
		* console_echo ["string"] - Output a string to the console
		* ! This is particularly useful for config scripts
		* @"string": the string that will be printed
		*/
		console_add_command(
			"echo",
			"Output a string to the console\n"
			"Primarily useful for config scripts",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				std::string output = ""; // Initialize a string to be used as output
				for (auto it=params.begin()+1; it!= params.end(); ++it) { // Iterate over the arguments
					output.append(string_unescape((*it).to_str())); // Append each argument to the output with a space in between
					output += " ";
				}
				if (!output.empty()) { // If the output isn't empty, send it to the console log
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, output);
				}
			}
		);

		/*
		* console_open - Open the console
		*/
		console_add_command(
			"console_open",
			"Open the console",
			[] (std::shared_ptr<MessageContents> msg) {
				console_open();
			}
		);
		/*
		* console_close - Close the console
		*/
		console_add_command(
			"console_close",
			"Close the console",
			[] (std::shared_ptr<MessageContents> msg) {
				console_close();
			}
		);
		/*
		* console_toggle - Toggle the open status of the console
		*/
		console_add_command(
			"console_toggle",
			"Toggle the status of the console\n"
			"Also see console_open and _close.",
			[] (std::shared_ptr<MessageContents> msg) {
				console_toggle();
			}
		);

		/*
		* console_alias ["alias"] ["commands"] - Alias multiple commands to a single command
		* ! When the multiple commands are omitted, the currently aliased commands are output
		* ! When the alias command is omitted, all aliased commands are output
		* @"alias": the alias to use
		* @"commands": the commands to run when alias is called
		*/
		console_add_command(
			"alias",
			"Alias multiple commands to a single command\n"
			"The aliases can be viewed by omitting the commands",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) {
					console_alias(params[1].s(), string_unescape(params[2].s()));
				} else if (params.size() > 1) {
					auto a = console_get_aliases();
					if (a.find(params[1].s()) != a.end()) {
						messenger_send({"engine", "console"}, E_MESSAGE::INFO, a[params[1].s()]);
					} else {
						messenger_send({"engine", "console"}, E_MESSAGE::INFO, "No alias set for \"" + params[1].s() + "\"");
					}
				} else {
					auto amap = console_get_aliases();
					std::string aliases = "";
					for (auto& a : amap) {
						aliases += a.first + "\n\t" + a.second + "\n";
					}
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, aliases);
				}
			}
		);
		/*
		* console_bind "key" ["command"] - Bind a key to a command
		* ! When the command is omitted, the currently bound command is output
		* @"key": the key to bind to
		* @"command": the command to run when the given key is pressed
		*/
		console_add_command(
			"bind",
			"Bind a key to a command\n"
			"The binds can be viewed by omitting the commands",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) { // If both a key and command are provided, bind the command to the key
					console_bind(keystrings_get_key(params[1].s()), string_unescape(params[2].s()));
				} else if (params.size() > 1) { // If only a command is provided, output the command that is bound to it
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, console_bind(keystrings_get_key(params[1].s())));
				} else { // If no key is provided, output a warning
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "No key specified for binding");
				}
			}
		);
		/*
		* console_unbind "key" - Unbind a key from a command
		* @"key": the key to unbind
		*/
		console_add_command(
			"unbind",
			"Unbind a key from a command\n"
			"All keys can be unbound by provided \"all\" as the key",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a key is provided, unbind it from any commands
					if (params[1].s() == "all") {
						console_unbind_all();
					} else {
						console_unbind(keystrings_get_key(params[1].s()));
					}
				} else { // If no key is provided, output a warning
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "No key specified for unbinding");
				}
			}
		);
		/*
		* console_exec "filename.cfg" - Execute the specified config file
		* ! Note that no commands from a config file will be appended to the console history but will appear in messenger logging
		* @"filename.cfg": the config file to execute
		*/
		console_add_command(
			"exec",
			"Execute the specified config file\n"
			"No commands from the specified file will be apended to console history",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a filename is provided, execute it
					std::string fn = "cfg/"+params[1].to_str(); // Construct the path
					if (file_exists(fn)) { // Ensure that the file exists before opening it
						std::string cfg = file_get_contents(fn); // Fetch the file contents
						std::map<int,std::string> commands = handle_newlines(cfg); // Separate the contents by line
						for (auto& c : commands) { // Iterate over each line and run it
							if (!c.second.empty()) {
								console_run(c.second, true, 0);
							}
						}
					} else { // If the file cannot be opened, output a warning
						messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "The requested exec file \"" + fn + "\" does not exist");
					}
				} else { // If no filename is provided, output a warning
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "No config file specified for execution");
				}
			}
		);
		/*
		* console_wait delay "command" - Execute the specified command after a given delay
		* ! Note that the given delay is a minimum value to wait and the messaging system may take longer to process it
		* @delay: the amount of milliseconds to delay running the command
		* @"command": the command to run
		*/
		console_add_command(
			"wait",
			"Execute the specified command after the given millisecond delay\n"
			"The given value is the minimum time before the message will be processed",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) { // If both required arguments were provided, execute the command
					console_run(string_unescape(params[2].s()), true, params[1].i());
				} else if (params.size() > 1) { // If no command was provided, output a warning
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "No command specified for wait");
				} else { // If no arguments were provided, output a warning
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "No arguments specified for wait");
				}
			}
		);
		/*
		* console_unwait - Remove all wait commands from the messaging system, useful for breaking endless loops
		*/
		console_add_command(
			"unwait",
			"Remove all wait commands from the messaging system\n"
			"Useful for breaking endless loops",
			[] (std::shared_ptr<MessageContents> msg) {
				const std::vector<std::string> wait_tags = {"engine", "console", "wait"};
				engine->messages.erase(std::remove_if(engine->messages.begin(), engine->messages.end(), [&wait_tags] (std::shared_ptr<MessageContents> m) {
					return (m->tags == wait_tags);
				}), engine->messages.end());
			}
		);
		/*
		* console_set - Set a console variable to the given  value
		* @name: the name of the variable
		* @value: the value to set
		*/
		console_add_command(
			"set",
			"Set a console variable to the given value",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) {
					console_set_var(params[1].s(), params[2]);
				} else {
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Not enough arguments specified for set");
				}
			}
		);
		/*
		* console_get - Output the value of a console variable
		* @name: the name of the variable
		*/
		console_add_command(
			"get",
			"Output the value of a console variable",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) {
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, console_get_var(params[1].s()).to_str());
				} else {
					messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Not enough arguments specified for get");
				}
			}
		);

		/*
		* console_screenshot ["filename.bmp"] - Save a screenshot to the given file
		* @"filename.bmp": the filename to save to
		*/
		console_add_command(
			"screenshot",
			"Save a screenshot to the given file",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a filename is provided, then save the screenshot to it
					save_screenshot(params[1].to_str());
				} else { // If no filename is provided, then save the screenshot to the default path
					save_screenshot("screenshot.bmp");
				}
			}
		);

		/*
		* console_debug [value] - Enable or disable debug mode
		* ! When the value is omitted, the current debug mode is output
		* @value: 0 to disable, 1 to enable
		*/
		console_add_command(
			"debug",
			"Enable or disable debug mode",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) {
					GameOptions o = get_options();
					o.is_debug_enabled = params[1].i();
					set_options(o);
				} else {
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, std::to_string(get_options().is_debug_enabled));
				}
			}
		);

		/*
		* console_volume [value] - Set the global sound volume from 0.0 to 1.0
		* ! When the value is omitted, the current global volume is output
		* @value: the new volume level to use
		*/
		console_add_command(
			"volume",
			"Set the global sound volume from 0.0 to 1.0",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = console_parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) {
					set_volume(params[1].d());
				} else {
					messenger_send({"engine", "console"}, E_MESSAGE::INFO, std::to_string(get_volume()));
				}
			}
		);

		/*
		* console_restart - Restart the game
		*/
		console_add_command(
			"restart",
			"Restart the game",
			[] (std::shared_ptr<MessageContents> msg) {
				restart_game();
			}
		);
		/*
		* console_restart_room - Restart the current room
		*/
		console_add_command(
			"restart_room",
			"Restart the current room",
			[] (std::shared_ptr<MessageContents> msg) {
				restart_room();
			}
		);

		/*
		* console_info - Output information about the current room
		*/
		console_add_command(
			"info",
			"Output information about the current room",
			[] (std::shared_ptr<MessageContents> msg) {
				messenger_send({"engine", "console"}, E_MESSAGE::INFO, get_current_room()->get_print());
			}
		);

		/*
		* console_pause - Toggle the pause state of the game
		*/
		console_add_command(
			"pause",
			"Toggle the pause state of the game",
			[] (std::shared_ptr<MessageContents> msg) {
				set_is_paused(!get_is_paused());
			}
		);

		console_run("exec \"config.cfg\"", true, 0); // Configure default binds

		return 0; // Return 0 on success
	}
	/*
	* internal::console_run_internal() - Run a command in the console, silently or urgently if requested
	* @command: the command string to run, including the arguments
	* @is_urgent: whether to Immediately send the command or to wait until the end of the frame
	* @delay: the amount of milliseconds to delay command execution
	*/
	int internal::console_run_internal(const std::string& command, bool is_urgent, Uint32 delay) {
		std::string c = trim(command);
		std::vector<SIDP> params = console_parse_parameters(c); // Parse the parameters from the given command in order to get the command name as params[0]

		// Remove comments from the end of commands
		size_t compos = c.find("//");
		if (compos != std::string::npos) {
			c = c.substr(0, compos);
		}

		if (c.empty()) { // Check whether the provided command is empty
			return 1; // Return 1 on an empty command
		}

		if (engine->console->commands.find(params[0].s()) == engine->console->commands.end()) { // If the command or alias does not exist, then output a warning
			if (engine->console->aliases.find(params[0].s()) == engine->console->aliases.end()) {
				messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to run command \"" + params[0].s() + "\", the command does not exist.");
				return 2; // Return 2 on non-existent command
			} else { // Otherwise if the alias exists, run it
				return console_run(engine->console->aliases[params[0].s()], true, delay);
			}
		}

		if (is_urgent) { // If the command is urgent, send it immediately
			internal::messenger_send_urgent(std::shared_ptr<MessageContents>(new MessageContents(
				get_ticks()+delay,
				{"engine", "console", params[0].s()},
				E_MESSAGE::GENERAL,
				c,
				nullptr
			)));
		} else { // Otherwise, send it normally
			messenger_send(std::shared_ptr<MessageContents>(new MessageContents(
				get_ticks()+delay,
				{"engine", "console", params[0].s()},
				E_MESSAGE::GENERAL,
				c,
				nullptr
			)));
		}

		return 0; // Return 0 on success
	}
	/*
	* internal::console_run() - Run a command in the console
	* @command: the command string to run, including the arguments
	* @is_silent: whether to append the command to history and display it in console
	* @delay: the amount of milliseconds to delay command execution
	*/
	int internal::console_run(const std::string& command, bool is_silent, Uint32 delay) {
		if (!is_silent) { // If the command is not silent, then log it
			if (command[0] != ' ') { // If the first character is not a space, then add it to the console history
				// Fetch the last command if one exists
				std::string last_command = "";
				if (!engine->console->history.empty()) {
					last_command = engine->console->history.back();
				}

				if (command != last_command) { // If the current command is not identical to the previous command, then add it to the console history
					engine->console->history.push_back(command);
				}
			}
			messenger_send({"engine", "console"}, E_MESSAGE::INFO, "> " + trim(command)); // Output the command to the messenger log
		}
		engine->console->history_index = -1; // Reset the history index

		std::map<int,std::string> cmultiple = split(command, ';', true); // Split the command on any semicolons
		if (cmultiple.size() > 1) { // If there are multiple commands in the input
			bool has_failed = false;
			for (auto& c : cmultiple) { // Iterate over each command
				int r = console_run_internal(trim(c.second), false, delay); // Run the command
				if (r == -1) {
					break;
				} else if (r) {
					has_failed = true; // Store whether the command fails to run
				}
			}
			return (has_failed) ? -2 : 0; // Return -2 on command run failure, return 0 on success
		} else if (cmultiple.size() == 1) { // If there is only one command, run it
			return console_run_internal(command, false, delay); // Return the command run status
		} else { // If there are no commands to run
			return -1; // Return -1 on command parse failure
		}
	}
	/*
	* internal::console_complete() - Complete console commands
	* @command: the command to complete
	*/
	int internal::console_complete(const std::string& command) {
		// Find any command matches
		for (auto& c : engine->console->commands) { // Iterate over the possible commands
			if (c.first.find(command) == 0) { // If the given command begins with the completion string
				engine->console->completion_commands.push_back(c.first); // Add the given command to the completion list
			}
		}
		if (engine->console->completion_commands.size() == 0) { // If no matches were found, then reset the completion state
			engine->console->completion_index = -1;
			engine->console->input_tmp.clear();

			return 1; // Return 1 on matching failure
		} else if (engine->console->completion_commands.size() == 1) { // If there was only a single match, then use it and reset the completion state
			engine->console->input = engine->console->completion_commands[0] + " "; // Replace the input with the full matched command

			// Reset the completion state
			engine->console->completion_index = -1;
			engine->console->completion_commands.clear();
			engine->console->input_tmp.clear();

			return 0; // Return 0 on match success to exit early
		}

		// Attempt partial command completion if all completion commands share a common root
		// ! Note that by definition, all completion commands will share at least as many characters as the length of console->input
		std::string matched = ""; // Define a string that contains the matched portion of multiple commands
		bool is_matched = true;
		for (size_t i=engine->console->input.length(); i<engine->console->completion_commands[0].length(); ++i) { // Iterate over the characters of the first completion command, beginning at the first character that might not match
			for (size_t j=1; j<engine->console->completion_commands.size(); ++j) { // Iterate over the other completion commands
				if (engine->console->completion_commands[0][i] != engine->console->completion_commands[j][i]) { // If the given character does not match each completion command, then break
					is_matched = false;
					break;
				}
			}

			if (is_matched) { // If the character matched, append it to the match string
				matched += engine->console->completion_commands[0][i];
			} else { // Otherwise, break;
				break;
			}
		}
		engine->console->input += matched; // Replace the input with the matched portion of a command

		std::sort(engine->console->completion_commands.begin(), engine->console->completion_commands.end()); // Sort the completion commands alphabetically

		return 0; // Return 0 on success
	}
	/*
	* internal::console_parse_parameters() - Convert the command to a parameter list
	* @command: the full command string
	*/
	std::vector<SIDP> internal::console_parse_parameters(const std::string& command) {
		std::map<int,std::string> params = split(command, ' ', true); // Split the command parameters by spaces, respecting quotes

		std::vector<SIDP> param_list; // Create a vector to store each parameter instead of the map from split()
		for (auto& p : params) { // Iterate over each parameter and store it as a SIDP interpreted type
			param_list.push_back(SIDP(p.second, true));
		}

		return param_list; // Return the vector of parameters on success
	}
	/*
	* internal::console_draw() - Draw the console and its output
	*/
	int internal::console_draw() {
		// Get the view offset
		int cx = engine->console->x;
		int cy = engine->console->y;
		if (get_current_room()->get_current_view() != nullptr) {
			cx -= get_current_room()->get_current_view()->view_x;
			cy -= get_current_room()->get_current_view()->view_y;
		}

		// Set the drawing sizes
		unsigned int input_line_y = engine->console->h - 30;

		// Draw console rectangle and input line
		draw_rectangle(cx, cy, engine->console->w, engine->console->h, true, {127, 127, 127, 225});
		draw_line(cx, cy + engine->console->h - engine->console->line_height, cx + engine->console->w, cy + engine->console->h - engine->console->line_height, E_RGB::BLACK);

		// Remove the top of the console log if it doesn't fit
		size_t line_amount = input_line_y/engine->console->line_height; // Calculate the total lines that can be stored in the console window
		std::vector<std::string> lines = splitv(engine->console->log.str(), '\n', false); // Separate the console log by each newline
		size_t total_lines = lines.size(); // Store the total line number for the below page number calculation

		// Split lines if they are wider than the console window
		for (auto it=lines.begin(); it!=lines.end(); ++it) {
			if ((unsigned int)engine->font_default->get_string_width(*it) > engine->console->w) {
				int c = 1.5*engine->console->w/engine->font_default->get_font_size()-1;

				lines.emplace(it+1, (*it).substr(c));
				*it = (*it).substr(0, c);

				++total_lines;
			}
		}

		if (total_lines > line_amount) { // If there are more lines than can fit in the console, then remove the ones which shouldn't be rendered
			if (lines.size() > line_amount + engine->console->page_index*(line_amount+1) + 1) { // If the console page has non-rendered lines before it, then remove them
				const size_t size_goal = lines.size() - line_amount - engine->console->page_index*(line_amount+1) - 1;

				std::vector<std::string>::iterator it = lines.begin();
				for (size_t i=0; i<size_goal; ++i) {
					++it;
				}

				lines.erase(lines.begin(), it);
			}

			if (engine->console->page_index > 0) { // If the console page has non-rendered lines after it, then remove them
				const size_t size_goal = lines.size() - line_amount - (engine->console->page_index-1)*(line_amount+1) - 1;

				std::vector<std::string>::iterator it = lines.begin();
				for (size_t i=0; i<size_goal; ++i) {
					++it;
				}

				lines.erase(it, lines.end());
			}
		}

		std::string short_log = joinv(lines, '\n'); // Create a shorter log from the cropped log
		if ((engine->console->page_index > 0)&&(lines.size() < line_amount)) { // If the console page has less lines than are renderable (e.g. it's the first page), then prepend extra newlines
			short_log = string_repeat(line_amount - lines.size() + 1, "\n") + short_log;
		}
		engine->console->td_log = engine->font_default->draw(engine->console->td_log, cx, cy, short_log, {0, 0, 0, 255}); // Draw the console log

		// Define several drawing colors
		RGBA c_text = get_enum_color(E_RGB::BLACK);
		RGBA c_back = {127, 127, 127, 127};

		// Draw the console page number
		std::string p = std::to_string(engine->console->page_index+1) + "/" + std::to_string(total_lines/line_amount+1);
		engine->font_default->draw_fast(cx + engine->console->w - 10 * p.length(), cy + engine->console->h - engine->console->line_height*2, p, c_text);

		// Draw the console input and a blinking cursor
		engine->font_default->draw_fast(cx, cy + engine->console->h - engine->console->line_height, engine->console->input, c_text); // Draw the console input
		if (get_ticks()/500 % 2) { // Draw a blinking cursor that changes every 500 ticks
			engine->font_default->draw_fast(cx + engine->font_default->get_string_width(engine->console->input), cy + engine->console->h - engine->console->line_height, "_", c_text);
		}

		// Draw any completion commands in a box below the input line
		if (engine->console->completion_commands.size() > 1) { // If completion commands exist, draw them
			draw_rectangle(cx, cy + engine->console->h, engine->console->w, engine->console->completion_commands.size()*engine->console->line_height, true, c_back); // Draw a box to contain the commands
			for (size_t i=0; i<engine->console->completion_commands.size(); ++i) { // Iterate over the completion commands
				std::string cmd = " " + engine->console->completion_commands[i]; // Prepend each command with a space
				if (i == (size_t)engine->console->completion_index) { // If the command is selected, replace the space with a cursor
					cmd[0] = '>';
				}
				engine->font_default->draw_fast(cx, cy + engine->console->h + engine->console->line_height*i, cmd, c_text); // Draw the console completion command
			}
		}

		return 0; // Return 0 on success
	}

	/*
	* console_open() - Open the console
	*/
	int console_open() {
		engine->console->is_open = true;
		return 0;
	}
	/*
	* console_close() - Close the console
	*/
	int console_close() {
		engine->console->is_open = false;
		return 0;
	}
	/*
	* console_toggle() - Toggle the open/close state of the console
	*/
	int console_toggle() {
		engine->console->is_open = !engine->console->is_open;
		return 0;
	}
	/*
	* console_get_is_open() - Return whether the console is open or not
	*/
	bool console_get_is_open() {
		return engine->console->is_open;
	}

	/*
	* console_add_command() - Add a function which will handle a certain command
	* @command: the command name to handle
	* @descr: the command description that will be displayed when the user runs `help command_name` in this console subsystem
	* @func: the function to call that will handle the command
	*/
	int console_add_command(const std::string& command, const std::string& descr, std::function<void (std::shared_ptr<MessageContents>)> func) {
		if (engine->console->commands.find(command) != engine->console->commands.end()) { // If the command already exists, then output a warning
			messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to register new command \"" + command + "\", the command name is already in use.");
			return 1; // Return 1 on command existence
		}

		engine->console->commands.emplace(command, std::make_pair(descr, func)); // Add the command to the console command map

		internal::messenger_register_protected("console_"+command, {"engine", "console", command}, true, func); // Register the command with the messaging system

		return 0; // Return 0 on success
	}
	/*
	* console_add_command() - Add a function which will handle a certain command
	* ! When the function is called without a description, simply call it with an empty description
	* @command: the command name to handle
	* @func: the function to call that will handle the command
	*/
	int console_add_command(const std::string& command, std::function<void (std::shared_ptr<MessageContents>)> func) {
		return console_add_command(command, "", func);
	}
	/*
	* console_bind() - Bind a key to a console command
	* @key: the keycode to bind to
	* @command: the command to bind to
	*/
	std::string console_bind(SDL_Keycode key, const std::string& command) {
		if (trim(command).empty()) { // If the provided command is empty, then attempt to return the previously bound command
			if (engine->console->bindings.find(key) != engine->console->bindings.end()) { // If the key has been bound, then return its command
				return engine->console->bindings[key]; // Return the command on success when no command argument is provided
			}
			return ""; // Return an empty string when the provided command is empty and the key has not been bound
		}

		if (engine->console->bindings.find(key) == engine->console->bindings.end()) { // If the key has not been bound, then bind it to the given command
			engine->console->bindings.emplace(key, trim(command));
		} else { // Otherwise, output a warning
			messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to bind key \"" + keystrings_get_string(key) + "\", the key is already in bound.");
		}

		return trim(command); // Return the bound command on success
	}
	/*
	* console_bind() - Return the command that is bound to the given key
	* @key: the keycode to bind to
	*/
	std::string console_bind(SDL_Keycode key) {
		return console_bind(key, "");
	}
	/*
	* console_unbind() - Unbind a key from a console command
	* @key: the keycode to unbind
	*/
	int console_unbind(SDL_Keycode key) {
		if (engine->console->bindings.find(key) != engine->console->bindings.end()) { // If the key has been bound, then unbind it
			engine->console->bindings.erase(key);
		}
		return 0; // Return 0 on success
	}
	/*
	* console_unbind_all() - Unbind all keys from the console
	*/
	int console_unbind_all() {
		engine->console->bindings.clear();
		return 0;
	}
	/*
	* console_alias() - Add a console alias to multiple commands
	* @alias: the alias to use
	* @commands: the commands to run when the alias is called
	*/
	int console_alias(const std::string& alias, const std::string& commands) {
		if (engine->console->commands.find(alias) != engine->console->commands.end()) { // If a command already exists, then return a warning
			messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to add alias \"" + alias + "\", a command with the same name exists.");
			return 1;
		}

		engine->console->aliases[alias] = commands; // Set the alias

		return 0; // Return 0 on success
	}
	/*
	* console_get_aliases() - Return the map of all the current aliases
	*/
	const std::unordered_map<std::string,std::string>& console_get_aliases() {
		return engine->console->aliases;
	}
	/*
	* console_set_var() - Set a console variable
	* @name: the name of the variable
	* @value: the value to set
	*/
	int console_set_var(const std::string& name, SIDP value) {
		if (engine->console->commands.find(name) != engine->console->commands.end()) { // If a command already exists, then return a warning
			messenger_send({"engine", "console"}, E_MESSAGE::WARNING, "Failed to set variable \"" + name + "\", a command with the same name exists.");
			return 1;
		}

		engine->console->variables[name] = value;

		return 0;
	}
	/*
	* console_get_var() - Return the value of a console variable
	* @name: the name of the variable
	*/
	SIDP console_get_var(const std::string& name) {
		if (engine->console->variables.find(name) != engine->console->variables.end()) {
			return engine->console->variables[name];
		}
		return SIDP();
	}


	/*
	* console_run() - Run a command in the console
	* @command: the command string to run, including the arguments
	*/
	int console_run(const std::string& command) {
		return internal::console_run(command, false, 0); // Return the status of running a non-silent command with no requested delay
	}
	/*
	* console_get_help() - Return the description string of the given command
	* @command: the command to get the description for
	*/
	std::string console_get_help(const std::string& command) {
		if (engine->console->commands.find(command) == engine->console->commands.end()) { // If the command does not exist, then return a warning
			return "No available help for non-existent \"" + command + "\"";
		}

		std::string h = engine->console->commands[command].first; // Get the command description
		if (h.empty()) { // If the description is empty, then return a diagnostic message
			return "No available help for \"" + command + "\"";
		}

		return h; // Return the command's description on success
	}
}

#endif // BEE_CORE_CONSOLE
