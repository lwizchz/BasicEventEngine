/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_OBJ_UI_GAUGE_H
#define BEE_OBJ_UI_GAUGE_H 1

#include "obj_element.hpp"

class ObjUIGauge : public ObjUIElement {
	public:
		ObjUIGauge();

		void create(bee::Instance*);
		void draw(bee::Instance*);

		int get_range(bee::Instance*);
		int get_value(bee::Instance*);
		void set_range(bee::Instance*, int);
		void set_value(bee::Instance*, int);
		void start_pulse(bee::Instance*);
		void stop_pulse(bee::Instance*);

		void set_color_back(bee::Instance*, bee::RGBA);
};

#endif // BEE_OBJ_UI_GAUGE_H
