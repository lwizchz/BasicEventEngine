/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SOUND
#define BEE_SOUND 1

#include <vector> // Include the required library headers
#include <algorithm>

#include "sound.hpp" // Include the class resource header

#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/soundeffects.hpp"

namespace bee {
	/**
	* Construct the effect from a preset.
	* @param _name the effect name
	* @param type the preset type whose functions are defined in bee/core/soundeffects.cpp
	* @param params the initial params to overwrite
	*/
	SoundEffect::SoundEffect(const std::string& _name, E_SOUNDEFFECT type, std::map<Variant,Variant> _params) :
		SoundEffect(_name, nullptr, nullptr, {})
	{
		params = soundeffects::get_params(type);

		switch (type) {
			case E_SOUNDEFFECT::CHORUS: {
				effect_func = [] (int channel, void* stream, int len, void* udata) {
					soundeffects::chorus(channel, stream, len, udata);
				};
				cleanup_func = [] (int channel, void* udata) {
					soundeffects::chorus_cleanup(channel, udata);
				};
				break;
			}
			case E_SOUNDEFFECT::ECHO: {
				effect_func = [] (int channel, void* stream, int len, void* udata) {
					soundeffects::echo(channel, stream, len, udata);
				};
				cleanup_func = [] (int channel, void* udata) {
					soundeffects::echo_cleanup(channel, udata);
				};
				break;
			}
			case E_SOUNDEFFECT::FLANGER: {
				effect_func = [] (int channel, void* stream, int len, void* udata) {
					soundeffects::flanger(channel, stream, len, udata);
				};
				cleanup_func = [] (int channel, void* udata) {
					soundeffects::flanger_cleanup(channel, udata);
				};
				break;
			}
			case E_SOUNDEFFECT::GARGLE: {
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "The gargle sound effect is currently unimplemented and will have no effect");
				break;
			}
			case E_SOUNDEFFECT::REVERB: {
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "The reverb sound effect is currently unimplemented and will have no effect");
				break;
			}
			case E_SOUNDEFFECT::COMPRESSOR: {
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "The compressor sound effect is currently unimplemented and will have no effect");
				break;
			}
			case E_SOUNDEFFECT::EQUALIZER: {
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "The equalizer sound effect is currently unimplemented and will have no effect");
				break;
			}
		}

		for (auto& p : _params) {
			params[p.first] = p.second;
		}
	}
	/**
	* Construct the effect from custom functions.
	* @param _name the effect name
	* @param _effect_func the main effect function
	* @param _cleanup_func the effect cleanup function
	* @param params the initial params to use
	*/
	SoundEffect::SoundEffect(const std::string& _name, Mix_EffectFunc_t _effect_func, Mix_EffectDone_t _cleanup_func, std::map<Variant,Variant> _params) :
		name(_name),
		params(_params),
		effect_func(_effect_func),
		cleanup_func(_cleanup_func)
	{}

	/**
	* Add the effect to the given channel.
	* @see https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_76.html#SEC76 for details
	* @param c the channel on which to register the effect
	*
	* @retval 0 success
	* @retval 1 failed to register
	*/
	int SoundEffect::add(int c) {
		if (this->cleanup_func == nullptr) {
			this->cleanup_func = [] (int channel, void* udata) {
				std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);
				delete data;
			};
		}
		return !Mix_RegisterEffect(c, this->effect_func, this->cleanup_func, static_cast<void*>(new std::map<Variant,Variant>({{"params", Variant(params)}})));
	}
	/**
	* Remove the effect from the given channel.
	* @see https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_77.html#SEC77 for details
	* @param channel the channel from which to unregister the effect
	*
	* @retval 0 success
	* @retval 1 failed to unregister
	*/
	int SoundEffect::remove(int channel) {
		return !Mix_UnregisterEffect(channel, this->effect_func);
	}

	/**
	* @returns the effect name
	*/
	std::string SoundEffect::get_name() const {
		return name;
	}

	std::map<int,Sound*> Sound::list;
	int Sound::next_id = 0;
	double Sound::master_volume = 1.0;

	/**
	* Default construct the sound.
	* @note This constructor should only be used for temporary sounds, the other constructor should be used for all other cases.
	*/
	Sound::Sound() :
		Resource(),

		id(-1),
		name(),
		path(),
		volume(1.0),
		pan(0.0),

		music(nullptr),
		chunk(nullptr),
		is_loaded(false),
		is_music(false),
		is_playing(false),
		is_looping(false),
		has_play_failed(false),

		effects(),

		finish_func(nullptr)
	{}
	/**
	* Construct the Sound, add it to the Sound resource list, and set the new name and path.
	* @param _name the name of the sound to use
	* @param _path the path of the sound's file
	* @param _is_music whether the sound should be treated as music or a sound effect
	*/
	Sound::Sound(const std::string& _name, const std::string& _path, bool _is_music) :
		Sound() // Default initialize all variables
	{
		add_to_resources(); // Add the sound to the appropriate resource list
		if (id < 0) { // If the sound could not be added to the resource list, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add sound resource: \"" + _name + "\" from " + _path);
			throw(-1); // Throw an exception
		}

		set_name(_name);
		set_path(_path);
		set_is_music(_is_music); // Set whether the sound should be treated as music or a sound effect
	}
	/**
	* Free the sound data and remove it from the resource list.
	*/
	Sound::~Sound() {
		this->free(); // Free all sound data
		Sound::list.erase(id); // Remove the sound from the resource list
	}

	/**
	* @returns the number of Sound resources
	*/
	size_t Sound::get_amount() {
		return Sound::list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id
	*/
	Sound* Sound::get(int id) {
		if (Sound::list.find(id) != Sound::list.end()) {
			return Sound::list.at(id);
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Sound
	*
	* @returns the Sound resource with the given name
	*/
	Sound* Sound::get_by_name(const std::string& name) {
		for (auto& sound : list) { // Iterate over the sounds in order to find the first one with the given name
			Sound* s = sound.second;
			if (s != nullptr) {
				if (s->get_name() == name) {
					return s; // Return the desired sound on success
				}
			}
		}
		return nullptr; // Return nullptr on failure
	}
	/**
	* Initiliaze, load, and return a newly created Sound resource.
	* @param name the name to initialize the sound with
	* @param path the path to initialize the sound with
	* @param is_music whether the sound should be considered music or not
	*
	* @returns the newly loaded Sound
	*/
	Sound* Sound::add(const std::string& name, const std::string& path, bool is_music) {
		Sound* new_sound = new Sound(name, path, is_music);
		new_sound->load();
		return new_sound;
	}

	/**
	* Called by Mix_ChannelFinished() whenever a channel finishes playback.
	* @see https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_37.html#SEC37 and bee::internal::init_sdl() for details
	* @param channel the channel which has finished playback
	*/
	void Sound::finished(int channel) {
		for (auto& sound : list) { // Iterate over the sounds in order to remove finished channels from each sound's list
			Sound* s = sound.second;
			if (s != nullptr) {
				s->finish(channel); // Remove the finished channel from the list
			}
		}
	}
	/**
	* Immediately stop all looping sounds.
	*/
	void Sound::stop_loops() {
		for (auto& sound : list) { // Iterate over the sounds and stop them individually
			Sound* s = sound.second;
			if ((s != nullptr)&&(s->get_is_looping())) {
				s->stop();
			}
		}
	}
	/**
	* Immediately stop all sounds.
	*/
	void Sound::stop_all() {
		for (auto& sound : list) { // Iterate over the sounds and stop them individually
			Sound* s = sound.second;
			if (s != nullptr) {
				s->stop();
			}
		}
	}
	/**
	* @returns the global sound volume from [0.0, 1.0]
	*/
	double Sound::get_master_volume() {
		return Sound::master_volume;
	}
	/**
	* Set a new global sound volume and update it for all currently playing sounds.
	* @param volume the new volume to use
	*/
	void Sound::set_master_volume(double volume) {
		Sound::master_volume = volume; // Set the volume

		for (auto& sound : list) { // Iterate over the sounds and update them to the new volume
			Sound* s = sound.second;
			if (s != nullptr) {
				s->update_volume();
			}
		}
	}

	/**
	* Add the Sound to the appropriate resource list.
	*
	* @returns the Sound id
	*/
	int Sound::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = Sound::next_id++;
			Sound::list.emplace(id, this); // Add the resource with the new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Sound::reset() {
		this->free(); // Free all memory used by this resource

		// Reset all properties
		name = "";
		path = "";
		volume = 1.0;
		pan = 0.0;

		// Reset sound chunk data
		music = nullptr;
		chunk = nullptr;
		is_loaded = false;
		is_music = false;

		// Reset sound play data
		is_playing = false;
		is_looping = false;
		current_channels.clear();
		has_play_failed = false;

		return 0;
	}

	/**
	* @returns a map of all the information required to restore a Sound
	*/
	std::map<Variant,Variant> Sound::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["volume"] = volume;
		info["pan"] = pan;

		info["music"] = music;
		info["chunk"] = chunk;
		info["is_loaded"] = is_loaded;
		info["is_music"] = is_music;
		info["is_playing"] = is_playing;
		info["is_looping"] = is_looping;
		info["has_play_failed"] = has_play_failed;

		std::vector<Variant> channels;
		for (auto& c : current_channels) {
			channels.emplace_back(c);
		}
		info["channels"] = channels;

		std::vector<Variant> sfx;
		for (auto& e : effects) {
			sfx.emplace_back(e.get_name());
		}
		info["effects"] = sfx;

		return info;
	}
	/**
	* Restore a Sound from its serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the Sound
	*/
	int Sound::deserialize(std::map<Variant,Variant>& m) {
		this->free();

		id = m["id"].i;
		name = m["name"].s;
		path = m["name"].s;

		volume = m["volume"].d;
		pan = m["pan"].d;

		music = nullptr;
		chunk = nullptr;
		is_loaded = false;
		is_music = m["is_music"].i;
		is_playing = m["is_playing"].i;
		is_looping = m["is_looping"].i;
		has_play_failed = m["has_play_failed"].i;

		if ((m["is_loaded"].i)&&(load())) {
			return 1;
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Sound::print() const {
		Variant m (serialize());
		messenger::send({"engine", "sound"}, E_MESSAGE::INFO, "Sound " + m.to_str(true));
	}

	int Sound::get_id() const {
		return id;
	}
	std::string Sound::get_name() const {
		return name;
	}
	std::string Sound::get_path() const {
		return path;
	}
	double Sound::get_volume() const {
		return volume;
	}
	double Sound::get_pan() const {
		return pan;
	}
	bool Sound::get_is_loaded() const {
		return is_loaded;
	}
	bool Sound::get_is_music() const {
		return is_music;
	}
	bool Sound::get_is_playing() const {
		return is_playing;
	}
	bool Sound::get_is_looping() const {
		return is_looping;
	}

	void Sound::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       sounds resource directory.
	*/
	void Sound::set_path(const std::string& _path) {
		if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the sound directory if not root
			path = "resources/sounds/"+_path;
		}
	}
	/**
	* Set the volume relative to the master volume.
	* @param _volume the new volume to use
	*/
	void Sound::set_volume(double _volume) {
		volume = _volume; // Set the relative volume

		if (is_loaded) {
			int v = static_cast<int>(128.0*get_master_volume()*volume); // Get the volume level relative to the master volume
			if (is_music) { // If the sound is music, set the volume appropriately
				Mix_VolumeMusic(v);
			} else { // Otherwise set the sound chunk volume
				Mix_VolumeChunk(chunk, v);
			}
		}
	}
	/**
	* Update the volume when certain conditions change.
	* @note This is mainly used to set the volume after loading or after adjusting the master volume.
	*/
	void Sound::update_volume() {
		set_volume(volume);
	}
	/**
	* Set the panning of the given channel to the pan value.
	* @param channel the channel number to pan
	*
	* @retval 0 success
	* @retval 1 failed to pan channel
	*/
	int Sound::set_pan_internal(int channel) {
		if (pan > 0.0) { // If the panning is greated than 0.0, pan the channel to the right
			return !Mix_SetPanning(channel, 255-static_cast<int>(pan*255), 255);
		} else if (pan < 0.0) { // If the panning is less than 0.0, pan the channel to the left
			return !Mix_SetPanning(channel, 255, 255+static_cast<int>(pan*255));
		} else { // If the panning is equal to 0.0, pan the channel to the center
			return !Mix_SetPanning(channel, 255, 255);
		}
	}
	/**
	* Set the panning.
	* @note Sounds must be loaded as chunk data (i.e. non-music) in order for panning to work.
	* @param _pan the new panning to use for the sound
	*
	* @retval 0 success
	* @retval 1 a channel could not be panned
	* @retval failed to set panning since the Sound is loaded as music
	*/
	int Sound::set_pan(double _pan) {
		pan = _pan;

		if (is_loaded) {
			if (is_music) { // If the sound is music, do not attempt to pan it
				return 2;
			} else { // Otherwise set the desired panning for each currently playing channel
				int r = 0;
				for (std::list<int>::iterator i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing channels
					r += set_pan_internal(*i);
				}

				return !!r;
			}
		}

		return 0;
	}
	/**
	* Set whether to treat the Sound as music or chunk data.
	* @param _is_music the new sound type
	*
	* @retval 0 success
	* @retval nonzero failed to reload the Sound
	* @see load() for return value info
	*/
	int Sound::set_is_music(bool _is_music) {
		if (is_loaded) { // If the sound is already loaded, reload it
			this->free();
			is_music = _is_music;
			return load();
		} else {
			is_music = _is_music;
		}
		return 0;
	}
	void Sound::set_finish(std::function<void (Sound*)> _finish_func) {
		finish_func = _finish_func;
	}

	/**
	* Load the Sound from its path.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load since the engine is in headless mode
	* @retval 3 failed to load the music or chunk
	*/
	int Sound::load() {
		if (is_loaded) { // Do not attempt to load the sound if it has already been loaded
			messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to load sound \"" + name + "\" because it has already been loaded");
			return 1;
		}

		if (get_option("is_headless").i) {
			has_play_failed = true;
			return 2;
		}

		if (is_music) { // If the sound should be treated as music, load it appropriately
			music = Mix_LoadMUS(path.c_str()); // Load the sound file as mixer music
			if (music == nullptr) { // If the music could not be loaded, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to load sound \"" + name + "\" as music: " + util::get_sdl_error());
				return 3;
			}
		} else { // Otherwise load the sound normally
			chunk = Mix_LoadWAV(path.c_str()); // Load the sound file as a chunk sound
			if (chunk == nullptr) { // If the chunk could not be loaded, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to load sound \"" + name + "\" as chunk: " + util::get_sdl_error());
				return 3;
			}
		}

		// Set the volume for the now-loaded sound
		update_volume();

		// Set the loaded booleans
		is_loaded = true;
		has_play_failed = false;

		return 0;
	}
	/**
	* Free the Sound and delete all of its effect buffers.
	*
	* @retval 0 success
	*/
	int Sound::free() {
		if (!is_loaded) { // Do not attempt to free the data if the Sound has not been loaded
			return 0;
		}

		stop(); // Stop playing the sound before freeing its data

		effect_remove_all();

		if (is_music) { // If the sound is music, free it appropriately
			Mix_FreeMusic(music);
			music = nullptr;
		} else { // Otherwise free the sound normally
			Mix_FreeChunk(chunk);
			chunk = nullptr;
		}

		is_loaded = false; // Set the loaded boolean

		return 0;
	}
	/**
	* Remove the given channel from the currently playing list.
	* @note This is called by finished() whenever a channel finishes playback.
	* @param channel the channel which has finished playback
	*/
	void Sound::finish(int channel) {
		effect_stop(channel);

		if (is_music) {
			is_playing = false;
			is_looping = false;
		} else {
			current_channels.remove(channel);

			if (current_channels.empty()) {
				is_playing = false;
				is_looping = false;
			}
		}

		if (finish_func != nullptr) {
			finish_func(this);
		}
	}

	/**
	* Play the sound with optional looping and fading.
	* @see https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_52.html#SEC52 for more informaion about music
	* @see https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_25.html#SEC25 for more information about chunks
	*
	* @param loop_amount the amount of times to play the sound, -1 will make it play until stopped
	* @param fade_in the amount of ticks over which to fade in the sound
	*
	* @retval 0 success
	* @retval 1 failed to play since it's not loaded
	* @retval 2 failed to play since no channels are available
	*/
	int Sound::play(int loop_amount, int fade_in) {
		if (!is_loaded) { // Do not attempt to play the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to play sound \"" + name + "\" because it is not loaded");
				has_play_failed = true;
			}
			return 1;
		}

		if (is_music) { // If the sound is music, play it appropriately
			Mix_FadeInMusic(music, loop_amount, fade_in);
			effect_start(MIX_CHANNEL_POST);
		} else { // Otherwise play the sound normally
			int c = Mix_FadeInChannel(-1, chunk, loop_amount, fade_in);
			if (c >= 0) { // If the chunk was played successfully, add its channel to the list
				current_channels.remove(c); // Remove any duplicate channels
				current_channels.emplace_back(c);
			} else { // If the chunk could not be played, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to play sound \"" + name + "\": " + util::get_sdl_error());
				return 2;
			}

			set_pan_internal(c); // Apply the desired panning
			effect_start(c);
		}

		// Set the playing booleans
		is_playing = true;
		is_looping = false;

		return 0;
	}
	/**
	* Play the sound with optional looping.
	* @note When the function is called without a fade-in, let the fade-in be 0.
	* @param loop_amount the amount of times to play the sound, -1 will make it play until stopped
	*
	* @returns whether the play call failed or not
	* @see play(int, int) for details
	*/
	int Sound::play(int loop_amount) {
		return play(loop_amount, 0);
	}
	/**
	* Play the sound.
	* @note When the function is called with no arguments, play it once with no fade-in.
	*
	* @returns whether the play call failed or not
	* @see play(int, int) for details
	*/
	int Sound::play() {
		return play(0);
	}
	/**
	* Play a sound but only if it is not already playing.
	* @note This can be used to handle multiple instances emitting the same sound at the same time.
	*
	* @retval 0 the Sound was played successfully
	* @retval -1 the Sound was not played
	* @retval >0 the Sound failed to play
	* @see play(int, int) for return values >0
	*/
	int Sound::play_once() {
		if (!is_playing) {
			return play();
		}
		return -1;
	}
	/**
	* Stop playing all instances of the sound on every channel.
	* @param fade_out the amount of ticks over which to fade out the sound
	*
	* @retval 0 success
	* @retval 1 failed to stop playing since it's not loaded
	*/
	int Sound::stop(int fade_out) {
		if (!is_loaded) { // Do not attempt to stop the sound if it hasn't been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to stop sound \"" + name + "\" because it is not loaded");
				has_play_failed = true;
			}
			return 1;
		}

		if (is_music) { // If the sound is music, stop it appropriately
			Mix_FadeOutMusic(fade_out);
		} else { // Otherwise play the sound normally
			std::list<int> tmp_channels (current_channels); // Use a temporary copy since halting the channels will remove them from the list
			for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) { // Iterate over the currently playing channels
				Mix_FadeOutChannel(*i, fade_out);
			}
		}

		return 0;
	}
	/**
	* Stop playing all instances of the sound on every channel.
	*
	* @retval 0 success
	* @retval 1 failed to stop playing since it's not loaded
	* @see stop() for details
	*/
	int Sound::stop() {
		return stop(0);
	}
	/**
	* Play the sound from the beginning on every current channel.
	*
	* @retval 0 success
	* @retval 1 failed to rewind since it's not loaded
	* @retval 2 some channels failed to rewind
	*/
	int Sound::rewind() {
		if (!is_loaded) { // Do not attempt to rewind the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to rewind sound \"" + name + "\" because it is not loaded");
				has_play_failed = true;
			}
			return 1;
		}

		if (is_music) { // If the sound is music, play it appropriately
			// Mix_RewindMusic(); // Only works for MOD, OGG, MP3, and MIDI
			Mix_HaltMusic();

			// Play the music from the beginning with the desired amount of looping
			if (is_looping) {
				Mix_PlayMusic(music, -1);
			} else {
				Mix_PlayMusic(music, 1);
			}

			effect_start(MIX_CHANNEL_POST);
		} else { // Otherwise play the sound normally
			if (!current_channels.empty()) { // If the sound is currently playing, rewind it
				std::list<int> tmp_channels (current_channels); // Use a temporary list copy since halting the channels will remove them from the list
				for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) { // Iterate over the currently playing channels
					Mix_HaltChannel(*i);

					// Play the chunk from the beginning with the desired amount of looping
					int c = -1;
					if (is_looping) {
						c = Mix_PlayChannel(*i, chunk, -1);
					} else {
						c = Mix_PlayChannel(*i, chunk, 0);
					}

					if (c < 0) { // The chunk failed to play
						return 2;
					}

					set_pan_internal(c);
					effect_start(*i);
				}
			} else { // If the sound is not playing, start playing it
				return (play() ? 2 : 0);
			}
		}

		return 0;
	}
	/**
	* Pause all instances of the sound on every channel.
	*
	* @retval 0 success
	* @retval 1 failed to pause since it's not loaded
	*/
	int Sound::pause() {
		if (!is_loaded) { // Do not attempt to pause the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to pause sound \"" + name + "\" because it is not loaded");
				has_play_failed = true;
			}
			return 1;
		}

		if (is_music) { // If the sound is music, pause it appropriately
			Mix_PauseMusic();
		} else { // Otherwise pause it normally
			for (auto i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing channels
				Mix_Pause(*i);
			}
		}

		is_playing = false;

		return 0;
	}
	/**
	* Resume all paused instances of the sound on every channel.
	*
	* @retval 0 success
	* @retval 1 failed to resume since it's not loaded
	*/
	int Sound::resume() {
		if (!is_loaded) { // Do not attempt to resume the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to resume sound \"" + name + "\" because it is not loaded");
				has_play_failed = true;
			}
			return 1;
		}

		if (is_music) { // If the sound is music, resume it appropriately
			Mix_ResumeMusic();
		} else { // Otherwise resume it normally
			for (auto i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing (or paused) channels
				Mix_Resume(*i);
			}
		}

		is_playing = true;

		return 0;
	}
	/**
	* Toggle the sound between playing and paused.
	*
	* @retval 0 success
	* @retval 1 failed to toggle the state since it's not loaded
	* @see pause() and resume()
	*/
	int Sound::toggle() {
		if (is_playing) { // If the sound is playing, then pause it
			return pause();
		} else { // Otherwise resume it
			return resume();
		}
	}
	/**
	* Play the sound in a continuous loop.
	*
	* @retval 0 success
	* @retval >0 failed to play the sound
	* @see play(int, int) for return values >0
	*/
	int Sound::loop() {
		int r = play(-1); // Loop the sound
		if (r == 0) {
			is_looping = true;
		}
		return r;
	}

	/**
	* Add a SoundEffect and apply it to any playing channels.
	* @param se the effect to add
	*
	* @retval 0 success
	* @retval 1 failed to add the effect since the Sound isn't loaded
	* @retval 2 failed to apply the effect to some channels
	*/
	int Sound::effect_add(SoundEffect se) {
		if (!is_loaded) { // Do not attempt to add any sound effects if the sound has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger::send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to set sound effects for \"" + name + "\" because it is not loaded");
				has_play_failed = true;
			}
			return 1;
		}

		if (is_music) { // If the sound is music, set the effects appropriately
			if (se.add(MIX_CHANNEL_POST) == 0) {
				effects.emplace_back(se);
			} else {
				return 2;
			}
		} else { // Otherwise set the effects normally
			int r = 0;
			for (auto& c : current_channels) { // Iterate over the currently playing channels
				r += se.add(c);
			}

			if (r == 0) {
				effects.emplace_back(se);
			} else {
				return 2;
			}
		}

		return 0;
	}
	/**
	* Remove a SoundEffect.
	* @param effect_name the name of the effect to remove
	*
	* @retval 0 success
	* @retval 1 failed to remove the effect from some channels
	*/
	int Sound::effect_remove(const std::string& effect_name) {
		int c = -1;
		if (is_music) {
			c = MIX_CHANNEL_POST;
		}

		int r = 0;
		effects.erase(std::remove_if(effects.begin(), effects.end(), [&effect_name, &c, &r] (SoundEffect se) {
			if (se.get_name() == effect_name) {
				r += se.remove(c);
				return true;
			}
			return false;
		}), effects.end());

		return !!r;
	}
	/**
	* Remove all SoundEffects.
	*
	* @retval 0 success
	* @retval 1 failed to remove an effect from some channels
	*/
	int Sound::effect_remove_all() {
		int c = -1;
		if (is_music) {
			c = MIX_CHANNEL_POST;
		}

		int r = 0;
		for (auto& se : effects) {
			r += se.remove(c);
		}

		effects.clear();

		return !!r;
	}
	/**
	* Add the existing effects to a given channel.
	* @param channel the channel to add the effects to
	*
	* @retval 0 success
	* @retval 1 failed to add some effects to the channel
	*/
	int Sound::effect_start(int channel) {
		int r = 0;
		for (auto& se : effects) {
			r += se.add(channel);
		}
		return !!r;
	}
	/**
	* Remove the existing effects from a given channel.
	* @param channel the channel to remove the effects from
	*
	* @retval 0 success
	* @retval 1 failed to remove some effects from the channel
	*/
	int Sound::effect_stop(int channel) {
		int r = 0;
		for (auto& se : effects) {
			r += se.remove(channel);
		}
		return !!r;
	}
}

#endif // BEE_SOUND
