/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_BUTTON_H
#define BEE_UI_OBJ_BUTTON_H 1

#include "element.hpp"

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

#endif // BEE_UI_OBJ_BUTTON_H
