/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSAGERECIPIENT_H
#define BEE_CORE_MESSAGERECIPIENT_H 1

#include <functional>

#include "messagecontents.hpp"

namespace bee {
	struct MessageRecipient {
		std::string name;
		std::vector<std::string> tags;
		bool is_strict;
		std::function<void (const MessageContents&)> func = nullptr;

		MessageRecipient();
		MessageRecipient(const std::string&, const std::vector<std::string>&, bool, std::function<void (const MessageContents&)>);

		bool operator<(const MessageRecipient&) const;
	};
}

#endif // BEE_CORE_MESSAGERECIPIENT_H
