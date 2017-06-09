/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSENGER
#define BEE_CORE_MESSENGER 1

#include <iostream>
#include <algorithm>

#include "../../engine.hpp" // Include the engine headers

#include "../../debug.hpp"

#include "../../util/string.hpp"
#include "../../util/platform.hpp"

#include "messagerecipient.hpp"
#include "messagecontents.hpp"

#include "../enginestate.hpp"

namespace bee {
	/*
	* messenger_register_protected() - Register the given recipient with protected tags within the messaging system
	* @recv: the recipient to register
	*/
	int messenger_register_protected(std::shared_ptr<MessageRecipient> recv) {
		for (auto& tag : recv->tags) { // Iterate over the requested tags
			if (engine->recipients.find(tag) == engine->recipients.end()) { // If the tag doesn't exist, then create it
				engine->recipients.emplace(tag, std::unordered_set<std::shared_ptr<MessageRecipient>>()); // Emplace an empty set of recipients
			}
			engine->recipients[tag].insert(recv); // Add the recipient to the list
		}
		return 0; // Return 0 on success
	}
	/*
	* messenger_register_protected() - Register the given recipient within the messaging system
	* @name: the name of the funciton
	* @tags: the tags to register the recipient with
	* @is_strict: whether the recipient should only take exact tags
	* @func: the function to use to handle the messages
	*/
	std::shared_ptr<MessageRecipient> messenger_register_protected(std::string name, const std::vector<std::string>& tags, bool is_strict, std::function<void (std::shared_ptr<MessageContents>)> func) {
		std::shared_ptr<MessageRecipient> recv (new MessageRecipient(name, tags, is_strict, func)); // Create a pointer for the given message data
		messenger_register_protected(recv); // Attempt to register the recipient
		return recv; // Return the newly constructed recipient
	}
	/*
	* messenger_unregister_protected() - Unregister the given recipient from protected tags within the messaging system
	* @recv: the recipient to unregister
	*/
	int messenger_unregister_protected(std::shared_ptr<MessageRecipient> recv) {
		for (auto& tag : recv->tags) { // Iterate over the recipient's tags
			auto rt = engine->recipients.find(tag);
			if (rt != engine->recipients.end()) { // If the tag exists within the recipient list
				if (rt->second.find(recv) != rt->second.end()) { // If the recipient exists within the list
					rt->second.erase(recv); // Remove the recipient
					if (rt->second.empty()) { // If the tag has no other recipients, remove it
						engine->recipients.erase(tag);
					}
				}
			}
		}
		return 0; // Return 0 on success
	}
	/*
	* messenger_send_urgent() - Immediately process the given message to its recipients
	* ! Warning: this can break things so use sparingly, most things can wait a couple milliseconds for handle_messages() to process at the end of each frame
	* @msg: the message to process
	*/
	int messenger_send_urgent(std::shared_ptr<MessageContents> msg) {
		const Uint32 t = get_ticks();
		msg->descr = trim(msg->descr); // Trim the message description

		bool should_output = true;
		switch (messenger_get_level()) { // Skip certain messages depending on the verbosity level
			case E_OUTPUT::NONE: { // When the verbosity is NONE, skip all message types
				should_output = false;
				break;
			}
			case E_OUTPUT::QUIET: { // When the verbosity is QUIET, skip all types except warnings and errors
				if (
					(msg->type != E_MESSAGE::WARNING)
					|| (msg->type != E_MESSAGE::ERROR)
				) {
					should_output = false;
				}
				break;
			}
			case E_OUTPUT::NORMAL: // When the verbosity is NORMAL, skip internal messages
			default: {
				if (msg->type == E_MESSAGE::INTERNAL) {
					should_output = false;
				}
				break;
			}
			case E_OUTPUT::VERBOSE: { // When the verbosity is VERBOSE, skip no message types
				break;
			}
		}

		if (should_output) {
			// Create a string of the message's tags
			std::string tags = joinv(msg->tags, ',');

			// Change the output color depending on the message type
			if (msg->type == E_MESSAGE::WARNING) {
				bee_commandline_color(11); // Yellow
			}
			if (msg->type == E_MESSAGE::ERROR) {
				bee_commandline_color(9); // Red
			}

			// Output to the appropriate stream
			std::ostream* o = &std::cout;
			if ((msg->type == E_MESSAGE::WARNING)||(msg->type == E_MESSAGE::ERROR)) {
				o = &std::cerr;
			}

			// Output the message metadata
			*o << "MSG (" << t << "ms)[" << messenger_get_type_string(msg->type) << "]<" << tags << ">: ";

			// Output the message description
			if (msg->descr.find("\n") != std::string::npos) { // If the description is multiple liness, indent it as necessary
				*o << "\n";
				*o << debug_indent(msg->descr, 1);
			} else { // Otherwise, output it as normal
				*o << msg->descr << "\n";
			}

			bee_commandline_color_reset(); // Reset the output color
			std::flush(std::cout); // Flush the output buffer before processing the recipients
		}

		std::exception_ptr ep; // Store any thrown values
		for (auto& tag : msg->tags) { // Iterate over the message tags
			auto rt = engine->recipients.find(tag); // Make sure that the message tag exists
			if (rt != engine->recipients.end()) {
				for (auto& recv : engine->recipients[tag]) { // Iterate over the recipients who wish to process the tag
					if (recv->is_strict) { // If the recipient is strict and the tags don't match, skip it
						if (recv->tags != msg->tags) {
							continue;
						}
					}

					// Call the recipient function
					if (recv->func != nullptr) {
						try {
							recv->func(msg);
						} catch (int e) { // Catch several kinds of exceptions that the recipient might throw
							ep = std::current_exception();
							bee_commandline_color(9);
							std::cerr << "MSG ERR (" << t << "ms): exception " << e << " thrown by recipient \"" << recv->name << "\"\n";
							bee_commandline_color_reset();
						} catch (const char* e) {
							ep = std::current_exception();
							bee_commandline_color(9);
							std::cerr << "MSG ERR (" << t << "ms): exception \"" << e << "\" thrown by recipient \"" << recv->name << "\"\n";
							bee_commandline_color_reset();
						} catch (...) {
							ep = std::current_exception(); // Store any miscellaneous exceptions to be rethrown
						}
					}
				}
			}
		}

		if (ep != nullptr) { // If an exception was thrown, throw it after finishing processing the message
			std::flush(std::cout);
			std::rethrow_exception(ep);
		}

		return 0; // Return 0 on success
	}

