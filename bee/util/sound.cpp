/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_SOUND
#define _BEE_UTIL_SOUND 1

// Sound effect functions, not fully implemented

#include <iostream> // Include the required library headers
#include <cmath>
#include <vector>

#include <SDL2/SDL_mixer.h> // Include the SDL2_mixer headers for the audio data types

#include "sound.hpp" // Include the function definitions

#include "real.hpp" // Include the required real number functions

/*
* sound_effect_chorus() - Operate on the given sound data to produce a chorus effect
* ! See http://jcatki.no-ip.org:8080/SDL_mixer/SDL_mixer.html#SEC76 for details
* @channel: the mixer channel which the sound is playing on
* @stream: the new stream data
* @length: the length of the new stream data in bytes
* @udata: the effect data struct
*/
void sound_effect_chorus(int channel, void* stream, int len, void* udata) {
	Sint16* newstream = (Sint16*) stream; // Cast the stream into the correct data format
	se_chorus_data* data = static_cast<se_chorus_data*>(udata); // Cast the effect struct into the correct format

	unsigned int length = len;
	Sint16* oldstream = (Sint16*)malloc(length); // Make a copy of the data stream in order to operate on the dry signal
	memcpy(oldstream, newstream, length);

	for (size_t i=0, e=0; i<length-1; i+=sizeof(Sint16)*2, e+=2, data->ticks+=2) { // Iterate over the ticks of the data stream
		int lfo = 44.1 * (sin(PI/10.0 * data->frequency * data->ticks)*30.0*data->depth + 30.0 + data->delay); // Calculate the offset of the low frequency oscillator
		if (data->ticks > lfo) { // Modify the stream data as long as the offset is within the bounds
			// Left stereo
			newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (data->stream[data->ticks - lfo] * data->wet); // Calculate the new stream value from the lfo offset
			newstream[e] += data->stream[data->ticks - lfo] * data->feedback; // Add the stream feedback
			// Right stereo
			newstream[e+1] = (oldstream[e+1] * (1.0 - data->wet)) + (data->stream[data->ticks - lfo + 1] * data->wet);
			newstream[e+1] += data->stream[data->ticks - lfo + 1] * data->feedback;
		}
		data->stream.push_back(newstream[e]); // Add the stream data to the effect struct copy
		data->stream.push_back(newstream[e+1]);
	}

	free(oldstream); // Free the copy of the stream data
}
/*
* sound_effect_chorus_cleanup() - Clean up the effect's data struct
* @channel: the mixer channel which the sound is playing on
* @udata: the effect data struct
*/
void sound_effect_chorus_cleanup(int channel, void* udata) {
	se_chorus_data* data = static_cast<se_chorus_data*>(udata); // Cast the effect struct into the correct format
	data->stream.clear(); // Clear the copy of the stream data
	data->ticks = 0; // Reset the stream position to the beginning
}
/*
* sound_effect_echo() - Operate on the given sound data to produce an echo effect
* @channel: the mixer channel which the sound is playing on
* @stream: the new stream data
* @length: the length of the new stream data in bytes
* @udata: the effect data struct
*/
void sound_effect_echo(int channel, void* stream, int len, void* udata) {
	Sint16* newstream = (Sint16*) stream; // Cast the stream into the correct data format
	se_echo_data* data = static_cast<se_echo_data*>(udata); // Cast the effect struct into the correct format

	unsigned int length = len;
	Sint16* oldstream = (Sint16*)malloc(length); // Make a copy of the data stream in order to operate on the dry signal
	memcpy(oldstream, newstream, length);

	int offset = 44.1 * data->delay; // Calculate the offset based on the effect struct
	for (size_t i=0, e=0; i<length-1; i+=sizeof(Sint16)*2, e+=2, data->ticks+=2) { // Iterate over the ticks of the data stream
		if (data->ticks > offset) { // Modify the stream data as long as the offset is within the bounds
			// Left stereo
			newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (data->stream[data->ticks - offset] * data->wet); // Calculate the new stream value from the echo offset
			newstream[e] += data->stream[data->ticks - offset] * data->feedback; // Add the stream feedback
			// Right stereo
			newstream[e+1] = (oldstream[e+1] * (1.0 - data->wet)) + (data->stream[data->ticks - offset + 1] * data->wet);
			newstream[e+1] += data->stream[data->ticks - offset + 1] * data->feedback;
		}
		data->stream.push_back(newstream[e]); // Add the stream data to the effect struct copy
		data->stream.push_back(newstream[e+1]);
	}

	free(oldstream); // Free the copy of the stream data
}
/*
* sound_effect_echo_cleanup() - Clean up the effect's data struct
* @channel: the mixer channel which the sound is playing on
* @udata: the effect data struct
*/
void sound_effect_echo_cleanup(int channel, void* udata) {
	se_echo_data* data = static_cast<se_echo_data*>(udata); // Cast the effect struct into the correct format
	data->stream.clear(); // Clear the copy of the stream data
	data->ticks = 0; // Reset the stream position to the beginning
}
/*
* sound_effect_flanger() - Operate on the given sound data to produce a flanger effect
* @channel: the mixer channel which the sound is playing on
* @stream: the new stream data
* @length: the length of the new stream data in bytes
* @udata: the effect data struct
*/
void sound_effect_flanger(int channel, void* stream, int len, void* udata) {
	Sint16* newstream = (Sint16*) stream; // Cast the stream into the correct data format
	se_flanger_data* data = static_cast<se_flanger_data*>(udata); // Cast the effect struct into the correct format

	unsigned int length = len;
	Sint16* oldstream = (Sint16*)malloc(length); // Make a copy of the data stream in order to operate on the dry signal
	memcpy(oldstream, newstream, length);

	double d = 0.5*data->delay; // Calculate the offset based on the effect struct
	for (size_t i=0, e=0; i<length-1; i+=sizeof(Sint16)*2, e+=2, data->ticks+=2) { // Iterate over the ticks of the data stream
		int lfo = 44.1 * ((sin(PI/220500.0 * data->frequency * data->ticks) + 1.0) * data->depth + 1.0) * d + d; // Calculate the offset of the low frequency oscillator
		if (data->ticks > lfo) { // Modify the stream data as long as the offset is within the bounds
			// Left stereo
			newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (data->stream[data->ticks - lfo] * data->wet); // Calculate the new stream value from the lfo offset
			newstream[e] += data->stream[data->ticks - lfo] * data->feedback; // Add the stream feedback
			// Right stereo
			newstream[e+1] = (oldstream[e+1] * (1.0 - data->wet)) + (data->stream[data->ticks - lfo + 1] * data->wet); // Calculate the new stream value from the lfo offset
			newstream[e+1] += data->stream[data->ticks - lfo + 1] * data->feedback; // Add the stream feedback
		}
		data->stream.push_back(newstream[e]); // Add the stream data to the effect struct copy
		data->stream.push_back(newstream[e+1]);
	}

	free(oldstream); // Free the copy of the stream data
}
/*
* sound_effect_flanger_cleanup() - Clean up the effect's data struct
* @channel: the mixer channel which the sound is playing on
* @udata: the effect data struct
*/
void sound_effect_flanger_cleanup(int channel, void* udata) {
	se_flanger_data* data = static_cast<se_flanger_data*>(udata); // Cast the effect struct into the correct format
	data->stream.clear(); // Clear the copy of the stream data
	data->ticks = 0; // Reset the stream position to the beginning
}
void sound_effect_gargle(int channel, void* stream, int len, void* udata) {
	// The gargle sound effect is currently unimplemented and will have no effect
}
void sound_effect_gargle_cleanup(int channel, void* udata) {
	// The gargle sound effect is currently unimplemented and will have no effect
}
void sound_effect_reverb(int channel, void* stream, int len, void* udata) {
	// The reverb sound effect is currently unimplemented and will have no effect
}
void sound_effect_reverb_cleanup(int channel, void* udata) {
	// The reverb sound effect is currently unimplemented and will have no effect
}
void sound_effect_compressor(int channel, void* stream, int len, void* udata) {
	// The compressor sound effect is currently unimplemented and will have no effect
}
void sound_effect_compressor_cleanup(int channel, void* udata) {
	// The compressor sound effect is currently unimplemented and will have no effect
}
void sound_effect_equalizer(int channel, void* stream, int len, void* udata) {
	// The equalizer sound effect is currently unimplemented and will have no effect
}
void sound_effect_equalizer_cleanup(int channel, void* udata) {
	// The equalizer sound effect is currently unimplemented and will have no effect
}

#endif // _BEE_UTIL_SOUND
