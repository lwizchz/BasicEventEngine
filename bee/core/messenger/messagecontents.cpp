/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_MESSAGECONTENTS
#define _BEE_CORE_MESSAGECONTENTS 1

#include "messagecontents.hpp"

BEE::MessageContents::MessageContents() :
	has_processed(false),
	tickstamp(0),
	tags(),
	type(bee::E_MESSAGE::GENERAL),
	descr(),
	data(nullptr)
{}
BEE::MessageContents::MessageContents(Uint32 tm, std::vector<std::string> tg, bee::E_MESSAGE tp, std::string de, std::shared_ptr<void> da) :
	has_processed(false),
	tickstamp(tm),
	tags(tg),
	type(tp),
	descr(de),
	data(da)
{}

#endif // _BEE_CORE_MESSAGECONTENTS
