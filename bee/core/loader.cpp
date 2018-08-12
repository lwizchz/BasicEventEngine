/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_LOADER
#define BEE_CORE_LOADER 1

#include <vector>
#include <memory>
#include <algorithm>

#include "loader.hpp"

#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

#include "../resource/resource.hpp"

namespace bee { namespace loader {
	namespace internal {
		std::vector<Resource*> resources;
		std::vector<Resource*>::iterator next_resource = resources.end();

		std::unordered_map<Resource*,std::function<int (Resource*)>> custom_loaders;

		size_t amount_loaded = 0;
		size_t total_amount = 0;
		size_t lazy_amount = 0;

		bool has_recipient = false;
	}

	/**
	* Load the next Resource and update the queue.
	*
	* @retval 0 success
	* @retval 1 there are no more Resources to load
	* @retval 2 the Resource failed to load
	*/
	int internal::load_next() {
		while ((next_resource != resources.end())&&((*next_resource)->get_is_loaded())) {
			++next_resource;
			amount_loaded++;
		}

		if (next_resource == resources.end()) {
			return 1;
		}

		messenger::send({"engine", "loader"}, E_MESSAGE::INTERNAL, "Loading resource \"" + (*next_resource)->get_name() + "\"...");

		int r = 0;
		std::unordered_map<Resource*,std::function<int (Resource*)>>::iterator loadfunc = custom_loaders.find(*next_resource);
		if (loadfunc == custom_loaders.end()) {
			r = (*next_resource)->load();
		} else {
			r = loadfunc->second(*next_resource);
		}

		++next_resource;
		amount_loaded++;

		return (r) ? 2 : 0;
	}
	/**
	* Lazily load the next amount of Resources from the queue.
	*
	* @retval 0 success
	* @retval >0 some Resources failed to load
	*/
	int internal::load_lazy() {
		int r = 0;
		for (size_t i=0; i<lazy_amount; ++i) {
			r += (load_next() != 0);
		}

		if (next_resource != resources.end()) {
			if (lazy_amount == 1) {
				messenger::send({"engine", "loader", "lazysignal"}, E_MESSAGE::INTERNAL, "Lazily loading the next resource");
			} else {
				messenger::send({"engine", "loader", "lazysignal"}, E_MESSAGE::INTERNAL, "Lazily loading the next " + std::to_string(lazy_amount) + " resources");
			}
		}

		return r;
	}

	/**
	* Add the given Resource to the loader queue.
	* @param res the Resource to add
	*/
	void queue(Resource* res) {
		internal::resources.push_back(res);
		++internal::total_amount;
	}
	/**
	* Add the given Resource to the queue with a custom loader.
	* @param res the Resource to add
	* @param loadfunc the function to call instead of the default load() method
	*/
	void queue(Resource* res, std::function<int (Resource*)> loadfunc) {
		queue(res);
		internal::custom_loaders.emplace(res, loadfunc);
	}
	/**
	* Clear the queue.
	*/
	void clear() {
		internal::resources.clear();
		internal::next_resource = internal::resources.end();

		internal::custom_loaders.clear();

		internal::amount_loaded = 0;
		internal::total_amount = 0;
	}

	/**
	* Immediately load the entire queue.
	*
	* @retval 0 success
	* @retval >0 some Resources failed to load
	*/
	int load() {
		internal::amount_loaded = 0;
		internal::total_amount = internal::resources.size();
		internal::next_resource = internal::resources.begin();

		int r = 0;
		while (internal::next_resource != internal::resources.end()) {
			r += internal::load_next();
		}

		return r;
	}
	/**
	* Load the given number of Resources per frame from the queue.
	* @param amount the amount to load per frame
	*
	* @retval 0 success
	* @retval >0 some Resources failed to load
	*/
	int load_lazy(size_t amount) {
		if (!internal::has_recipient) { // If lazy loading hasn't been initialized, register with the messenger
			messenger::internal::register_protected(
				"lazy_loader",
				{"engine", "loader", "lazysignal"},
				true,
				[] (const MessageContents& msg) {
					loader::internal::load_lazy();
				}
			);
			internal::has_recipient = true;
		}

		if (amount < 1) {
			amount = 1;
		}

		internal::amount_loaded = 0;
		internal::total_amount = internal::resources.size();
		internal::lazy_amount = amount;
		internal::next_resource = internal::resources.begin();

		return internal::load_lazy();
	}
	/**
	* Load one Resource per frame from the queue.
	*
	* @retval 0 success
	* @retval 1 the Resource failed to load
	*/
	int load_lazy() {
		return load_lazy(1);
	}

	/**
	* @returns the amount of Resources that have been loaded
	*/
	size_t get_amount_loaded() {
		return internal::amount_loaded;
	}
	/**
	* @returns the total amount of queued Resources
	*/
	size_t get_total() {
		return internal::total_amount;
	}
}}

#endif // BEE_CORE_LOADER
