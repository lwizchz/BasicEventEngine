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
#include <fstream>

#include "messenger.hpp"

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "messagerecipient.hpp"
#include "messagecontents.hpp"

#include "../core/enginestate.hpp"

#include "../util/files.hpp"
#include "../util/windefine.hpp"

namespace bee { namespace messenger{
	namespace internal {
		std::unordered_map<std::string,std::unordered_set<std::shared_ptr<MessageRecipient>>> recipients;
		const std::unordered_set<std::string> protected_tags = {"engine", "console"};
		std::vector<std::shared_ptr<MessageContents>> messages;

		std::unordered_set<std::string> filter;
		bool is_filter_blacklist = true;

		std::unordered_map<std::string,std::pair<E_OUTPUT,std::ostream*>> logfiles = {{"stdout", {E_OUTPUT::NORMAL, nullptr}}};
	}

	/*
	* clear() - Clear all recipients and messages for when the game ends
	*/
	int clear() {
		if (!internal::messages.empty()) { // If there are messages that haven't been processed, output a warning
			internal::send_urgent(std::shared_ptr<MessageContents>(new MessageContents(
				get_ticks(),
				{"engine", "close"},
				E_MESSAGE::WARNING,
				"Messenger closing with "+bee_itos(internal::messages.size())+" messages left in the queue",
				nullptr
			)));
		}

		clear_logs(false);

		internal::recipients.clear();
		internal::messages.clear();

		return 0; // Return 0 on success
	}

