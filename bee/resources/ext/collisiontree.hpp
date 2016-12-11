/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_COLLISIONTREE_H
#define _BEE_COLLISIONTREE_H 1

#include "../../game.hpp"

class BEE::CollisionTree {
	public:
		const unsigned int max_depth = 8;
		const unsigned int default_capacity = 16;
		unsigned int max_capacity = default_capacity;

		BEE* game = nullptr;
		SDL_Rect region;
		std::vector<BEE::InstanceData*> instances;

		unsigned int depth = 0;
		CollisionTree* topleft = nullptr;
		CollisionTree* topright = nullptr;
		CollisionTree* bottomleft = nullptr;
		CollisionTree* bottomright = nullptr;

		CollisionTree(BEE*, int, int, int, int);
		~CollisionTree();
		std::string print();
		int draw();

		int insert(BEE::InstanceData*);
		int remove(BEE::InstanceData*);
		int divide();
		int combine();
		int reset(int, int, int);
		int set_capacity(int);

		std::vector<BEE::InstanceData*> get_instances();
		int check_collisions();
};

#endif // _BEE_COLLISIONTREE_H
