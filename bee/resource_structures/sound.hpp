/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SOUND_H
#define _BEE_SOUND_H 1

#include <list>

#include "../game.hpp"

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
		bool has_play_failed = false;

		int sound_effects = (1u << 0);
		se_chorus_data* chorus_data = NULL;
		se_echo_data* echo_data = NULL;
		se_flanger_data* flanger_data = NULL;
		se_gargle_data* gargle_data = NULL;
		se_reverb_data* reverb_data = NULL;
		se_compressor_data* compressor_data = NULL;
		se_equalizer_data* equalizer_data = NULL;
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
		int effect_reset_data();
};

#endif // _BEE_SOUND_H
