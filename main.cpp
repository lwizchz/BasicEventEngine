#include <stdio.h>

#include "ege/game.hpp"

int main(int argc, char* argv[]) {
	if (game.init(argc, argv)) {
		return 1;
	}
	
	/*RmTest rm_test;
	rm_test.print();
	rm_test.start();*/
	
	game.loop();
	
	game.close();
	
	return 0;
}