	/*
	* messenger_register() - Register the given recipient within the messaging system
	* @recv: the recipient to register
	*/
	int messenger_register(std::shared_ptr<MessageRecipient> recv) {
		int r = 0; // Store any attempts to register a protected tag

		for (auto& tag : recv->tags) { // Iterate over the requested tags
			if (engine->protected_tags.find(tag) != engine->protected_tags.end()) { // If the requested tag is protected, deny registration
				// Output an error message
				bee_commandline_color(11);
				std::cerr << "MSG failed to register recipient \"" << recv->name << "\" with protected tag \"" << tag << "\".\n";
				bee_commandline_color_reset();

				r++; // Increment the protection counter

				continue; // Skip to the next tag
			}

			if (engine->recipients.find(tag) == engine->recipients.end()) { // If the tag doesn't exist, then create it
				engine->recipients.emplace(tag, std::unordered_set<std::shared_ptr<MessageRecipient>>()); // Emplace an empty set of recipients
			}
			engine->recipients[tag].insert(recv); // Add the recipient to the list
		}

		return r; // Return the amount of attempts to register a protected tag
	}
	/*
	* messenger_register() - Register the given recipient within the messaging system
	* @name: the name of the funciton
	* @tags: the tags to register the recipient with
	* @is_strict: whether the recipient should only take exact tags
	* @func: the function to use to handle the messages
	*/
	std::shared_ptr<MessageRecipient> messenger_register(std::string name, const std::vector<std::string>& tags, bool is_strict, std::function<void (std::shared_ptr<MessageContents>)> func) {
		std::shared_ptr<MessageRecipient> recv (new MessageRecipient(name, tags, is_strict, func)); // Create a pointer for the given message data
		messenger_register(recv); // Attempt to register the recipient
		return recv; // Return the newly constructed recipient
	}
	/*
	* messenger_register() - Register the given recipient within the messaging system
	* ! When the function is called without a name and strictness, simply call it with an empty name and unstrictness
	* @tags: the tags to register the recipient with
	* @func: the function to use to handle the messages
	*/
	std::shared_ptr<MessageRecipient> messenger_register(const std::vector<std::string>& tags, std::function<void (std::shared_ptr<MessageContents>)> func) {
		return messenger_register("", tags, false, func);
	}
	/*
	* messenger_unregister() - Unregister the given recipient within the messaging system
	* @recv: the recipient to unregister
	*/
	int messenger_unregister(std::shared_ptr<MessageRecipient> recv) {
		for (auto& tag : recv->tags) { // Iterate over the recipient's tags
			if (engine->protected_tags.find(tag) != engine->protected_tags.end()) { // If the specific tag is protected, deny removal for any tags
				// Output an error message
				bee_commandline_color(11);
				std::cerr << "MSG failed to unregister recipient \"" << recv->name << "\" because of protected tag \"" << tag << "\".\n";
				bee_commandline_color_reset();

				return 1; // Return 1 on denial by protected tag
			}

			auto rt = engine->recipients.find(tag);
			if (rt != engine->recipients.end()) { // If the tag exists within the recipient list
				if (rt->second.find(recv) != rt->second.end()) { // If the recipient exists within the list
					rt->second.erase(recv); // Remove the recipient
					if (rt->second.empty()) { // If the tag has no other recipients, remove it
						engine->recipients.erase(tag);
					}
				}
			}
		}
		return 0; // Return 0 on success
	}
	/*
	* messenger_unregister_name() - Unregister the recipient with the given name within the messaging system
	* @name: the name of the recipient to unregister
	*/
	int messenger_unregister_name(const std::string& name) {
		std::shared_ptr<MessageRecipient> recv (nullptr);

		for (auto& tag : engine->recipients) { // Iterate over all tags
			for (auto& r : tag.second) { // Iterate over the recipients for a specific tag
				if (r->name == name) { // If the specific recipient matches the desired recipient's name
					recv = r; // Assign the recipient and break out
					break;
				}
			}

			if (recv != nullptr) { // If the recipient has been found, break out
				break;
			}
		}

		return messenger_unregister(recv); // Return the attempt to remove the recipient
	}
	/*
	* messenger_unregister_all() - Unregister all messaging system recipients
	* ! Note that this function is not able to protect functions which register to additional tags such as console commands
	*   e.g. {"engine", "console", "help"} will be reduced to {"engine", "console"}
	*   This function may be protected in the future
	*/
	int messenger_unregister_all() {
		int r = 0; // Store any attempts to unregister a protected tag

		for (auto& tag : engine->recipients) { // Iterate over all recipient tags
			if (engine->protected_tags.find(tag.first) != engine->protected_tags.end()) { // If the specific tag is protected, deny removal
				r++; // Increment the protection counter
				continue; // Skip to the next tag
			}

			tag.second.clear(); // Remove all recipients who are registered with this tag
		}

		// Remove all non-protected tags
		for (auto tag=engine->recipients.begin(); tag!=engine->recipients.end();) {
			if (engine->protected_tags.find((*tag).first) == engine->protected_tags.end()) { // If the tag is not protected, remove it
				tag = engine->recipients.erase(tag);
			} else { // It the tag is protected, skip it
				++tag;
			}
		}

		return r; // Return the amount of attempts to unregister a protected tag
	}

