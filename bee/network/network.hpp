/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_NETWORK_H
#define BEE_CORE_NETWORK_H 1

#include <string>
#include <map>

#include "networkdata.hpp"

namespace bee {
	// Forward declaration
	class Instance;

	int net_init();
	bool net_get_is_initialized();
	int net_close();
	int net_handle_events();
	const NetworkData& net_get_data();

	int net_session_start(const std::string&, int, const std::string&);
	std::map<std::string,std::string> net_session_find();
	int net_session_join(const std::string&, const std::string&);
	bool net_get_is_connected();
	int net_session_end();

	int net_session_sync_data(const std::string&, const std::string&);
	int net_session_sync_instance(Instance*);
}

#endif // BEE_CORE_NETWORK_H
