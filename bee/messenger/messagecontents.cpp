/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSAGECONTENTS
#define BEE_CORE_MESSAGECONTENTS 1

#include <vector>
#include <memory>

#include "messagecontents.hpp"

namespace bee {
	MessageContents::MessageContents() :
		has_processed(false),
		tickstamp(0),
		tags(),
		type(E_MESSAGE::GENERAL),
		descr(),
		data(nullptr)
	{}
	MessageContents::MessageContents(Uint32 tm, std::vector<std::string> tg, E_MESSAGE tp, std::string de, std::shared_ptr<void> da) :
		has_processed(false),
		tickstamp(tm),
		tags(tg),
		type(tp),
		descr(de),
		data(da)
	{}
}

#endif // BEE_CORE_MESSAGECONTENTS