	/*
	* internal::output_msg() - Output the given message if the verbosity level is high enough
	* @msg: the message to process
	*/
	int internal::output_msg(std::shared_ptr<MessageContents> msg) {
		bool is_filtered = false;
		for (auto& t : msg->tags) {
			for (auto& f : filter) {
				if (t == f) {
					is_filtered = true;
					break;
				}
			}

			if (is_filtered) {
				break;
			}
		}
		if (!(is_filtered ^ is_filter_blacklist)) {
			return 2; // Return 2 when the message got filtered
		}

		if ((get_options().is_headless)&&(!get_options().is_debug_enabled)) {
			std::stringstream h; // Combine the message metadata
			h << msg->tickstamp << "ms> ";

			print_msg(h.str(), msg);

			return 3; // Return 3 when in headless mode
		}

		// Create a string of the message's tags
		std::string tags = joinv(msg->tags, ',');

		std::stringstream h; // Combine the message metadata
		h << "MSG (" << msg->tickstamp << "ms)[" << get_type_string(msg->type) << "]<" << tags << ">: ";

		print_msg(h.str(), msg);

		return 0; // Return 0 on success
	}
	/*
	* internal::print_msg() - Print the given message description with the given header
	* @header: the metadata to print before the message description
	* @msg: the message whose description should be printed
	*/
	int internal::print_msg(const std::string& header, std::shared_ptr<MessageContents> msg) {
		for (auto& lf : logfiles) {
			if (!should_print(lf.second.first, msg->type)) {
				continue;
			}

			std::ostream* o = lf.second.second;

			if (lf.first == "stdout") {
				// Change the output color depending on the message type
				if (msg->type == E_MESSAGE::WARNING) {
					bee_commandline_color(11); // Yellow
				}
				if (msg->type == E_MESSAGE::ERROR) {
					bee_commandline_color(9); // Red
				}

				// Output to the appropriate stream
				o = &std::cout;
				if ((msg->type == E_MESSAGE::WARNING)||(msg->type == E_MESSAGE::ERROR)) {
					o = &std::cerr;
				}
			}

			// Output the message metadata
			*o << header;

			// Output the message description
			if (msg->descr.find("\n") != std::string::npos) { // If the description is multiple liness, indent it as necessary
				*o << "\n";
				*o << debug_indent(msg->descr, 1);
			} else { // Otherwise, output it as normal
				*o << msg->descr << "\n";
			}

			bee_commandline_color_reset(); // Reset the output color

			std::flush(*o); // Flush the output buffer after printing
		}

		return 0;
	}
	/*
	*
	*/
	bool internal::should_print(E_OUTPUT level, E_MESSAGE type) {
		switch (level) { // Skip certain messages depending on the verbosity level
			case E_OUTPUT::NONE: { // When the verbosity is NONE, skip all message types
				return false; // Return 1 when the message should not be printed
			}
			case E_OUTPUT::QUIET: { // When the verbosity is QUIET, skip all types except warnings and errors
				if (
					(type != E_MESSAGE::WARNING)
					|| (type != E_MESSAGE::ERROR)
				) 		{
					return false; // Return 1 when the message should not be printed
				}
				break;
			}
			case E_OUTPUT::NORMAL: // When the verbosity is NORMAL, skip internal messages
			default: {
				if (type == E_MESSAGE::INTERNAL) {
					return false; // Return 1 when the message should not be printed
				}
				break;
			}
			case E_OUTPUT::VERBOSE: { // When the verbosity is VERBOSE, skip no message types
				break;
			}
		}
		return true;
	}
	/*
	* internal::call_recipients() - Call the recipients who are registered for the given message's tags
	* @msg: the message to pass to the recipients
	*/
	std::exception_ptr internal::call_recipients(std::shared_ptr<MessageContents> msg) {
		std::exception_ptr ep = nullptr;

		if (msg->tags[0] == "direct") { // Send the message directly to a recipient rather than handling tags
			auto rt = recipients.find("direct"); // Recipients must register for the direct tag in order to receive direct messages
			if (rt != recipients.end()) {
				for (size_t i=1; i<msg->tags.size(); ++i) {
					for (auto recv : rt->second) {
						if (recv->name == msg->tags[i]) {
							std::exception_ptr e = handle_recipient(recv, msg);
							if (e != nullptr) {
								ep = e;
							}
						}
					}
				}
			}
		} else {
			for (auto& tag : msg->tags) { // Iterate over the message's tags
				if (msg->has_processed) { // If the message has already been processed, skip it
					continue;
				}

				auto rt = recipients.find(tag); // Make sure that the message tag exists
				if (rt != recipients.end()) {
					for (auto recv : recipients[tag]) { // Iterate over the recipients who wish to process the tag
						std::exception_ptr e = handle_recipient(recv, msg);
						if (e != nullptr) {
							ep = e;
						}
					}
				}
			}
		}

		msg->has_processed = true; // Set the processed flag after iterating over all message tags

		return ep;
	}
	/*
	* internal::handle_recipient() - Handle the individual recipient
	* @recv: the recipient to send the message to
	* @msg: the message to send
	*/
	std::exception_ptr internal::handle_recipient(std::shared_ptr<MessageRecipient> recv, std::shared_ptr<MessageContents> msg) {
		std::exception_ptr ep = nullptr;

		if (recv->is_strict) { // If the recipient is strict
			if (recv->tags != msg->tags) { // If the tags don't match, skip it
				if (recv->tags[0] != "direct") {
					return nullptr;
				}

				// Allow strict messages that don't match the direct tag
				std::vector<std::string> rt (recv->tags);
				std::vector<std::string> mt (msg->tags);
				mt.insert(mt.begin(), "direct");

				if (rt != mt) {
					return nullptr;
				}
			} else { // Otherwise set the processed flag so that the message isn't handled multiple times
				msg->has_processed = true;
			}
		}

		// Call the recipient function
		if (recv->func != nullptr) {
			#ifdef NDEBUG
				try {
					recv->func(msg);
				} catch (int e) { // Catch several kinds of exceptions that the recipient might throw
					ep = std::current_exception();
					bee_commandline_color(9);
					std::cerr << "MSG ERR (" << get_ticks() << "ms): exception " << e << " thrown by recipient \"" << recv->name << "\"\n";
					bee_commandline_color_reset();
				} catch (const char* e) {
					ep = std::current_exception();
					bee_commandline_color(9);
					std::cerr << "MSG ERR (" << get_ticks() << "ms): exception \"" << e << "\" thrown by recipient \"" << recv->name << "\"\n";
					bee_commandline_color_reset();
				} catch (...) {
					ep = std::current_exception(); // Store any miscellaneous exceptions to be rethrown
				}
			#else // If in debugging mode, just let the recipient crash
				recv->func(msg);
			#endif
		}

		return ep;
	}

