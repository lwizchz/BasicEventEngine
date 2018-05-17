/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DATA_VARIANT
#define BEE_DATA_VARIANT 1

#include <sstream>
#include <regex>

#include "variant.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/template/string.hpp"

#include "../enum.hpp"
#include "../engine.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	Variant::Variant() :
		Variant(E_DATA_TYPE::NONE)
	{}
	Variant::Variant(E_DATA_TYPE _type) :
		type(_type)
	{
		switch (type) {
			case E_DATA_TYPE::NONE: {
				p = nullptr;
				break;
			}
			case E_DATA_TYPE::CHAR: {
				c = '\0';
				break;
			}
			case E_DATA_TYPE::INT: {
				i = 0;
				break;
			}
			case E_DATA_TYPE::FLOAT: {
				f = 0.0f;
				break;
			}
			case E_DATA_TYPE::DOUBLE: {
				d = 0.0;
				break;
			}
			case E_DATA_TYPE::STRING: {
				new (&s) std::string;
				break;
			}
			case E_DATA_TYPE::VECTOR: {
				new (&v) std::vector<Variant>;
				break;
			}
			case E_DATA_TYPE::MAP: {
				new (&m) std::map<Variant,Variant>;
				break;
			}
			case E_DATA_TYPE::SERIAL: {
				new (&sd) SerialData;
				break;
			}
		}
	}
	Variant::Variant(const Variant& var) :
		type(var.type)
	{
		switch (type) {
			case E_DATA_TYPE::NONE: {
				p = var.p;
				break;
			}
			case E_DATA_TYPE::CHAR: {
				c = var.c;
				break;
			}
			case E_DATA_TYPE::INT: {
				i = var.i;
				break;
			}
			case E_DATA_TYPE::FLOAT: {
				f = var.f;
				break;
			}
			case E_DATA_TYPE::DOUBLE: {
				d = var.d;
				break;
			}
			case E_DATA_TYPE::STRING: {
				new (&s) std::string(var.s);
				break;
			}
			case E_DATA_TYPE::VECTOR: {
				new (&v) std::vector<Variant>(var.v);
				break;
			}
			case E_DATA_TYPE::MAP: {
				new (&m) std::map<Variant,Variant>(var.m);
				break;
			}
			case E_DATA_TYPE::SERIAL: {
				new (&sd) SerialData(var.sd);
				break;
			}
		}
	}
	Variant::Variant(const std::string& _s, bool should_interpret) :
		Variant()
	{
		if (should_interpret) {
			interpret(_s);
		} else {
			type = E_DATA_TYPE::STRING;
			new (&s) std::string(_s);
		}
	}
	Variant::Variant(const char* _s) :
		Variant(std::string(_s))
	{}

	Variant::Variant(unsigned char _c) :
		type(E_DATA_TYPE::CHAR),
		c(_c)
	{}
	Variant::Variant(int _i) :
		type(E_DATA_TYPE::INT),
		i(_i)
	{}
	Variant::Variant(float _f) :
		type(E_DATA_TYPE::FLOAT),
		f(_f)
	{}
	Variant::Variant(double _d) :
		type(E_DATA_TYPE::DOUBLE),
		d(_d)
	{}
	Variant::Variant(const std::string& _s) :
		type(E_DATA_TYPE::STRING),
		s(_s)
	{}
	Variant::Variant(const std::vector<Variant>& _v) :
		type(E_DATA_TYPE::VECTOR),
		v(_v)
	{}
	Variant::Variant(const std::map<Variant,Variant>& _m) :
		type(E_DATA_TYPE::MAP),
		m(_m)
	{}
	Variant::Variant(const SerialData& _sd) :
		type(E_DATA_TYPE::SERIAL),
		sd(_sd)
	{}
	Variant::Variant(void* _p) :
		type(E_DATA_TYPE::NONE),
		p(_p)
	{}

	Variant::~Variant() {
		reset();
	}
	int Variant::reset() {
		switch (type) {
			case E_DATA_TYPE::STRING: {
				s.clear();
				s.~basic_string();
				break;
			}
			case E_DATA_TYPE::VECTOR: {
				v.clear();
				v.~vector<Variant>();
				break;
			}
			case E_DATA_TYPE::MAP: {
				m.clear();
				m.~map<Variant,Variant>();
				break;
			}
			case E_DATA_TYPE::SERIAL: {
				sd.reset();
				sd.~SerialData();
				break;
			}
			default: {}
		}

		type = E_DATA_TYPE::NONE;
		p = nullptr;

		return 0;
	}

	E_DATA_TYPE Variant::get_type() const {
		return type;
	}

	int Variant::interpret(const std::string& ns) {
		reset();
		if (ns.empty()) {
			return 0;
		}

		try {
			std::string _ns (util::trim(ns));
			if ((_ns[0] == '"')&&(_ns[_ns.length()-1] == '"')) { // String
				type = E_DATA_TYPE::STRING;
				new (&s) std::string(ns.substr(1, ns.length()-2));
			} else if ((_ns[0] == '[')&&(_ns[_ns.length()-1] == ']')) { // Array
				type = E_DATA_TYPE::VECTOR;
				new (&v) std::vector<Variant>;
				util::vector_deserialize(_ns, &v);
			} else if ((_ns[0] == '{')&&(_ns[_ns.length()-1] == '}')) { // Map
				type = E_DATA_TYPE::MAP;
				new (&m) std::map<Variant,Variant>;
				util::map_deserialize(_ns, &m);
			} else if (util::string::is_integer(_ns)) { // Integer
				type = E_DATA_TYPE::INT;
				i = std::stoi(_ns);
			} else if (util::string::is_floating(_ns)) { // Double
				type = E_DATA_TYPE::DOUBLE;
				d = std::stod(_ns);
			} else if ((_ns == "true")||(_ns == "false")) { // Boolean
				type = E_DATA_TYPE::INT;
				if (_ns == "true") {
					i = 1;
				} else {
					i = 0;
				}
			} else { // Probably a string
				messenger::send({"engine", "variant"}, E_MESSAGE::WARNING, "Variant type not determined, storing as string: \"" + ns + "\"");
				type = E_DATA_TYPE::STRING;
				new (&s) std::string(ns);
			}
		} catch (const std::invalid_argument&) {}

		if (type == E_DATA_TYPE::NONE) { // No possible type, this will only occur when std::stod or std::stoi fails
			messenger::send({"engine", "variant"}, E_MESSAGE::WARNING, "Variant type not determined, storing as string: \"" + ns + "\"");
			type = E_DATA_TYPE::STRING;
			new (&s) std::string(ns);
		}

		return 0; // Return 0 on success
	}
	std::string Variant::to_str(bool should_pretty_print) const {
		switch (type) {
			case E_DATA_TYPE::NONE: {
				std::stringstream ss;
				ss << p;
				return ss.str();
			}
			case E_DATA_TYPE::CHAR: {
				return std::string(1, c);
			}
			case E_DATA_TYPE::INT: {
				return std::to_string(i);
			}
			case E_DATA_TYPE::FLOAT: {
				return std::to_string(f);
			}
			case E_DATA_TYPE::DOUBLE: {
				return std::to_string(d);
			}
			case E_DATA_TYPE::STRING: {
				return s;
			}
			case E_DATA_TYPE::VECTOR: {
				return util::vector_serialize(v, should_pretty_print);
			}
			case E_DATA_TYPE::MAP: {
				return util::map_serialize(m, should_pretty_print);
			}
			case E_DATA_TYPE::SERIAL: {
				return util::vector_serialize(sd.get(), should_pretty_print);
			}
			default: {
				throw std::runtime_error("Error: Attempt to stringify Variant of invalid type");
			}
		}
	}
	std::string Variant::to_str() const {
		return to_str(false);
	}

	Variant& Variant::operator=(const Variant& rhs) {
		if (this != &rhs) {
			this->reset();

			this->type = rhs.type;
			switch (this->type) {
				case E_DATA_TYPE::NONE: {
					this->p = nullptr;
					break;
				}
				case E_DATA_TYPE::CHAR: {
					this->c = rhs.c;
					break;
				}
				case E_DATA_TYPE::INT: {
					this->i = rhs.i;
					break;
				}
				case E_DATA_TYPE::FLOAT: {
					this->f = rhs.f;
					break;
				}
				case E_DATA_TYPE::DOUBLE: {
					this->d = rhs.d;
					break;
				}
				case E_DATA_TYPE::STRING: {
					new (&s) std::string(rhs.s);
					break;
				}
				case E_DATA_TYPE::VECTOR: {
					new (&v) std::vector<Variant>(rhs.v);
					break;
				}
				case E_DATA_TYPE::MAP: {
					new (&m) std::map<Variant,Variant>(rhs.m);
					break;
				}
				case E_DATA_TYPE::SERIAL: {
					new (&sd) SerialData(rhs.sd);
					break;
				}
			}
		}
		return *this;
	}
	Variant& Variant::operator=(unsigned char _c) {
		*this = Variant(_c);
		return *this;
	}
	Variant& Variant::operator=(int _i) {
		*this = Variant(_i);
		return *this;
	}
	Variant& Variant::operator=(float _f) {
		*this = Variant(_f);
		return *this;
	}
	Variant& Variant::operator=(double _d) {
		*this = Variant(_d);
		return *this;
	}
	Variant& Variant::operator=(const std::string& _s) {
		*this = Variant(_s);
		return *this;
	}
	Variant& Variant::operator=(const char* _s) {
		*this = Variant(_s);
		return *this;
	}
	Variant& Variant::operator=(const std::vector<Variant>& _v) {
		*this = Variant(_v);
		return *this;
	}
	Variant& Variant::operator=(const std::map<Variant,Variant>& _m) {
		*this = Variant(_m);
		return *this;
	}
	Variant& Variant::operator=(const SerialData& _sd) {
		*this = Variant(_sd);
		return *this;
	}
	Variant& Variant::operator=(void* _p) {
		*this = Variant(_p);
		return *this;
	}

	bool Variant::operator==(const Variant& rhs) const {
		if (this->type != rhs.type) {
			return false;
		}

		switch (this->type) {
			case E_DATA_TYPE::NONE: {
				return (this->p == rhs.p);
			}
			case E_DATA_TYPE::CHAR: {
				return (this->c == rhs.c);
			}
			case E_DATA_TYPE::INT: {
				return (this->i == rhs.i);
			}
			case E_DATA_TYPE::FLOAT: {
				return (this->f == rhs.f);
			}
			case E_DATA_TYPE::DOUBLE: {
				return (this->d == rhs.d);
			}
			case E_DATA_TYPE::STRING: {
				return (this->s == rhs.s);
			}
			case E_DATA_TYPE::VECTOR: {
				return (this->v == rhs.v);
			}
			case E_DATA_TYPE::MAP: {
				return (this->m == rhs.m);
			}
			case E_DATA_TYPE::SERIAL: {
				//return (this->sd == rhs.sd);
				return false;
			}
		}

		return false;
	}

	bool operator<(const Variant& a, const Variant& b) {
		if (a.type != b.type) {
			//throw std::runtime_error("Error: Invalid comparison on Variants of differing types");
			return (static_cast<int>(a.type) < static_cast<int>(b.type)); // Compare the type values so different types can be used in an ordered map
		}

		switch (a.type) {
			case E_DATA_TYPE::NONE: {
				return (a.p < b.p);
			}
			case E_DATA_TYPE::CHAR: {
				return (a.c < b.c);
			}
			case E_DATA_TYPE::INT: {
				return (a.i < b.i);
			}
			case E_DATA_TYPE::FLOAT: {
				return (a.f < b.f);
			}
			case E_DATA_TYPE::DOUBLE: {
				return (a.d < b.d);
			}
			case E_DATA_TYPE::STRING: {
				return (a.s < b.s);
			}
			case E_DATA_TYPE::VECTOR: {
				return (a.v < b.v);
			}
			case E_DATA_TYPE::MAP: {
				return (a.m < b.m);
			}
			case E_DATA_TYPE::SERIAL: {
				//return (a.sd < b.sd);
				return false;
			}
			default: {
				throw std::runtime_error("Error: Attempt to compare Variants of invalid types");
			}
		}
	}
	std::ostream& operator<<(std::ostream& os, const Variant& var) {
		os << var.to_str();
		return os;
	}
	std::istream& operator>>(std::istream& is, Variant& var) {
		std::string s (std::istreambuf_iterator<char>(is), {});
		var.interpret(s);
		return is;
	}
}

#endif // BEE_DATA_VARIANT
