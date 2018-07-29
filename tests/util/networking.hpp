/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_NETWORKING
#define TESTS_UTIL_NETWORKING 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/networking.hpp"

#include "../../bee/init/gameoptions.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("networking") {
	if (!bee::get_option("is_network_enabled").i) {
		return;
	}

	REQUIRE(util::network::init() == 0);
	int port = 3054;
	IPaddress* ipa = util::network::resolve_host("127.0.0.1", port);
	REQUIRE(ipa != nullptr);
	REQUIRE(util::network::get_address(ipa->host) == "127.0.0.1");
	delete ipa;

	TCPsocket tcp = util::network::tcp_open("", port);
	REQUIRE(tcp != static_cast<TCPsocket>(nullptr));
	util::network::tcp_close(&tcp);
	REQUIRE(tcp == static_cast<TCPsocket>(nullptr));

	UDPsocket udp = util::network::udp_open(port);
	REQUIRE(udp != static_cast<UDPsocket>(nullptr));
	REQUIRE(util::network::udp_bind(&udp, -1, "127.0.0.1", port) != -1);
	util::network::udp_unbind(&udp, -1);
	util::network::udp_close(&udp);
	REQUIRE(udp == static_cast<UDPsocket>(nullptr));

	util::network::close();
}

TEST_SUITE_END();

#endif // TESTS_UTIL_NETWORKING
