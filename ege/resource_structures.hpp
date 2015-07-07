#include <iostream>
#include <string>
#include <map>

#include "debug.hpp"

class Resource {
	public:
		virtual int reset() =0;
		virtual int print() =0;
		virtual int get_id() =0;
		virtual std::string get_name() =0;
		virtual std::string get_path() =0;
		virtual int set_name(std::string) =0;
		virtual int set_path(std::string) =0;
};
class ResourceList {
		std::map<int,Resource*> resources;
		int next_index;
	public:
		ResourceList();
		int add_resource(Resource*);
		Resource* get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites, sounds, backgrounds, paths, objects, rooms; //, all;
} resource_list;

ResourceList::ResourceList() {
	next_index = 0;
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
Resource* ResourceList::get_resource(int id) {
	return resources[id];
}
int ResourceList::get_amount() {
	return resources.size();
}
int ResourceList::remove_resource(int id) {
	resources.erase(id);
	return 0;
}

#include "resource_structures/sprite.hpp"
#include "resource_structures/sound.hpp"
#include "resource_structures/background.hpp"
#include "resource_structures/path.hpp"
#include "resource_structures/object.hpp"
#include "resource_structures/room.hpp"

Sprite* sprite(int id) {
	return dynamic_cast<Sprite*>(resource_list.sprites.get_resource(id));
}
Sound* sound(int id) {
	return dynamic_cast<Sound*>(resource_list.sounds.get_resource(id));
}
Background* background(int id) {
	return dynamic_cast<Background*>(resource_list.backgrounds.get_resource(id));
}
Path* path(int id) {
	return dynamic_cast<Path*>(resource_list.paths.get_resource(id));
}
Object* object(int id) {
	return dynamic_cast<Object*>(resource_list.objects.get_resource(id));
}
Room* room(int id) {
	return dynamic_cast<Room*>(resource_list.rooms.get_resource(id));
}
