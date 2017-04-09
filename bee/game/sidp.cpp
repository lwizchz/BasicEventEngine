/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_SIDP
#define _BEE_GAME_SIDP 1

#include "../game.hpp" // Include the engine headers

BEE::SIDP::SIDP() {}
BEE::SIDP::SIDP(const std::string& ns) :
	type(0),
	str(ns)
{}
BEE::SIDP::SIDP(const std::string& ns, bool should_interpret) {
	if (should_interpret) {
		interpret(ns);
	} else {
		type = 0;
		str = ns;
	}
}
BEE::SIDP::SIDP(int ni) :
	type(1),
	integer(ni)
{}
BEE::SIDP::SIDP(double nf) :
	type(2),
	floating(nf)
{}
BEE::SIDP::SIDP(void* np) :
	type(3),
	pointer(np)
{}

int BEE::SIDP::reset() {
	str.clear();
	integer = 0;
	floating = 0.0;
	pointer = nullptr;
	return 0;
}

int BEE::SIDP::interpret(const std::string& ns) {
	reset();
	try {
		if ((ns[0] == '"')&&(ns[ns.length()-1] == '"')) { // String
			str = ns.substr(1, ns.length()-2);
			type = 0;
		} else if (!std::isdigit(ns[0])) { // Probably a string
			str = ns;
			type = 0;
		} else if (std::regex_match(ns, std::regex("^-?\\d*\\.\\d+"))) { // Double
			floating = std::stod(ns);
			type = 2;
		} else if (std::regex_match(ns, std::regex("^-?\\d+"))) { // Integer
			integer = std::stoi(ns);
			type = 1;
		}
	} catch (const std::invalid_argument &e) {}

	if (type == -1) { // No possible type
		std::cerr << "WARN: SIDP type not determined, storing as string: \"" + ns + "\"\n";
		type = 0;
		str = ns;
	}

	return 0; // Return 0 on success
}
std::string BEE::SIDP::to_str() {
	switch (type) {
		case 0:
			return str;
		case 1:
			return std::to_string(integer);
		case 2:
			return std::to_string(floating);
		case 3:
			std::stringstream ss;
			ss << pointer;
			return ss.str();
	}
	return "";
}

// Return the requested type
std::string BEE::SIDP::s(std::string file, int line) {
	if (type != 0) {
		std::cerr << "WARN: SIDP type is " << type << ", not a string but the string was requested, called from " << file << ":" << line << "\n";
	}
	return str;
}
int BEE::SIDP::i(std::string file, int line) {
	if ((type != 1)&&(type != 2)) {
		std::cerr << "WARN: SIDP type is " << type << ", not an integer but the integer was requested, called from " << file << ":" << line << "\n";
	}
	if (type == 2) {
		return (int)floating;
	}
	return integer;
}
double BEE::SIDP::d(std::string file, int line) {
	if ((type != 1)&&(type != 2)) {
		std::cerr << "WARN: SIDP type is " << type << ", not a double but the double was requested, called from " << file << ":" << line << "\n";
	}
	if (type == 1) {
		return (double)integer;
	}
	return floating;
}
void* BEE::SIDP::p(std::string file, int line) {
	if (type != 3) {
		std::cerr << "WARN: SIDP type is " << type << ", not a pointer but the pointer was requested, called from " << file << ":" << line << "\n";
	}
	return pointer;
}
std::string BEE::SIDP::s() {
	if (type != 0) {
		std::cerr << "WARN: SIDP type not a string but the string was requested\n";
	}
	return str;
}
int BEE::SIDP::i() {
	if ((type != 1)&&(type != 2)) {
		std::cerr << "WARN: SIDP type not an integer but the integer was requested\n";
	}
	if (type == 2) {
		return (int)floating;
	}
	return integer;
}
double BEE::SIDP::d() {
	if ((type != 1)&&(type != 2)) {
		std::cerr << "WARN: SIDP type not a double but the double was requested\n";
	}
	if (type == 1) {
		return (double)integer;
	}
	return floating;
}
void* BEE::SIDP::p() {
	if (type != 3) {
		std::cerr << "WARN: SIDP type not a pointer but the pointer was requested\n";
	}
	return pointer;
}

BEE::SIDP& BEE::SIDP::operator=(const SIDP& rhs) {
	reset();
	this->type = rhs.type;

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
			this->pointer = rhs.pointer;
			break;
		}
		default: {
			throw std::runtime_error("Error: SIDP operator invalid type");
		}
	}

	return *this;
}
BEE::SIDP& BEE::SIDP::operator=(const std::string& rhs) {
	reset();
	type = 0;
	str = rhs;
	return *this;
}
BEE::SIDP& BEE::SIDP::operator=(int rhs) {
	reset();
	type = 1;
	integer = rhs;
	return *this;
}
BEE::SIDP& BEE::SIDP::operator=(double rhs) {
	reset();
	type = 2;
	floating = rhs;
	return *this;
}
BEE::SIDP& BEE::SIDP::operator=(void* rhs) {
	reset();
	type = 3;
	pointer = rhs;
	return *this;
}

BEE::SIDP& BEE::SIDP::operator+=(const SIDP& rhs) {
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
BEE::SIDP& BEE::SIDP::operator+=(int rhs) {
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
			char* tmp = (char*)this->pointer;
			tmp += rhs;
			this->pointer = (void*)tmp;
		}
		default: {
			throw std::runtime_error("Error: SIDP operator invalid type");
		}
	}

	return *this;
}

BEE::SIDP& BEE::SIDP::operator-=(const SIDP& rhs) {
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
			//this->pointer += rhs.pointer;
		}
		default: {
			throw std::runtime_error("Error: SIDP operator invalid type");
		}
	}

	return *this;
}
BEE::SIDP& BEE::SIDP::operator-=(int rhs) {
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
			char* tmp = (char*)this->pointer;
			tmp -= rhs;
			this->pointer = (void*)tmp;
		}
		default: {
			throw std::runtime_error("Error: SIDP operator invalid type");
		}
	}

	return *this;
}

#endif // _BEE_GAME_SIDP
