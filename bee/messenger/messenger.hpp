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

#include "../util/windefine.hpp"

#include "messagecontents.hpp"
#include "messagerecipient.hpp"

namespace bee { namespace messenger {
	int clear();

	namespace internal {
		int output_msg(const MessageContents&);
		int print_msg(const std::string&, const MessageContents&);
		bool should_print(E_OUTPUT, E_MESSAGE);
		std::exception_ptr call_recipients(const MessageContents&);
		std::exception_ptr handle_recipient(const MessageRecipient&, const MessageContents&);

		int register_protected(const MessageRecipient&);
		int register_protected(const std::string&, const std::vector<std::string>&, bool, std::function<void (const MessageContents&)>);
		int unregister_protected(const std::string&);
		int send_urgent(const MessageContents&);
		int send_urgent(const std::vector<std::string>&, E_MESSAGE, const std::string&, std::shared_ptr<void>);

		int remove_messages(std::function<bool (const MessageContents&)>);
	}

	int register_recipient(const MessageRecipient&);
	int register_recipient(const std::string&, const std::vector<std::string>&, bool, std::function<void (const MessageContents&)>);
	int register_recipient(const std::vector<std::string>&, std::function<void (const MessageContents&)>);
	int unregister(const std::string&);
	int unregister_all();

	int send(const MessageContents&);
	int send(const std::vector<std::string>&, E_MESSAGE, const std::string&, std::shared_ptr<void>);
	int send(const std::vector<std::string>&, E_MESSAGE, const std::string&);
	int log(const std::string&);

	int add_filter(const std::string&);
	int set_filter_blacklist(bool);
	int reset_filter();

	int add_log(const std::string&, E_OUTPUT);
	int remove_log(const std::string&, bool);
	int clear_logs(bool);

	int set_level(E_OUTPUT);
	E_OUTPUT get_level();

	int handle();
	std::string get_type_string(E_MESSAGE);
}}

#endif // BEE_INIT_MESSENGER_H
