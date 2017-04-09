/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SOUND_H
#define _BEE_SOUND_H 1

#include <list> // Include the required library headers

#include "../game.hpp" // Include the engine headers

class BEE::Sound: public Resource {
		int id = -1; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the sound file
		double volume; // The volume to play the sound at, from 0.0 to 1.0
		double pan; // The panning of the sound, from -1.0 to 1.0 as Left to Right

		bool is_loaded = false; // Whether the sound file was successfully loaded as a mixer chunk/music
		bool is_music = false; // Whether the sound should be treated as music or a sound effect
		Mix_Music* music; // The internal sound storage struct for music
		Mix_Chunk* chunk; // The internal sound storage struct for a sound effect
		bool is_playing = false; // Whether the sound is currently playing
		bool is_looping = false; // Whether the sound is currently looping while playing
		std::list<int> current_channels; // A list of the current channels that the sound is playing on
		bool has_play_failed = false; // Whether the play function has previously failed, this prevents continuous warning outputs

		int sound_effects = (1u << 0); // A bit mask describing the effects that will be applied to the sound
		// Pointers to the structs which hold parameters for each effect
		se_chorus_data* chorus_data = nullptr;
		se_echo_data* echo_data = nullptr;
		se_flanger_data* flanger_data = nullptr;
		se_gargle_data* gargle_data = nullptr;
		se_reverb_data* reverb_data = nullptr;
		se_compressor_data* compressor_data = nullptr;
		se_equalizer_data* equalizer_data = nullptr;

		// See bee/resources/sound.cpp for function comments
		int set_pan_internal(int);

		int effect_add(int, int);
		int effect_add_post(int);
		int effect_remove(int, int);
		int effect_remove_post(int);
	public:
		// See bee/resources/sound.cpp for function comments
		Sound();
		Sound(const std::string&, const std::string&, bool);
		~Sound();
		int add_to_resources();
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		bool get_is_music();
		double get_volume();
		double get_pan();
		bool get_is_playing();
		bool get_is_looping();
		int get_effects();

		int set_name(const std::string&);
		int set_path(const std::string&);
		int set_is_music(bool);
		int set_volume(double);
		int update_volume();
		int set_pan(double);

		int load();
		int free();
		int finished(int);

		int play(int);
		int play();
		int stop();
		int rewind();
		int pause();
		int resume();
		int toggle();
		int loop();
		int fade_in(int);
		int fade_out(int);

		int effect_set(int);
		int effect_reset_data();
};

#endif // _BEE_SOUND_H
