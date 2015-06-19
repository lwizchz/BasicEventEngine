#include <iostream>
#include <string>
#include <map>

class Resource {};
class ResourceList {
		std::map<int,Resource> resources;
		int next_index;
	public:
		ResourceList();
		int add_resource(Resource);
		Resource get_resource(int);
		int get_amount();
		int remove_resource(int);
};
class MetaResourceList {
	public:
		ResourceList sprites, sounds, backgrounds, paths, objects; //, all;
} resource_list;

ResourceList::ResourceList() {
	next_index = 0;
}
int ResourceList::add_resource(Resource new_resource) {
	int index = next_index;
	std::pair<std::map<int,Resource>::iterator,bool> ret;
	ret = resources.insert(std::pair<int,Resource>(index, new_resource));
	if (ret.second) {
		next_index++;
		return index;
	}
	return -1;
}
Resource ResourceList::get_resource(int id) {
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
