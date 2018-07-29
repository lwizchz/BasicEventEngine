/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSAGECONTENTS
#define BEE_CORE_MESSAGECONTENTS 1

#include <vector>
#include <memory>

#include "messagecontents.hpp"

namespace bee {
	MessageContents::MessageContents() :
		tickstamp(0),
		tags(),
		type(E_MESSAGE::GENERAL),
		descr(),
		data(nullptr)
	{}
	MessageContents::MessageContents(Uint32 tm, const std::vector<std::string>& tg, E_MESSAGE tp, const std::string& de, std::shared_ptr<void> da) :
		tickstamp(tm),
		tags(tg),
		type(tp),
		descr(de),
		data(da)
	{}
}

#endif // BEE_CORE_MESSAGECONTENTS
