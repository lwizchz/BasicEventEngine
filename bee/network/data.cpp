/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_DATA
#define BEE_NETWORK_DATA 1

#include "data.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	NetworkData::NetworkData() :
		signals(0),
		data()
	{}
	NetworkData::NetworkData(Uint8 combined_signal) :
		NetworkData()
	{
		signals = combined_signal;
	}
	NetworkData::NetworkData(E_NETSIG1 signal1, E_NETSIG2 signal2) :
		NetworkData()
	{
		if ((signal1 == E_NETSIG1::INVALID)||(signal2 == E_NETSIG2::INVALID)) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Invalid network signal");
		}

		Uint8 s1 = static_cast<Uint8>(signal1);
		Uint8 s2 = static_cast<Uint8>(signal2);
		signals = s1 << 4;
		signals += s2 << 4 >> 4;
	}
	NetworkData::NetworkData(E_NETSIG1 signal1, E_NETSIG2 signal2, const std::vector<Uint8>& _data) :
		NetworkData(signal1, signal2)
	{
		append_data(_data);
	}
	NetworkData::NetworkData(const NetworkData& other) :
		signals(other.signals),
		data(other.data)
	{}
	NetworkData::~NetworkData() {
		reset();
	}
	int NetworkData::reset() {
		signals = 0;
		data.clear();
		return 0;
	}

	NetworkData& NetworkData::operator=(const NetworkData& rhs) {
		if (this != &rhs) {
			this->reset();
			this->signals = rhs.signals;
			this->data = rhs.data;
		}
		return *this;
	}

	int NetworkData::append_data(const std::vector<Uint8>& new_data) {
		data.reserve(data.size() + new_data.size());
		for (auto& e : new_data) {
			data.push_back(e);
		}
		return 0;
	}
	int NetworkData::append_data(const std::vector<Uint8>& new_data, size_t offset) {
		data.reserve(data.size() + new_data.size() - offset);
		for (size_t i=offset; i<new_data.size(); ++i) {
			data.push_back(new_data[i]);
		}
		return 0;
	}

	std::vector<Uint8> NetworkData::get() const {
		return data;
	}

	Uint8 NetworkData::get_signals() const {
		return signals;
	}
	E_NETSIG1 NetworkData::get_signal1() const {
		return static_cast<E_NETSIG1>(
			signals >> 4
		);
	}
	E_NETSIG2 NetworkData::get_signal2() const {
		return static_cast<E_NETSIG2>(
			signals - (signals >> 4 << 4)
		);
	}
}

#endif // BEE_NETWORK_DATA
