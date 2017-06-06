/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_MESSAGECONTENTS_H
#define _BEE_CORE_MESSAGECONTENTS_H 1

#include "../../game.hpp"

struct BEE::MessageContents {
	bool has_processed;
	Uint32 tickstamp;
	std::vector<std::string> tags;
	bee::E_MESSAGE type;
	std::string descr;
	std::shared_ptr<void> data;

	MessageContents();
	MessageContents(Uint32, std::vector<std::string>, bee::E_MESSAGE, std::string, std::shared_ptr<void>);
};

#endif // _BEE_CORE_MESSAGECONTENTS_H
