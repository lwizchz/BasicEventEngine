/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_SOUNDEFFECTS_H
#define BEE_CORE_SOUNDEFFECTS_H 1

#include "../enum.hpp"

#include "../data/variant.hpp"

namespace bee { namespace soundeffects {
	std::map<Variant,Variant> get_params(E_SOUNDEFFECT);

	void chorus(int, void*, int, void*);
	void chorus_cleanup(int, void*);
	void echo(int, void*, int, void*);
	void echo_cleanup(int, void*);
	void flanger(int, void*, int, void*);
	void flanger_cleanup(int, void*);
	void gargle(int, void*, int, void*);
	void gargle_cleanup(int, void*);
	void reverb(int, void*, int, void*);
	void reverb_cleanup(int, void*);
	void compressor(int, void*, int, void*);
	void compressor_cleanup(int, void*);
	void equalizer(int, void*, int, void*);
	void equalizer_cleanup(int, void*);
}}

#endif // BEE_CORE_SOUNDEFFECTS_H
