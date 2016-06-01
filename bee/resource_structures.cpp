/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RESOURCE_STRUCTURES
#define _BEE_RESOURCE_STRUCTURES 1

#include "resource_structures.hpp"

ResourceList::ResourceList() {
	next_index = 0;
}
int ResourceList::reset(BEE* new_game) {
	game = new_game;
	resources.clear();
	next_index = 0;
	return 0;
}
int ResourceList::add_resource(Resource* new_resource) {
	int index = next_index;
	std::pair<std::map<int,Resource*>::iterator,bool> ret;
	ret = resources.insert(std::pair<int,Resource*>(index, new_resource));
	if (ret.second) {
		next_index++;
		return index;
	}
	return -1;
}
int ResourceList::set_resource(int id, Resource* resource) {
	resources.at(id) = resource;
	return 0;
}
Resource* ResourceList::get_resource(int id) {
	if (resources.find(id) != resources.end()) {
		return resources.at(id);
	}
	return NULL;
}
int ResourceList::get_amount() {
	return next_index; //resources.size();
}
int ResourceList::remove_resource(int id) {
	resources.erase(id);
	return 0;
}

MetaResourceList::MetaResourceList(BEE* game) {
	reset(game);
}
int MetaResourceList::reset(BEE* game) {
	sprites.reset(game);
	sounds.reset(game);
	backgrounds.reset(game);
	fonts.reset(game);
	paths.reset(game);
	timelines.reset(game);
	objects.reset(game);
	lights.reset(game);
	rooms.reset(game);
	return 0;
}

#endif // _BEE_RESOURCE_STRUCTURES
