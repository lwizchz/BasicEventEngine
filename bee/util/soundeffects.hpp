/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_SOUNDEFFECTS_H
#define BEE_UTIL_SOUNDEFFECTS_H 1

void sound_effect_chorus(int, void*, int, void*);
void sound_effect_chorus_cleanup(int, void*);
void sound_effect_echo(int, void*, int, void*);
void sound_effect_echo_cleanup(int, void*);
void sound_effect_flanger(int, void*, int, void*);
void sound_effect_flanger_cleanup(int, void*);
void sound_effect_gargle(int, void*, int, void*);
void sound_effect_gargle_cleanup(int, void*);
void sound_effect_reverb(int, void*, int, void*);
void sound_effect_reverb_cleanup(int, void*);
void sound_effect_compressor(int, void*, int, void*);
void sound_effect_compressor_cleanup(int, void*);
void sound_effect_equalizer(int, void*, int, void*);
void sound_effect_equalizer_cleanup(int, void*);

// Define structs for sound effect data
class se_chorus_data {
	public:
		double wet = 0.5;        // Ratio of processed signal from 0.0 to 1.0
		double depth = 0.25;     // Percentage by which delay is modulated from 0.0 to 1.0
		double feedback = 0.0;   // Percentage of output to feed back into the input from 0.0 to 1.0
		double frequency = 10.0; // Frequency of the Low Frequency Oscillator from 0.0 to 20.0
		double delay = 5.0;      // Number of milliseconds to offset the processed signal from the input signal from 0.0 to 20.0

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_echo_data {
	public:
		double wet = 0.5;      // Ratio of processed signal from 0.0 to 1.0
		double feedback = 0.0; // Percentage of output to feed back into the input from 0.0 to 1.0
		double delay = 300.0;  // Number of milliseconds to offset the processed signal from the input signal from 1.0 to 2000.0

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_flanger_data {
	public:
		double wet = 0.5;        // Ratio of processed signal from 0.0 to 1.0
		double depth = 0.25;     // Percentage by which delay is modulated from 0.0 to 1.0
		double feedback = 0.0;   // Percentage of output to feed back into the input from 0.0 to 1.0
		double frequency = 10.0; // Frequency of the Low Frequency Oscillator from 1.0 to 20.0
		double delay = 10.0;     // Maximum number of milliseconds to offset the processed signal from the input signal from 1.0 to 10.0

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_gargle_data {
	public:
		int rate = 1, wavetype = 1;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_reverb_data {
	public:
		double gain = 0.0;
		double mix = 0.0;
		double time = 1000.0;
		double ratio = 0.001;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_compressor_data {
	public:
		double gain = 0.0;
		double attack = 0.01;
		double threshold = -10.0;
		double ratio = 0.1;
		int release = 50;
		int delay = 0;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};
class se_equalizer_data {
	public:
		double gain = 0.0;
		int center = 10000;
		int bandwidth = 36;

		std::vector<Sint16> stream; // Stores a copy of the processed stream data
		int ticks = 0; // The position of the stream, i.e. the size of the stream data
};

#endif // BEE_UTIL_SOUNDEFFECTS_H
