/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RESOURCE_H
#define BEE_RESOURCE_H 1

namespace bee {
	/// The abstract class for all resource types
	class Resource {
		public:
			virtual ~Resource() {};
			virtual int reset() =0;
			virtual void print() const =0;
			virtual int get_id() const =0;
			virtual std::string get_name() const {return "";}
			virtual int load() {return 0;}
			virtual int free() {return 0;}
	};
}

#endif // BEE_RESOURCE_H
