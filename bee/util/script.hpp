/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_SCRIPT_H
#define _BEE_UTIL_SCRIPT_H 1

#ifdef _BEE_SCRIPTING

// Scripting functions

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>

template <typename T>
T execute_string(std::string e) {
        chaiscript::ChaiScript c (chaiscript::Std_Lib::library());
        return c.eval<T>(e);
}
template <typename T>
T execute_file(std::string fname) {
        chaiscript::ChaiScript c (chaiscript::Std_Lib::library());
        return c.eval_file(fname);
}

#endif // _BEE_SCRIPTING

#endif // _BEE_UTIL_SCRIPT_H
