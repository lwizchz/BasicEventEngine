/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSENGER_H
#define BEE_CORE_MESSENGER_H 1

#include "messagecontents.hpp"
#include "messagerecipient.hpp"

namespace bee {
	namespace internal {
		int messenger_register_protected(std::shared_ptr<MessageRecipient>);
		std::shared_ptr<MessageRecipient> messenger_register_protected(std::string, const std::vector<std::string>&, bool, std::function<void (std::shared_ptr<MessageContents>)>);
		int messenger_unregister_protected(std::shared_ptr<MessageRecipient>);
		int messenger_send_urgent(std::shared_ptr<MessageContents>);
	}

	int messenger_register(std::shared_ptr<MessageRecipient>);
	std::shared_ptr<MessageRecipient> messenger_register(std::string, const std::vector<std::string>&, bool, std::function<void (std::shared_ptr<MessageContents>)>);
	std::shared_ptr<MessageRecipient> messenger_register(const std::vector<std::string>&, std::function<void (std::shared_ptr<MessageContents>)>);
	int messenger_unregister(std::shared_ptr<MessageRecipient>);
	int messenger_unregister_name(const std::string&);
	int messenger_unregister_all();

	int messenger_send(std::shared_ptr<MessageContents>);
	int messenger_send(const std::vector<std::string>&, E_MESSAGE, const std::string&, std::shared_ptr<void>);
	int messenger_send(const std::vector<std::string>&, E_MESSAGE, const std::string&);
	int messenger_log(const std::string&);

	int messenger_set_level(E_OUTPUT);
	E_OUTPUT messenger_get_level();

	int handle_messages();
	std::string messenger_get_type_string(E_MESSAGE);
}

#endif // BEE_INIT_MESSENGER_H
