/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RESOURCE_STRUCTURES_H
#define _BEE_RESOURCE_STRUCTURES_H 1

#include <string>
#include <map>

class Resource {
	public:
		BEE* game = NULL;

		virtual ~Resource() {};
		virtual int reset() =0;
		virtual int print() =0;
		virtual int get_id() =0;
		virtual std::string get_name() =0;
		virtual std::string get_path() =0;
		virtual int set_name(std::string) =0;
		virtual int set_path(std::string) =0;
};
class ResourceList {
	public:
		std::map<int,Resource*> resources;
		int next_index;

		ResourceList();
		int reset();
		int add_resource(Resource*);
		int set_resource(int, Resource*);
		Resource* get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites, sounds, backgrounds, fonts, paths, objects, rooms;
};

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

#endif // _BEE_RESOURCE_STRUCTURES_H
