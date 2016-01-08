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

#ifndef _BEE_GAME_H
class BEE;
#endif

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
		BEE* game = NULL;
		std::map<int,Resource*> resources;
		int next_index;

		ResourceList();
		int reset(BEE*);
		int add_resource(Resource*);
		int set_resource(int, Resource*);
		Resource* get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites, sounds, backgrounds, fonts, paths, timelines, objects, rooms;

		MetaResourceList(BEE*);
		int reset(BEE*);
};

#endif // _BEE_RESOURCE_STRUCTURES_H
