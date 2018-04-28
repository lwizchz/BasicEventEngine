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

#include "../enum.hpp"

#include "../data/variant.hpp"

namespace bee {
	/**
	* Used to apply audio effects to the Sound class
	* @see https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_76.html#SEC76 for details
	*/
	class SoundEffect {
		std::string name; ///< The effect name
		std::map<Variant,Variant> params; ///< Various parameters for the effect
		Mix_EffectFunc_t effect_func; ///< The main effect function
		Mix_EffectDone_t cleanup_func; ///< The effect cleanup function
	public:
		// See bee/resource/sound.cpp for function comments
		SoundEffect(const std::string&, E_SOUNDEFFECT, std::map<Variant,Variant>);
		SoundEffect(const std::string&, Mix_EffectFunc_t, Mix_EffectDone_t, std::map<Variant,Variant>);

		int add(int);
		int remove(int);

		std::string get_name() const;
	};

	/// Used to play all audio
	class Sound: public Resource {
		static std::map<int,Sound*> list;
		static int next_id;

		static double master_volume;

		int id; ///< The unique Sound identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the sound file

		double volume; ///< The relative volume to play the Sound at, from 0.0 to 1.0
		double pan; ///< The panning of the Sound, from -1.0 to 1.0 as Left to Right

		Mix_Music* music; ///< The internal sound storage struct for music
		Mix_Chunk* chunk; ///< The internal sound storage struct for a sound effect chunk
		bool is_loaded; ///< Whether the sound file was successfully loaded
		bool is_music; ///< Whether the Sound should be treated as music or a sound effect chunk
		bool is_playing; ///< Whether the Sound is currently playing
		bool is_looping; ///< Whether the Sound is currently looping while playing
		std::list<int> current_channels; ///< A list of the current channels that the Sound is playing on
		bool has_play_failed; ///< Whether the play function has previously failed, this prevents continuous warning outputs

		std::vector<SoundEffect> effects; ///< A vector of the effects that are applied to the Sound

		std::function<void (Sound*)> finish_func; ///< A function called whenever the Sound finishes playing

		// See bee/resource/sound.cpp for function comments
		void update_volume();
		int set_pan_internal(int);

		int effect_start(int);
		int effect_stop(int);
	public:
		// See bee/resource/sound.cpp for function comments
		Sound();
		Sound(const std::string&, const std::string&, bool);
		~Sound();

		static size_t get_amount();
		static Sound* get(int);
		static Sound* get_by_name(const std::string&);
		static Sound* add(const std::string&, const std::string&, bool);

		static void finished(int);
		static void stop_loops();
		static void stop_all();
		static double get_master_volume();
		static void set_master_volume(double);

		int add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		double get_volume() const;
		double get_pan() const;
		bool get_is_loaded() const;
		bool get_is_music() const;
		bool get_is_playing() const;
		bool get_is_looping() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
		void set_volume(double);
		int set_pan(double);
		int set_is_music(bool);
		void set_finish(std::function<void (Sound*)>);

		int load();
		int free();
		void finish(int);

		int play(int, int);
		int play(int);
		int play();
		int play_once();
		int stop(int);
		int stop();
		int rewind();
		int pause();
		int resume();
		int toggle();
		int loop();

		int effect_add(SoundEffect);
		int effect_remove(const std::string&);
		int effect_remove_all();
	};
}

#endif // BEE_SOUND_H
