/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_LOADER_H
#define BEE_CORE_LOADER_H 1

namespace bee {
	// Forward declaration
	class Resource;
namespace loader {
	namespace internal {
		int load_next();
		int load_lazy();
	}

	int queue(Resource*);

	int load();
	int load_lazy(int);
	int load_lazy();

	size_t get_amount_loaded();
	size_t get_total();
}}

#endif // BEE_CORE_ROOM_H
