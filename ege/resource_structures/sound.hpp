/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _EGE_SOUND_H
#define _EGE_SOUND_H 1

#include <AL/alut.h>

#define SOUND_AMOUNT 8

class Sound: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string sound_path;
		float volume; // From 0.0 to 1.0
		float pan; // From -1.0 to 1.0 as Left to Right
		int play_type; // In memory vs continuous
		int channel_amount; // Mono vs stereo
		int sample_rate;
		int sample_format;
		int bit_rate;

		ALuint buffer[SOUND_AMOUNT];
		ALuint source[SOUND_AMOUNT];
		const int sound_amount = SOUND_AMOUNT;
		bool is_playing;
		int next_sound;
	public:
		Sound();
		Sound(std::string, std::string);
		~Sound();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		float get_volume();
		float get_pan();
		int get_play_type();
		int get_channel_amount();
		int get_sample_rate();
		int get_sample_format();
		int get_bit_rate();

		int set_name(std::string);
		int set_path(std::string);
		int set_volume(float);
		int set_pan(float);
		int set_play_type(int);
		int set_channel_ammount(int);
		int set_sample_rate(int);
		int set_sample_format(int);
		int set_bit_rate(int);

		int play();
		int stop();
		int rewind();
		int pause();
		int toggle();
		int loop();
		bool get_is_playing();
};
Sound::Sound () {
	id = -1;
	reset();
}
Sound::Sound (std::string new_name, std::string path) {
	id = -1;
	reset();

	add_to_resources("resources/sounds/"+path);
	if (id < 0) {
		std::cerr << "Failed to add sound resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
Sound::~Sound() {
	resource_list.sounds.remove_resource(id);
	alDeleteBuffers(SOUND_AMOUNT, buffer);
	alDeleteSources(SOUND_AMOUNT, source);
}
int Sound::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == sound_path) {
			return 1;
		}
		resource_list.sounds.remove_resource(id);
		id = -1;
	} else {
		for (auto i : resource_list.sounds.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.sounds.add_resource(this);
	}
	resource_list.sounds.set_resource(id, this);

	return 0;
}
int Sound::reset() {
	name = "";
	sound_path = "";
	volume = 1.0;
	pan = 0.0;
	play_type = 0;
	channel_amount = 1;
	sample_rate = 44100;
	sample_format = 16;
	bit_rate = 192;

	/*alDeleteBuffers(1, &buffer);
	buffer = AL_NONE;
	source = AL_NONE;*/
	is_playing = false;
	next_sound = 0;

	return 0;
}
int Sound::print() {
	std::cout <<
	"Sound { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	sound_path	" << sound_path <<
	"\n	play_type	" << play_type <<
	"\n	channel_amount	" << channel_amount <<
	"\n	sample_rate	" << sample_rate <<
	"\n	sample_format	" << sample_format <<
	"\n	bit_rate	" << bit_rate <<
	"\n	sound_amount	" << sound_amount <<
	"\n	is_playing	" << is_playing <<
	"\n	next_sound	" << next_sound <<
	"\n}\n";

	return 0;
}
int Sound::get_id() {
	return id;
}
std::string Sound::get_name() {
	return name;
}
std::string Sound::get_path() {
	return sound_path;
}
float Sound::get_volume() {
	return volume;
}
float Sound::get_pan() {
	return pan;
}
int Sound::get_play_type() {
	return play_type;
}
int Sound::get_channel_amount() {
	return channel_amount;
}
int Sound::get_sample_rate() {
	return sample_rate;
}
int Sound::get_sample_format() {
	return sample_format;
}
int Sound::get_bit_rate() {
	return bit_rate;
}
int Sound::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Sound::set_path(std::string path) {
	add_to_resources("resources/sounds/"+path);
	sound_path = "resources/sounds/"+path;

	// Load OpenAL sound
	alGenSources(sound_amount, source);
	for (int i=0; i<sound_amount; i++) {
		buffer[i] = alutCreateBufferFromFile(sound_path.c_str());
		alSourcei(source[i], AL_BUFFER, buffer[i]);

		// For stereo panning
		alSourcei(source[i], AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(source[i], AL_MAX_DISTANCE, 1.0f);
		alSourcef(source[i], AL_REFERENCE_DISTANCE, 0.5f);
	}

	return 0;
}
int Sound::set_volume(float new_volume) {
	volume = new_volume;
	for (int i=0; i<sound_amount; i++) {
		alSourcef(source[i], AL_GAIN, volume);
	}
	return 0;
}
int Sound::set_pan(float new_pan) {
	pan = new_pan;
	for (int i=0; i<sound_amount; i++) {
		alSource3f(source[i], AL_POSITION, pan, 0.0, 0.0);
	}
	//alListener3f(AL_POSITION, pan, 0.0, 0.0);
	return 0;
}
int Sound::set_play_type(int new_play_type) {
	play_type = new_play_type;
	return 0;
}
int Sound::set_channel_ammount(int new_channel_amount) {
	channel_amount = new_channel_amount;
	return 0;
}
int Sound::set_sample_rate(int new_sample_rate) {
	sample_rate = new_sample_rate;
	return 0;
}
int Sound::set_sample_format(int new_sample_format) {
	sample_format = new_sample_format;
	return 0;
}
int Sound::set_bit_rate(int new_bit_rate) {
	bit_rate = new_bit_rate;
	return 0;
}
int Sound::play() {
	is_playing = true;
	alSourcePlay(source[next_sound++]);
	if (next_sound > sound_amount-1) {
		next_sound = 0;
	}
	return 0;
}
int Sound::stop() {
	is_playing = false;
	for (int i=0; i<sound_amount; i++) {
		alSourceStop(source[i]);
	}
	return 0;
}
int Sound::rewind() {
	for (int i=0; i<sound_amount; i++) {
		alSourceRewind(source[i]);
	}
	return 0;
}
int Sound::pause() {
	is_playing = false;
	for (int i=0; i<sound_amount; i++) {
		alSourcePause(source[i]);
	}
	next_sound--;
	return 0;
}
int Sound::toggle() {
	if (is_playing) {
		pause();
	} else {
		play();
	}
	return 0;
}
int Sound::loop() {
	alSourcei(source[next_sound], AL_LOOPING, AL_TRUE);
	play();
	return 0;
}
bool Sound::get_is_playing() {
	return is_playing;
}

#endif // _EGE_SOUND_H
