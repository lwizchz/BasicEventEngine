/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_SOUND_H
#define _BEE_UTIL_SOUND_H 1

// Sound effect functions, not fully implemented

#include <cmath>
#include <vector>
#include <SDL2/SDL_mixer.h>

#include "real.hpp"

class se_chorus_data {
        public:
                double wet = 0.5;       // Ratio of processed signal from 0.0 to 1.0
                double depth = 0.5;     // Percentage by which delay is modulated from 0.0 to 1.0
                double feedback = 0.0;  // Percentage of output to feed back into the input from 0.0 to 1.0
                int frequency = 10;     // Frequency of the Low Frequency Oscillator from 0 to 20
                int delay = 0;          // Number of milliseconds to offset the processed signal from the input signal from 0 to 20

                std::vector<Sint16> stream;
                int ticks;
};
void sound_effect_chorus(int channel, void* stream, int length, void* udata) {
        /*Sint16* oldstream;
        Sint16* newstream = (Sint16*) stream;
        se_chorus_data* data = (se_chorus_data*) udata;

        oldstream = (Sint16*)malloc(length);
        memcpy(oldstream, newstream, length);

        for (int i=0, e=0; i<length; i+=sizeof(Sint16)*2, e+=2, data->ticks+=2) {
                int lfo = max<int>(1, data->delay) * data->depth * abs(radtodeg(sin(degtorad(2*PI * data->frequency * data->ticks))));
                if (data->ticks >= lfo) {
                        if ((!data->stream.empty())&&(lfo < data->ticks)) {
                                //std::cerr << "o:" << lfo << ",";
                                newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (data->stream[data->ticks - lfo - 2] * data->wet);
                                newstream[e+1] = (oldstream[e+1] * (1.0 - data->wet)) + (data->stream[data->ticks - lfo - 1] * data->wet);
                        } else if (lfo <= e) {
                                newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (oldstream[e] * data->wet);
                                newstream[e+1] = (oldstream[e+1] * (1.0 - data->wet)) + (oldstream[e+1] * data->wet);
                        }
                        if (lfo < length) {
                                newstream[e] += sign(newstream[e]) * ((newstream[e] * (1.0 - data->wet)) + (newstream[length - lfo - 2] * data->wet)) * data->feedback;
                                newstream[e+1] += sign(newstream[e+1]) * ((newstream[e+1] * (1.0 - data->wet)) + (newstream[length - lfo - 1] * data->wet)) * data->feedback;
                        }
                }
                data->stream.push_back(oldstream[e]);
                data->stream.push_back(oldstream[e+1]);
        }

        free(oldstream);

        return;*/
}
void sound_effect_chorus_cleanup(int channel, void* udata) {
        /*se_chorus_data* data = (se_chorus_data*) udata;
        data->stream.clear();
        data->ticks = 0;*/
}
class se_echo_data {
        public:
                double wet = 0.5;       // Ratio of processed signal from 0.0 to 1.0
                double feedback = 0.2;  // Percentage of output to feed back into the input from 0.0 to 1.0
                int delay = 500;        // Number of milliseconds to offset the processed signal from the input signal from 1 to 2000

                std::vector<Sint16> stream;
                int ticks = 0;
};
void sound_effect_echo(int channel, void* stream, int length, void* udata) {
        Sint16* oldstream;
        Sint16* newstream = (Sint16*) stream;
        se_echo_data* data = (se_echo_data*) udata;

        oldstream = (Sint16*)malloc(length);
        memcpy(oldstream, newstream, length);

        int offset = data->delay * 44.1 * 2;
        for (int i=0, e=0; i<length; i+=sizeof(Sint16), e++, data->ticks++) {
                if (data->ticks >= offset) {
                        if ((!data->stream.empty())&&(data->ticks >= offset)&&(e < offset)) {
                                newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (data->stream[data->ticks - offset] * data->wet);
                        } else if (e >= offset) {
                                newstream[e] = (oldstream[e] * (1.0 - data->wet)) + (oldstream[e - offset] * data->wet);
                        }
                        if (e >= offset) {
                                newstream[e] += sign(newstream[e]) * ((newstream[e] * (1.0 - data->wet)) + (newstream[e - offset] * data->wet)) * data->feedback;
                        }
                }
                data->stream.push_back(newstream[e]);
        }

        free(oldstream);

        return;
}
void sound_effect_echo_cleanup(int channel, void* udata) {
        se_echo_data* data = (se_echo_data*) udata;
        data->stream.clear();
        data->ticks = 0;
}
class se_flanger_data {
        public:
                float wet = 0.5, depth = 0.25, feedback = 0.0;
                int frequency = 0, wavetype = 1, delay = 0, phase = 2;
};
void sound_effect_flanger(int channel, void* stream, int length, void* udata) {

}
void sound_effect_flanger_cleanup(int channel, void* udata) {

}
class se_gargle_data {
        public:
                int rate = 1, wavetype = 1;
};
void sound_effect_gargle(int channel, void* stream, int length, void* udata) {

}
void sound_effect_gargle_cleanup(int channel, void* udata) {

}
class se_reverb_data {
        public:
                double gain = 0.0, mix = 0.0, time = 1000.0, ratio = 0.001;
};
void sound_effect_reverb(int channel, void* stream, int length, void* udata) {

}
void sound_effect_reverb_cleanup(int channel, void* udata) {

}
class se_compressor_data {
        public:
                double gain = 0.0, attack = 0.01, threshold = -10.0, ratio = 0.1;
                int release = 50, delay = 0;
};
void sound_effect_compressor(int channel, void* stream, int length, void* udata) {

}
void sound_effect_compressor_cleanup(int channel, void* udata) {

}
class se_equalizer_data {
        public:
                double gain = 0.0;
                int center = 10000, bandwidth = 36;
};
void sound_effect_equalizer(int channel, void* stream, int length, void* udata) {

}
void sound_effect_equalizer_cleanup(int channel, void* udata) {

}

#endif // _BEE_UTIL_SOUND_H
