/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_TEXTENTRY_H
#define BEE_UI_OBJ_TEXTENTRY_H 1

#include "element.hpp"

class ObjUITextEntry : public ObjUIElement {
	public:
		ObjUITextEntry();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void keyboard_press(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		void reset(bee::Instance*);
		void reset_completion(bee::Instance*);
		void set_size(bee::Instance*, int, int);
		void set_input(bee::Instance*, const std::string&);

		std::string get_input(bee::Instance*) const;

		void complete(bee::Instance*, const std::string&);
};

#endif // BEE_UI_OBJ_TEXTENTRY_H