	/*
	* internal::register_protected() - Register the given recipient with protected tags within the messaging system
	* @recv: the recipient to register
	*/
	int internal::register_protected(std::shared_ptr<MessageRecipient> recv) {
		for (auto& tag : recv->tags) { // Iterate over the requested tags
			if (recipients.find(tag) == recipients.end()) { // If the tag doesn't exist, then create it
				recipients.emplace(tag, std::unordered_set<std::shared_ptr<MessageRecipient>>()); // Emplace an empty set of recipients
			}
			recipients[tag].insert(recv); // Add the recipient to the list
		}
		return 0; // Return 0 on success
	}
	/*
	* internal::register_protected() - Register the given recipient within the messaging system
	* @name: the name of the funciton
	* @tags: the tags to register the recipient with
	* @is_strict: whether the recipient should only take exact tags
	* @func: the function to use to handle the messages
	*/
	std::shared_ptr<MessageRecipient> internal::register_protected(std::string name, const std::vector<std::string>& tags, bool is_strict, std::function<void (std::shared_ptr<MessageContents>)> func) {
		std::shared_ptr<MessageRecipient> recv (new MessageRecipient(name, tags, is_strict, func)); // Create a pointer for the given message data
		register_protected(recv); // Attempt to register the recipient
		return recv; // Return the newly constructed recipient
	}
	/*
	* internal::unregister_protected() - Unregister the given recipient from protected tags within the messaging system
	* @recv: the recipient to unregister
	*/
	int internal::unregister_protected(std::shared_ptr<MessageRecipient> recv) {
		for (auto& tag : recv->tags) { // Iterate over the recipient's tags
			auto rt = recipients.find(tag);
			if (rt != recipients.end()) { // If the tag exists within the recipient list
				if (rt->second.find(recv) != rt->second.end()) { // If the recipient exists within the list
					rt->second.erase(recv); // Remove the recipient
					if (rt->second.empty()) { // If the tag has no other recipients, remove it
						recipients.erase(tag);
					}
				}
			}
		}
		return 0; // Return 0 on success
	}
	/*
	* internal::send_urgent() - Immediately process the given message to its recipients
	* ! Warning: this can break things so use sparingly, most things can wait a couple milliseconds for handle_messages() to process at the end of each frame
	* @msg: the message to process
	*/
	int internal::send_urgent(std::shared_ptr<MessageContents> msg) {
		msg->descr = trim(msg->descr); // Trim the message description

		output_msg(msg);

		msg->has_processed = false;
		std::exception_ptr ep = call_recipients(msg);

		if (ep != nullptr) { // If an exception was thrown, throw it after finishing processing the message
			std::flush(std::cout);
			std::rethrow_exception(ep);
		}

		return 0; // Return 0 on success
	}

	/*
	* internal::remove_messages() - Remove messages from processing based on the given criterion function
	* @func: the callback which determines whether to remove a message
	*/
	int internal::remove_messages(std::function<bool (std::shared_ptr<MessageContents>)> func) {
		size_t amount = messages.size();

		messages.erase(std::remove_if(messages.begin(), messages.end(), func), messages.end());

		amount -= messages.size();

		return amount;
	}

