/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_ELEMENT_H
#define BEE_UI_OBJ_ELEMENT_H 1

#include "../../resource/object.hpp"

class ObjUIElement : public bee::Object {
	public:
		ObjUIElement(const std::string&, const std::string&);
		virtual ~ObjUIElement();

		virtual void create(bee::Instance*);
		virtual void destroy(bee::Instance*);
		virtual void mouse_press(bee::Instance*, SDL_Event*);
		virtual void mouse_input(bee::Instance*, SDL_Event*);
		virtual void mouse_release(bee::Instance*, SDL_Event*);
		virtual void draw(bee::Instance*) =0;

		virtual void reset(bee::Instance*);
		virtual void set_focus(bee::Instance*, bool);
		virtual void set_is_visible(bee::Instance*, bool);
		virtual void set_color(bee::Instance*, const bee::RGBA&);
};

#endif // BEE_UI_OBJ_ELEMENT_H
