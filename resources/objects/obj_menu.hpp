/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_MENU_H
#define RES_OBJ_MENU_H 1

class ObjMenu : public bee::Object {
	public:
		ObjMenu();

		void create(bee::Instance*);
		void draw(bee::Instance*);
		void step_mid(bee::Instance*);
		void game_start(bee::Instance*);
};

#endif // RES_OBJ_MENU_H
