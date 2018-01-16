/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_SLIDER_H
#define BEE_UI_OBJ_SLIDER_H 1

#include "element.hpp"

class ObjUISlider : public ObjUIElement {
	public:
		ObjUISlider();

		void create(bee::Instance*);
		void mouse_input(bee::Instance*, SDL_Event*);
		void mouse_release(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		int get_range(bee::Instance*);
		int get_value(bee::Instance*);
		void set_range(bee::Instance*, int);
		void set_value(bee::Instance*, int);
		void show_text(bee::Instance*, bool);
		void set_is_continuous(bee::Instance*, bool);

		void set_track_color(bee::Instance*, bee::RGBA);
};

#endif // BEE_UI_OBJ_SLIDER_H
