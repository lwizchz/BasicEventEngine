#include <stdio.h>

#include "resource_structures.hpp"
#include "game.hpp"
#include "resources.hpp"

int main(int argc, char* argv[]) {
	init_video(argc, argv);
	init_sound();
	init_resources();
	
	RmTest rm_test;
	rm_test.print();
	rm_test.start();
	
	close_resources();
	close_sound();
	close_video();
	
	return 0;
}