	/*
	* register() - Register the given recipient within the messaging system
	* @recv: the recipient to register
	*/
	int register_recipient(std::shared_ptr<MessageRecipient> recv) {
		int r = 0; // Store any attempts to register a protected tag

		for (auto& tag : recv->tags) { // Iterate over the requested tags
			if (internal::protected_tags.find(tag) != internal::protected_tags.end()) { // If the requested tag is protected, deny registration
				// Output an error message
				bee_commandline_color(11);
				std::cerr << "MSG failed to register recipient \"" << recv->name << "\" with protected tag \"" << tag << "\".\n";
				bee_commandline_color_reset();

				r++; // Increment the protection counter

				continue; // Skip to the next tag
			}

			if (internal::recipients.find(tag) == internal::recipients.end()) { // If the tag doesn't exist, then create it
				internal::recipients.emplace(tag, std::unordered_set<std::shared_ptr<MessageRecipient>>()); // Emplace an empty set of recipients
			}
			internal::recipients[tag].insert(recv); // Add the recipient to the list
		}

		return r; // Return the amount of attempts to register a protected tag
	}
	/*
	* register_recipient() - Register the given recipient within the messaging system
	* @name: the name of the funciton
	* @tags: the tags to register the recipient with
	* @is_strict: whether the recipient should only take exact tags
	* @func: the function to use to handle the messages
	*/
	std::shared_ptr<MessageRecipient> register_recipient(std::string name, const std::vector<std::string>& tags, bool is_strict, std::function<void (std::shared_ptr<MessageContents>)> func) {
		std::shared_ptr<MessageRecipient> recv (new MessageRecipient(name, tags, is_strict, func)); // Create a pointer for the given message data
		register_recipient(recv); // Attempt to register the recipient
		return recv; // Return the newly constructed recipient
	}
	/*
	* register_recipient() - Register the given recipient within the messaging system
	* ! When the function is called without a name and strictness, simply call it with an empty name and unstrictness
	* @tags: the tags to register the recipient with
	* @func: the function to use to handle the messages
	*/
	std::shared_ptr<MessageRecipient> register_recipient(const std::vector<std::string>& tags, std::function<void (std::shared_ptr<MessageContents>)> func) {
		return register_recipient("", tags, false, func);
	}
	/*
	* unregister() - Unregister the given recipient within the messaging system
	* @recv: the recipient to unregister
	*/
	int unregister(std::shared_ptr<MessageRecipient> recv) {
		for (auto& tag : recv->tags) { // Iterate over the recipient's tags
			if (internal::protected_tags.find(tag) != internal::protected_tags.end()) { // If the specific tag is protected, deny removal for any tags
				// Output an error message
				bee_commandline_color(11);
				std::cerr << "MSG failed to unregister recipient \"" << recv->name << "\" because of protected tag \"" << tag << "\".\n";
				bee_commandline_color_reset();

				return 1; // Return 1 on denial by protected tag
			}

			auto rt = internal::recipients.find(tag);
			if (rt != internal::recipients.end()) { // If the tag exists within the recipient list
				if (rt->second.find(recv) != rt->second.end()) { // If the recipient exists within the list
					rt->second.erase(recv); // Remove the recipient
					if (rt->second.empty()) { // If the tag has no other recipients, remove it
						internal::recipients.erase(tag);
					}
				}
			}
		}
		return 0; // Return 0 on success
	}
	/*
	* unregister_name() - Unregister the recipient with the given name within the messaging system
	* @name: the name of the recipient to unregister
	*/
	int unregister_name(const std::string& name) {
		std::shared_ptr<MessageRecipient> recv (nullptr);

		for (auto& tag : internal::recipients) { // Iterate over all tags
			for (auto r : tag.second) { // Iterate over the recipients for a specific tag
				if (r->name == name) { // If the specific recipient matches the desired recipient's name
					recv = r; // Assign the recipient and break out
					break;
				}
			}

			if (recv != nullptr) { // If the recipient has been found, break out
				break;
			}
		}

		if (recv == nullptr) {
			return 2; // Return 2 when the recipient does not exist
		}

		return unregister(recv); // Return the attempt to remove the recipient
	}
	/*
	* unregister_all() - Unregister all messaging system recipients
	* ! Note that this function is not able to protect functions which register to additional tags such as console commands
	*   e.g. {"engine", "console", "help"} will be reduced to {"engine", "console"}
	*   This function may be protected in the future
	*/
	int unregister_all() {
		int r = 0; // Store any attempts to unregister a protected tag

		for (auto& tag : internal::recipients) { // Iterate over all recipient tags
			if (internal::protected_tags.find(tag.first) != internal::protected_tags.end()) { // If the specific tag is protected, deny removal
				r++; // Increment the protection counter
				continue; // Skip to the next tag
			}

			tag.second.clear(); // Remove all recipients who are registered with this tag
		}

		// Remove all non-protected tags
		for (auto tag=internal::recipients.begin(); tag!=internal::recipients.end();) {
			if (internal::protected_tags.find((*tag).first) == internal::protected_tags.end()) { // If the tag is not protected, remove it
				tag = internal::recipients.erase(tag);
			} else { // It the tag is protected, skip it
				++tag;
			}
		}

		return r; // Return the amount of attempts to unregister a protected tag
	}

