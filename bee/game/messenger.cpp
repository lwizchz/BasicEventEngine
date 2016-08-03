/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_MESSENGER
#define _BEE_GAME_MESSENGER 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::messenger_register() - Register the given recipient within the messaging system
* @recv: the recipient to register
*/
int BEE::messenger_register(std::shared_ptr<MessageRecipient> recv) {
	for (auto& tag : recv->tags) {
		if (recipients.find(tag) != recipients.end()) {
			recipients[tag].insert(recv);
		}
	}
	return 0;
}
/*
* BEE::messenger_register() - Register the given recipient within the messaging system
* @recv: the recipient to register
*/
int BEE::messenger_register(const std::vector<std::string>& tags, std::function<void (BEE*, std::shared_ptr<MessageContents>)> func) {
	std::shared_ptr<MessageRecipient> recv (new MessageRecipient(tags, func));
	return messenger_register(recv);
}
/*
* BEE::messenger_unregister() - Unregister the given recipient within the messaging system
* @recv: the recipient to unregister
*/
int BEE::messenger_unregister(std::shared_ptr<MessageRecipient> recv) {
	for (auto& tag : recv->tags) {
		if (recipients.find(tag) != recipients.end()) {
			if (recipients[tag].find(recv) != recipients[tag].end()) {
				recipients[tag].erase(recv);
				if (recipients[tag].empty()) {
					recipients.erase(tag);
				}
			}
		}
	}
	return 0;
}
/*
* BEE::messenger_unregister_all() - Unregister all messaging system recipients
*/
int BEE::messenger_unregister_all() {
	for (auto& tag : recipients) {
		tag.second.clear();
	}
	recipients.clear();
	return 0;
}

/*
* BEE::messenger_send() - Queue the given message in the messaging system
* @m: the message to queue
*/
int BEE::messenger_send(std::shared_ptr<MessageContents> msg) {
	msg->data = trim(msg->data);
	for (auto& tag : msg->tags) {
		tagged_messages[tag].push_back(msg);
	}
	messages.push_back(msg);
	return 0;
}
/*
* BEE::messenger_send() - Queue the given message in the messaging system
* ! When the function is called with separate message contents, create a pointer and call it again
* @tickstamp: the tickstamp of the given message, normally should be the current tick
* @tags: the list of tags that the message should be sent to
* @data: the message data
*/
int BEE::messenger_send(const std::vector<std::string>& tags, bee_message_t type, const std::string& data) {
	std::shared_ptr<MessageContents> msg (new MessageContents(get_ticks(), tags, type, data));
	return messenger_send(msg);
}

/*
* BEE::handle_messages() - Handle all queued messaged and execute the recipients' functions
* ! This function will be called from the main loop at the end of every frame
	and is guaranteed to send every queued message. The queue shall be cleared after running
* ! Note that in certain situations the engine might call this more than once per frame
*/
int BEE::handle_messages() {
	for (auto& msg : messages) {
		std::string tags = "";
		for (auto& t : msg->tags) {
			tags += t + ",";
		}
		tags.pop_back();

		std::cout << "MSG <" << tags << ">[" << msg->type << "](" << msg->tickstamp << "): ";

		if (msg->data.find("\n") != std::string::npos) {
			std::cout << "\n";
			std::cout << debug_indent(msg->data, 1);
		} else {
			std::cout << msg->data << "\n";
		}
	}
	std::cout << std::flush;

	for (auto& tag : tagged_messages) {
		for (auto& msg : tag.second) {
			for (auto& recv : recipients[tag.first]) {
				if (recv->func != nullptr) {
					recv->func(this, msg);
				}
			}
		}
	}

	tagged_messages.clear();
	messages.clear();
	return 0;
}

#endif // _BEE_GAME_MESSENGER
