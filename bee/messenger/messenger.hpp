/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSENGER_H
#define BEE_CORE_MESSENGER_H 1

#include <unordered_map>
#include <unordered_set>

#include "messagecontents.hpp"
#include "messagerecipient.hpp"

namespace bee { namespace messenger {
	int clear();

	namespace internal {
		extern std::unordered_map<std::string,std::unordered_set<std::shared_ptr<MessageRecipient>>> recipients;
		extern const std::unordered_set<std::string> protected_tags;
		extern std::vector<std::shared_ptr<MessageContents>> messages;
		extern E_OUTPUT messenger_output_level;

		int print_msg(std::shared_ptr<MessageContents>);
		std::exception_ptr call_recipients(std::shared_ptr<MessageContents>);

		int register_protected(std::shared_ptr<MessageRecipient>);
		std::shared_ptr<MessageRecipient> register_protected(std::string, const std::vector<std::string>&, bool, std::function<void (std::shared_ptr<MessageContents>)>);
		int unregister_protected(std::shared_ptr<MessageRecipient>);
		int send_urgent(std::shared_ptr<MessageContents>);
	}

	int register_recipient(std::shared_ptr<MessageRecipient>);
	std::shared_ptr<MessageRecipient> register_recipient(std::string, const std::vector<std::string>&, bool, std::function<void (std::shared_ptr<MessageContents>)>);
	std::shared_ptr<MessageRecipient> register_recipient(const std::vector<std::string>&, std::function<void (std::shared_ptr<MessageContents>)>);
	int unregister(std::shared_ptr<MessageRecipient>);
	int unregister_name(const std::string&);
	int unregister_all();

	int send(std::shared_ptr<MessageContents>);
	int send(const std::vector<std::string>&, E_MESSAGE, const std::string&, std::shared_ptr<void>);
	int send(const std::vector<std::string>&, E_MESSAGE, const std::string&);
	int log(const std::string&);

	int set_level(E_OUTPUT);
	E_OUTPUT get_level();

	int handle();
	std::string get_type_string(E_MESSAGE);
}}

#endif // BEE_INIT_MESSENGER_H