	/*
	* send() - Queue the given message in the messaging system
	* @m: the message to queue
	*/
	int send(std::shared_ptr<MessageContents> msg) {
		if (msg == nullptr) {
			return 1; // Return 1 when passed a null message
		}

		msg->descr = trim(msg->descr); // Trim the message description
		internal::messages.push_back(msg); // Add the message to the list

		return 0; // Return 0 on success
	}
	/*
	* send() - Queue the given message in the messaging system
	* ! When the function is called with separate message contents, create a pointer and call it again
	* @tickstamp: the tickstamp of the given message, normally should be the current tick
	* @tags: the list of tags that the message should be sent to
	* @descr: the message description
	* @data: the message data
	*/
	//int send(const std::vector<std::string>& tags, E_MESSAGE type, const std::string& descr, void* data) {
	int send(const std::vector<std::string>& tags, E_MESSAGE type, const std::string& descr, std::shared_ptr<void> data) {
		std::shared_ptr<MessageContents> msg (new MessageContents(get_ticks(), tags, type, descr, data)); // Create a pointer for the given message data
		return send(msg); // Return the attempt at message sending
	}
	/*
	* send() - Queue the given message in the messaging system
	* ! When no data has been provided, call the function again with it set to nullptr
	* @tickstamp: the tickstamp of the given message, normally should be the current tick
	* @tags: the list of tags that the message should be sent to
	* @descr: the message description
	*/
	int send(const std::vector<std::string>& tags, E_MESSAGE type, const std::string& descr) {
		return send(tags, type, descr, nullptr);
	}
	/*
	* log() - Queue the given string as a simple log message
	* @msg: the string to queue
	*/
	int log(const std::string& msg) {
		return send({"log"}, E_MESSAGE::INFO, msg, nullptr);
	}

	/*
	* add_filter() - Add a tag to the filter list
	* @f: the filter to add
	*/
	int add_filter(const std::string& f) {
		internal::filter.emplace(f);
		return internal::filter.size();
	}
	/*
	* set_filter_blacklist() - Set the filter type
	* @type: whether the filter should be a blacklist or whitelist
	*/
	int set_filter_blacklist(bool type) {
		internal::is_filter_blacklist = type;
		return 0;
	}
	/*
	* reset_filter() - Remove all filters
	*/
	int reset_filter() {
		internal::filter.clear();
		return 0;
	}

