/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_BUTTON_H
#define BEE_OBJ_UI_BUTTON_H 1

#include "obj_element.hpp"

class ObjUIButton : public ObjUIElement {
	public:
		ObjUIButton();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void mouse_release(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		void center_width(bee::Instance*);
};

#endif // BEE_OBJ_UI_BUTTON_H
