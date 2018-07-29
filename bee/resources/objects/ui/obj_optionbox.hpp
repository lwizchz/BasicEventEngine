/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_OPTIONBOX_H
#define BEE_OBJ_UI_OPTIONBOX_H 1

#include "obj_element.hpp"

class ObjUIOptionBox : public ObjUIElement {
	public:
		ObjUIOptionBox();

		void create(bee::Instance*);
		void mouse_input(bee::Instance*, SDL_Event*);
		void mouse_release(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);

		void set_type(bee::Instance*, int);
		void reset_options(bee::Instance*);
		void push_option(bee::Instance*, const std::string&, bool, std::function<void (bee::Instance*, bool)>);
		void pop_option(bee::Instance*);

		std::string get_option(bee::Instance*, size_t);
		bool get_option_state(bee::Instance*, size_t);
		int get_option_at(bee::Instance*, int, int);
		std::vector<int> get_selected_options(bee::Instance*);

		void set_option_state(bee::Instance*, size_t, bool);
};

#endif // BEE_OBJ_UI_OPTIONBOX_H