	/*
	* add_log() - Add a filename as a log file
	* ! This function can also be used to change the output level of existing log files
	* @filename: the file to log to
	* @level: the output level to log
	*/
	int add_log(const std::string& filename, E_OUTPUT level) {
		if (filename == "stdout") { // Deny overwriting stdout via this function
			return 1;
		}

		std::ofstream* logfile = new std::ofstream(filename);
		if (!logfile->is_open()) {
			send({"engine", "messenger"}, E_MESSAGE::ERROR, "Failed to open log file \"" + filename + "\"");
			return 2;
		}

		internal::logfiles[filename] = std::make_pair(level, logfile);

		return 0;
	}
	/*
	* remove_log() - Remove the given filename from being a log file
	* @filename: the filename to remove
	* @should_delete: whether the file should also be deleted
	*/
	int remove_log(const std::string& filename, bool should_delete) {
		if (filename == "stdout") { // Deny removing stdout via this function
			return 1;
		}

		if (internal::logfiles.find(filename) == internal::logfiles.end()) {
			send({"engine", "messenger"}, E_MESSAGE::WARNING, "The log file \"" + filename + "\" could not be removed because it has not been added");
			return 2;
		}

		std::ofstream* logfile = static_cast<std::ofstream*>(internal::logfiles.at(filename).second);
		logfile->close();

		internal::logfiles.erase(filename);

		if (should_delete) {
			file_delete(filename);
		}

		return 0;
	}
	/*
	* clear_logs() - Clear all log files except stdout
	* @should_delete: Whether the log files should be deleted
	*/
	int clear_logs(bool should_delete) {
		for (auto it=internal::logfiles.begin(); it!=internal::logfiles.end(); ) {
			if (it->first == "stdout") {
				++it;
				continue;
			}

			std::ofstream* logfile = static_cast<std::ofstream*>(it->second.second);
			logfile->close();

			if (should_delete) {
				file_delete(it->first);
			}

			internal::logfiles.erase(it++);
		}
		return 0;
	}

	/*
	* set_level() - Set the output level when printing message descriptions
	* @new_level: the output level to use
	*/
	int set_level(E_OUTPUT new_level) {
		internal::logfiles["stdout"] = std::make_pair(new_level, nullptr);
		return 0;
	}
	/*
	* get_level() - Return the output level when printing message descriptions
	*/
	E_OUTPUT get_level() {
		return internal::logfiles["stdout"].first;
	}

	/*
	* handle() - Handle all queued messaged and execute the recipients' functions
	* ! This function will be called from the main loop at the end of every frame
		and is guaranteed to send every queued message. The queue shall be cleared after running
	* ! Note that in certain situations the engine might call this more than once per frame
	*/
	int handle() {
		const Uint32 t = get_ticks(); // Get the current tick to compare with message tickstamps

		// Print message descriptions
		for (auto msg : internal::messages) {
			if (t < msg->tickstamp) { // If the message should be processed in the future, skip it
				continue;
			}

			internal::output_msg(msg);
		}

		// Process messages with recipient functions
		std::exception_ptr ep = nullptr; // Store any thrown values
		for (auto msg : internal::messages) { // Iterate over the messages
			if (t < msg->tickstamp) { // If the message should be processed in the future, skip it
				continue;
			}

			std::exception_ptr e = internal::call_recipients(msg);
			if (e != nullptr) {
				ep = e;
			}
		}

		// Remove processed messages
		internal::messages.erase(std::remove_if(internal::messages.begin(), internal::messages.end(), [] (std::shared_ptr<MessageContents> msg) {
			if (msg == nullptr) {
				return true;
			}
			return msg->has_processed;
		}), internal::messages.end());

		if (ep != nullptr) { // If an exception was thrown, throw it after finishing processing the message
			std::flush(std::cout);
			std::rethrow_exception(ep);
		}

		return 0; // Return 0 on success
	}
	/*
	* get_type_string() - Return the name describing the message type
	* @type: the type of message to evaluate
	*/
	std::string get_type_string(E_MESSAGE type) {
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
}}

#endif // BEE_INIT_MESSENGER
