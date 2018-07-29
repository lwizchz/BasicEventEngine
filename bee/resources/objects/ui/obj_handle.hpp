/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_HANDLE_H
#define BEE_OBJ_UI_HANDLE_H 1

#include "obj_element.hpp"

class ObjUIHandle : public ObjUIElement {
	public:
		ObjUIHandle();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void step_end(bee::Instance*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void mouse_input(bee::Instance*, SDL_Event*);
		void mouse_release(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		void bind(bee::Instance*, bee::Instance*);
};

#endif // BEE_OBJ_UI_HANDLE_H
