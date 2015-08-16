/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL
#define _BEE_UTIL 1

#include "util.hpp"

#include "util/real.hpp"
#include "util/string.hpp"
#include "util/dates.hpp"
#include "util/collision.hpp"
#include "util/sound.hpp"
#include "util/messagebox.hpp"
#include "util/files.hpp"
#include "util/script.hpp"
#include "util/network.hpp"

std::pair<int,int> coord_approach(int x1, int y1, int x2, int y2, int speed) {
	float d = distance(x1, y1, x2, y2);
	if (d <= speed) {
		return std::make_pair(x2, y2);
	}
 	float ratio = speed/d;

	int x3 = x1 + (x2-x1)*ratio;
	int y3 = y1 + (y2-y1)*ratio;

	return std::make_pair(x3, y3);
}

#endif // _BEE_UTIL
