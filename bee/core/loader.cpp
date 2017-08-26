/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_LOADER
#define BEE_CORE_LOADER 1

#include <vector>
#include <memory>

#include "loader.hpp"

#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

#include "../resource/resource.hpp"

namespace bee { namespace loader {
	namespace internal {
		std::vector<Resource*> resources;
		std::vector<Resource*>::iterator next_resource = resources.end();

		size_t amount_loaded = 0;
		size_t total_amount = 0;
		size_t lazy_amount = 0;

		std::shared_ptr<MessageRecipient> lazy_recipient = nullptr;
	}

	/*
	* internal::load_next() - Load the next resource and update the queue
	*/
	int internal::load_next() {
		if (next_resource == resources.end()) {
			return 1;
		}

		(*next_resource)->load();
		++next_resource;
		amount_loaded++;

		return 0;
	}
	/*
	* internal::load_lazy() - Lazily load the next amount of resources from the queue
	*/
	int internal::load_lazy() {
		for (size_t i=0; i<lazy_amount; ++i) {
			load_next();
		}

		if (next_resource != resources.end()) {
			messenger::send({"engine", "loader"}, E_MESSAGE::INTERNAL, "Lazily load the next " + bee_itos(lazy_amount) + " resources");
		}

		return 0;
	}

	/*
	* queue() - Add the given resource to the loader queue
	* @res: the resource to add
	*/
	int queue(Resource* res) {
		internal::resources.push_back(res);
		++internal::total_amount;
		return 0;
	}
	/*
	* clear() - Clear the queue
	*/
	int clear() {
		internal::resources.clear();
		internal::next_resource = internal::resources.end();

		internal::amount_loaded = 0;
		internal::total_amount = 0;

		return 0;
	}

	/*
	* load() - Immediately load the entire queue
	*/
	int load() {
		internal::amount_loaded = 0;
		internal::total_amount = internal::resources.size();
		internal::next_resource = internal::resources.begin();

		while (internal::next_resource != internal::resources.end()) {
			internal::load_next();
		}

		return 0;
	}
	/*
	* load_lazy() - Load the given number of resources per frame from the queue
	* @amount: the amount to load per frame
	*/
	int load_lazy(int amount) {
		if (internal::lazy_recipient == nullptr) { // If lazy loading hasn't been initialized, register with the messenger
			internal::lazy_recipient = std::shared_ptr<MessageRecipient>(new MessageRecipient(
				"lazy_loader",
				{"engine", "loader"},
				true,
				[] (std::shared_ptr<MessageContents> msg) {
					loader::internal::load_lazy();
				}
			));
			messenger::internal::register_protected(internal::lazy_recipient);
		}

		internal::amount_loaded = 0;
		internal::total_amount = internal::resources.size();
		internal::lazy_amount = amount;
		internal::next_resource = internal::resources.begin();

		internal::load_lazy();

		return 0;
	}
	/*
	* load_lazy() - Load one resource per frame from the queue
	*/
	int load_lazy() {
		return load_lazy(1);
	}

	size_t get_amount_loaded() {
		return internal::amount_loaded;
	}
	size_t get_total() {
		return internal::total_amount;
	}
}}

#endif // BEE_CORE_LOADER
