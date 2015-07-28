/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INSTANCEDATA_H
#define _BEE_INSTANCEDATA_H 1

#define ALARM_COUNT 8

class InstanceData {
	public:
		BEE* game = NULL;

		int id=-1;
		Object* object=NULL;
		Uint32 subimage_time=0;
		float x=0.0, y=0.0;
		int vx=0, vy=0;
		std::list<std::pair<double,double>> velocity;
		double gravity=0.0;
		Uint32 alarm_end[ALARM_COUNT];

		InstanceData();
		InstanceData(BEE*, int, Object*, int, int);
		int init(int, Object*, int, int);
		int print();

		int remove();

		int move(double, double);
		int move_to(double, double, double);
		int move_away(double, double, double);
		int set_gravity(double);
		int set_alarm(int, Uint32);
};
InstanceData::InstanceData() {
	velocity.clear();
	for (int i=0; i<ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}
}
InstanceData::InstanceData(BEE* new_game, int new_id, Object* new_object, int new_x, int new_y) {
	game = new_game;
	init(new_id, new_object, new_x, new_y);
}
int InstanceData::init(int new_id, Object* new_object, int new_x, int new_y) {
	id = new_id;
	object = new_object;
	subimage_time = SDL_GetTicks();
	x = (float)new_x;
	y = (float)new_y;
	vx = x;
	vy = y;
	velocity.clear();
	gravity = 0.0;

	for (int i=0; i<ALARM_COUNT; i++) {
		alarm_end[i] = 0xffffffff;
	}

	return 0;
}
int InstanceData::print() {
	std::cout <<
	"InstanceData { "
	"\n	id		" << id <<
	"\n	object		" << object <<
	"\n	subimage_time	" << subimage_time <<
	"\n	x, y		" << x << ", " << y <<
	//"\n	velocity	" << velocity <<
	"\n	gravity		" << gravity <<
	"\n}\n";

	return 0;
}

int InstanceData::remove() {
	object->destroy(this);
	game->get_room()->remove_instance(id);
	return 0;
}

int InstanceData::move(double new_magnitude, double new_direction) {
	if (new_direction < 0.0) {
		new_direction = 360.0 + new_direction;
	}
	velocity.push_back(std::make_pair(new_magnitude, fmod(new_direction, 360.0)));
	return 0;
}
int InstanceData::move_to(double new_magnitude, double other_x, double other_y) {
	if (distance(x, y, other_x, other_y) < new_magnitude) {
		return 1;
	}
	move(new_magnitude, direction_of(x, y, other_x, other_y));
	return 0;
}
int InstanceData::move_away(double new_magnitude, double other_x, double other_y) {
	move(new_magnitude, direction_of(x, y, other_x, other_y)+180.0);
	return 0;
}
int InstanceData::set_gravity(double new_gravity) {
	gravity = new_gravity;
	return 0;
}
int InstanceData::set_alarm(int alarm, Uint32 elapsed_ticks) {
	alarm_end[alarm] = elapsed_ticks + SDL_GetTicks();
	return 0;
}

#endif // _BEE_INSTANCEDATA_H
