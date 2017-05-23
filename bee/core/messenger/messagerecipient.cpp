/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_MESSAGERECIPIENT
#define _BEE_CORE_MESSAGERECIPIENT 1

#include "messagerecipient.hpp"

BEE::MessageRecipient::MessageRecipient() :
	name(),
	tags(),
	is_strict(false),
	func(nullptr)
{}
BEE::MessageRecipient::MessageRecipient(std::string n, std::vector<std::string> t, bool s, std::function<void (BEE*, std::shared_ptr<MessageContents>)> f) :
	name(n),
	tags(t),
	is_strict(s),
	func(f)
{}

#endif // _BEE_CORE_MESSAGERECIPIENT
