/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_HANDLE_H
#define BEE_UI_OBJ_HANDLE_H 1

#include "element.hpp"

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

#endif // BEE_UI_OBJ_HANDLE_H
