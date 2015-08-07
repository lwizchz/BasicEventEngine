/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

class RmTest: public BEE::Room {
	public:
		RmTest();
		int init();
};
RmTest::RmTest() : Room("RmTest", "test.hpp") {}

int RmTest::init() {
	// set backgrounds
	add_background(-1, bk_green, true, false, 0, 0, true, true, 10, 10, false);

	// set up instances
	add_instance(-1, obj_bee, 0, 0);

	return 0;
}
