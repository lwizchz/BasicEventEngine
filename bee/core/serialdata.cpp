/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_SERIALDATA
#define BEE_CORE_SERIALDATA 1

#include "serialdata.hpp" // Include the engine headers

#include <sstream>

namespace bee {
	SerialData::SerialData() :
		data(),
		pos(0),
		is_writing(true)
	{}
	SerialData::SerialData(size_t initial_size) :
		SerialData()
	{
		data.reserve(initial_size);
	}
	SerialData::SerialData(const std::vector<Uint8>& new_data) :
		SerialData()
	{
		is_writing = false;
		data = new_data;
	}

	int SerialData::reset() {
		data.clear();
		pos = 0;
		return 0;
	}
	int SerialData::rewind() {
		pos = 0;
		return 0;
	}
	int SerialData::set_writing(bool new_is_writing) {
		is_writing = new_is_writing;
		return 0;
	}

	int SerialData::store_char(unsigned char& d) {
		if (is_writing) {
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::CHAR));
			data.push_back(d);

			pos = data.size();
		} else {
			if (pos+1 >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Char deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::CHAR)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Char deserialization failed: incorrect type");
				return 2;
			}

			d = data[pos++];
		}

		return 0;
	}
	int SerialData::store_int(int& d) {
		/*int min = -32767;
		int max = 32767;*/

		if (is_writing) {
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::INT));
			data.push_back(d >> 8);
			data.push_back(d);

			pos = data.size();
		} else {
			if (pos+2 >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Int deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::INT)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Int deserialization failed: incorrect type");
				return 2;
			}

			d = data[pos++] << 8;
			d += data[pos++];
		}

		return 0;
	}
	int SerialData::store_float(float& d) {
		/*int min = -32767;
		int max = 32767;*/

		float factor = 10.0f;

		if (is_writing) {
			int f = static_cast<int>(d * factor);

			data.push_back(static_cast<Uint8>(E_DATA_TYPE::FLOAT));
			data.push_back(f >> 8);
			data.push_back(f);

			pos = data.size();
		} else {
			if (pos+2 >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Float deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::FLOAT)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Float deserialization failed: incorrect type");
				return 2;
			}

			int f = data[pos++] << 8;
			f += data[pos++];
			d = f / factor;
		}

		return 0;
	}
	int SerialData::store_double(double& d) {
		if (is_writing) {
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::DOUBLE));

			std::stringstream ss;
			ss << d;
			std::string s = ss.str();

			data.push_back(static_cast<Uint8>(s.size() >> 8));
			data.push_back(static_cast<Uint8>(s.size()));

			for (auto& c : s) {
				data.push_back(c);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Double deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::DOUBLE)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Double deserialization failed: incorrect type");
				return 2;
			}

			std::string s;

			size_t size = data[pos++] << 8;
			size += data[pos++];

			if (pos+size > data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "Double deserialization failed: out of bounds size");
				return 1;
			}

			for (size_t i=0; i<size; ++i) {
				s.push_back(data[pos+i]);
			}

			std::stringstream ss (s);
			ss >> d;

			pos += size;
		}

		return 0;
	}
	int SerialData::store_string(std::string& d) {
		if (is_writing) {
			data.reserve(data.size() + d.size() + 1);
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::STRING));

			data.push_back(static_cast<Uint8>(d.size() >> 8));
			data.push_back(static_cast<Uint8>(d.size()));

			for (auto& c : d) {
				data.push_back(c);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "String deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::STRING)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "String deserialization failed: incorrect type");
				return 2;
			}

			d.clear();

			size_t size = data[pos++] << 8;
			size += data[pos++];

			if (pos+size > data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "String deserialization failed: out of bounds size");
				return 1;
			}

			for (size_t i=0; i<size; ++i) {
				d.push_back(data[pos+i]);
			}
			pos += size;
		}

		return 0;
	}
	int SerialData::store_serial_v(std::vector<Uint8>& d) {
		if (is_writing) {
			data.reserve(data.size() + d.size() + 1);
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::SERIAL));

			data.push_back(static_cast<Uint8>(d.size() >> 8));
			data.push_back(static_cast<Uint8>(d.size()));

			for (auto& e : d) {
				data.push_back(e);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "SVector deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::SERIAL)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "SVector deserialization failed: incorrect type");
				return 2;
			}

			d.clear();

			size_t size = data[pos++] << 8;
			size += data[pos++];

			if (pos+size > data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "SVector deserialization failed: out of bounds size");
				return 1;
			}

			d.reserve(size);
			for (size_t i=0; i<size; ++i) {
				d.push_back(data[pos+i]);
			}
			pos += size;
		}

		return 0;
	}
	int SerialData::store_serial_m(std::map<std::string,std::vector<Uint8>>& d) {
		if (is_writing) {
			data.reserve(data.size() + d.size() + 1);
			data.push_back(static_cast<Uint8>(E_DATA_TYPE::SERIAL));

			data.push_back(static_cast<Uint8>(d.size() >> 8));
			data.push_back(static_cast<Uint8>(d.size()));
			pos += 3;

			for (auto& e : d) {
				std::string k (e.first);
				this->store_string(k);
				this->store_serial_v(e.second);
			}

			pos = data.size();
		} else {
			if (pos >= data.size()) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "SMap deserialization failed: out of bounds");
				return 1;
			}

			if (data.at(pos++) != static_cast<Uint8>(E_DATA_TYPE::SERIAL)) {
				messenger::send({"engine", "serialdata"}, E_MESSAGE::ERROR, "SMap deserialization failed: incorrect type");
				return 2;
			}

			d.clear();

			size_t size = data[pos++] << 8;
			size += data[pos++];
			for (size_t i=0; i<size; ++i) {
				std::string k;
				this->store_string(k);

				std::vector<Uint8> v;
				this->store_serial_v(v);

				d[k] = v;
			}
		}

		return 0;
	}

	int SerialData::store(unsigned char& d) {
		return store_char(d);
	}
	int SerialData::store(int& d) {
		return store_int(d);
	}
	int SerialData::store(float& d) {
		return store_float(d);
	}
	int SerialData::store(double& d) {
		return store_double(d);
	}
	int SerialData::store(std::string& d) {
		return store_string(d);
	}
	int SerialData::store(SIDP& d) {
		if (is_writing) {
			std::string s = d.to_str();
			return store_string(s);
		} else {
			std::string s;
			int r = store_string(s);
			d.interpret(s);
			return r;
		}
	}

	std::vector<Uint8> SerialData::get() const {
		return data;
	}
	Uint8 SerialData::peek() const {
		if (pos >= data.size()) {
			return 0;
		}

		return data.at(pos);
	}
}

#endif // BEE_CORE_SERIALDATA
