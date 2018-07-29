/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_BEE_H
#define RES_OBJ_BEE_H 1

class ObjBee : public bee::Object {
	public:
		ObjBee();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void commandline_input(bee::Instance*, const std::string&);
		void path_end(bee::Instance*, bee::PathFollower*);
		void outside_room(bee::Instance*);
		void draw(bee::Instance*);
};

#endif // RES_OBJ_BEE_H
