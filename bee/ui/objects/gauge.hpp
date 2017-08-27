/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_OBJ_GAUGE_H
#define BEE_UI_OBJ_GAUGE_H 1

#include "element.hpp"

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

#endif // BEE_UI_OBJ_GAUGE_H