	/*
	* messenger_send() - Queue the given message in the messaging system
	* @m: the message to queue
	*/
	int messenger_send(std::shared_ptr<MessageContents> msg) {
		if (msg == nullptr) {
			return 1; // Return 1 when passed a null message
		}

		msg->descr = trim(msg->descr); // Trim the message description
		engine->messages.push_back(msg); // Add the message to the list

		return 0; // Return 0 on success
	}
	/*
	* messenger_send() - Queue the given message in the messaging system
	* ! When the function is called with separate message contents, create a pointer and call it again
	* @tickstamp: the tickstamp of the given message, normally should be the current tick
	* @tags: the list of tags that the message should be sent to
	* @descr: the message description
	* @data: the message data
	*/
	//int messenger_send(const std::vector<std::string>& tags, E_MESSAGE type, const std::string& descr, void* data) {
	int messenger_send(const std::vector<std::string>& tags, E_MESSAGE type, const std::string& descr, std::shared_ptr<void> data) {
		std::shared_ptr<MessageContents> msg (new MessageContents(get_ticks(), tags, type, descr, data)); // Create a pointer for the given message data
		return messenger_send(msg); // Return the attempt at message sending
	}
	/*
	* messenger_send() - Queue the given message in the messaging system
	* ! When no data has been provided, call the function again with it set to nullptr
	* @tickstamp: the tickstamp of the given message, normally should be the current tick
	* @tags: the list of tags that the message should be sent to
	* @descr: the message description
	*/
	int messenger_send(const std::vector<std::string>& tags, E_MESSAGE type, const std::string& descr) {
		return messenger_send(tags, type, descr, nullptr);
	}

