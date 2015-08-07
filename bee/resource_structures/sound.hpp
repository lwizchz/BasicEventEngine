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

enum se_type {
	se_none		= (1u << 0),
	se_chorus	= (1u << 1),
	se_echo		= (1u << 2),
	se_flanger	= (1u << 3),
	se_gargle	= (1u << 4),
	se_reverb	= (1u << 5),
	se_compressor	= (1u << 6),
	se_equalizer	= (1u << 7)
};

class BEE::Sound: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string sound_path;
		double volume; // From 0.0 to 1.0
		double pan; // From -1.0 to 1.0 as Left to Right
		int play_type; // In memory vs continuous
		int channel_amount; // Mono vs stereo
		int sample_rate;
		int sample_format;
		int bit_rate;

		bool is_loaded = false, is_music;
		Mix_Music* music;
		Mix_Chunk* chunk;
		bool is_playing, is_looping;
		std::list<int> current_channels;

		int sound_effects = (1u << 0);
		se_chorus_data chorus_data;
		se_echo_data echo_data;
		se_flanger_data flanger_data;
		se_gargle_data gargle_data;
		se_reverb_data reverb_data;
		se_compressor_data compressor_data;
		se_equalizer_data equalizer_data;
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
		double get_volume();
		double get_pan();
		int get_play_type();
		int get_channel_amount();
		int get_sample_rate();
		int get_sample_format();
		int get_bit_rate();

		int set_name(std::string);
		int set_path(std::string);
		int set_is_music(bool);
		int set_volume(double);
		int update_volume();
		int set_pan(double);
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
		int fade_in(int);
		int fade_out(int);
		bool get_is_playing();
		bool get_is_looping();

		int effect_add(int);
		int effect_set(int, int);
		int effect_set_post(int);
		int effect_remove(int, int);
		int effect_remove_post(int);
};
BEE::Sound::Sound () {
	reset();
}
BEE::Sound::Sound (std::string new_name, std::string path, bool new_is_music) {
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
BEE::Sound::~Sound() {
	if (is_loaded) {
		free();
	}
	BEE::resource_list.sounds.remove_resource(id);
}
int BEE::Sound::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == sound_path) {
			return 1;
		}
		BEE::resource_list.sounds.remove_resource(id);
		id = -1;
	} else {
		for (auto i : BEE::resource_list.sounds.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list.sounds.add_resource(this);
	}
	BEE::resource_list.sounds.set_resource(id, this);

	return 0;
}
int BEE::Sound::reset() {
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
int BEE::Sound::print() {
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
int BEE::Sound::get_id() {
	return id;
}
std::string BEE::Sound::get_name() {
	return name;
}
std::string BEE::Sound::get_path() {
	return sound_path;
}
bool BEE::Sound::get_is_music() {
	return is_music;
}
double BEE::Sound::get_volume() {
	return volume;
}
double BEE::Sound::get_pan() {
	return pan;
}
int BEE::Sound::get_play_type() {
	return play_type;
}
int BEE::Sound::get_channel_amount() {
	return channel_amount;
}
int BEE::Sound::get_sample_rate() {
	return sample_rate;
}
int BEE::Sound::get_sample_format() {
	return sample_format;
}
int BEE::Sound::get_bit_rate() {
	return bit_rate;
}
int BEE::Sound::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Sound::set_path(std::string path) {
	add_to_resources("resources/sounds/"+path);
	sound_path = "resources/sounds/"+path;

	return 0;
}
int BEE::Sound::set_is_music(bool new_is_music) {
	if (is_loaded) {
		free();
		is_music = new_is_music;
		return load();
	} else {
		is_music = new_is_music;
	}
	return 0;
}
int BEE::Sound::set_volume(double new_volume) {
	volume = new_volume;
	if (is_music) {
		Mix_VolumeMusic(game->get_volume()*volume*128);
	} else {
		Mix_VolumeChunk(chunk, game->get_volume()*volume*128);
	}
	return 0;
}
int BEE::Sound::update_volume() {
	set_volume(volume);
	return 0;
}
int BEE::Sound::set_pan(double new_pan) {
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
int BEE::Sound::set_pan_internal(int channel) {
	if (pan > 0) {
		Mix_SetPanning(channel, 255-pan*255, 255); // Pan right
	} else if (pan < 0) {
		Mix_SetPanning(channel, 255, 255+pan*255); // Pan left
	} else {
		Mix_SetPanning(channel, 255, 255); // Pan center
	}
	return 0;
}
int BEE::Sound::set_play_type(int new_play_type) {
	play_type = new_play_type;
	return 0;
}
int BEE::Sound::set_channel_ammount(int new_channel_amount) {
	channel_amount = new_channel_amount;
	return 0;
}
int BEE::Sound::set_sample_rate(int new_sample_rate) {
	sample_rate = new_sample_rate;
	return 0;
}
int BEE::Sound::set_sample_format(int new_sample_format) {
	sample_format = new_sample_format;
	return 0;
}
int BEE::Sound::set_bit_rate(int new_bit_rate) {
	bit_rate = new_bit_rate;
	return 0;
}

int BEE::Sound::load() {
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
int BEE::Sound::free() {
	stop();
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
int BEE::Sound::finished(int channel) {
	if (!is_music) {
		if (!is_playing) {
			current_channels.remove(channel);
		}
	}

	return 0;
}

int BEE::Sound::play() {
	if (is_music) {
		Mix_PlayMusic(music, 1);
		effect_set_post(sound_effects);
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
		effect_set(c, sound_effects);
	}

	is_playing = true;
	is_looping = false;

	return 0;
}
int BEE::Sound::stop() {
	is_playing = false;
	is_looping = false;

	if (is_music) {
		effect_set_post(0);
		Mix_HaltMusic();
	} else {
		std::list<int> tmp_channels = current_channels;
		for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) {
			effect_set(*i, 0);
			Mix_HaltChannel(*i);
		}
	}

	return 0;
}
int BEE::Sound::rewind() {
	if (is_music) {
		// Mix_RewindMusic(); // Only works for MOD, OGG, MP3, and MIDI
		Mix_HaltMusic();
		if (is_looping) {
			Mix_PlayMusic(music, -1);
		} else {
			Mix_PlayMusic(music, 1);
		}
		effect_set_post(sound_effects);
	} else {
		for (auto i=current_channels.begin(); i != current_channels.end(); ++i) {
			Mix_HaltChannel(*i);
			if (is_looping) {
				Mix_PlayChannel(*i, chunk, -1);
			} else {
				Mix_PlayChannel(*i, chunk, 0);
			}
			effect_set(*i, sound_effects);
		}
	}

	return 0;
}
int BEE::Sound::pause() {
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
int BEE::Sound::resume() {
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
int BEE::Sound::toggle() {
	if (is_playing) {
		return pause();
	} else {
		return resume();
	}
}
int BEE::Sound::loop() {
	if (is_music) {
		Mix_PlayMusic(music, -1);
		effect_set_post(sound_effects);
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
		effect_set(c, sound_effects);
	}

	is_looping = true;

	return 0;
}
int BEE::Sound::fade_in(int ticks) {
	if (is_music) {
		Mix_FadeInMusic(music, 1, ticks);
	} else {
		int c = Mix_FadeInChannel(-1, chunk, 0, ticks);
		if (c >= 0) {
			current_channels.remove(c);
			current_channels.push_back(c);
		} else {
			std::cerr << "Failed to play sound " << name << ": " << Mix_GetError() << "\n";
			return 1;
		}

		set_pan_internal(c);
		effect_set(c, sound_effects);
	}

	is_playing = true;
	is_looping = false;

	return 0;
}
int BEE::Sound::fade_out(int ticks) {
	is_playing = false;
	is_looping = false;

	if (is_music) {
		Mix_FadeOutMusic(ticks);
	} else {
		std::list<int> tmp_channels = current_channels;
		for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) {
			Mix_FadeOutChannel(*i, ticks);
		}
	}
	return 0;
}
bool BEE::Sound::get_is_playing() {
	return is_playing;
}
bool BEE::Sound::get_is_looping() {
	return is_looping;
}

int BEE::Sound::effect_add(int new_sound_effects) {
	int old_sound_effects = sound_effects;
	sound_effects = new_sound_effects;

	if (is_music) {
		effect_remove_post(old_sound_effects ^ sound_effects);
		effect_set_post(sound_effects);
	} else {
		std::list<int> tmp_channels = current_channels;
		for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) {
			effect_remove(*i, old_sound_effects ^ sound_effects);
			effect_set(*i, sound_effects);
		}
	}

	return 0;
}
int BEE::Sound::effect_set(int channel, int se_mask) {
	if (se_mask & se_none) {
		Mix_UnregisterAllEffects(channel);
	} else {
		if (se_mask & se_chorus) {
			std::cerr << "The requested sound effect 'se_chorus' is not implemented and will not be applied\n";
			Mix_RegisterEffect(channel, sound_effect_chorus, sound_effect_chorus_cleanup, &chorus_data);
		}
		if (se_mask & se_echo) {
			Mix_RegisterEffect(channel, sound_effect_echo, sound_effect_echo_cleanup, &echo_data);
		}
		if (se_mask & se_flanger) {
			std::cerr << "The requested sound effect 'se_flanger' is not implemented and will not be applied\n";
			Mix_RegisterEffect(channel, sound_effect_flanger, sound_effect_flanger_cleanup, &flanger_data);
		}
		if (se_mask & se_gargle) {
			std::cerr << "The requested sound effect 'se_gargle' is not implemented and will not be applied\n";
			Mix_RegisterEffect(channel, sound_effect_gargle, sound_effect_gargle_cleanup, &gargle_data);
		}
		if (se_mask & se_reverb) {
			std::cerr << "The requested sound effect 'se_reverb' is not implemented and will not be applied\n";
			Mix_RegisterEffect(channel, sound_effect_reverb, sound_effect_reverb_cleanup, &reverb_data);
		}
		if (se_mask & se_compressor) {
			std::cerr << "The requested sound effect 'se_compressor' is not implemented and will not be applied\n";
			Mix_RegisterEffect(channel, sound_effect_compressor, sound_effect_compressor_cleanup, &compressor_data);
		}
		if (se_mask & se_equalizer) {
			std::cerr << "The requested sound effect 'se_equalizer' is not implemented and will not be applied\n";
			Mix_RegisterEffect(channel, sound_effect_equalizer, sound_effect_equalizer_cleanup, &equalizer_data);
		}
	}

	return 0;
}
int BEE::Sound::effect_remove(int channel, int se_mask) {
	if (!(se_mask & se_none)) {
		if ((se_mask & se_chorus)&&(!(sound_effects & se_chorus))) {
			Mix_UnregisterEffect(channel, sound_effect_chorus);
		}
		if ((se_mask & se_echo)&&(!(sound_effects & se_echo))) {
			Mix_UnregisterEffect(channel, sound_effect_echo);
		}
		if ((se_mask & se_flanger)&&(!(sound_effects & se_flanger))) {
			Mix_UnregisterEffect(channel, sound_effect_flanger);
		}
		if ((se_mask & se_gargle)&&(!(sound_effects & se_gargle))) {
			Mix_UnregisterEffect(channel, sound_effect_gargle);
		}
		if ((se_mask & se_reverb)&&(!(sound_effects & se_reverb))) {
			Mix_UnregisterEffect(channel, sound_effect_reverb);
		}
		if ((se_mask & se_compressor)&&(!(sound_effects & se_compressor))) {
			Mix_UnregisterEffect(channel, sound_effect_compressor);
		}
		if ((se_mask & se_equalizer)&&(!(sound_effects & se_equalizer))) {
			Mix_UnregisterEffect(channel, sound_effect_equalizer);
		}
	}

	return 0;
}
int BEE::Sound::effect_set_post(int se_mask) {
	if (se_mask & se_none) {
		Mix_UnregisterAllEffects(MIX_CHANNEL_POST);
	} else {
		if (se_mask & se_chorus) {
			std::cerr << "The requested sound effect 'se_chorus' is not implemented and will not be applied\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_chorus, sound_effect_chorus_cleanup, &chorus_data);
		}
		if (se_mask & se_echo) {
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_echo, sound_effect_echo_cleanup, &echo_data);
		}
		if (se_mask & se_flanger) {
			std::cerr << "The requested sound effect 'se_flanger' is not implemented and will not be applied\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_flanger, sound_effect_flanger_cleanup, &flanger_data);
		}
		if (se_mask & se_gargle) {
			std::cerr << "The requested sound effect 'se_gargle' is not implemented and will not be applied\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_gargle, sound_effect_gargle_cleanup, &gargle_data);
		}
		if (se_mask & se_reverb) {
			std::cerr << "The requested sound effect 'se_reverb' is not implemented and will not be applied\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_reverb, sound_effect_reverb_cleanup, &reverb_data);
		}
		if (se_mask & se_compressor) {
			std::cerr << "The requested sound effect 'se_compressor' is not implemented and will not be applied\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_compressor, sound_effect_compressor_cleanup, &compressor_data);
		}
		if (se_mask & se_equalizer) {
			std::cerr << "The requested sound effect 'se_equalizer' is not implemented and will not be applied\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_equalizer, sound_effect_equalizer_cleanup, &equalizer_data);
		}
	}

	return 0;
}
int BEE::Sound::effect_remove_post(int se_mask) {
	if (!(se_mask & se_none)) {
		if ((se_mask & se_chorus)&&(!(sound_effects & se_chorus))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_chorus);
		}
		if ((se_mask & se_echo)&&(!(sound_effects & se_echo))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_echo);
		}
		if ((se_mask & se_flanger)&&(!(sound_effects & se_flanger))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_flanger);
		}
		if ((se_mask & se_gargle)&&(!(sound_effects & se_gargle))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_gargle);
		}
		if ((se_mask & se_reverb)&&(!(sound_effects & se_reverb))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_reverb);
		}
		if ((se_mask & se_compressor)&&(!(sound_effects & se_compressor))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_compressor);
		}
		if ((se_mask & se_equalizer)&&(!(sound_effects & se_equalizer))) {
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_equalizer);
		}
	}

	return 0;
}

#endif // _BEE_SOUND_H
