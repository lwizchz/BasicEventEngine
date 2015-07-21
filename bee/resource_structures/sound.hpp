/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SOUND_H
#define _BEE_SOUND_H 1

#include <list>

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

		bool is_loaded, is_music;
		Mix_Music* music;
		Mix_Chunk* chunk;
		bool is_playing, is_looping;
		std::list<int> current_channels;
	public:
		Sound();
		Sound(std::string, std::string, bool);
		~Sound();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		bool get_is_music();
		float get_volume();
		float get_pan();
		int get_play_type();
		int get_channel_amount();
		int get_sample_rate();
		int get_sample_format();
		int get_bit_rate();

		int set_name(std::string);
		int set_path(std::string);
		int set_is_music(bool);
		int set_volume(float);
		int set_pan(float);
		int set_pan_internal(int);
		int set_play_type(int);
		int set_channel_ammount(int);
		int set_sample_rate(int);
		int set_sample_format(int);
		int set_bit_rate(int);

		int load();
		int free();
		int finished(int);

		int play();
		int stop();
		int rewind();
		int pause();
		int resume();
		int toggle();
		int loop();
		bool get_is_playing();
		bool get_is_looping();
};
Sound::Sound () {
	id = -1;
	reset();
}
Sound::Sound (std::string new_name, std::string path, bool new_is_music) {
	id = -1;
	is_loaded = false;
	reset();

	add_to_resources("resources/sounds/"+path);
	if (id < 0) {
		std::cerr << "Failed to add sound resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
	set_is_music(new_is_music);
}
Sound::~Sound() {
	if (is_loaded) {
		free();
	}
	resource_list.sounds.remove_resource(id);
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
	if (is_loaded) {
		free();
	}

	name = "";
	sound_path = "";
	volume = 1.0;
	pan = 0.0;
	play_type = 0;
	channel_amount = 1;
	sample_rate = 44100;
	sample_format = 16;
	bit_rate = 192;

	is_loaded = false;
	is_music = false;
	is_playing = false;
	is_looping = false;
	current_channels.clear();

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
	"\n	is_playing	" << is_playing <<
	"\n	is_music	" << is_music <<
	"\n	is_looping	" << is_looping <<
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
bool Sound::get_is_music() {
	return is_music;
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

	return 0;
}
int Sound::set_is_music(bool new_is_music) {
	if (is_loaded) {
		free();
		is_music = new_is_music;
		return load();
	} else {
		is_music = new_is_music;
	}
	return 0;
}
int Sound::set_volume(float new_volume) {
	volume = new_volume;
	if (is_music) {
		Mix_VolumeMusic(volume*128);
	} else {
		Mix_VolumeChunk(chunk, volume*128);
		/*for (std::list<int>::iterator i=current_channels.begin(); i != current_channels.end(); ++i) {
			Mix_Volume(*i, volume*128);
		}*/
	}
	return 0;
}
int Sound::set_pan(float new_pan) {
	pan = new_pan;
	if (is_music) {
		return 1; // I'm not sure how to pan music at the moment
	} else {
		for (std::list<int>::iterator i=current_channels.begin(); i != current_channels.end(); ++i) {
			set_pan_internal(*i);
		}
	}
	return 0;
}
int Sound::set_pan_internal(int channel) {
	if (pan > 0) {
		Mix_SetPanning(channel, 255-pan*255, 255); // Pan right
	} else if (pan < 0) {
		Mix_SetPanning(channel, 255, 255+pan*255); // Pan left
	} else {
		Mix_SetPanning(channel, 255, 255); // Pan center
	}
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

int Sound::load() {
	// Load SDL2 Mixer sound
	if (is_music) {
		music = Mix_LoadMUS(sound_path.c_str());
		if (music == NULL) {
			std::cerr << "Failed to load sound " << name << " as music: " << Mix_GetError() << "\n";
			return 1;
		}
	} else {
		chunk = Mix_LoadWAV(sound_path.c_str());
		if (chunk == NULL) {
			std::cerr << "Failed to load sound " << name << " as chunk: " << Mix_GetError() << "\n";
			return 1;
		}

	}
	is_loaded = true;

	return 0;
}
int Sound::free() {
	if (is_music) {
		Mix_FreeMusic(music);
		music = NULL;
	} else {
		Mix_FreeChunk(chunk);
		chunk = NULL;
	}
	is_loaded = false;

	return 0;
}
int Sound::finished(int channel) {
	if (!is_music) {
		if (!is_playing) {
			current_channels.remove(channel);
		}
	}

	return 0;
}

int Sound::play() {
	if (is_music) {
		Mix_PlayMusic(music, 1);
	} else {
		int c = Mix_PlayChannel(-1, chunk, 0);
		if (c >= 0) {
			current_channels.remove(c);
			current_channels.push_back(c);
		} else {
			std::cerr << "Failed to play sound " << name << ": " << Mix_GetError() << "\n";
			return 1;
		}

		set_pan_internal(c);
	}

	is_playing = true;
	is_looping = false;

	return 0;
}
int Sound::stop() {
	is_playing = false;
	is_looping = false;

	if (is_music) {
		Mix_HaltMusic();
	} else {
		std::list<int> tmp_channels = current_channels;
		for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) {
			Mix_HaltChannel(*i);
		}
	}

	return 0;
}
int Sound::rewind() {
	if (is_music) {
		// Mix_RewindMusic(); // Only works for MOD, OGG, MP3, and MIDI
		Mix_HaltMusic();
		if (is_looping) {
			Mix_PlayMusic(music, -1);
		} else {
			Mix_PlayMusic(music, 1);
		}
	} else {
		for (auto i=current_channels.begin(); i != current_channels.end(); ++i) {
			Mix_HaltChannel(*i);
			if (is_looping) {
				Mix_PlayChannel(*i, chunk, -1);
			} else {
				Mix_PlayChannel(*i, chunk, 0);
			}
		}
	}

	return 0;
}
int Sound::pause() {
	if (is_music) {
		Mix_PauseMusic();
	} else {
		for (auto i=current_channels.begin(); i != current_channels.end(); ++i) {
			Mix_Pause(*i);
		}
	}

	is_playing = false;

	return 0;
}
int Sound::resume() {
	if (is_music) {
		Mix_ResumeMusic();
	} else {
		for (auto i=current_channels.begin(); i != current_channels.end(); ++i) {
			Mix_Resume(*i);
		}
	}

	is_playing = true;

	return 0;
}
int Sound::toggle() {
	if (is_playing) {
		return pause();
	} else {
		return resume();
	}
}
int Sound::loop() {
	if (is_music) {
		Mix_PlayMusic(music, -1);
	} else {
		int c = Mix_PlayChannel(-1, chunk, -1);
		if (c >= 0) {
			current_channels.remove(c);
			current_channels.push_back(c);
		} else {
			std::cerr << "Failed to play sound " << name << ": " << Mix_GetError() << "\n";
			return 1;
		}

		set_pan_internal(c);
	}

	is_looping = true;

	return 0;
}
bool Sound::get_is_playing() {
	return is_playing;
}
bool Sound::get_is_looping() {
	return is_looping;
}

#endif // _BEE_SOUND_H
