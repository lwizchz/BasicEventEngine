/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_OPTIONBOX_H
#define BEE_UI_OBJ_OPTIONBOX_H 1

#include "element.hpp"

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

		std::string get_option(bee::Instance*, int);
		bool get_option_state(bee::Instance*, int);
		int get_option_at(bee::Instance*, int, int);
		std::vector<int> get_selected_options(bee::Instance*);

		void set_option_state(bee::Instance*, int, bool);
};

#endif // BEE_UI_OBJ_OPTIONBOX_H
