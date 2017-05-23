/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_H
#define _BEE_UTIL_H 1

// General utility functions bee/util.cpp
bool verify_assertions(int, char**);
bool verify_assertions();

#include "util/platform.hpp"
#include "util/real.hpp"
#include "util/string.hpp"
#include "util/dates.hpp"
#include "util/collision.hpp"
#include "util/sound.hpp"
#include "util/messagebox.hpp"
#include "util/files.hpp"
#include "util/network.hpp"

#include "util/template/template.hpp" // Include functions which require templates

#endif // _BEE_UTIL_H
