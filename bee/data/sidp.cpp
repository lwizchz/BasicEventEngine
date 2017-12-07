/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DATA_SIDP
#define BEE_DATA_SIDP 1

#include <regex>

#include "sidp.hpp" // Include the engine headers

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/template/string.hpp"

#include "../enum.hpp"
#include "../engine.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	SIDP::SIDP() :
		type(-1), // Possible types: 0=string, 1=int, 2=double, 3=pointer
		container_type(0), // Possible containers: 0=plain, 1=vector, 2=map
		str(),
		integer(0),
		floating(0.0),
		pointer(nullptr)
	{}
	SIDP::SIDP(const SIDP& sidp) :
		type(sidp.type),
		container_type(sidp.container_type),
		str(sidp.str),
		integer(sidp.integer),
		floating(sidp.floating),
		pointer(nullptr)
	{
		if (container_type == 1) {
			pointer = new std::vector<SIDP>(*static_cast<std::vector<SIDP>*>(sidp.pointer));
		} else if (container_type == 2) {
			pointer = new std::map<SIDP,SIDP>(*static_cast<std::map<SIDP,SIDP>*>(sidp.pointer));
		} else {
			pointer = sidp.pointer;
		}
	}
	SIDP::SIDP(const std::string& ns, bool should_interpret) :
		type(0),
		container_type(0),
		str(ns),
		integer(0),
		floating(0.0),
		pointer(nullptr)
	{
		if (should_interpret) {
			interpret(ns);
		}
	}
	SIDP::SIDP(const std::string& ns) :
		SIDP(ns, true)
	{}
	SIDP::SIDP(const char* c_str) :
		SIDP(std::string(c_str), true)
	{}
	SIDP::SIDP(int ni) :
		type(1),
		container_type(0),
		str(),
		integer(ni),
		floating(0.0),
		pointer(nullptr)
	{}
	SIDP::SIDP(double nf) :
		type(2),
		container_type(0),
		str(),
		integer(0),
		floating(nf),
		pointer(nullptr)
	{}
	SIDP::SIDP(void* np) :
		type(3),
		container_type(0),
		str(),
		integer(0),
		floating(0.0),
		pointer(np)
	{}
	SIDP::SIDP(std::vector<SIDP>* nv) :
		SIDP()
	{
		vector(nv);
	}
	SIDP::SIDP(std::map<SIDP,SIDP>* nm) :
		SIDP()
	{
		map(nm);
	}
	SIDP::~SIDP() {
		reset();
	}

	int SIDP::reset() {
		if (pointer != nullptr) {
			if (container_type == 1) {
				delete static_cast<std::vector<SIDP>*>(pointer);
			} else if (container_type == 2) {
				delete static_cast<std::map<SIDP,SIDP>*>(pointer);
			}
		}

		type = -1;
		container_type = 0;

		str.clear();
		integer = 0;
		floating = 0.0;
		pointer = nullptr;

		return 0;
	}

	int SIDP::interpret(const std::string& ns) {
		reset();
		try {
			if ((ns[0] == '"')&&(ns[ns.length()-1] == '"')) { // String
				str = ns.substr(1, ns.length()-2);
				type = 0;
			} else if ((ns[0] == '[')&&(ns[ns.length()-1] == ']')) { // Array
				std::vector<SIDP>* v = new std::vector<SIDP>();
				vector_deserialize(ns, v);
				vector(v);
			} else if ((ns[0] == '{')&&(ns[ns.length()-1] == '}')) { // Map
				std::map<SIDP,SIDP>* m = new std::map<SIDP,SIDP>();
				map_deserialize(ns, m);
				map(m);
			} else if (is_str_integer(ns)) { // Integer
				integer = std::stoi(ns);
				type = 1;
			} else if (is_str_floating(ns)) { // Double
				floating = std::stod(ns);
				type = 2;
			} else if ((ns == "true")||(ns == "false")) { // Boolean
				type = 1;
				if (ns == "true") {
					integer = 1;
				} else {
					integer = 0;
				}
			} else { // Probably a string
				str = ns;
				type = 0;
			}
		} catch (const std::invalid_argument) {}

		if (type == -1) { // No possible type, this will only occur when std::stod or std::stoi fails
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "WARN: SIDP type not determined, storing as string: \"" + ns + "\"");
			type = 0;
			str = ns;
		}

		return 0; // Return 0 on success
	}
	int SIDP::vector(std::vector<SIDP>* v) {
		reset();
		type = 3;
		container_type = 1;
		pointer = v;
		return 0;
	}
	int SIDP::map(std::map<SIDP,SIDP>* m) {
		reset();
		type = 3;
		container_type = 2;
		pointer = m;
		return 0;
	}
	std::string SIDP::to_str() const {
		switch (type) {
			case 0: {
				return str;
			}
			case 1: {
				return std::to_string(integer);
			}
			case 2: {
				return std::to_string(floating);
			}
			case 3: {
				if (container_type == 1) {
					return vector_serialize(*static_cast<std::vector<SIDP>*>(pointer), false);
				} else if (container_type == 2) {
					return map_serialize(*static_cast<std::map<SIDP,SIDP>*>(pointer), false);
				} else {
					std::stringstream ss;
					ss << pointer;
					return ss.str();
				}
			}
			default: {
				return std::string();
			}
		}
	}

	// Return the requested type
	std::string SIDP::s(const std::string& file, int line) const {
		if (type != 0) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + ", not a string but the string was requested, called from " + file + ":" + bee_itos(line));
		}
		return str;
	}
	int SIDP::i(const std::string& file, int line) const {
		if ((type != 1)&&(type != 2)) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + ", not an integer but the integer was requested, called from " + file + ":" + bee_itos(line));
		}
		if (type == 2) {
			return static_cast<int>(floating);
		}
		return integer;
	}
	double SIDP::d(const std::string& file, int line) const {
		if ((type != 1)&&(type != 2)) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + ", not a double but the double was requested, called from " + file + ":" + bee_itos(line));
		}
		if (type == 1) {
			return static_cast<double>(integer);
		}
		return floating;
	}
	void* SIDP::p(const std::string& file, int line) const {
		if (type != 3) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + ", not a pointer but the pointer was requested, called from " + file + ":" + bee_itos(line));
		}
		return pointer;
	}
	SIDP SIDP::p(size_t index, const std::string& file, int line) const {
		if (type != 3) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + ", not a pointer but the pointer array was requested, called from " + file + ":" + bee_itos(line));
			return SIDP();
		}

		std::vector<SIDP>* v = static_cast<std::vector<SIDP>*>(pointer);
		if (index >= v->size()) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Vector index " + bee_itos(index) + " out of range");
			return SIDP();
		}

		return v->at(index);
	}
	SIDP SIDP::p(SIDP key, const std::string& file, int line) const {
		if (type != 3) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + ", not a pointer but the pointer array was requested, called from " + file + ":" + bee_itos(line));
			return SIDP();
		}

		std::map<SIDP,SIDP>* m = static_cast<std::map<SIDP,SIDP>*>(pointer);
		if (m->find(key) == m->end()) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Map key " + key.to_str() + " not found");
			return SIDP();
		}

		return m->at(key);
	}
	std::vector<SIDP>& SIDP::v(const std::string& file, int line) const {
		if ((type != 3)||(container_type != 1)) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + "." + bee_itos(container_type) + ", not a vector pointer, called from " + file + ":" + bee_itos(line));
		}

		return *static_cast<std::vector<SIDP>*>(pointer);
	}
	std::map<SIDP,SIDP>& SIDP::m(const std::string& file, int line) const {
		if ((type != 3)||(container_type != 2)) {
			messenger::send({"engine", "sidp"}, E_MESSAGE::WARNING, "Type is " + bee_itos(type) + "." + bee_itos(container_type) + ", not a map pointer, called from " + file + ":" + bee_itos(line));
		}

		return *static_cast<std::map<SIDP,SIDP>*>(pointer);
	}

	SIDP& SIDP::operator=(const SIDP& rhs) {
		if (this == &rhs) {
			return *this;
		}

		reset();
		this->type = rhs.type;
		this->container_type = rhs.container_type;

		switch (rhs.type) {
			case 0: {
				this->str = rhs.str;
				break;
			}
			case 1: {
				this->integer = rhs.integer;
				break;
			}
			case 2: {
				this->floating = rhs.floating;
				break;
			}
			case 3: {
				if (container_type == 1) {
					pointer = new std::vector<SIDP>(*static_cast<std::vector<SIDP>*>(rhs.pointer));
				} else if (container_type == 2) {
					pointer = new std::map<SIDP,SIDP>(*static_cast<std::map<SIDP,SIDP>*>(rhs.pointer));
				} else {
					pointer = rhs.pointer;
				}
				break;
			}
			default: {
				//throw std::runtime_error("Error: SIDP operator invalid type");
				break; // This should never be reached
			}
		}

		return *this;
	}
	SIDP& SIDP::operator=(const std::string& rhs) {
		reset();
		type = 0;
		str = rhs;
		return *this;
	}
	SIDP& SIDP::operator=(const char* rhs) {
		reset();
		type = 0;
		str = std::string(rhs);
		return *this;
	}
	SIDP& SIDP::operator=(int rhs) {
		reset();
		type = 1;
		integer = rhs;
		return *this;
	}
	SIDP& SIDP::operator=(double rhs) {
		reset();
		type = 2;
		floating = rhs;
		return *this;
	}
	SIDP& SIDP::operator=(void* rhs) {
		reset();
		type = 3;
		pointer = rhs;
		return *this;
	}
	SIDP& SIDP::operator=(const std::vector<SIDP>& rhs) {
		reset();
		type = 3;
		container_type = 1;
		pointer = new std::vector<SIDP>(rhs);
		return *this;
	}
	SIDP& SIDP::operator=(const std::map<SIDP,SIDP>& rhs) {
		reset();
		type = 3;
		container_type = 2;
		pointer = new std::map<SIDP,SIDP>(rhs);
		return *this;
	}

	SIDP& SIDP::operator+=(const SIDP& rhs) {
		if (this->type != rhs.type) {
			throw std::runtime_error("Error: SIDP type mismatch");
		}

		switch (this->type) {
			case 0: {
				this->str += rhs.str;
				break;
			}
			case 1: {
				this->integer += rhs.integer;
				break;
			}
			case 2: {
				this->floating += rhs.floating;
				break;
			}
			case 3: {
				//this->pointer += rhs.pointer;
			}
			default: {
				throw std::runtime_error("Error: SIDP operator invalid type");
			}
		}

		return *this;
	}
	SIDP& SIDP::operator+=(int rhs) {
		switch (this->type) {
			case 1: {
				this->integer += rhs;
				break;
			}
			case 2: {
				this->floating += rhs;
				break;
			}
			case 3: {
				/*char* tmp = (char*)this->pointer;
				tmp += rhs;
				this->pointer = (void*)tmp;
				break;*/
			}
			default: {
				throw std::runtime_error("Error: SIDP operator invalid type");
			}
		}

		return *this;
	}

	SIDP& SIDP::operator-=(const SIDP& rhs) {
		if (this->type != rhs.type) {
			throw std::runtime_error("Error: SIDP type mismatch");
		}

		switch (this->type) {
			case 1: {
				this->integer -= rhs.integer;
				break;
			}
			case 2: {
				this->floating -= rhs.floating;
				break;
			}
			case 3: {
				//this->pointer -= rhs.pointer;
			}
			default: {
				throw std::runtime_error("Error: SIDP operator invalid type");
			}
		}

		return *this;
	}
	SIDP& SIDP::operator-=(int rhs) {
		switch (this->type) {
			case 1: {
				this->integer -= rhs;
				break;
			}
			case 2: {
				this->floating -= rhs;
				break;
			}
			case 3: {
				/*char* tmp = (char*)this->pointer;
				tmp -= rhs;
				this->pointer = (void*)tmp;
				break;*/
			}
			default: {
				throw std::runtime_error("Error: SIDP operator invalid type");
			}
		}

		return *this;
	}

	bool operator<(const SIDP& a, const SIDP& b) {
		if (a.type != b.type) {
			return (a.type < b.type);
		} else if ((a.container_type != b.container_type)) {
			return (a.container_type < b.container_type);
		} else {
			switch (a.type) {
				case 0: {
					return (a.str < b.str);
				}
				case 1: {
					return (a.integer < b.integer);
				}
				case 2: {
					return (a.floating < b.floating);
				}
				case 3: {
					return (a.pointer < b.pointer);
				}
				default: {
					return true;
				}
			}
		}
	}
	std::ostream& operator<<(std::ostream& os, const SIDP& sidp) {
		switch (sidp.type) {
			case 0: {
				os << sidp.str;
				break;
			}
			case 1: {
				os << sidp.integer;
				break;
			}
			case 2: {
				os << sidp.floating;
				break;
			}
			case 3: {
				if (sidp.container_type == 1) {
					std::vector<SIDP>* v = static_cast<std::vector<SIDP>*>(sidp.pointer);
					os << "[";

					size_t i = 0;
					for (auto& e : *v) {
						os << e;
						if (i < v->size()-1) {
							os << ",";
						}
						++i;
					}

					os << "]";
				} else if (sidp.container_type == 2) {
					std::map<SIDP,SIDP>* m = static_cast<std::map<SIDP,SIDP>*>(sidp.pointer);
					os << "{";

					size_t i = 0;
					for (auto& p : *m) {
						os << p.first << ":" << p.second;
						if (i < m->size()-1) {
							os << ",";
						}
						++i;
					}

					os << "}";
				} else {
					os << sidp.pointer;
				}
				break;
			}
			default: {
				throw std::runtime_error("Error: SIDP operator invalid type");
			}
		}
		return os;
	}
	std::istream& operator>>(std::istream& is, SIDP& sidp) {
		std::string s;
		is >> s;
		sidp.interpret(s);
		return is;
	}
}

#endif // BEE_DATA_SIDP
