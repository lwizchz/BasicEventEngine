/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DATA_VARIANT_H
#define BEE_DATA_VARIANT_H 1

#include <map>
#include <vector>

#include "../enum.hpp"

#include "serialdata.hpp"

namespace bee {
	class Variant { // This class can hold various types and is meant to allow multiple types in the same container
		E_DATA_TYPE type;

	public:
		union {
			unsigned char c;
			int i;
			float f;
			double d;
			std::string s;
			std::vector<Variant> v;
			std::map<Variant,Variant> m;
			SerialData sd;
			void* p;
		};

		Variant();
		Variant(E_DATA_TYPE);
		Variant(const Variant&);
		Variant(const std::string&, bool);
		Variant(const char*);

		explicit Variant(unsigned char);
		explicit Variant(int);
		explicit Variant(float);
		explicit Variant(double);
		explicit Variant(const std::string&);
		explicit Variant(const std::vector<Variant>&);
		explicit Variant(const std::map<Variant,Variant>&);
		explicit Variant(const SerialData&);
		explicit Variant(void*);

		~Variant();
		int reset();

		E_DATA_TYPE get_type() const;

		int interpret(const std::string&);
		std::string to_str() const;

		Variant& operator=(const Variant&);
		Variant& operator=(unsigned char);
		Variant& operator=(int);
		Variant& operator=(float);
		Variant& operator=(double);
		Variant& operator=(const std::string&);
		Variant& operator=(const char*);
		Variant& operator=(const std::vector<Variant>&);
		Variant& operator=(const std::map<Variant,Variant>&);
		Variant& operator=(const SerialData&);
		Variant& operator=(void*);

		bool operator==(const Variant&) const;

		friend bool operator<(const Variant&, const Variant&);

		friend std::ostream& operator<<(std::ostream&, const Variant&);
		friend std::istream& operator>>(std::istream&, Variant&);
	};
	bool operator<(const Variant&, const Variant&);

	std::ostream& operator<<(std::ostream&, const Variant&);
	std::istream& operator>>(std::istream&, Variant&);
}

// Shorthand for easier use in Instances
#define _a(x) (*s)[x]
#define _c(x) (*s)[x].c
#define _i(x) (*s)[x].i
#define _f(x) (*s)[x].f
#define _d(x) (*s)[x].d
#define _s(x) (*s)[x].s
#define _v(x) (*s)[x].v
#define _m(x) (*s)[x].m
#define _sd(x) (*s)[x].sd
#define _p(x) (*s)[x].p

#endif // BEE_DATA_VARIANT_H
