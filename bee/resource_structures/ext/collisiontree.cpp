/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_COLLISIONTREE
#define _BEE_COLLISIONTREE 1

#include "collisiontree.hpp"

BEE::CollisionTree::CollisionTree(BEE* new_game, int x, int y, int w, int new_depth) {
	game = new_game;

	reset(x, y, w);

	depth = new_depth;
}
BEE::CollisionTree::~CollisionTree() {
	if (topleft != NULL) {
		delete topleft;
		delete topright;
		delete bottomleft;
		delete bottomright;
	} else {
		instances.clear();
	}
}
std::string BEE::CollisionTree::print() {
	std::string s = "";
	if (topleft == NULL) {
		for (auto& i : instances) {
			s += "\b\b\b\b\b" + i->object->get_name() + "\n";
		}
	} else {
		s = "tree\n";
		std::string t = debug_indent(topleft->print(), 1, "│");
		if (!trim(t).empty()) {
			s += t + "└────────\n";
		}
		t = debug_indent(topright->print(), 1, "│");
		if (!trim(t).empty()) {
			s += t + "└────────\n";
		}
		t = debug_indent(bottomleft->print(), 1, "│");
		if (!trim(t).empty()) {
			s += t + "└────────\n";
		}
		s += debug_indent(bottomright->print(), 1, "│");
	}

	return s;
}
int BEE::CollisionTree::draw() {
	if (topleft != NULL) {
		topleft->draw();
		topright->draw();
		bottomleft->draw();
		bottomright->draw();
	}

	return game->draw_rectangle(region.x, region.y, region.w, region.h, false, c_aqua, false);
}
int BEE::CollisionTree::insert(InstanceData* inst) {
	SDL_Rect r = {(int)inst->x, (int)inst->y, inst->get_width(), inst->get_height()};
	if (!check_collision(region, r)) {
		return 1;
	}

	if (topleft == NULL) {
		if (instances.size() < max_capacity) {
			instances.push_back(inst);
			std::sort(instances.begin(), instances.end());
			return 0;
		}

		if (divide()) {
			return 2;
		}
	}

	topleft->insert(inst);
	topright->insert(inst);
	bottomleft->insert(inst);
	bottomright->insert(inst);

	return 0;
}
int BEE::CollisionTree::remove(InstanceData* inst) {
	unsigned int n = 0;
	if (topleft == NULL) {
		instances.erase(std::remove_if(instances.begin(), instances.end(), [&](const InstanceData* i) {
			return (i == inst);
		}), instances.end());
		n = instances.size();
	} else {
		n += topleft->remove(inst);
		n += topright->remove(inst);
		n += bottomleft->remove(inst);
		n += bottomright->remove(inst);

		if (n < max_capacity) {
			combine();
		}
	}
	return (int)n;
}
int BEE::CollisionTree::divide() {
	if (depth+1 >= max_depth) {
		return 1;
	}

	int x = region.x, y = region.y, w = region.w/2;

	topleft = new CollisionTree(game, x, y, w, depth+1);
	topright = new CollisionTree(game, x+w, y, w, depth+1);
	bottomleft = new CollisionTree(game, x, y+w, w, depth+1);
	bottomright = new CollisionTree(game, x+w, y+w, w, depth+1);

	for (auto& i : instances) {
		topleft->insert(i);
		topright->insert(i);
		bottomleft->insert(i);
		bottomright->insert(i);
	}
	instances.clear();

	return 0;
}
int BEE::CollisionTree::combine() {
	if (topleft == NULL) {
		return 1;
	} else {
		int tl = topleft->combine();
		int tr = topright->combine();
		int bl = bottomleft->combine();
		int br = bottomright->combine();

		if (tl+tr+bl+br != 0) {
			instances.reserve(max_capacity);
		}

		if (tl == 1) {
			instances.insert(instances.end(), topleft->instances.begin(), topleft->instances.end());
		}
		if (tr == 1) {
			instances.insert(instances.end(), topright->instances.begin(), topright->instances.end());
		}
		if (bl == 1) {
			instances.insert(instances.end(), bottomleft->instances.begin(), bottomleft->instances.end());
		}
		if (br == 1) {
			instances.insert(instances.end(), bottomright->instances.begin(), bottomright->instances.end());
		}

		std::sort(instances.begin(), instances.end());

		delete topleft;
		topleft = NULL;
		delete topright;
		topright = NULL;
		delete bottomleft;
		bottomleft = NULL;
		delete bottomright;
		bottomright = NULL;
	}

	return 0;
}
int BEE::CollisionTree::reset(int x, int y, int w) {
	region.x = x;
	region.y = y;
	region.w = w;
	region.h = w;

	depth = 0;
	max_capacity = default_capacity;

	instances.clear();
	instances.reserve(max_capacity);

	if (topleft != NULL) {
		topleft->reset(x, y, w);
		topright->reset(x, y, w);
		bottomleft->reset(x, y, w);
		bottomright->reset(x, y, w);

		delete topleft;
		delete topright;
		delete bottomleft;
		delete bottomright;

		topleft = NULL;
		topright = NULL;
		bottomleft = NULL;
		bottomright = NULL;
	}

	return 0;
}
int BEE::CollisionTree::set_capacity(int new_capacity) {
	max_capacity = new_capacity;

	if (topleft != NULL) {
		topleft->set_capacity(max_capacity);
		topright->set_capacity(max_capacity);
		bottomleft->set_capacity(max_capacity);
		bottomright->set_capacity(max_capacity);
	} else {
		instances.reserve(max_capacity);
	}

	return 0;
}