	/*
	* messenger_set_level() - Set the output level when printing message descriptions
	* @new_level: the output level to use
	*/
	int messenger_set_level(E_OUTPUT new_level) {
		engine->messenger_output_level = new_level;
		return 0;
	}
	/*
	* messenger_get_level() - Return the output level when printing message descriptions
	*/
	E_OUTPUT messenger_get_level() {
		return engine->messenger_output_level;
	}

	/*
	* handle_messages() - Handle all queued messaged and execute the recipients' functions
	* ! This function will be called from the main loop at the end of every frame
		and is guaranteed to send every queued message. The queue shall be cleared after running
	* ! Note that in certain situations the engine might call this more than once per frame
	*/
	int handle_messages() {
		const Uint32 t = get_ticks(); // Get the current tick to compare with message tickstamps

		// Print message descriptions
		for (auto& msg : engine->messages) {
			if (t < msg->tickstamp) { // If the message should be processed in the future, skip it
				continue;
			}

			switch (messenger_get_level()) { // Skip certain messages depending on the verbosity level
				case E_OUTPUT::NONE: { // When the verbosity is NONE, skip all message types
					continue;
				}
				case E_OUTPUT::QUIET: { // When the verbosity is QUIET, skip all types except warnings and errors
					if (
						(msg->type != E_MESSAGE::WARNING)
						|| (msg->type != E_MESSAGE::ERROR)
					) {
						continue;
					}
					break;
				}
				case E_OUTPUT::NORMAL: // When the verbosity is NORMAL, skip internal messages
				default: {
					if (msg->type == E_MESSAGE::INTERNAL) {
						continue;
					}
					break;
				}
				case E_OUTPUT::VERBOSE: { // When the verbosity is VERBOSE, skip no message types
					break;
				}
			}

			// Create a string of the message's tags
			std::string tags = joinv(msg->tags, ',');

			// Change the output color depending on the message type
			if (msg->type == E_MESSAGE::WARNING) {
				bee_commandline_color(11); // Yellow
			}
			if (msg->type == E_MESSAGE::ERROR) {
				bee_commandline_color(9); // Red
			}

			// Output to the appropriate stream
			std::ostream* o = &std::cout;
			if ((msg->type == E_MESSAGE::WARNING)||(msg->type == E_MESSAGE::ERROR)) {
				o = &std::cerr;
			}

			// Output the message metadata
			*o << "MSG (" << msg->tickstamp << "ms)[" << messenger_get_type_string(msg->type) << "]<" << tags << ">: ";
			//*o << "MSG (" << t << "ms)[" << messenger_get_type_string(msg->type) << "]<" << tags << ">: ";

			// Output the message description
			if (msg->descr.find("\n") != std::string::npos) { // If the description is multiple liness, indent it as necessary
				*o << "\n";
				*o << debug_indent(msg->descr, 1);
			} else { // Otherwise, output it as normal
				*o << msg->descr << "\n";
			}

			bee_commandline_color_reset(); // Reset the output color
		}
		std::flush(std::cout); // Flush the output buffer before processing the recipients

		// Process messages with recipient functions
		std::exception_ptr ep; // Store any thrown values
		for (auto& msg : engine->messages) { // Iterate over the messages
			auto m = msg; // Create an extra reference to the smart pointer to prevent it from being deallocated during the loop
			if (t < m->tickstamp) { // If the message should be processed in the future, skip it
				continue;
			}

			for (auto& tag : m->tags) { // Iterate over the message's tags
				if (m->has_processed) { // If the message has already been processed, skip it
					continue;
				}

				auto rt = engine->recipients.find(tag); // Make sure that the message tag exists
				if (rt != engine->recipients.end()) {
					for (auto& recv : engine->recipients[tag]) { // Iterate over the recipients who wish to process the tag
						if (recv->is_strict) { // If the recipient is strict
							if (recv->tags != m->tags) { // If the tags don't match, skip it
								continue;
							} else { // Otherwise set the processed flag so that the message isn't handled multiple times
								m->has_processed = true;
							}
						}

						// Call the recipient function
						if (recv->func != nullptr) {
							try {
								recv->func(m);
							} catch (int e) { // Catch several kinds of exceptions that the recipient might throw
								ep = std::current_exception();
								bee_commandline_color(9);
								std::cerr << "MSG ERR (" << t << "ms): exception " << e << " thrown by recipient \"" << recv->name << "\"\n";
								bee_commandline_color_reset();
							} catch (const char* e) {
								ep = std::current_exception();
								bee_commandline_color(9);
								std::cerr << "MSG ERR (" << t << "ms): exception \"" << e << "\" thrown by recipient \"" << recv->name << "\"\n";
								bee_commandline_color_reset();
							} catch (...) {
								ep = std::current_exception(); // Store any miscellaneous exceptions to be rethrown
							}
						}
					}
				}
			}
			if (m != nullptr) { // Set the processed flag after iterating over all message tags
				m->has_processed = true;
			}
		}

		// Remove processed messages
		engine->messages.erase(std::remove_if(engine->messages.begin(), engine->messages.end(), [] (std::shared_ptr<MessageContents> msg) {
			return msg->has_processed;
		}), engine->messages.end());

		if (ep != nullptr) { // If an exception was thrown, throw it after finishing processing the message
			std::flush(std::cout);
			std::rethrow_exception(ep);
		}

		return 0; // Return 0 on success
	}
	/*
	* messenger_get_type_string() - Return the name describing the message type
	* @type: the type of message to evaluate
	*/
	std::string messenger_get_type_string(E_MESSAGE type) {
		switch (type) { // Return the string for the requested type
			case E_MESSAGE::GENERAL:  return "general";
			case E_MESSAGE::START:    return "start";
			case E_MESSAGE::END:      return "end";
			case E_MESSAGE::INFO:     return "info";
			case E_MESSAGE::WARNING:  return "warning";
			case E_MESSAGE::ERROR:    return "error";
			case E_MESSAGE::INTERNAL: return "internal";
			default:                       return "unknown"; // Return "unknown" when an undefined type of provided
		}
	}
}

#endif // BEE_INIT_MESSENGER
