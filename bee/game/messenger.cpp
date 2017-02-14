/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_MESSENGER
#define _BEE_GAME_MESSENGER 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::messenger_register_protected() - Register the given recipient with protected tags within the messaging system
* @recv: the recipient to register
*/
int BEE::messenger_register_protected(std::shared_ptr<MessageRecipient> recv) {
	for (auto& tag : recv->tags) { // Iterate over the requested tags
		if (recipients.find(tag) == recipients.end()) { // If the tag doesn't exist, then create it
			recipients.emplace(tag, std::unordered_set<std::shared_ptr<MessageRecipient>>()); // Emplace an empty set of recipients
		}
		recipients[tag].insert(recv); // Add the recipient to the list
	}
	return 0; // Return 0 on success
}
/*
* BEE::messenger_register_protected() - Register the given recipient within the messaging system
* @name: the name of the funciton
* @tags: the tags to register the recipient with
* @is_strict: whether the recipient should only take exact tags
* @func: the function to use to handle the messages
*/
int BEE::messenger_register_protected(std::string name, const std::vector<std::string>& tags, bool is_strict, std::function<void (BEE*, std::shared_ptr<MessageContents>)> func) {
	std::shared_ptr<MessageRecipient> recv (new MessageRecipient(name, tags, is_strict, func)); // Create a pointer for the given message data
	return messenger_register_protected(recv); // Return the attempt at message registration
}
/*
* BEE::messenger_unregister_protected() - Unregister the given recipient from protected tags within the messaging system
* @recv: the recipient to unregister
*/
int BEE::messenger_unregister_protected(std::shared_ptr<MessageRecipient> recv) {
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
* BEE::messenger_send_urgent() - Immediately process the given message to its recipients
* ! Warning: this can break things so use sparingly, most things can wait a couple milliseconds for handle_messages() to process at the end of each frame
* @msg: the message to process
*/
int BEE::messenger_send_urgent(std::shared_ptr<MessageContents> msg) {
	msg->descr = trim(msg->descr); // Trim the message description

	std::exception_ptr ep; // Store any thrown values
	for (auto& tag : msg->tags) { // Iterate over the message tags
		auto rt = recipients.find(tag); // Make sure that the message tag exists
		if (rt != recipients.end()) {
			for (auto& recv : recipients[tag]) { // Iterate over the recipients who wish to process the tag
				if (recv->is_strict) { // If the recipient is strict and the tags don't match, skip it
					if (recv->tags != msg->tags) {
						continue;
					}
				}

				// Call the recipient function
				if (recv->func != nullptr) {
					try {
						recv->func(this, msg);
					} catch (int e) { // Catch several kinds of exceptions that the recipient might throw
						ep = std::current_exception();
						bee_commandline_color(9);
						std::cout << "MSG ERR (" << get_ticks() << "ms): exception " << e << " thrown by recipient \"" << recv->name << "\"\n";
						bee_commandline_color_reset();
					} catch (const char* e) {
						ep = std::current_exception();
						bee_commandline_color(9);
						std::cout << "MSG ERR (" << get_ticks() << "ms): exception \"" << e << "\" thrown by recipient \"" << recv->name << "\"\n";
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
* BEE::messenger_register() - Register the given recipient within the messaging system
* @recv: the recipient to register
*/
int BEE::messenger_register(std::shared_ptr<MessageRecipient> recv) {
	int r = 0; // Store any attempts to register a protected tag

	for (auto& tag : recv->tags) { // Iterate over the requested tags
		if (protected_tags.find(tag) != protected_tags.end()) { // If the requested tag is protected, deny registration
			// Output an error message
			bee_commandline_color(11);
			std::cout << "MSG failed to register recipient \"" << recv->name << "\" with protected tag \"" << tag << "\".\n";
			bee_commandline_color_reset();

			r++; // Increment the protection counter

			continue; // Skip to the next tag
		}

		if (recipients.find(tag) == recipients.end()) { // If the tag doesn't exist, then create it
			recipients.emplace(tag, std::unordered_set<std::shared_ptr<MessageRecipient>>()); // Emplace an empty set of recipients
		}
		recipients[tag].insert(recv); // Add the recipient to the list
	}

	return r; // Return the amount of attempts to register a protected tag
}
/*
* BEE::messenger_register() - Register the given recipient within the messaging system
* @name: the name of the funciton
* @tags: the tags to register the recipient with
* @is_strict: whether the recipient should only take exact tags
* @func: the function to use to handle the messages
*/
int BEE::messenger_register(std::string name, const std::vector<std::string>& tags, bool is_strict, std::function<void (BEE*, std::shared_ptr<MessageContents>)> func) {
	std::shared_ptr<MessageRecipient> recv (new MessageRecipient(name, tags, is_strict, func)); // Create a pointer for the given message data
	return messenger_register(recv); // Return the attempt at message registration
}
/*
* BEE::messenger_register() - Register the given recipient within the messaging system
* ! When the function is called without a name and strictness, simply call it with an empty name and unstrictness
* @tags: the tags to register the recipient with
* @func: the function to use to handle the messages
*/
int BEE::messenger_register(const std::vector<std::string>& tags, std::function<void (BEE*, std::shared_ptr<MessageContents>)> func) {
	return messenger_register("", tags, false, func);
}
/*
* BEE::messenger_unregister() - Unregister the given recipient within the messaging system
* @recv: the recipient to unregister
*/
int BEE::messenger_unregister(std::shared_ptr<MessageRecipient> recv) {
	for (auto& tag : recv->tags) { // Iterate over the recipient's tags
		if (protected_tags.find(tag) != protected_tags.end()) { // If the specific tag is protected, deny removal for any tags
			// Output an error message
			bee_commandline_color(11);
			std::cout << "MSG failed to unregister recipient \"" << recv->name << "\" because of protected tag \"" << tag << "\".\n";
			bee_commandline_color_reset();

			return 1; // Return 1 on denial by protected tag
		}

		auto rt = recipients.find(tag);
		if (rt != recipients.end()) { // If the tag exists within the recipient list
			if (rt->second.find(recv) != rt->second.end()) { // If te recipient exists within the list
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
* BEE::messenger_unregister_all() - Unregister all messaging system recipients
* ! Note that this function is not able to protect function which register to additional tags such as console commands
*   e.g. {"engine", "console", "help"} will be reduced to {"engine", "console"}
*   This function may be protected in the future
*/
int BEE::messenger_unregister_all() {
	int r = 0; // Store any attempts to unregister a protected tag

	for (auto& tag : recipients) { // Iterate over all recipient tags
		if (protected_tags.find(tag.first) != protected_tags.end()) { // If the specific tag is protected, deny removal
			r++; // Increment the protection counter
			continue; // Skip to the next tag
		}

		tag.second.clear(); // Remove all recipients who are registered with this tag
	}

	// Remove all non-protected tags
	for (auto tag=recipients.begin(); tag!=recipients.end();) {
		if (protected_tags.find((*tag).first) == protected_tags.end()) { // If the tag is not protected, remove it
			tag = recipients.erase(tag);
		} else { // It the tag is protected, skip it
			++tag;
		}
	}

	return r; // Return the amount of attempts to unregister a protected tag
}

/*
* BEE::messenger_send() - Queue the given message in the messaging system
* @m: the message to queue
*/
int BEE::messenger_send(std::shared_ptr<MessageContents> msg) {
	if (msg == nullptr) {
		return 1; // Return 1 when passed a null message
	}

	msg->descr = trim(msg->descr); // Trim the message description
	messages.push_back(msg); // Add the message to the list

	return 0; // Return 0 on success
}
/*
* BEE::messenger_send() - Queue the given message in the messaging system
* ! When the function is called with separate message contents, create a pointer and call it again
* @tickstamp: the tickstamp of the given message, normally should be the current tick
* @tags: the list of tags that the message should be sent to
* @descr: the message description
* @data: the message data
*/
//int BEE::messenger_send(const std::vector<std::string>& tags, bee_message_t type, const std::string& descr, void* data) {
int BEE::messenger_send(const std::vector<std::string>& tags, bee_message_t type, const std::string& descr, std::shared_ptr<void> data) {
	std::shared_ptr<MessageContents> msg (new MessageContents(get_ticks(), tags, type, descr, data)); // Create a pointer for the given message data
	return messenger_send(msg); // Return the attempt at message sending
}

/*
* BEE::messenger_send() - Queue the given message in the messaging system
* ! When no data has been provided, call the function again with it set to nullptr
* @tickstamp: the tickstamp of the given message, normally should be the current tick
* @tags: the list of tags that the message should be sent to
* @descr: the message description
*/
int BEE::messenger_send(const std::vector<std::string>& tags, bee_message_t type, const std::string& descr) {
	return messenger_send(tags, type, descr, nullptr);
}

/*
* BEE::handle_messages() - Handle all queued messaged and execute the recipients' functions
* ! This function will be called from the main loop at the end of every frame
	and is guaranteed to send every queued message. The queue shall be cleared after running
* ! Note that in certain situations the engine might call this more than once per frame
*/
int BEE::handle_messages() {
	Uint32 t = get_ticks(); // Get the current tick to compare with message tickstamps

	// Print message descriptions
	for (auto& msg : messages) {
		if (t < msg->tickstamp) { // If the message should be processed in the future, skip it
			continue;
		}

		// Create a string of the message's tags
		std::string tags = joinv(msg->tags, ',');

		// Change the output color depending on the message type
		if (msg->type == BEE_MESSAGE_WARNING) {
			bee_commandline_color(11); // Yellow
		}
		if (msg->type == BEE_MESSAGE_ERROR) {
			bee_commandline_color(9); // Red
		}

		// Output the message metadata
		std::cout << "MSG <" << tags << ">[" << messenger_get_type_string(msg->type) << "](" << msg->tickstamp << "ms): ";

		// Output the message description
		if (msg->descr.find("\n") != std::string::npos) { // If the description is multiple liness, indent it as necessary
			std::cout << "\n";
			std::cout << debug_indent(msg->descr, 1);
		} else { // Otherwise, output it as normal
			std::cout << msg->descr << "\n";
		}

		bee_commandline_color_reset(); // Reset the output color
	}
	std::cout << std::flush; // Flush the output buffer before processing the recipients

	// Process messages with recipient functions
	std::exception_ptr ep; // Store any thrown values
	for (auto& msg : messages) { // Iterate over the messages
		if (t < msg->tickstamp) { // If the message should be processed in the future, skip it
			continue;
		}

		for (auto& tag : msg->tags) { // Iterate over the message's tags
			if (msg->has_processed) { // If the message has already been processed, skip it
				continue;
			}

			auto rt = recipients.find(tag); // Make sure that the message tag exists
			if (rt != recipients.end()) {
				for (auto& recv : recipients[tag]) { // Iterate over the recipients who wish to process the tag
					if (recv->is_strict) { // If the recipient is strict
						if (recv->tags != msg->tags) { // If the tags don't match, skip it
							continue;
						} else { // Otherwise set the processed flag so that the message isn't handled multiple times
							msg->has_processed = true;
						}
					}

					// Call the recipient function
					if (recv->func != nullptr) {
						try {
							recv->func(this, msg);
						} catch (int e) { // Catch several kinds of exceptions that the recipient might throw
							ep = std::current_exception();
							bee_commandline_color(9);
							std::cout << "MSG ERR (" << get_ticks() << "ms): exception " << e << " thrown by recipient \"" << recv->name << "\"\n";
							bee_commandline_color_reset();
						} catch (const char* e) {
							ep = std::current_exception();
							bee_commandline_color(9);
							std::cout << "MSG ERR (" << get_ticks() << "ms): exception \"" << e << "\" thrown by recipient \"" << recv->name << "\"\n";
							bee_commandline_color_reset();
						} catch (...) {
							ep = std::current_exception(); // Store any miscellaneous exceptions to be rethrown
						}
					}
				}
			}
		}
		if (msg != nullptr) { // Set the processed flag after iterating over all message tags
			msg->has_processed = true;
		}
	}

	// Remove processed messages
	messages.erase(std::remove_if(messages.begin(), messages.end(), [] (std::shared_ptr<MessageContents> msg) {
		return msg->has_processed;
	}), messages.end());

	if (ep != nullptr) { // If an exception was thrown, throw it after finishing processing the message
		std::flush(std::cout);
		std::rethrow_exception(ep);
	}

	return 0; // Return 0 on success
}
/*
* BEE::messenger_get_type_string() - Return the name describing the message type
* @type: the type of message to evaluate
*/
std::string BEE::messenger_get_type_string(bee_message_t type) const {
	switch (type) { // Return the string for the requested type
		case BEE_MESSAGE_GENERAL: return "general";
		case BEE_MESSAGE_START:   return "start";
		case BEE_MESSAGE_END:     return "end";
		case BEE_MESSAGE_INFO:    return "info";
		case BEE_MESSAGE_WARNING: return "warning";
		case BEE_MESSAGE_ERROR:   return "error";
		default:                  return "unknown"; // Return "unknown" when an undefined type of provided
	}
}

#endif // _BEE_GAME_MESSENGER
