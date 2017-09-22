/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_NETWORKING
#define TESTS_UTIL_NETWORKING 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/networking.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("networking") {
	REQUIRE(network_init() == 0);
	int port = 3054;
	IPaddress* ipa = network_resolve_host("127.0.0.1", port);
	REQUIRE(ipa != nullptr);
	REQUIRE(network_get_address(ipa->host) == "127.0.0.1");
	delete ipa;

	TCPsocket tcp = network_tcp_open("", port);
	REQUIRE(tcp != static_cast<TCPsocket>(nullptr));
	REQUIRE(network_tcp_close(&tcp) == 0);
	REQUIRE(tcp == static_cast<TCPsocket>(nullptr));

	UDPsocket udp = network_udp_open(port);
	REQUIRE(udp != static_cast<UDPsocket>(nullptr));
	REQUIRE(network_udp_bind(&udp, -1, "127.0.0.1", port) != -1);
	REQUIRE(network_udp_unbind(&udp, -1) == 0);
	REQUIRE(network_udp_close(&udp) == 0);
	REQUIRE(udp == static_cast<UDPsocket>(nullptr));

	REQUIRE(network_close() == 0);
}

TEST_SUITE_END();

#endif // TESTS_UTIL_NETWORKING
