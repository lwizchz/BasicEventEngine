/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_LATTICE_H
#define RES_OBJ_LATTICE_H 1

class ObjLattice : public bee::Object {
	public:
		ObjLattice();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void step_mid(bee::Instance*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);
};

#endif // RES_OBJ_LATTICE_H
