/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_LOADER_H
#define BEE_CORE_LOADER_H 1

#include <functional>

namespace bee {
	// Forward declaration
	class Resource;
namespace loader {
	namespace internal {
		int load_next();
		int load_lazy();
	}

	void queue(Resource*);
	void queue(Resource*, std::function<int (Resource*)>);
	void clear();

	int load();
	int load_lazy(size_t);
	int load_lazy();

	size_t get_amount_loaded();
	size_t get_total();
}}

#endif // BEE_CORE_ROOM_H
