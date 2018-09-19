/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_SOUNDEFFECT
#define BEE_CORE_SOUNDEFFECT 1

// Sound effect functions, not fully implemented

#include <cmath>

#include <SDL2/SDL_mixer.h> // Include the SDL2_mixer headers for the audio data types

#include "soundeffects.hpp"

#include "../util/real.hpp"

#include "../resource/sound.hpp"

namespace bee { namespace soundeffects {
	namespace internal {
		const std::map<Variant,Variant> chorus_params {
			{"wet", Variant(0.5)},        // from 0.0 to 1.0
			{"depth", Variant(0.25)},     // from 0.0 to 1.0
			{"feedback", Variant(0.0)},   // from 0.0 to 1.0
			{"frequency", Variant(10.0)}, // from 1.0 to 100.0
			{"delay", Variant(5.0)}       // from 0.5 to 50.0
		};
		const std::map<Variant,Variant> echo_params {
			{"wet", Variant(0.5)},      // from 0.0 to 1.0
			{"feedback", Variant(0.2)}, // from 0.0 to 1.0
			{"delay", Variant(300.0)}   // from 100.0 to 1000.0
		};
		const std::map<Variant,Variant> flanger_params {
			{"wet", Variant(0.5)},        // from 0.0 to 1.0
			{"depth", Variant(0.25)},     // from 0.0 to 1.0
			{"feedback", Variant(0.0)},   // from 0.0 to 1.0
			{"frequency", Variant(10.0)}, // from 1.0 to 100.0
			{"delay", Variant(10.0)}      // from 0.5 to 20.0
		};
		const std::map<Variant,Variant> gargle_params {};
		const std::map<Variant,Variant> reverb_params {};
		const std::map<Variant,Variant> compressor_params {};
		const std::map<Variant,Variant> equalizer_params {};
	}

	std::map<Variant,Variant> get_params(E_SOUNDEFFECT type) {
		switch (type) {
			case E_SOUNDEFFECT::CHORUS:     return internal::chorus_params;
			case E_SOUNDEFFECT::ECHO:       return internal::echo_params;
			case E_SOUNDEFFECT::FLANGER:    return internal::flanger_params;
			case E_SOUNDEFFECT::GARGLE:     return internal::gargle_params;
			case E_SOUNDEFFECT::REVERB:     return internal::reverb_params;
			case E_SOUNDEFFECT::COMPRESSOR: return internal::compressor_params;
			case E_SOUNDEFFECT::EQUALIZER:  return internal::equalizer_params;
		}
		return {};
	}

	/**
	* Operate on the given sound data to produce a chorus effect.
	* @param channel the mixer channel which the sound is playing on
	* @param stream the new stream data
	* @param length the length of the new stream data in bytes
	* @param udata the effect data struct
	*/
	void chorus(int channel, void* stream, int len, void* udata) {
		Sint16* istream = static_cast<Sint16*>(stream);
		std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);

		if (data->find("stream") == data->end()) {
			data->emplace("stream", Variant(E_DATA_TYPE::VECTOR));
		}
		std::vector<Variant>& vstream = data->at("stream").v;
		std::map<Variant,Variant> p = data->at("params").m;

		const double p_wet = p["wet"].f;
		const double p_depth = p["depth"].f;
		const double p_feedback = p["feedback"].f;
		const double p_frequency = p["frequency"].f;
		const double p_delay = p["delay"].f;

