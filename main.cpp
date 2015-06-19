#include <stdio.h>

#include "resource_structures.hpp"

int main() {
	Sprite spr_test ("spr_test", "sprites/test.png");
	Object obj_test ("obj_test", "objects/test.obj");
	obj_test.set_sprite(&spr_test);
	
	obj_test.print();
	
	//printf("name: %s\npath: %s\n", test.get_name().c_str(), test.get_path().c_str());
	//printf("amount: %d\npath: %s\n", resource_list.all.get_amount(), "a");//resource_list.sprites.get_resource(test.get_id()).get_path().c_str());
	
	return 0;
}
