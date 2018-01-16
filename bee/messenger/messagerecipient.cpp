/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSAGERECIPIENT
#define BEE_CORE_MESSAGERECIPIENT 1

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "messagerecipient.hpp"

namespace bee {
	MessageRecipient::MessageRecipient() :
		name(),
		tags(),
		is_strict(false),
		func(nullptr)
	{}
	MessageRecipient::MessageRecipient(const std::string& n, const std::vector<std::string>& t, bool s, std::function<void (const MessageContents&)> f) :
		name(n),
		tags(t),
		is_strict(s),
		func(f)
	{}

	bool MessageRecipient::operator<(const MessageRecipient& rhs) const {
		return (this->name < rhs.name);
	}
}

#endif // BEE_CORE_MESSAGERECIPIENT
