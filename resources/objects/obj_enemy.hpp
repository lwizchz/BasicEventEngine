/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_OBJ_ENEMY_H
#define RES_OBJ_ENEMY_H 1

class ObjEnemy : public bee::Object {
	public:
		ObjEnemy();

		void create(bee::Instance*);
		void destroy(bee::Instance*);
		void step_mid(bee::Instance*);
		void draw(bee::Instance*);

		void update_position(bee::Instance*);
		void hurt(bee::Instance*, int);
};

#endif // RES_OBJ_ENEMY_H
