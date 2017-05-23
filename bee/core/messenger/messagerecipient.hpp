/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_MESSAGERECIPIENT_H
#define _BEE_CORE_MESSAGERECIPIENT_H 1

#include "../../game.hpp"

struct BEE::MessageRecipient {
	std::string name;
	std::vector<std::string> tags;
	bool is_strict;
	std::function<void (BEE*, std::shared_ptr<MessageContents>)> func = nullptr;

	MessageRecipient();
	MessageRecipient(std::string, std::vector<std::string>, bool, std::function<void (BEE*, std::shared_ptr<MessageContents>)>);
};

#endif // _BEE_CORE_MESSAGERECIPIENT_H
