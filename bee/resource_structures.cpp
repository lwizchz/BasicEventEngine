/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
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
int ResourceList::reset() {
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

int MetaResourceList::reset() {
	sprites.reset();
	sounds.reset();
	backgrounds.reset();
	fonts.reset();
	paths.reset();
	timelines.reset();
	objects.reset();
	rooms.reset();
	return 0;
}

#endif // _BEE_RESOURCE_STRUCTURES
