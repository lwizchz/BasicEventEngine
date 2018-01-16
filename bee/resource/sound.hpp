/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SOUND_H
#define BEE_SOUND_H 1

#include <string> // Include the required library headers
#include <map>
#include <list>

#include <SDL2/SDL_mixer.h> // Include the SDL2_mixer headers for the audio data types

#include "resource.hpp"

#include "../util/soundeffects.hpp"

namespace bee {
	class Sound: public Resource { // The sound resource class is used to play all audio
		static std::map<int,Sound*> list;
		static int next_id;

		int id; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the sound file
		double volume; // The volume to play the sound at, from 0.0 to 1.0
		double pan; // The panning of the sound, from -1.0 to 1.0 as Left to Right

		bool is_loaded; // Whether the sound file was successfully loaded as a mixer chunk/music
		bool is_music; // Whether the sound should be treated as music or a sound effect
		Mix_Music* music; // The internal sound storage struct for music
		Mix_Chunk* chunk; // The internal sound storage struct for a sound effect
		bool is_playing; // Whether the sound is currently playing
		bool is_looping; // Whether the sound is currently looping while playing
		std::list<int> current_channels; // A list of the current channels that the sound is playing on
		bool has_play_failed; // Whether the play function has previously failed, this prevents continuous warning outputs

		int sound_effects; // A bit mask describing the effects that will be applied to the sound
		// Pointers to the structs which hold parameters for each effect
		se_chorus_data* chorus_data;
		se_echo_data* echo_data;
		se_flanger_data* flanger_data;
		se_gargle_data* gargle_data;
		se_reverb_data* reverb_data;
		se_compressor_data* compressor_data;
		se_equalizer_data* equalizer_data;

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

		static size_t get_amount();
		static Sound* get(int);
		static Sound* get_by_name(const std::string&);
		static Sound* add(const std::string&, const std::string&, bool);

		static void finished(int);
		static int stop_loops();
		static int stop_all();
		static double get_master_volume();
		static int set_master_volume(double);

		int add_to_resources();
		int reset();
		int print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		bool get_is_loaded() const;
		bool get_is_music() const;
		double get_volume() const;
		double get_pan() const;
		bool get_is_playing() const;
		bool get_is_looping() const;
		int get_effects() const;

		int set_name(const std::string&);
		int set_path(const std::string&);
		int set_is_music(bool);
		int set_volume(double);
		int update_volume();
		int set_pan(double);

		int load();
		int free();
		int finish(int);

		int play(int);
		int play();
		int play_once();
		int stop();
		int rewind();
		int pause();
		int resume();
		int toggle();
		int loop();
		int fade_in(int, bool);
		int fade_in(int);
		int fade_out(int);

		int effect_set(int);
		int effect_reset_data();
	};
}

#endif // BEE_SOUND_H
