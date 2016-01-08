/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SOUNDGROUP_H
#define _BEE_SOUNDGROUP_H 1

#include "../../game.hpp"

class BEE::SoundGroup {
	std::string name;

	std::map<std::string,Sound*> sounds;
	bool is_loaded = false;

	bool is_playing, is_looping;

	public:
		BEE* game = NULL;

		SoundGroup();
		SoundGroup(std::string, std::map<std::string,std::string>);
		~SoundGroup();
		int reset();
		int print();

		Sound* operator[] (const char*);
		std::map<std::string,Sound*>::iterator begin();
		std::map<std::string,Sound*>::iterator end();

		std::string get_name();
		bool get_is_looping();

		int set_name(std::string);
		int add_sound(std::string, std::string);
		int set_volume(std::string, double);
		int set_volume(double);
		int set_is_looping(bool);

		int load();
		int free();

		bool check_has_stopped();

		int play(std::string);
		int stop(std::string);
		int toggle(std::string);
		int play();
		int stop();
		int toggle();
};

#endif // _BEE_SOUNDGROUP_H
