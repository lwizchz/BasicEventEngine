/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_CONSOLE
#define _BEE_CORE_CONSOLE 1

#include "console.hpp"

/*
* BEE::Console::Console() - Initialize the console
*/
BEE::Console::Console() :
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
* BEE::Console::~Console() - Free the console log TextData
*/
BEE::Console::~Console() {
	if (td_log != nullptr){
		delete td_log;
		td_log = nullptr;
	}
}

/*
* BEE::console_handle_input() - Handle the input as a console keypress
* @e: the keyboard input event
*/
int BEE::console_handle_input(SDL_Event* e) {
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

	append_input(&console->input, &e->key); // Append the input to the console input line

	// Handle certain key presses in order to manipulate history or the command line
	switch (e->key.keysym.sym) {
		case SDLK_UP: { // The up arrow cycles through completion commands or reverse history
			if (console->completion_commands.size() > 1) { // If a command is being completed
				if (console->completion_index > 0) { // If a completion command is already selected, lower the index and set the input line to the given command
					console->completion_index = fit_bounds(console->completion_index-1, 0, (int)console->completion_commands.size()-1);
					console->input = (*(console->completion_commands.begin()+console->completion_index));
				} else { // If the first completion command is selected, reset the input line to the previous user input
					console->completion_index = -1;
					console->input = console->input_tmp;
				}
			} else { // If a command is not being completed, cycle through history
				if (console->history.size() > 0) { // If there is a history to look up
					console->history_index = fit_bounds(console->history_index+1, 0, (int)console->history.size()-1); // Prevent the index from going past the end
					// Replace the command line with the history item
					console->input.clear();
					console->input = (*(console->history.rbegin()+console->history_index));
				}
			}
			break;
		}
		case SDLK_DOWN: { // The down arrow cycles through the forward history
			if (console->completion_commands.size() > 1) { // If a command is being completed
				if (console->completion_index == -1) { // If not completion command has been selected, store the previous user input
					console->input_tmp = console->input;
				}
				// Raise the index and set the input line to the given command
				console->completion_index = fit_bounds(console->completion_index+1, 0, (int)console->completion_commands.size()-1);
				console->input = (*(console->completion_commands.begin()+console->completion_index));
			} else { // If a commandis not being completed, cycle through history
				if (console->history_index > 0) { // If the index is in previous history
					console->history_index = fit_bounds(console->history_index-1, 0, (int)console->history.size()-1); // Prevent the index from going past the front
					// Replace the command line with the history item
					console->input.clear();
					console->input = (*(console->history.rbegin()+console->history_index));
				} else { // If the index is new history
					console->history_index = -1; // Set the index appropriately
					console->input.clear(); // Clear the command line
				}
			}
			break;
		}

		case SDLK_PAGEUP: { // The pageup key scrolls backward through the console log
			if (handle_newlines(console->log.str()).size()/((console->h-30)/console->line_height + 1) > console->page_index) { // If the page index is lower than the full amount of pages in the log, increment the index
				++console->page_index;
			}
			break;
		}
		case SDLK_PAGEDOWN: { // The pagedown key scrolls forward through the console log
			if (console->page_index > 0) { // Limit the page index to greater than 0
				--console->page_index;
			}
			break;
		}

		case SDLK_TAB: { // The tab key attempts to complete commands from user input
			console->input.pop_back(); // Remove the tab character from the command line

			std::vector<SIDP> params = console_parse_parameters(console->input); // Parse the current parameters from the input line
			if (params.size() == 1) { // Complete the command if it's the only parameter
				console->completion_commands.clear(); // Clear the old completion commands
				console_complete(params[0].s()); // Find new completion comands
			} else { // TODO: Complete command arguments

			}

			break;
		}
		case SDLK_RETURN: { // The enter key runs the command currently in the input line
			console_run(console->input); // Run the command

			// Reset the console state
			console->input.clear();
			console->input_tmp.clear();
			console->completion_commands.clear();
			console->completion_index = -1;
			console->page_index = 0;

			break;
		}
		case SDLK_ESCAPE: { // The escape key clears the input line
			// Clear the input and completion states
			console->input.clear();
			console->input_tmp.clear();
			console->completion_commands.clear();
			console->completion_index = -1;
			break;
		}

		case SDLK_BACKQUOTE: { // The tilde key toggles the console open state
			// TODO: allow different keys to open the console
			console->input.pop_back(); // Remove the backquote from input
			console_toggle(); // Toggle the console
			break;
		}

		default: { // Handle all other keys
			if (console->input.empty()) { // If the input line is now empty (i.e. backspace), clear the completion commands
				console->completion_commands.clear();
			}

			if (console->completion_index > -1) { // Clear the completion commands when additional input is received
				console->completion_index = -1;
				console->input_tmp.clear();
				console->completion_commands.clear();
			}

			std::vector<SIDP> params = console_parse_parameters(console->input); // Parse the current parameters from the input line
			if (params.size() == 1) { // If the command is the only parameter
				if (console->completion_commands.size() > 1) { // If a command is being completed, update the completion list for the new input
					console->completion_commands.clear();
					console_complete(params[0].s());
				}
			}
		}
	}

	return 0; // Return 0 on successful handling of console input
}
/*
* BEE::console_init_commands() - Initialize the default console commands
*/
int BEE::console_init_commands() {
	console->line_height = font_default->get_string_height(); // Store the default drawing line height for later console drawing operations

	// Register the console logger
	messenger_register_protected("consolelog", {"engine", "console"}, true, [this] (BEE* g, std::shared_ptr<MessageContents> msg) {
		this->console->log << msg->descr << "\n";
	});

	// Below comments will use [optional] to specify certain argument rules
	/*
	* console_quit - End the game
	*/
	console_add_command(
		"quit",
		"End the game",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, "Quitting...");
			g->set_transition_type(bee::E_TRANSITION::NONE);
			g->end_game();
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) { // If a specific command was specified, output its entire help text
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, "Command help for \"" + params[1].s() + "\":\n" + g->console_get_help(params[1].s()));
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
					std::vector<SIDP> p = g->console_parse_parameters(c); // Get the list of command parameters in order to remove the usage string parts
					std::string h = g->console_get_help(p[0].s()); // Get the command help text

					if (!h.empty()) { // If the help text exists, output the command and its text
						help += c + "\n\t" + handle_newlines(h)[0] + "\n";
					}
				}
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, help); // Send the output to the console log
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
		[this] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			std::string commands = "";
			std::string search = params[1].s();
			for (auto& c : this->console->commands) {
				std::string h = g->console_get_help(c.first);
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

			g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, commands); // Send the output to the console log
		}
	);
	/*
	* console_clear - Clear the console log
	*/
	console_add_command(
		"clear",
		"Clear the console log",
		[this] (BEE* g, std::shared_ptr<MessageContents> msg) {
			// Clear the console log and reset the page index
			this->console->log.str(std::string());
			this->console->log.clear();
			this->console->page_index = 0;
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			std::string output = ""; // Initialize a string to be used as output
			for (auto it=params.begin()+1; it!= params.end(); ++it) { // Iterate over the arguments
				output.append(string_unescape((*it).to_str())); // Append each argument to the output with a space in between
				output += " ";
			}
			if (!output.empty()) { // If the output isn't empty, send it to the console log
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, output);
			}
		}
	);

	/*
	* console_open - Open the console
	*/
	console_add_command(
		"console_open",
		"Open the console",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->console_open();
		}
	);
	/*
	* console_close - Close the console
	*/
	console_add_command(
		"console_close",
		"Close the console",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->console_close();
		}
	);
	/*
	* console_toggle - Toggle the open status of the console
	*/
	console_add_command(
		"console_toggle",
		"Toggle the status of the console\n"
		"Also see console_open and _close.",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->console_toggle();
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 2) {
				g->console_alias(params[1].s(), string_unescape(params[2].s()));
			} else if (params.size() > 1) {
				auto a = g->console_get_aliases();
				if (a.find(params[1].s()) != a.end()) {
					g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, a[params[1].s()]);
				} else {
					g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, "No alias set for \"" + params[1].s() + "\"");
				}
			} else {
				auto amap = g->console_get_aliases();
				std::string aliases = "";
				for (auto& a : amap) {
					aliases += a.first + "\n\t" + a.second + "\n";
				}
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, aliases);
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 2) { // If both a key and command are provided, bind the command to the key
				g->console_bind(g->keystrings_get_key(params[1].s()), string_unescape(params[2].s()));
			} else if (params.size() > 1) { // If only a command is provided, output the command that is bound to it
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, g->console_bind(g->keystrings_get_key(params[1].s())));
			} else { // If no key is provided, output a warning
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "No key specified for binding");
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) { // If a key is provided, unbind it from any commands
				if (params[1].s() == "all") {
					g->console_unbind_all();
				} else {
					g->console_unbind(g->keystrings_get_key(params[1].s()));
				}
			} else { // If no key is provided, output a warning
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "No key specified for unbinding");
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
		[this] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) { // If a filename is provided, execute it
				std::string fn = "cfg/"+params[1].to_str(); // Construct the path
				if (file_exists(fn)) { // Ensure that the file exists before opening it
					std::string cfg = file_get_contents(fn); // Fetch the file contents
					std::map<int,std::string> commands = handle_newlines(cfg); // Separate the contents by line
					for (auto& c : commands) { // Iterate over each line and run it
						if (!c.second.empty()) {
							this->console_run(c.second, true, 0);
						}
					}
				} else { // If the file cannot be opened, output a warning
					g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "The requested exec file \"" + fn + "\" does not exist");
				}
			} else { // If no filename is provided, output a warning
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "No config file specified for execution");
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
		[this] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 2) { // If both required arguments were provided, execute the command
				this->console_run(string_unescape(params[2].s()), true, params[1].i());
			} else if (params.size() > 1) { // If no command was provided, output a warning
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "No command specified for wait");
			} else { // If no arguments were provided, output a warning
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "No arguments specified for wait");
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
		[this] (BEE* g, std::shared_ptr<MessageContents> msg) {
			const std::vector<std::string> wait_tags = {"engine", "console", "wait"};
			messages.erase(std::remove_if(messages.begin(), messages.end(), [&wait_tags] (std::shared_ptr<MessageContents> msg) {
				return (msg->tags == wait_tags);
			}), messages.end());
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 2) {
				g->console_set_var(params[1].s(), params[2]);
			} else {
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Not enough arguments specified for set");
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) {
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, g->console_get_var(params[1].s()).to_str());
			} else {
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Not enough arguments specified for get");
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) { // If a filename is provided, then save the screenshot to it
				g->save_screenshot(params[1].to_str());
			} else { // If no filename is provided, then save the screenshot to the default path
				g->save_screenshot("screenshot.bmp");
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) {
				GameOptions o = g->get_options();
				o.is_debug_enabled = params[1].i();
				g->set_options(o);
			} else {
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, std::to_string(g->get_options().is_debug_enabled));
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
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			std::vector<SIDP> params = g->console_parse_parameters(msg->descr); // Parse the parameters from the given command

			if (params.size() > 1) {
				g->set_volume(params[1].d());
			} else {
				g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, std::to_string(g->get_volume()));
			}
		}
	);

	/*
	* console_restart - Restart the game
	*/
	console_add_command(
		"restart",
		"Restart the game",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->restart_game();
		}
	);
	/*
	* console_restart_room - Restart the current room
	*/
	console_add_command(
		"restart_room",
		"Restart the current room",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->restart_room();
		}
	);

	/*
	* console_info - Output information about the current room
	*/
	console_add_command(
		"info",
		"Output information about the current room",
		[] (BEE* g, std::shared_ptr<MessageContents> msg) {
			g->messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, g->get_current_room()->get_print());
		}
	);

	console_run("exec \"config.cfg\"", true, 0); // Configure default binds

	return 0; // Return 0 on success
}
/*
* BEE::console_run_internal() - Run a command in the console, silently or urgently if requested
* @command: the command string to run, including the arguments
* @is_urgent: whether to Immediately send the command or to wait until the end of the frame
* @delay: the amount of milliseconds to delay command execution
*/
int BEE::console_run_internal(const std::string& command, bool is_urgent, Uint32 delay) {
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

	if (console->commands.find(params[0].s()) == console->commands.end()) { // If the command or alias does not exist, then output a warning
		if (console->aliases.find(params[0].s()) == console->aliases.end()) {
			messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Failed to run command \"" + params[0].s() + "\", the command does not exist.");
			return 2; // Return 2 on non-existent command
		} else { // Otherwise if the alias exists, run it
			return console_run(console->aliases[params[0].s()], true, delay);
		}
	}

	if (is_urgent) { // If the command is urgent, send it immediately
		messenger_send_urgent(std::shared_ptr<MessageContents>(new MessageContents(
			get_ticks()+delay,
			{"engine", "console", params[0].s()},
			bee::E_MESSAGE::GENERAL,
			c,
			nullptr
		)));
	} else { // Otherwise, send it normally
		messenger_send(std::shared_ptr<MessageContents>(new MessageContents(
			get_ticks()+delay,
			{"engine", "console", params[0].s()},
			bee::E_MESSAGE::GENERAL,
			c,
			nullptr
		)));
	}

	return 0; // Return 0 on success
}
/*
* BEE::console_run() - Run a command in the console
* @command: the command string to run, including the arguments
* @is_silent: whether to append the command to history and display it in console
* @delay: the amount of milliseconds to delay command execution
*/
int BEE::console_run(const std::string& command, bool is_silent, Uint32 delay) {
	if (!is_silent) { // If the command is not silent, then log it
		if (command[0] != ' ') { // If the first character is not a space, then add it to the console history
			// Fetch the last command if one exists
			std::string last_command = "";
			if (!console->history.empty()) {
				last_command = console->history.back();
			}

			if (command != last_command) { // If the current command is not identical to the previous command, then add it to the console history
				console->history.push_back(command);
			}
		}
		messenger_send({"engine", "console"}, bee::E_MESSAGE::INFO, "> " + trim(command)); // Output the command to the messenger log
	}
	console->history_index = -1; // Reset the history index

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
* BEE::console_complete() - Complete console commands
* @command: the command to complete
*/
int BEE::console_complete(const std::string& command) {
	// Find any command matches
	for (auto& c : console->commands) { // Iterate over the possible commands
		if (c.first.find(command) == 0) { // If the given command begins with the completion string
			console->completion_commands.push_back(c.first); // Add the given command to the completion list
		}
	}
	if (console->completion_commands.size() == 0) { // If no matches were found, then reset the completion state
		console->completion_index = -1;
		console->input_tmp.clear();

		return 1; // Return 1 on matching failure
	} else if (console->completion_commands.size() == 1) { // If there was only a single match, then use it and reset the completion state
		console->input = console->completion_commands[0] + " "; // Replace the input with the full matched command

		// Reset the completion state
		console->completion_index = -1;
		console->completion_commands.clear();
		console->input_tmp.clear();

		return 0; // Return 0 on match success to exit early
	}

	// Attempt partial command completion if all completion commands share a common root
	// ! Note that by definition, all completion commands will share at least as many characters as the length of console->input
	std::string matched = ""; // Define a string that contains the matched portion of multiple commands
	bool is_matched = true;
	for (size_t i=console->input.length(); i<console->completion_commands[0].length(); ++i) { // Iterate over the characters of the first completion command, beginning at the first character that might not match
		for (size_t j=1; j<console->completion_commands.size(); ++j) { // Iterate over the other completion commands
			if (console->completion_commands[0][i] != console->completion_commands[j][i]) { // If the given character does not match each completion command, then break
				is_matched = false;
				break;
			}
		}

		if (is_matched) { // If the character matched, append it to the match string
			matched += console->completion_commands[0][i];
		} else { // Otherwise, break;
			break;
		}
	}
	console->input += matched; // Replace the input with the matched portion of a command

	std::sort(console->completion_commands.begin(), console->completion_commands.end()); // Sort the completion commands alphabetically

	return 0; // Return 0 on success
}
/*
* BEE::console_parse_parameters() - Convert the command to a parameter list
* @command: the full command string
*/
std::vector<BEE::SIDP> BEE::console_parse_parameters(const std::string& command) const {
	std::map<int,std::string> params = split(command, ' ', true); // Split the command parameters by spaces, respecting quotes

	std::vector<SIDP> param_list; // Create a vector to store each parameter instead of the map from split()
	for (auto& p : params) { // Iterate over each parameter and store it as a SIDP interpreted type
		param_list.push_back(SIDP(p.second, true));
	}

	return param_list; // Return the vector of parameters on success
}
/*
* BEE::console_draw() - Draw the console and its output
*/
int BEE::console_draw() {
	// Get the view offset
	int cx = console->x;
	int cy = console->y;
	if (get_current_room()->get_current_view() != nullptr) {
		cx -= get_current_room()->get_current_view()->view_x;
		cy -= get_current_room()->get_current_view()->view_y;
	}

	// Set the drawing sizes
	unsigned int input_line_y = console->h - 30;

	// Draw console rectangle and input line
	draw_rectangle(cx, cy, console->w, console->h, true, {127, 127, 127, 225});
	draw_line(cx, cy + console->h - console->line_height, cx + console->w, cy + console->h - console->line_height, bee::E_RGB::BLACK);

	// Remove the top of the console log if it doesn't fit
	size_t line_amount = input_line_y/console->line_height; // Calculate the total lines that can be stored in the console window
	std::vector<std::string> lines = splitv(console->log.str(), '\n', false); // Separate the console log by each newline
	size_t total_lines = lines.size(); // Store the total line number for the below page number calculation

	// Split lines if they are wider than the console window
	for (auto it=lines.begin(); it!=lines.end(); ++it) {
		if ((unsigned int)font_default->get_string_width(*it) > console->w) {
			int c = 1.5*console->w/font_default->get_font_size()-1;

			lines.emplace(it+1, (*it).substr(c));
			*it = (*it).substr(0, c);

			++total_lines;
		}
	}

	if (total_lines > line_amount) { // If there are more lines than can fit in the console, then remove the ones which shouldn't be rendered
		if (lines.size() > line_amount + console->page_index*(line_amount+1) + 1) { // If the console page has non-rendered lines before it, then remove them
			const size_t size_goal = lines.size() - line_amount - console->page_index*(line_amount+1) - 1;

			std::vector<std::string>::iterator it = lines.begin();
			for (size_t i=0; i<size_goal; ++i) {
				++it;
			}

			lines.erase(lines.begin(), it);
		}

		if (console->page_index > 0) { // If the console page has non-rendered lines after it, then remove them
			const size_t size_goal = lines.size() - line_amount - (console->page_index-1)*(line_amount+1) - 1;

			std::vector<std::string>::iterator it = lines.begin();
			for (size_t i=0; i<size_goal; ++i) {
				++it;
			}

			lines.erase(it, lines.end());
		}
	}

	std::string short_log = joinv(lines, '\n'); // Create a shorter log from the cropped log
	if ((console->page_index > 0)&&(lines.size() < line_amount)) { // If the console page has less lines than are renderable (e.g. it's the first page), then prepend extra newlines
		short_log = string_repeat(line_amount - lines.size() + 1, "\n") + short_log;
	}
	console->td_log = font_default->draw(console->td_log, cx, cy, short_log, {0, 0, 0, 255}); // Draw the console log

	// Define several drawing colors
	RGBA c_text = get_enum_color(bee::E_RGB::BLACK);
	RGBA c_back = {127, 127, 127, 127};

	// Draw the console page number
	std::string p = std::to_string(console->page_index+1) + "/" + std::to_string(total_lines/line_amount+1);
	font_default->draw_fast(cx + console->w - 10 * p.length(), cy + console->h - console->line_height*2, p, c_text);

	// Draw the console input and a blinking cursor
	font_default->draw_fast(cx, cy + console->h - console->line_height, console->input, c_text); // Draw the console input
	if (get_ticks()/500 % 2) { // Draw a blinking cursor that changes every 500 ticks
		font_default->draw_fast(cx + font_default->get_string_width(console->input), cy + console->h - console->line_height, "_", c_text);
	}

	// Draw any completion commands in a box below the input line
	if (console->completion_commands.size() > 1) { // If completion commands exist, draw them
		draw_rectangle(cx, cy + console->h, console->w, console->completion_commands.size()*console->line_height, true, c_back); // Draw a box to contain the commands
		for (size_t i=0; i<console->completion_commands.size(); ++i) { // Iterate over the completion commands
			std::string cmd = " " + console->completion_commands[i]; // Prepend each command with a space
			if (i == (size_t)console->completion_index) { // If the command is selected, replace the space with a cursor
				cmd[0] = '>';
			}
			font_default->draw_fast(cx, cy + console->h + console->line_height*i, cmd, c_text); // Draw the console completion command
		}
	}

	return 0; // Return 0 on success
}

/*
* BEE::console_open() - Open the console
*/
int BEE::console_open() {
	console->is_open = true;
	return 0;
}
/*
* BEE::console_close() - Close the console
*/
int BEE::console_close() {
	console->is_open = false;
	return 0;
}
/*
* BEE::console_toggle() - Toggle the open/close state of the console
*/
int BEE::console_toggle() {
	console->is_open = !console->is_open;
	return 0;
}
/*
* BEE::console_get_is_open() - Return whether the console is open or not
*/
bool BEE::console_get_is_open() const {
	return console->is_open;
}

/*
* BEE::console_add_command() - Add a function which will handle a certain command
* @command: the command name to handle
* @descr: the command description that will be displayed when the user runs `help command_name` in this console subsystem
* @func: the function to call that will handle the command
*/
int BEE::console_add_command(const std::string& command, const std::string& descr, std::function<void (BEE*, std::shared_ptr<MessageContents>)> func) {
	if (console->commands.find(command) != console->commands.end()) { // If the command already exists, then output a warning
		messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Failed to register new command \"" + command + "\", the command name is already in use.");
		return 1; // Return 1 on command existence
	}

	console->commands.emplace(command, std::make_pair(descr, func)); // Add the command to the console command map

	messenger_register_protected("console_"+command, {"engine", "console", command}, true, func); // Register the command with the messaging system

	return 0; // Return 0 on success
}
/*
* BEE::console_add_command() - Add a function which will handle a certain command
* ! When the function is called without a description, simply call it with an empty description
* @command: the command name to handle
* @func: the function to call that will handle the command
*/
int BEE::console_add_command(const std::string& command, std::function<void (BEE*, std::shared_ptr<MessageContents>)> func) {
	return console_add_command(command, "", func);
}
/*
* BEE::console_bind() - Bind a key to a console command
* @key: the keycode to bind to
* @command: the command to bind to
*/
std::string BEE::console_bind(SDL_Keycode key, const std::string& command) {
	if (trim(command).empty()) { // If the provided command is empty, then attempt to return the previously bound command
		if (console->bindings.find(key) != console->bindings.end()) { // If the key has been bound, then return its command
			return console->bindings[key]; // Return the command on success when no command argument is provided
		}
		return ""; // Return an empty string when the provided command is empty and the key has not been bound
	}

	if (console->bindings.find(key) == console->bindings.end()) { // If the key has not been bound, then bind it to the given command
		console->bindings.emplace(key, trim(command));
	} else { // Otherwise, output a warning
		messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Failed to bind key \"" + keystrings_get_string(key) + "\", the key is already in bound.");
	}

	return trim(command); // Return the bound command on success
}
/*
* BEE::console_bind() - Return the command that is bound to the given key
* @key: the keycode to bind to
*/
std::string BEE::console_bind(SDL_Keycode key) {
	return console_bind(key, "");
}
/*
* BEE::console_unbind() - Unbind a key from a console command
* @key: the keycode to unbind
*/
int BEE::console_unbind(SDL_Keycode key) {
	if (console->bindings.find(key) != console->bindings.end()) { // If the key has been bound, then unbind it
		console->bindings.erase(key);
	}
	return 0; // Return 0 on success
}
/*
* BEE::console_unbind_all() - Unbind all keys from the console
*/
int BEE::console_unbind_all() {
	console->bindings.clear();
	return 0;
}
/*
* BEE::console_alias() - Add a console alias to multiple commands
* @alias: the alias to use
* @commands: the commands to run when the alias is called
*/
int BEE::console_alias(const std::string& alias, const std::string& commands) {
	if (console->commands.find(alias) != console->commands.end()) { // If a command already exists, then return a warning
		messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Failed to add alias \"" + alias + "\", a command with the same name exists.");
		return 1;
	}

	console->aliases[alias] = commands; // Set the alias

	return 0; // Return 0 on success
}
/*
* BEE::console_get_aliases() - Return the map of all the current aliases
*/
const std::unordered_map<std::string,std::string>& BEE::console_get_aliases() const {
	return console->aliases;
}
/*
* BEE::console_set_var() - Set a console variable
* @name: the name of the variable
* @value: the value to set
*/
int BEE::console_set_var(const std::string& name, SIDP value) {
	if (console->commands.find(name) != console->commands.end()) { // If a command already exists, then return a warning
		messenger_send({"engine", "console"}, bee::E_MESSAGE::WARNING, "Failed to set variable \"" + name + "\", a command with the same name exists.");
		return 1;
	}

	console->variables[name] = value;

	return 0;
}
/*
* BEE::console_get_var() - Return the value of a console variable
* @name: the name of the variable
*/
BEE::SIDP BEE::console_get_var(const std::string& name) const {
	if (console->variables.find(name) != console->variables.end()) {
		return console->variables[name];
	}
	return SIDP();
}


/*
* BEE::console_run() - Run a command in the console
* @command: the command string to run, including the arguments
*/
int BEE::console_run(const std::string& command) {
	return console_run(command, false, 0); // Return the status of running a non-silent command with no requested delay
}
/*
* BEE::console_get_help() - Return the description string of the given command
* @command: the command to get the description for
*/
std::string BEE::console_get_help(const std::string& command) const {
	if (console->commands.find(command) == console->commands.end()) { // If the command does not exist, then return a warning
		return "No available help for non-existent \"" + command + "\"";
	}

	std::string h = console->commands[command].first; // Get the command description
	if (h.empty()) { // If the description is empty, then return a diagnostic message
		return "No available help for \"" + command + "\"";
	}

	return h; // Return the command's description on success
}

#endif // _BEE_CORE_CONSOLE
