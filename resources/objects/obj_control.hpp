/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_CONTROL_H
#define RES_OBJ_CONTROL_H 1

class ObjControl : public bee::Object {
	public:
		ObjControl();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void step_mid(bee::Instance*);
		void mouse_press(bee::Instance*, SDL_Event*);
		void draw(bee::Instance*);
		void room_start(bee::Instance*);
		void game_start(bee::Instance*);
};

#endif // RES_OBJ_CONTROL_H
