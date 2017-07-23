/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_COMMANDS
#define BEE_CORE_COMMANDS 1

#include "console.hpp"

#include "../engine.hpp"

#include "../util/string.hpp"

#include "../messenger/messenger.hpp"

#include "../render/transition.hpp"

namespace bee{ namespace console {
	/*
	* internal::init_commands() - Initialize the default console commands
	*/
	int internal::init_commands() {
		// Below comments will use [optional] to specify certain argument rules
		/*
		* console_quit - End the game
		*/
		add_command(
			"quit",
			"End the game",
			[] (std::shared_ptr<MessageContents> msg) {
				messenger::send({"engine", "console"}, E_MESSAGE::INFO, "Quitting...");
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
		add_command(
			"help",
			"Show help text for specified commands",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a specific command was specified, output its entire help text
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, "Command help for \"" + params[1].s() + "\":\n" + get_help(params[1].s()));
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
						std::vector<SIDP> p = parse_parameters(c); // Get the list of command parameters in order to remove the usage string parts
						std::string h = get_help(p[0].s()); // Get the command help text

						if (!h.empty()) { // If the help text exists, output the command and its text
							help += c + "\n\t" + handle_newlines(h)[0] + "\n";
						}
					}
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, help); // Send the output to the console log
				}
			}
		);
		/*
		* console_find "search" - Output all commands which match a certain string
		* @"search": the string to search for
		*/
		add_command(
			"find",
			"Output all commands which match a certain string",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				std::string commands = "";
				std::string search = params[1].s();
				for (auto& c : internal::commands) {
					std::string h = get_help(c.first);
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

				messenger::send({"engine", "console"}, E_MESSAGE::INFO, commands); // Send the output to the console log
			}
		);
		/*
		* console_clear - Clear the console log
		*/
		add_command(
			"clear",
			"Clear the console log",
			[] (std::shared_ptr<MessageContents> msg) {
				// Clear the console log and reset the page index
				log.str(std::string());
				log.clear();
				page_index = 0;

				if (get_options().is_headless) {
					bee_commandline_clear();
				}
			}
		);
		/*
		* console_echo ["string"] - Output a string to the console
		* ! This is particularly useful for config scripts
		* @"string": the string that will be printed
		*/
		add_command(
			"echo",
			"Output a string to the console\n"
			"Primarily useful for config scripts",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				std::string output = ""; // Initialize a string to be used as output
				for (auto it=params.begin()+1; it!= params.end(); ++it) { // Iterate over the arguments
					output.append(string_unescape((*it).to_str())); // Append each argument to the output with a space in between
					output += " ";
				}
				if (!output.empty()) { // If the output isn't empty, send it to the console log
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, output);
				}
			}
		);

		/*
		* console_open - Open the console
		*/
		add_command(
			"console_open",
			"Open the console",
			[] (std::shared_ptr<MessageContents> msg) {
				open();
			}
		);
		/*
		* console_close - Close the console
		*/
		add_command(
			"console_close",
			"Close the console",
			[] (std::shared_ptr<MessageContents> msg) {
				bee::console::close();
			}
		);
		/*
		* console_toggle - Toggle the open status of the console
		*/
		add_command(
			"console_toggle",
			"Toggle the status of the console\n"
			"Also see console_open and _close.",
			[] (std::shared_ptr<MessageContents> msg) {
				toggle();
			}
		);

		/*
		* console_alias ["alias"] ["commands"] - Alias multiple commands to a single command
		* ! When the multiple commands are omitted, the currently aliased commands are output
		* ! When the alias command is omitted, all aliased commands are output
		* @"alias": the alias to use
		* @"commands": the commands to run when alias is called
		*/
		add_command(
			"alias",
			"Alias multiple commands to a single command\n"
			"The aliases can be viewed by omitting the commands",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) {
					alias(params[1].s(), string_unescape(params[2].s()));
				} else if (params.size() > 1) {
					auto a = get_aliases();
					if (a.find(params[1].s()) != a.end()) {
						messenger::send({"engine", "console"}, E_MESSAGE::INFO, a[params[1].s()]);
					} else {
						messenger::send({"engine", "console"}, E_MESSAGE::INFO, "No alias set for \"" + params[1].s() + "\"");
					}
				} else {
					auto amap = get_aliases();
					std::string aliases = "";
					for (auto& a : amap) {
						aliases += a.first + "\n\t" + a.second + "\n";
					}
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, aliases);
				}
			}
		);
		/*
		* console_bind "key" ["command"] - Bind a key to a command
		* ! When the command is omitted, the currently bound command is output
		* @"key": the key to bind to
		* @"command": the command to run when the given key is pressed
		*/
		add_command(
			"bind",
			"Bind a key to a command\n"
			"The binds can be viewed by omitting the commands",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) { // If both a key and command are provided, bind the command to the key
					bind(keystrings_get_key(params[1].s()), string_unescape(params[2].s()));
				} else if (params.size() > 1) { // If only a command is provided, output the command that is bound to it
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, bind(keystrings_get_key(params[1].s())));
				} else { // If no key is provided, output a warning
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "No key specified for binding");
				}
			}
		);
		/*
		* console_unbind "key" - Unbind a key from a command
		* @"key": the key to unbind
		*/
		add_command(
			"unbind",
			"Unbind a key from a command\n"
			"All keys can be unbound by provided \"all\" as the key",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a key is provided, unbind it from any commands
					if (params[1].s() == "all") {
						unbind_all();
					} else {
						unbind(keystrings_get_key(params[1].s()));
					}
				} else { // If no key is provided, output a warning
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "No key specified for unbinding");
				}
			}
		);
		/*
		* console_exec "filename.cfg" - Execute the specified config file
		* ! Note that no commands from a config file will be appended to the console history but will appear in messenger logging
		* @"filename.cfg": the config file to execute
		*/
		add_command(
			"exec",
			"Execute the specified config file\n"
			"No commands from the specified file will be apended to console history",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) { // If a filename is provided, execute it
					std::string fn = "cfg/"+params[1].to_str(); // Construct the path
					if (file_exists(fn)) { // Ensure that the file exists before opening it
						std::string cfg = file_get_contents(fn); // Fetch the file contents
						std::map<int,std::string> commands = handle_newlines(cfg); // Separate the contents by line
						for (auto& c : commands) { // Iterate over each line and run it
							if (!c.second.empty()) {
								run(c.second, true, 0);
							}
						}
					} else { // If the file cannot be opened, output a warning
						messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "The requested exec file \"" + fn + "\" does not exist");
					}
				} else { // If no filename is provided, output a warning
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "No config file specified for execution");
				}
			}
		);
		/*
		* console_wait delay "command" - Execute the specified command after a given delay
		* ! Note that the given delay is a minimum value to wait and the messaging system may take longer to process it
		* @delay: the amount of milliseconds to delay running the command
		* @"command": the command to run
		*/
		add_command(
			"wait",
			"Execute the specified command after the given millisecond delay\n"
			"The given value is the minimum time before the message will be processed",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) { // If both required arguments were provided, execute the command
					run(string_unescape(params[2].s()), true, params[1].i());
				} else if (params.size() > 1) { // If no command was provided, output a warning
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "No command specified for wait");
				} else { // If no arguments were provided, output a warning
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "No arguments specified for wait");
				}
			}
		);
		/*
		* console_unwait - Remove all wait commands from the messaging system, useful for breaking endless loops
		*/
		add_command(
			"unwait",
			"Remove all wait commands from the messaging system\n"
			"Useful for breaking endless loops",
			[] (std::shared_ptr<MessageContents> msg) {
				const std::vector<std::string> wait_tags = {"engine", "console", "wait"};
				messenger::internal::remove_messages([&wait_tags] (std::shared_ptr<MessageContents> m) {
					return (m->tags == wait_tags);
				});
			}
		);
		/*
		* console_set - Set a console variable to the given  value
		* @name: the name of the variable
		* @value: the value to set
		*/
		add_command(
			"set",
			"Set a console variable to the given value",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 2) {
					set_var(params[1].s(), params[2]);
				} else {
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Not enough arguments specified for set");
				}
			}
		);
		/*
		* console_get - Output the value of a console variable
		* @name: the name of the variable
		*/
		add_command(
			"get",
			"Output the value of a console variable",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) {
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, get_var(params[1].s()).to_str());
				} else {
					messenger::send({"engine", "console"}, E_MESSAGE::WARNING, "Not enough arguments specified for get");
				}
			}
		);

		/*
		* console_screenshot ["filename.bmp"] - Save a screenshot to the given file
		* @"filename.bmp": the filename to save to
		*/
		add_command(
			"screenshot",
			"Save a screenshot to the given file",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

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
		add_command(
			"debug",
			"Enable or disable debug mode",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) {
					GameOptions o = get_options();
					o.is_debug_enabled = params[1].i();
					set_options(o);
				} else {
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, std::to_string(get_options().is_debug_enabled));
				}
			}
		);

		/*
		* console_volume [value] - Set the global sound volume from 0.0 to 1.0
		* ! When the value is omitted, the current global volume is output
		* @value: the new volume level to use
		*/
		add_command(
			"volume",
			"Set the global sound volume from 0.0 to 1.0",
			[] (std::shared_ptr<MessageContents> msg) {
				std::vector<SIDP> params = parse_parameters(msg->descr); // Parse the parameters from the given command

				if (params.size() > 1) {
					set_volume(params[1].d());
				} else {
					messenger::send({"engine", "console"}, E_MESSAGE::INFO, std::to_string(get_volume()));
				}
			}
		);

		/*
		* console_restart - Restart the game
		*/
		add_command(
			"restart",
			"Restart the game",
			[] (std::shared_ptr<MessageContents> msg) {
				restart_game();
			}
		);
		/*
		* console_restart_room - Restart the current room
		*/
		add_command(
			"restart_room",
			"Restart the current room",
			[] (std::shared_ptr<MessageContents> msg) {
				restart_room();
			}
		);

		/*
		* console_info - Output information about the current room
		*/
		add_command(
			"info",
			"Output information about the current room",
			[] (std::shared_ptr<MessageContents> msg) {
				messenger::send({"engine", "console"}, E_MESSAGE::INFO, get_current_room()->get_print());
			}
		);

		/*
		* console_pause - Toggle the pause state of the game
		*/
		add_command(
			"pause",
			"Toggle the pause state of the game",
			[] (std::shared_ptr<MessageContents> msg) {
				set_is_paused(!get_is_paused());
			}
		);

		/*
		* console_netstatus - List information about the network session
		*/
		add_command(
			"netstatus",
			"List information about the network session",
			[] (std::shared_ptr<MessageContents> msg) {
				messenger::send({"engine", "console"}, E_MESSAGE::INFO, net::get_print());
			}
		);

		return 0; // Return 0 on success
	}
}}

#endif // BEE_CORE_COMMANDS