		for (int i=0, e=0; i<len-1; i+=sizeof(Sint16)*2, e+=2) {
			int lfo = 44.1 * (sin(PI/10.0 * p_frequency * vstream.size())*30.0*p_depth + 30.0 + p_delay);
			unsigned int _lfo = abs(lfo);
			if (vstream.size() > _lfo) {
				// Left stereo
				istream[e] = (istream[e] * (1.0 - p_wet)) + (vstream[vstream.size() - lfo].i * p_wet);
				istream[e] += vstream[vstream.size() - lfo].i * p_feedback;
				// Right stereo
				istream[e+1] = (istream[e+1] * (1.0 - p_wet)) + (vstream[vstream.size() - lfo + 1].i * p_wet);
				istream[e+1] += vstream[vstream.size() - lfo + 1].i * p_feedback;
			}
			vstream.push_back(Variant(static_cast<int>(istream[e])));
			vstream.push_back(Variant(static_cast<int>(istream[e+1])));
		}
	}
	/**
	* Clean up the chorus effect's data struct.
	* @param channel the mixer channel which the sound is playing on
	* @param udata the effect data struct
	*/
	void chorus_cleanup(int channel, void* udata) {
		std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);
		delete data;
	}
	/**
	* Operate on the given sound data to produce an echo effect.
	* @param channel the mixer channel which the sound is playing on
	* @param stream the new stream data
	* @param length the length of the new stream data in bytes
	* @param udata the effect data struct
	*/
	void echo(int channel, void* stream, int len, void* udata) {
		Sint16* istream = static_cast<Sint16*>(stream);
		std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);

		if (data->find("stream") == data->end()) {
			data->emplace("stream", Variant(E_DATA_TYPE::VECTOR));
		}
		std::vector<Variant>& vstream = data->at("stream").v;
		std::map<Variant,Variant> p = data->at("params").m;

		const double p_wet = p["wet"].f;
		const double p_feedback = p["feedback"].f;
		const double p_delay = p["delay"].f;

		unsigned int offset = 44.1 * p_delay;
		for (int i=0, e=0; i<len-1; i+=sizeof(Sint16)*2, e+=2) {
			if (vstream.size() > offset) {
				// Left stereo
				istream[e] = (istream[e] * (1.0 - p_wet)) + (vstream[vstream.size() - offset].i * p_wet);
				istream[e] += vstream[vstream.size() - offset].i * p_feedback;

				// Right stereo
				istream[e+1] = (istream[e+1] * (1.0 - p_wet)) + (vstream[vstream.size() - offset + 1].i * p_wet);
				istream[e+1] += vstream[vstream.size() - offset + 1].i * p_feedback;
			}
			vstream.push_back(Variant(static_cast<int>(istream[e])));
			vstream.push_back(Variant(static_cast<int>(istream[e+1])));
		}
	}
	/**
	* Clean up the echo effect's data struct.
	* @param channel the mixer channel which the sound is playing on
	* @param udata the effect data struct
	*/
	void echo_cleanup(int channel, void* udata) {
		std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);
		delete data;
	}
	/**
	* Operate on the given sound data to produce a flanger effect.
	* @param channel the mixer channel which the sound is playing on
	* @param stream the new stream data
	* @param length the length of the new stream data in bytes
	* @param udata the effect data struct
	*/
	void flanger(int channel, void* stream, int len, void* udata) {
		Sint16* istream = static_cast<Sint16*>(stream);
		std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);

		if (data->find("stream") == data->end()) {
			data->emplace("stream", Variant(E_DATA_TYPE::VECTOR));
		}
		std::vector<Variant>& vstream = data->at("stream").v;
		std::map<Variant,Variant> p = data->at("params").m;

		const double p_wet = p["wet"].f;
		const double p_depth = p["depth"].f;
		const double p_feedback = p["feedback"].f;
		const double p_frequency = p["frequency"].f;
		const double p_delay = p["delay"].f;

		const double d = 0.5 * p_delay;

		for (int i=0, e=0; i<len-1; i+=sizeof(Sint16)*2, e+=2) {
			//int lfo = 44.1 * ((sin(PI/220500.0 * p_frequency * vstream.size()) + 1.0) * p_depth + 1.0) * d + d;
			int lfo = 44.1 * (sin(PI/10.0 * p_frequency * vstream.size())*30.0*p_depth + 30.0 + d);
			unsigned int _lfo = abs(lfo);
			if (vstream.size() > _lfo) {
				// Left stereo
				istream[e] = (istream[e] * (1.0 - p_wet)) + (vstream[vstream.size() - lfo].i * p_wet);
				istream[e] += vstream[vstream.size() - lfo].i * p_feedback;
				// Right stereo
				istream[e+1] = (istream[e+1] * (1.0 - p_wet)) + (vstream[vstream.size() - lfo + 1].i * p_wet);
				istream[e+1] += vstream[vstream.size() - lfo + 1].i * p_feedback;
			}
			vstream.push_back(Variant(static_cast<int>(istream[e])));
			vstream.push_back(Variant(static_cast<int>(istream[e+1])));
		}
	}
	/**
	* Clean up the flanger effect's data struct.
	* @param channel the mixer channel which the sound is playing on
	* @param udata the effect data struct
	*/
	void flanger_cleanup(int channel, void* udata) {
		std::map<Variant,Variant>* data = static_cast<std::map<Variant,Variant>*>(udata);
		delete data;
	}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void gargle(int channel, void* stream, int len, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void gargle_cleanup(int channel, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void reverb(int channel, void* stream, int len, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void reverb_cleanup(int channel, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void compressor(int channel, void* stream, int len, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void compressor_cleanup(int channel, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void equalizer(int channel, void* stream, int len, void* udata) {}
	/**
	* @unimplemented This sound effect is currently unimplemented and will have no effect.
	*/
	void equalizer_cleanup(int channel, void* udata) {}
}}

#endif // BEE_CORE_SOUNDEFFECT
