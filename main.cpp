#include <stdio.h>

#include "ege/game.hpp"

int main(int argc, char* argv[]) {
	if (init_game(argc, argv)) {
		return 1;
	}
	
	RmTest rm_test;
	rm_test.print();
	rm_test.start();
	
	loop_game();
	
	close_game();
	
	return 0;
}
