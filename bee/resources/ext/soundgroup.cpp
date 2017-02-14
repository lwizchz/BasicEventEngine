/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SOUNDGROUP
#define _BEE_SOUNDGROUP 1

#include "soundgroup.hpp"

BEE::SoundGroup::SoundGroup() {
	reset();
}
BEE::SoundGroup::SoundGroup(std::string new_name, std::map<std::string,std::string> sound_file_list) {
	reset();

	set_name(new_name);

	for (auto& new_sound : sound_file_list) {
		add_sound(new_sound.first, new_sound.second);
	}
}
BEE::SoundGroup::~SoundGroup() {
	if (is_loaded) {
		free();
	}
}
int BEE::SoundGroup::reset() {
	name = "";

	sounds.clear();
	is_loaded = false;

	is_playing = false;
	is_looping = false;

	return 0;
}
int BEE::SoundGroup::print() {
	std::stringstream s;
	s <<
	"SoundGroup { "
	//"\n	id		" << id <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}

BEE::Sound* BEE::SoundGroup::operator[] (const char* sound_name) {
	return sounds[sound_name];
}
std::map<std::string,BEE::Sound*>::iterator BEE::SoundGroup::begin() {
	return sounds.begin();
}
std::map<std::string,BEE::Sound*>::iterator BEE::SoundGroup::end() {
	return sounds.end();
}

std::string BEE::SoundGroup::get_name() {
	return name;
}
bool BEE::SoundGroup::get_is_looping() {
	return is_looping;
}

int BEE::SoundGroup::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::SoundGroup::add_sound(std::string sound_name, std::string sound_path) {
	Sound* new_sound = new Sound(sound_name, sound_path, false);
	new_sound->set_volume(0.0);
	sounds.insert(std::make_pair(sound_name, new_sound));
	return 0;
}
int BEE::SoundGroup::set_volume(std::string sound_name, double new_volume) {
	if (sounds.find(sound_name) == sounds.end()) {
		return 1;
	}

	sounds[sound_name]->set_volume(new_volume);

	return 0;
}
int BEE::SoundGroup::set_volume(double new_volume) {
	for (auto& s : sounds) {
		set_volume(s.first, new_volume);
	}
	return 0;
}
int BEE::SoundGroup::set_is_looping(bool new_is_looping) {
	is_looping = new_is_looping;
	return 0;
}

int BEE::SoundGroup::load() {
	for (auto& s : sounds) {
		if (s.second->load()) {
			game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to load " + s.first + " from soundgroup " + name);
			return 1;
		}
	}

	is_loaded = true;

	return 0;
}
int BEE::SoundGroup::free() {
	for (auto& s : sounds) {
		s.second->free();
	}

	is_loaded = false;

	return 0;
}

bool BEE::SoundGroup::check_has_stopped() {
	for (auto& s : sounds) {
		if ((s.second->get_is_playing())||(s.second->get_is_looping())) {
			return false;
		}
	}
	return true;
}

int BEE::SoundGroup::play(std::string sound_name) {
	if (sounds.find(sound_name) == sounds.end()) {
		return 1;
	}

	if (check_has_stopped()) {
		is_playing = false;
	}

	if (!is_playing) {
		for (auto& s : sounds) {
			s.second->set_volume(0.0);
			if (is_looping) {
				s.second->loop();
			} else {
				s.second->play();
			}
		}
	}
	sounds[sound_name]->set_volume(1.0);
	is_playing = true;

	return 0;
}
int BEE::SoundGroup::stop(std::string sound_name) {
	if (sounds.find(sound_name) == sounds.end()) {
		return 1;
	}

	sounds[sound_name]->set_volume(0.0);

	return 0;
}
int BEE::SoundGroup::toggle(std::string sound_name) {
	if (sounds.find(sound_name) == sounds.end()) {
		return 1;
	}

	if (check_has_stopped()) {
		is_playing = false;
	}

	if (!is_playing) {
		play(sound_name);
	} else {
		if (sounds[sound_name]->get_volume() == 0.0) {
			sounds[sound_name]->set_volume(1.0);
		} else {
			sounds[sound_name]->set_volume(0.0);
		}
	}

	return 0;
}
int BEE::SoundGroup::play() {
	for (auto& s : sounds) {
		play(s.first);
	}
	is_playing = true;
	return 0;
}
int BEE::SoundGroup::stop() {
	for (auto& s : sounds) {
		stop(s.first);
		s.second->stop();
	}
	is_playing = false;
	return 0;
}
int BEE::SoundGroup::toggle() {
	for (auto& s : sounds) {
		toggle(s.first);
	}
	return 0;
}

#endif // _BEE_SOUNDGROUP
