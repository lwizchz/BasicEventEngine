/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SOUND
#define BEE_SOUND 1

#include <sstream>
#include <vector>

#include "sound.hpp" // Include the class resource header

#include "../enum.hpp"
#include "../engine.hpp"

#include "../util/platform.hpp"

#include "../core/messenger/messenger.hpp"

namespace bee {
	/*
	* Sound::Sound() - Construct the sound and set its engine pointer
	* ! This constructor should only be used for temporary sounds, the other constructor should be used for all other cases
	*/
	Sound::Sound() :
		id(-1),
		name(),
		path(),
		volume(1.0),
		pan(0.0),

		is_loaded(false),
		is_music(false),
		music(nullptr),
		chunk(nullptr),
		is_playing(false),
		is_looping(false),
		has_play_failed(false),

		sound_effects((int)E_SOUNDEFFECT::NONE),
		chorus_data(new se_chorus_data()),
		echo_data(new se_echo_data()),
		flanger_data(new se_flanger_data()),
		gargle_data(new se_gargle_data()),
		reverb_data(new se_reverb_data()),
		compressor_data(new se_compressor_data()),
		equalizer_data(new se_equalizer_data())
	{}
	/*
	* Sound::Sound() - Construct the sound, reset all variables, add it to the sound resource list, and set the new name and path
	* @new_name: the name of the sound to use
	* @new_path: the path of the sound's file
	* @new_is_music: whether the sound should be treated as music or a sound effect
	*/
	Sound::Sound(const std::string& new_name, const std::string& new_path, bool new_is_music) :
		Sound() // Default initialize all variables
	{
		add_to_resources(); // Add the sound to the appropriate resource list
		if (id < 0) { // If the sound could not be added to the resource list, output a warning
			messenger_send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add sound resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the sound name
		set_path(new_path); // Set the sound file path
		set_is_music(new_is_music); // Set whether the sound should be treated as music or a sound effect
	}
	/*
	* Sound::~Sound() - Free the sound data and remove it from the resource list
	*/
	Sound::~Sound() {
		this->free(); // Free all sound data
		resource_list->sounds.remove_resource(id); // Remove the sound from the resource list
	}
	/*
	* Sound::add_to_resources() - Add the sound to the appropriate resource list
	*/
	int Sound::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = resource_list->sounds.add_resource(this); // Add the resource and get the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::reset() - Reset all resource variables for reinitialization
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

		// Reset the effect data
		effect_reset_data();

		return 0; // Return 0 on success
	}
	/*
	* Sound::print() - Print all relevant information about the resource
	*/
	int Sound::print() const {
		// Generate a string for the current number of channels
		std::string channels;
		for (auto& c : current_channels) {
			channels.append(bee_itos(c));
			channels.append(", ");
		}
		if (!channels.empty()) {
			channels.pop_back();
			channels.pop_back();
		}

		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Sound { "
		"\n	id              " << id <<
		"\n	name            " << name <<
		"\n	path            " << path <<
		"\n	volume          " << volume <<
		"\n	pan             " << pan <<
		"\n	is_loaded       " << is_loaded <<
		"\n	is_music        " << is_music <<
		"\n	music           " << music <<
		"\n	chunk           " << chunk <<
		"\n	is_playing      " << is_playing <<
		"\n	is_looping      " << is_looping <<
		"\n	channels        {" << channels << "}" <<
		"\n	has_play_failed " << has_play_failed <<
		"\n	sound_effects   " << sound_effects <<
		"\n}\n";
		messenger_send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for ouptut

		return 0; // Return 0 on success
	}

	/*
	* Sound::get_*() - Return the requested information
	*/
	int Sound::get_id() const {
		return id;
	}
	std::string Sound::get_name() const {
		return name;
	}
	std::string Sound::get_path() const {
		return path;
	}
	bool Sound::get_is_music() {
		return is_music;
	}
	double Sound::get_volume() {
		return volume;
	}
	double Sound::get_pan() {
		return pan;
	}
	bool Sound::get_is_playing() {
		return is_playing;
	}
	bool Sound::get_is_looping() {
		return is_looping;
	}
	int Sound::get_effects() {
		return sound_effects;
	}

	/*
	* Sound::set_name() - Set the resource name
	* @new_name: the new name to use for the resource
	*/
	int Sound::set_name(const std::string& new_name) {
		name = new_name; // Set the name
		return 0; // Return 0 on success
	}
	/*
	* Sound::set_path() - Set the resource path
	* @new_path: the new path to use for the resource
	*/
	int Sound::set_path(const std::string& new_path) {
		path = "resources/sounds/"+new_path; // Append the path to the sprite directory
		return 0; // Return 0 on success
	}
	/*
	* Sound::set_is_music() - Set whether the sound should be treated as music or as a sound effect
	* @new_is_music: the new way to treat the music
	*/
	int Sound::set_is_music(bool new_is_music) {
		if (is_loaded) { // If the sound is already loaded, reload it
			this->free();
			is_music = new_is_music; // Set the sound type
			return load(); // Return the attempt at reloading the sound
		} else { // Otherwise, simply set the is_music boolean
			is_music = new_is_music;
		}
		return 0; // Return 0 on success
	}
	/*
	* Sound::set_volume() - Set the volume of the sound, relative to the global volume level
	* @new_volume: the new volume to use for the sound
	*/
	int Sound::set_volume(double new_volume) {
		volume = new_volume; // Set the volume

		if (!is_loaded) { // Do not attempt to set the volume if the sound is not loaded
			return 1; // Return 1 when the sound is not loaded
		}

		if (is_music) { // If the sound is music, set the volume appropriately
			Mix_VolumeMusic(get_volume()*volume*128);
		} else { // Otherwise set the sound chunk volume
			Mix_VolumeChunk(chunk, get_volume()*volume*128);
		}
		return 0; // Return 0 on success
	}
	/*
	* Sound::update_volume() - Update the sound volume when certain conditions change
	* ! This is mainly used to set the volume after loading and to adjust the global volume level
	*/
	int Sound::update_volume() {
		return set_volume(volume); // Return the attempt to update the volume
	}
	/*
	* Sound::set_pan() - Set the panning of the sound
	* ! Music cannot be panned in-engine, so if it is necessary then use it as non-music or edit the sound file manually
	* @new_pan: the new panning to use for the sound
	*/
	int Sound::set_pan(double new_pan) {
		pan = new_pan; // Set the panning

		if (!is_loaded) { // Do not attempt to set the panning if the sound is not loaded
			return 1; // Return 1 when the sound is not loaded
		}

		if (is_music) { // If the sound is music, do not attempt to pan it
			return 2; // Return 2 since music cannot be panned
		} else { // Otherwise set the desired panning for each currently playing channel
			for (std::list<int>::iterator i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing channels
				set_pan_internal(*i); // Set the channel panning to the desired value
			}
		}
		return 0; // Return 0 on success
	}
	/*
	* Sound::set_pan_internal() - Set the panning of the given channel to the desired value
	* @channel: the channel number to pan
	*/
	int Sound::set_pan_internal(int channel) {
		if (pan > 0.0) { // If the panning is greated than 0.0, pan the channel to the right
			Mix_SetPanning(channel, 255-pan*255, 255);
		} else if (pan < 0.0) { // If the panning is less than 0.0, pan the channel to the left
			Mix_SetPanning(channel, 255, 255+pan*255);
		} else { // If the panning is equal to 0.0, pan the channel to the center
			Mix_SetPanning(channel, 255, 255);
		}
		return 0; // Return 0 on success
	}

	/*
	* Sound::load() - Load the sound from its given filename
	*/
	int Sound::load() {
		if (is_loaded) { // If the sound has already been loaded, output a warning
			messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to load sound \"" + name + "\" because it has already been loaded");
			return 1; // Return 1 when already loaded
		}

		effect_reset_data(); // Reset the effect data structs

		if (is_music) { // If the sound should be treated as music, load it appropriately
			music = Mix_LoadMUS(path.c_str()); // Load the sound file as mixer music
			if (music == nullptr) { // If the music could not be loaded, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to load sound \"" + name + "\" as music: " + Mix_GetError());
				return 2; // Return 2 on music loading failure
			}
		} else { // Otherwise load the sound normally
			chunk = Mix_LoadWAV(path.c_str()); // Load the sound file as a chunk sound
			if (chunk == nullptr) { // If the chunk could not be loaded, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to load sound \"" + name + "\" as chunk: " + Mix_GetError());
				return 3; // Return 3 on chunk loading failure
			}

		}

		// Set the volume for the now-loaded sound
		update_volume();

		// Set loaded booleans
		is_loaded = true;
		has_play_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Sound::free() - Free the sound data and delete all of its effect buffers
	*/
	int Sound::free() {
		if (!is_loaded) { // Do not attempt to free the data if the sound has not been loaded
			return 0; // Return 0 on success
		}

		stop(); // Stop playing the sound before freeing its data

		// Delete all of the sound's effect buffers
		delete chorus_data;
		chorus_data = nullptr;
		delete echo_data;
		echo_data = nullptr;
		delete flanger_data;
		flanger_data = nullptr;
		delete gargle_data;
		gargle_data = nullptr;
		delete reverb_data;
		reverb_data = nullptr;
		delete compressor_data;
		compressor_data = nullptr;
		delete equalizer_data;
		equalizer_data = nullptr;

		// Free the sound data
		if (is_music) { // If the sound is music, free it appropriately
			Mix_FreeMusic(music);
			music = nullptr;
		} else { // Otherwise free the sound normally
			Mix_FreeChunk(chunk);
			chunk = nullptr;
		}

		is_loaded = false; // Set the loaded boolean

		return 0; // Return 0 on success
	}
	/*
	* Sound::finished() - Remove the given channel from the currently playing list
	* ! This is called by sound_finished() whenever a channel finishes playback
	* @channel: the channel which has finished playback
	*/
	int Sound::finished(int channel) {
		if (!is_music) { // Music cannot be played on multiple channels
			current_channels.remove(channel); // Remove the channel from the currently playing list
		}
		return 0; // Return 0 on success
	}

	/*
	* Sound::play() - Play the sound with optional looping
	* ! Note that the loop amount means different things for music versus for chunks
	* ! See http://jcatki.no-ip.org:8080/SDL_mixer/SDL_mixer.html#SEC28 for more information about chunks
	* ! See http://jcatki.no-ip.org:8080/SDL_mixer/SDL_mixer.html#SEC57 for more informaion about music
	* @loop_amount: the amount of times to play the sound, -1 will make it play until stopped
	*/
	int Sound::play(int loop_amount) {
		if (!is_loaded) { // Do not attempt to play the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to play sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, play it appropriately
			Mix_PlayMusic(music, loop_amount); // Play the music
			effect_add_post(sound_effects); // Apply the desired sound effects
		} else { // Otherwise play the sound normally
			int c = Mix_PlayChannel(-1, chunk, loop_amount); // Play the chunk on the first available channel
			if (c >= 0) { // If the chunk was played successfully, add its channel to the list
				current_channels.remove(c); // Remove any duplicate channels
				current_channels.push_back(c); // Add the channel to the end of the currently playing list
			} else { // If the chunk could not be played, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to play sound \"" + name + "\": " + Mix_GetError());
				return 2; // Return 2 on play failure
			}

			set_pan_internal(c); // Apply the desired panning
			effect_add(c, sound_effects); // Apply the desired sound effects
		}

		// Set the playing booleans
		is_playing = true;
		is_looping = false;

		return 0; // Return 0 on success
	}
	/*
	* Sound::play() - Play the sound
	* ! If the function is called without a loop amount, simply play it once
	*/
	int Sound::play() {
		return play((is_music) ? 1 : 0);
	}
	/*
	* Sound::stop() - Stop playing all instances of the sound on every channel
	*/
	int Sound::stop() {
		if (!is_loaded) { // Do not attempt to stop the sound if it hasn't been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to fade out sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, stop it appropriately
			effect_add_post(0); // Remove all previous sound effects
			Mix_HaltMusic(); // Stop the music
		} else { // Otherwise play the sound normally
			std::list<int> tmp_channels = current_channels; // Use a temporary list copy since halting the channels will remove them from the list
			for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) { // Iterate over the currently playing channels
				effect_add(*i, 0); // Remove all previous sound effects
				Mix_HaltChannel(*i); // Stop the specific channel
			}
		}

		// Set the playing booleans
		is_playing = false;
		is_looping = false;

		return 0; // Return 0 on success
	}
	/*
	* Sound::rewind() - Play the sound from the beginning on every current channel
	*/
	int Sound::rewind() {
		if (!is_loaded) { // Do not attempt to rewind the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to rewind sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, play it appropriately
			// Mix_RewindMusic(); // Only works for MOD, OGG, MP3, and MIDI
			Mix_HaltMusic(); // Stop the music

			// Play the music from the beginning with the desired amount of looping
			if (is_looping) {
				Mix_PlayMusic(music, -1);
			} else {
				Mix_PlayMusic(music, 1);
			}

			effect_add_post(sound_effects); // Apply the desired sound effects
		} else { // Otherwise play the sound normally
			if (!current_channels.empty()) { // If the sound is currently playing, rewind it
				std::list<int> tmp_channels = current_channels; // Use a temporary list copy since halting the channels will remove them from the list
				for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) { // Iterate over the currently playing channels
					Mix_HaltChannel(*i); // Stop the specific channel

					// Play the chunk from the beginning with the desired amount of looping
					if (is_looping) {
						Mix_PlayChannel(*i, chunk, -1);
					} else {
						Mix_PlayChannel(*i, chunk, 0);
					}

					effect_add(*i, sound_effects); // Apply the desired sound effects
				}
			} else { // If the sound is not playing, start playing it
				play();
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::pause() - Pause all instances of the sound on every channel
	*/
	int Sound::pause() {
		if (!is_loaded) { // Do not attempt to pause the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to pause sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, pause it appropriately
			Mix_PauseMusic(); // Pause the music
		} else { // Otherwise pause it normally
			for (auto i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing channels
				Mix_Pause(*i); // Pause the specific channel
			}
		}

		is_playing = false; // Set the playing boolean

		return 0; // Return 0 on success
	}
	/*
	* Sound::resume() - Resume all paused instances of the sound on every channel
	*/
	int Sound::resume() {
		if (!is_loaded) { // Do not attempt to resume the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to resume sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, resume it appropriately
			Mix_ResumeMusic(); // Resume the music
		} else { // Otherwise resume it normally
			for (auto i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing (or paused) channels
				Mix_Resume(*i); // Resume playing on the specific channel
			}
		}

		is_playing = true; // Set the playing boolean

		return 0; // Return 0 on success
	}
	/*
	* Sound::toggle() - Toggle the sound between playing and paused
	*/
	int Sound::toggle() {
		if (is_playing) { // If the sound is playing, then pause it
			return pause(); // Return the attempt to pause
		} else { // Otherwise resume it
			return resume(); // Return the attempt to resume
		}
	}
	/*
	* Sound::loop() - Play the sound in a continuous loop
	*/
	int Sound::loop() {
		return play(-1); // Return the attempt to play the sound until it is stopped
	}
	/*
	* Sound::fade_in() - Fade in the sound over a given amount of time
	* ! This function might be able to be combined with play()
	* @ticks: the amount of ticks over which to fade in
	*/
	int Sound::fade_in(int ticks) {
		if (!is_loaded) { // Do not attempt to fade in the sound if it has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed yet, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to fade in sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, fade it in appropriately
			Mix_FadeInMusic(music, 1, ticks); // Fade in the music
			effect_add_post(sound_effects); // Apply the desired sound effects
		} else { // Otherwise fade it in normally
			int c = Mix_FadeInChannel(-1, chunk, 0, ticks); // Fade in the chunk on the first available channel
			if (c >= 0) { // If the chunk was played successfully, add its channel to the list
				current_channels.remove(c); // Remove any duplicate channels
				current_channels.push_back(c); // Add the channel to the end of the currently playing list
			} else { // If the chunk could not be played, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to play sound \"" + name + "\": " + Mix_GetError());
				return 2; // Return 2 on play failure
			}

			set_pan_internal(c); // Apply the desired panning
			effect_add(c, sound_effects); // Apply the desired sound effects
		}

		// Set the playing booleans
		is_playing = true;
		is_looping = false;

		return 0; // Return 0 on success
	}
	/*
	* Sound::fade_out() - Fade out the sound over a given amount of time
	* @ticks: the amount of ticks over which to fade out
	*/
	int Sound::fade_out(int ticks) {
		if (!is_loaded) { // Do not attempt to fade out the sound if it hasn't been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to fade out sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (is_music) { // If the sound is music, fade it out appropriately
			Mix_FadeOutMusic(ticks); // Fade out the music
		} else { // Otherwise fade it out normally
			std::list<int> tmp_channels = current_channels; // Use a temporary list copy since halting the channels will remove them from the list
			for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) { // Iterate over the currently playing channels
				Mix_FadeOutChannel(*i, ticks); // Fade out the specific channel
			}
		}

		// Set the playing booleans
		is_playing = false;
		is_looping = false;

		return 0; // Return 0 on success
	}

	/*
	* Sound::effect_set() - Add the given sound effects to the sound
	* @new_sound_effects - a bitmask containing the desired sound effects
	*/
	int Sound::effect_set(int new_sound_effects) {
		if (!is_loaded) { // Do not attempt to add any sound effects if the sound has not been loaded
			if (!has_play_failed) { // If the play call hasn't failed before, output a warning
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "Failed to play sound \"" + name + "\" because it is not loaded");
				has_play_failed = true; // Set the play failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		// Swap the sound effect bitmasks
		int old_sound_effects = sound_effects;
		sound_effects = new_sound_effects;

		if (is_music) { // If the sound is music, set the effects appropriately
			effect_remove_post(old_sound_effects ^ sound_effects); // Remove all old sound effects
			effect_add_post(sound_effects); // Apply the new sound effects
		} else { // Otherwise set the effects normally
			//std::list<int> tmp_channels = current_channels; // Use a temporary list copy (is this necessary?)
			//for (auto i=tmp_channels.begin(); i != tmp_channels.end(); ++i) { // Iterate over the currently playing channels
			for (auto i=current_channels.begin(); i != current_channels.end(); ++i) { // Iterate over the currently playing channels
				effect_remove(*i, old_sound_effects ^ sound_effects); // Remove all previous sound effects
				effect_add(*i, sound_effects); // Apply the new desired sound effects
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::effect_add() - Apply the given sound effects on the given channel
	* ! Note that E_SOUNDEFFECT::NONE is a separate bit in the mask and will override all other effects
	* @channel: the channel to set the sound effects for
	* @se_mask: the desired sound effects to register
	*/
	int Sound::effect_add(int channel, int se_mask) {
		if (se_mask & (int)E_SOUNDEFFECT::NONE) { // If no effects are desired, remove all previous effects
			Mix_UnregisterAllEffects(channel); // Remove the effects
			effect_reset_data(); // Reset the effect data
		} else { // Otherwise set the desired effects
			if (se_mask & (int)E_SOUNDEFFECT::CHORUS) { // If the chorus effect is requeted, apply it
				Mix_RegisterEffect(channel, sound_effect_chorus, sound_effect_chorus_cleanup, chorus_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::ECHO) { // If the echo effect is requeted, apply it
				Mix_RegisterEffect(channel, sound_effect_echo, sound_effect_echo_cleanup, echo_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::FLANGER) { // If the flanger effect is requeted, apply it
				Mix_RegisterEffect(channel, sound_effect_flanger, sound_effect_flanger_cleanup, flanger_data);
			}
			// Output warnings for the gargle, reverb, compressor, and equalizer effects which are currently unimplemented
			if (se_mask & (int)E_SOUNDEFFECT::GARGLE) { // If the gargle effect is requeted, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The gargle sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(channel, sound_effect_gargle, sound_effect_gargle_cleanup, gargle_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::REVERB) { // If the reverb effect is requeted, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The reverb sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(channel, sound_effect_reverb, sound_effect_reverb_cleanup, reverb_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::COMPRESSOR) { // If the compressor effect is requeted, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The compressor sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(channel, sound_effect_compressor, sound_effect_compressor_cleanup, compressor_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::EQUALIZER) { // If the equalizer effect is requeted, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The equalizer sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(channel, sound_effect_equalizer, sound_effect_equalizer_cleanup, equalizer_data);
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::effect_add_post() - Apply the given sound effects to the music
	* ! Note that E_SOUNDEFFECT::NONE is a separate bit in the mask and will override all other effects
	* @se_mask: the desired sound effects to register
	*/
	int Sound::effect_add_post(int se_mask) {
		if (se_mask & (int)E_SOUNDEFFECT::NONE) { // If no effects are desired, remove all previous effects
			Mix_UnregisterAllEffects(MIX_CHANNEL_POST); // Remove the effects
			effect_reset_data(); // Reset the effect data
		} else { // Otherwise set the desired effect
			if (se_mask & (int)E_SOUNDEFFECT::CHORUS) { // If the chorus effect is requested, apply it
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_chorus, sound_effect_chorus_cleanup, chorus_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::ECHO) { // If the echo effect is requested, apply it
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_echo, sound_effect_echo_cleanup, echo_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::FLANGER) { // If the flanger effect is requested, apply it
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_flanger, sound_effect_flanger_cleanup, flanger_data);
			}
			// Output warnings for the gargle, reverb, compressor, and equalizer effects which are currently unimplemented
			if (se_mask & (int)E_SOUNDEFFECT::GARGLE) { // If the gargle effect is requested, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The gargle sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_gargle, sound_effect_gargle_cleanup, gargle_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::REVERB) { // If the reverb effect is requested, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The reverb sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_reverb, sound_effect_reverb_cleanup, reverb_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::COMPRESSOR) { // If the compressor effect is requested, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The compressor sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_compressor, sound_effect_compressor_cleanup, compressor_data);
			}
			if (se_mask & (int)E_SOUNDEFFECT::EQUALIZER) { // If the equalizer effect is requested, apply it
				messenger_send({"engine", "sound"}, E_MESSAGE::WARNING, "The equalizer sound effect is currently unimplemented and will have no effect");
				Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_equalizer, sound_effect_equalizer_cleanup, equalizer_data);
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::effect_remove() - Remove the given sound effects from the given channel
	* ! Note that E_SOUNDEFFECT::NONE is a separate bit in the mask and will override all other effects
	* @channel: the channel to remove the sound effects from
	* @se_mask: the desired sound effects to unregister
	*/
	int Sound::effect_remove(int channel, int se_mask) {
		if (se_mask & (int)E_SOUNDEFFECT::NONE) { // If there are no undesired sound effects, do not attempt to remove any
			return 0; // Return 0 on success
		}

		if (se_mask & (int)E_SOUNDEFFECT::CHORUS) { // If the chorus effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_chorus); // Unregister the effect
			if (chorus_data != nullptr) { // Clean up the old data
				sound_effect_chorus_cleanup(-1, (void*)chorus_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::ECHO) { // If the echo effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_echo); // Unregister the effect
			if (echo_data != nullptr) { // Clean up the old data
				sound_effect_echo_cleanup(-1, (void*)echo_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::FLANGER) { // If the flanger effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_flanger); // Unregister the effect
			if (flanger_data != nullptr) { // Clean up the old data
				sound_effect_flanger_cleanup(-1, (void*)flanger_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::GARGLE) { // If the gargle effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_gargle); // Unregister the effect
			if (gargle_data != nullptr) { // Clean up the old data
				sound_effect_gargle_cleanup(-1, (void*)gargle_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::REVERB) { // If the reverb effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_reverb); // Unregister the effect
			if (reverb_data != nullptr) { // Clean up the old data
				sound_effect_reverb_cleanup(-1, (void*)reverb_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::COMPRESSOR) { // If the compressor effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_compressor); // Unregister the effect
			if (compressor_data != nullptr) { // Clean up the old data
				sound_effect_compressor_cleanup(-1, (void*)compressor_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::EQUALIZER) { // If the equalizer effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(channel, sound_effect_equalizer); // Unregister the effect
			if (equalizer_data != nullptr) { // Clean up the old data
				sound_effect_equalizer_cleanup(-1, (void*)equalizer_data);
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::effect_remove() - Remove the given sound effects from the given channel
	* ! Note that E_SOUNDEFFECT::NONE is a separate bit in the mask and will override all other effects
	* @se_mask: the desired sound effects to unregister
	*/
	int Sound::effect_remove_post(int se_mask) {
		if (se_mask & (int)E_SOUNDEFFECT::NONE) { // If there are no undesired sound effects, do not attempt to remove any
			return 0; // Return 0 on success
		}

		if (se_mask & (int)E_SOUNDEFFECT::CHORUS) { // If the chorus effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_chorus); // Unregister the effect
			if (chorus_data != nullptr) { // Clean up the old data
				sound_effect_chorus_cleanup(-1, (void*)chorus_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::ECHO) { // If the echo effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_echo); // Unregister the effect
			if (echo_data != nullptr) { // Clean up the old data
				sound_effect_echo_cleanup(-1, (void*)echo_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::FLANGER) { // If the flanger effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_flanger); // Unregister the effect
			if (flanger_data != nullptr) { // Clean up the old data
				sound_effect_flanger_cleanup(-1, (void*)flanger_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::GARGLE) { // If the gargle effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_gargle); // Unregister the effect
			if (gargle_data != nullptr) { // Clean up the old data
				sound_effect_gargle_cleanup(-1, (void*)gargle_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::REVERB) { // If the reverb effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_reverb); // Unregister the effect
			if (reverb_data != nullptr) { // Clean up the old data
				sound_effect_reverb_cleanup(-1, (void*)reverb_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::COMPRESSOR) { // If the compressor effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_compressor); // Unregister the effect
			if (compressor_data != nullptr) { // Clean up the old data
				sound_effect_compressor_cleanup(-1, (void*)compressor_data);
			}
		}
		if (se_mask & (int)E_SOUNDEFFECT::EQUALIZER) { // If the equalizer effect is no longer desired, remove it and clean up the old data
			Mix_UnregisterEffect(MIX_CHANNEL_POST, sound_effect_equalizer); // Unregister the effect
			if (equalizer_data != nullptr) { // Clean up the old data
				sound_effect_equalizer_cleanup(-1, (void*)equalizer_data);
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sound::effect_reset_data() - Reset the data for all sound effects
	*/
	int Sound::effect_reset_data() {
		if (chorus_data == nullptr) { // If the chorus data has not been initialized, do so
			chorus_data = new se_chorus_data();
		} else { // Otherwise clean up the old data
			sound_effect_chorus_cleanup(-1, (void*)chorus_data);
		}

		if (echo_data == nullptr) { // If the echo data has not been initialized, do so
			echo_data = new se_echo_data();
		} else { // Otherwise clean up the old data
			sound_effect_echo_cleanup(-1, (void*)echo_data);
		}

		if (flanger_data == nullptr) { // If the flanger data has not been initialized, do so
			flanger_data = new se_flanger_data();
		} else { // Otherwise clean up the old data
			sound_effect_flanger_cleanup(-1, (void*)flanger_data);
		}

		if (gargle_data == nullptr) { // If the gargle data has not been initialized, do so
			gargle_data = new se_gargle_data();
		} else { // Otherwise clean up the old data
			sound_effect_gargle_cleanup(-1, (void*)gargle_data);
		}

		if (reverb_data == nullptr) { // If the gargle data has not been initialized, do so
			reverb_data = new se_reverb_data();
		} else { // Otherwise clean up the old data
			sound_effect_reverb_cleanup(-1, (void*)reverb_data);
		}

		if (compressor_data == nullptr) { // If the compressor data has not been initialized, do so
			compressor_data = new se_compressor_data();
		} else { // Otherwise clean up the old data
			sound_effect_compressor_cleanup(-1, (void*)compressor_data);
		}

		if (equalizer_data == nullptr) { // If the equalizer data has not been initialized, do so
			equalizer_data = new se_equalizer_data();
		} else { // Otherwise clean up the old data
			sound_effect_equalizer_cleanup(-1, (void*)equalizer_data);
		}

		return 0; // Return 0 on success
	}
}

#endif // BEE_SOUND
