/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DATA_SERIALDATA_H
#define BEE_DATA_SERIALDATA_H 1

#include <vector>
#include <map>

#include <SDL2/SDL_net.h>

#include "../enum.hpp"

#include "sidp.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	class SerialData {
			std::vector<Uint8> data;
			size_t pos;
			bool is_writing;
		public:
			SerialData();
			explicit SerialData(size_t);
			explicit SerialData(const std::vector<Uint8>&);

			int reset();
			int rewind();
			int set_writing(bool);

			int store_char(unsigned char&);
			int store_int(int&);
			int store_float(float&);
			int store_double(double&);
			int store_string(std::string&);
			int store_serial_v(std::vector<Uint8>&);
			int store_serial_m(std::map<std::string,std::vector<Uint8>>&);

			int store(unsigned char);
			int store(int);
			int store(float);
			int store(double);
			int store(std::string);
			int store(SIDP);

			template <typename A>
			int store_vector(std::vector<A>&);
			template <typename A, typename B>
			int store_map(std::map<A,B>&);
			template <typename T, typename A, typename B>
			int store_map_v(std::map<A,B>&);

			template <typename A, typename B>
			typename std::enable_if<std::is_same<A, std::vector<B>>::value, int>::type store(A&);
			template <typename A, typename B, typename C>
			typename std::enable_if<std::is_same<A, std::map<B,C>>::value, int>::type store(A&);

			int get(unsigned char&);
			int get(int&);
			int get(float&);
			int get(double&);
			int get(std::string&);
			int get(SIDP&);

			std::vector<Uint8> get() const;
			Uint8 peek() const;
	};

	template <typename A>
	int SerialData::store_vector(std::vector<A>& d) {
		if (is_writing) {
			data.reserve(data.size() + d.size() + 1);
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::VECTOR));

			data.push_back(static_cast<Uint8>(d.size() >> 8));
			data.push_back(static_cast<Uint8>(d.size()));

			for (auto& e : d) {
				this->store(e);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Vector deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::VECTOR)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Vector deserialization failed: incorrect type");
				return 2;
			}

			d.clear();

			size_t size = data[pos++] << 8;
			size += data[pos++];
			for (size_t i=0; i<size; ++i) {
				A j;
				this->get(j);
				d.push_back(j);
			}
		}

		return 0;
	}
	template <typename A, typename B>
	int SerialData::store_map(std::map<A,B>& d) {
		if (is_writing) {
			data.reserve(data.size() + d.size() + 1);
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::MAP));

			data.push_back(static_cast<Uint8>(d.size() >> 8));
			data.push_back(static_cast<Uint8>(d.size()));

			for (auto& kv : d) {
				A k = kv.first;
				B v = kv.second;
				this->store(k);
				this->store(v);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Map deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::MAP)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Map deserialization failed: incorrect type");
				return 2;
			}

			d.clear();

			size_t size = data[pos++] << 8;
			size += data[pos++];
			for (size_t i=0; i<size; ++i) {
				A k;
				B v;
				this->get(k);
				this->get(v);
				d.emplace(k, v);
			}
		}

		return 0;
	}
	template <typename T, typename A, typename B>
	int SerialData::store_map_v(std::map<A,B>& d) {
		if (is_writing) {
			data.reserve(data.size() + d.size() + 1);
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::MAP));

			data.push_back(static_cast<Uint8>(d.size() >> 8));
			data.push_back(static_cast<Uint8>(d.size()));

			for (auto& kv : d) {
				A k = kv.first;
				B v = kv.second;
				this->store(k);
				this->store_vector(v);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Map deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::MAP)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Map deserialization failed: incorrect type");
				return 2;
			}

			d.clear();

			size_t size = data[pos++] << 8;
			size += data[pos++];
			for (size_t i=0; i<size; ++i) {
				A k;
				B v;
				this->get(k);
				this->store_vector(v);
				d.emplace(k, v);
			}
		}

		return 0;
	}

	template <typename A, typename B>
	typename std::enable_if<std::is_same<A, std::vector<B>>::value, int>::type
	SerialData::store(A& d) {
		return store_vector<B>(d);
	}
	template <typename A, typename B, typename C>
	typename std::enable_if<std::is_same<A, std::map<B,C>>::value, int>::type
	SerialData::store(A& d) {
		return store_map<B,C>(d);
	}
}

#endif // BEE_DATA_SERIALDATA_H
