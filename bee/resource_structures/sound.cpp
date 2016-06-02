/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SOUND
#define _BEE_SOUND 1

#include "sound.hpp"

BEE::Sound::Sound () {
	if (BEE::resource_list->sounds.game != nullptr) {
		game = BEE::resource_list->sounds.game;
	}

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
	free();
	BEE::resource_list->sounds.remove_resource(id);
}
int BEE::Sound::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == sound_path) {
			return 1;
		}
		BEE::resource_list->sounds.remove_resource(id);
		id = -1;
	} else {
		for (auto i : BEE::resource_list->sounds.resources) {
			if ((i.second != nullptr)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list->sounds.add_resource(this);
	}
	BEE::resource_list->sounds.set_resource(id, this);

	if (BEE::resource_list->sounds.game != nullptr) {
		game = BEE::resource_list->sounds.game;
	}

	return 0;
}
int BEE::Sound::reset() {
	free();

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
	has_play_failed = false;

	effect_reset_data();

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
	if (is_loaded) {
		if (is_music) {
			Mix_VolumeMusic(game->get_volume()*volume*128);
		} else {
			Mix_VolumeChunk(chunk, game->get_volume()*volume*128);
		}
		return 0;
	}
	return 1;
}
int BEE::Sound::update_volume() {
	set_volume(volume);
	return 0;
}
int BEE::Sound::set_pan(double new_pan) {
	pan = new_pan;
	if (is_loaded) {
		if (is_music) {
			return 1; // I'm not sure how to pan music at the moment
		} else {
			for (std::list<int>::iterator i=current_channels.begin(); i != current_channels.end(); ++i) {
				set_pan_internal(*i);
			}
		}
		return 0;
	}
	return 1;
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
		if (music == nullptr) {
			std::cerr << "Failed to load sound " << name << " as music: " << Mix_GetError() << "\n";
			return 1;
		}
	} else {
		chunk = Mix_LoadWAV(sound_path.c_str());
		if (chunk == nullptr) {
			std::cerr << "Failed to load sound " << name << " as chunk: " << Mix_GetError() << "\n";
			return 1;
		}

	}
	is_loaded = true;
	has_play_failed = false;

	return 0;
}
int BEE::Sound::free() {
	if (is_loaded) {
		stop();
		if (is_music) {
			Mix_FreeMusic(music);
			music = nullptr;
		} else {
			Mix_FreeChunk(chunk);
			chunk = nullptr;
		}
		is_loaded = false;
	}

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
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to play sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (is_loaded) {
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
	return 1;
}
int BEE::Sound::rewind() {
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to rewind sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to pause sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to resume sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to loop sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to fade in sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (!is_loaded) {
		if (!has_play_failed) {
			std::cerr << "Failed to fade out sound '" << name << "'" << " because it is not loaded\n";
			has_play_failed = true;
		}
		return 1;
	}

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
	if (is_loaded) {
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
	return 1;
}
int BEE::Sound::effect_set(int channel, int se_mask) {
	if (se_mask & se_none) {
		Mix_UnregisterAllEffects(channel);
		effect_reset_data();
	} else {
		if (se_mask & se_chorus) {
			Mix_RegisterEffect(channel, sound_effect_chorus, sound_effect_chorus_cleanup, chorus_data);
		}
		if (se_mask & se_echo) {
			Mix_RegisterEffect(channel, sound_effect_echo, sound_effect_echo_cleanup, echo_data);
		}
		if (se_mask & se_flanger) {
			Mix_RegisterEffect(channel, sound_effect_flanger, sound_effect_flanger_cleanup, flanger_data);
		}
		if (se_mask & se_gargle) {
			std::cerr << "The gargle sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(channel, sound_effect_gargle, sound_effect_gargle_cleanup, gargle_data);
		}
		if (se_mask & se_reverb) {
			std::cerr << "The reverb sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(channel, sound_effect_reverb, sound_effect_reverb_cleanup, reverb_data);
		}
		if (se_mask & se_compressor) {
			std::cerr << "The compressor sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(channel, sound_effect_compressor, sound_effect_compressor_cleanup, compressor_data);
		}
		if (se_mask & se_equalizer) {
			std::cerr << "The equalizer sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(channel, sound_effect_equalizer, sound_effect_equalizer_cleanup, equalizer_data);
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
		effect_reset_data();
	} else {
		if (se_mask & se_chorus) {
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_chorus, sound_effect_chorus_cleanup, chorus_data);
		}
		if (se_mask & se_echo) {
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_echo, sound_effect_echo_cleanup, echo_data);
		}
		if (se_mask & se_flanger) {
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_flanger, sound_effect_flanger_cleanup, flanger_data);
		}
		if (se_mask & se_gargle) {
			std::cerr << "The gargle sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_gargle, sound_effect_gargle_cleanup, gargle_data);
		}
		if (se_mask & se_reverb) {
			std::cerr << "The reverb sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_reverb, sound_effect_reverb_cleanup, reverb_data);
		}
		if (se_mask & se_compressor) {
			std::cerr << "The compressor sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_compressor, sound_effect_compressor_cleanup, compressor_data);
		}
		if (se_mask & se_equalizer) {
			std::cerr << "The equalizer sound effect is currently unimplemented and will have no effect.\n";
			Mix_RegisterEffect(MIX_CHANNEL_POST, sound_effect_equalizer, sound_effect_equalizer_cleanup, equalizer_data);
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
int BEE::Sound::effect_reset_data() {
	if (chorus_data == nullptr) {
		chorus_data = new se_chorus_data();
	} else {
		sound_effect_chorus_cleanup(-1, (void*)chorus_data);
	}

	if (echo_data == nullptr) {
		echo_data = new se_echo_data();
	} else {
		sound_effect_echo_cleanup(-1, (void*)echo_data);
	}

	if (flanger_data == nullptr) {
		flanger_data = new se_flanger_data();
	} else {
		sound_effect_flanger_cleanup(-1, (void*)flanger_data);
	}

	if (gargle_data == nullptr) {
		gargle_data = new se_gargle_data();
	} else {
		sound_effect_gargle_cleanup(-1, (void*)gargle_data);
	}

	if (reverb_data == nullptr) {
		reverb_data = new se_reverb_data();
	} else {
		sound_effect_reverb_cleanup(-1, (void*)reverb_data);
	}

	if (compressor_data == nullptr) {
		compressor_data = new se_compressor_data();
	} else {
		sound_effect_compressor_cleanup(-1, (void*)compressor_data);
	}

	if (equalizer_data == nullptr) {
		equalizer_data = new se_equalizer_data();
	} else {
		sound_effect_equalizer_cleanup(-1, (void*)equalizer_data);
	}

	return 0;
}

#endif // _BEE_SOUND
