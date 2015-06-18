#include <stdio.h>
#include <string>
#include <iostream>

#include "resource_structures.hpp"

std::string array_to_string(char** array, int length) {
	std::string output = "{\n";
	for (int i=0; i<length; i++) {
		output.append("\t");
		output.append(array[i]);
		output.append("\n");
	}
	output.append("}\n");
	return output;
}

int main() {
	Sprite test ("spr_test", "sprites/test.png");
	test.print();
	//printf("name: %s\npath: %s\n", test.get_name().c_str(), test.get_path().c_str());
	//printf("amount: %d\npath: %s\n", resource_list.all.get_amount(), "a");//resource_list.sprites.get_resource(test.get_id()).get_path().c_str());
	
	return 0;
}
