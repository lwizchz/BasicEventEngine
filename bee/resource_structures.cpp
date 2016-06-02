/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RESOURCE_STRUCTURES
#define _BEE_RESOURCE_STRUCTURES 1

#include "resource_structures.hpp" // Include the class declarations

/*
* ResourceList::ResourceList() - Construct the list with its index set to 0
*/
ResourceList::ResourceList() {
	next_index = 0;
}
/*
* ResourceList::reset() - Reset the list with a pointer to the engine
* @new_game: a pointer to the current engine instantiation
*/
int ResourceList::reset(BEE* new_game) {
	game = new_game; // Set the engine pointer
	resources.clear(); // Clear the internal list
	next_index = 0; // Reset the index
	return 0; // Return 0 on success
}
/*
* ResourceList::add_resource() - Add a resource to the list
* @new_resource: the pointer of the resource to add to the list
*/
int ResourceList::add_resource(Resource* new_resource) {
	int index = next_index; // Get the index of the resource which is currently being added
	std::pair<std::map<int,Resource*>::iterator,bool> ret;
	ret = resources.insert(std::pair<int,Resource*>(index, new_resource)); // Insert the pointer to the resource along with its index
	if (ret.second) { // If the resource was successfully added
		next_index++; // Increment the index
		return index; // Return the index of the added resource on success
	}
	return -1; // Return -1 on failure to add the resource
}
/*
* ResourceList::set_resource() - Set a resource index to a given resource pointer
* @id: the index to change
* @resource: the pointer to set it to
*/
int ResourceList::set_resource(int id, Resource* resource) {
	resources.at(id) = resource;
	return 0; // Return 0 on success
}
/*
* ResourceList::get_resource() - Return the pointer to the requested resource
* @id: the index of the requested resource
*/
Resource* ResourceList::get_resource(int id) {
	if (resources.find(id) != resources.end()) { // If the index exists in the list
		return resources.at(id); // Return the resource pointer on success
	}
	return nullptr; // Otherwise return nullptr
}
/*
* ResourceList::get_amount() - Return the next index
* ! This does not return the amount of resources in the list because if some
	have been deleted in the middle, then the new resources should still
	be added to the very end
*/
int ResourceList::get_amount() {
	return next_index; // Return the index on success
}
/*
* ResourceList::remove_resource() - Remove the resource at the given index
* ! Note that this does not free the memory of the resource
* @id: the index of the resource to remove from the list
*/
int ResourceList::remove_resource(int id) {
	resources.erase(id); // Remove the resource from the list
	return 0; // Return 0 on success
}

/*
* MetaResourceList::MetaResourceList() - Construct the lists by resetting them and setting the engine pointer
* @game: a pointer to the current engine instantiation
*/
MetaResourceList::MetaResourceList(BEE* game) {
	reset(game);
}
/*
* MetaResourceList::reset() - Reset the resource lists
* @game: a pointer to the current engine instantiation
*/
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
	return 0; // Return 0 on success
}

#endif // _BEE_RESOURCE_STRUCTURES