std::vector<BEE::InstanceData*> BEE::CollisionTree::get_instances() {
	if (topleft == NULL) {
		return instances;
	}

	std::vector<InstanceData*> r1 = topleft->get_instances();
	std::vector<InstanceData*> r2 = topright->get_instances();
	std::vector<InstanceData*> r3 = bottomleft->get_instances();
	std::vector<InstanceData*> r4 = bottomright->get_instances();

	std::vector<InstanceData*> r;
	r.reserve(r1.size() + r2.size() + r3.size() + r4.size());
	r.insert(r.end(), r1.begin(), r1.end());
	r.insert(r.end(), r2.begin(), r2.end());
	r.insert(r.end(), r3.begin(), r3.end());
	r.insert(r.end(), r4.begin(), r4.end());

	return r;
}
int BEE::CollisionTree::check_collisions() {
	if (topleft == NULL) {
		if (instances.size() < 2) {
			return 0;
		}

		for (auto& i1 : instances) {
			i1->mask.x = (int)i1->x;
			i1->mask.y = (int)i1->y;

			for (auto& i2 : instances) {
				if (i1 == i2) {
					continue;
				}

				i1->object->update(i1);
				if (!i1->object->check_collision_list(i2->object)) {
					continue;
				}
				i2->object->update(i2);
				if (!i2->object->check_collision_list(i1->object)) {
					continue;
				}

				i2->mask.x = (int)i2->x;
				i2->mask.y = (int)i2->y;

				if (i1->check_collision_polygon(i2->mask)) {
					if ((i1->object->get_is_solid())&&(i2->object->get_is_solid())) {
						if ((i1->x != i1->xprevious)||(i1->y != i1->yprevious)) {
							i1->x = i1->xprevious;
							i1->y = i1->yprevious;
							i1->move_avoid(i2->mask);
						}

						if ((i2->x != i2->xprevious)||(i2->y != i2->yprevious)) {
							i2->x = i2->xprevious;
							i2->y = i2->yprevious;
							i2->move_avoid(i1->mask);
						}
					}

					i1->object->update(i1);
					i1->object->collision(i1, i2);
					i2->object->update(i2);
					i2->object->collision(i2, i1);
				}
			}
		}
	} else {
		topleft->check_collisions();
		topright->check_collisions();
		bottomleft->check_collisions();
		bottomright->check_collisions();
	}

	return 0;
}

#endif // _BEE_COLLISIONTREE
