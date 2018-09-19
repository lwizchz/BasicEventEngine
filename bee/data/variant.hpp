/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DATA_VARIANT_H
#define BEE_DATA_VARIANT_H 1

#include <map>
#include <vector>
#include <typeinfo>
#include <typeindex>

#include "../enum.hpp"

#include "serialdata.hpp"

namespace bee {
	class Variant { // This class can hold various types and is meant to allow multiple types in the same container
		E_DATA_TYPE type;
		std::type_index ptype;

	public:
		union {
			unsigned char c;
			long i;
			double f;
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
		//Variant(std::initializer_list<Variant>);

		explicit Variant(unsigned char);
		explicit Variant(bool);
		explicit Variant(int);
		explicit Variant(long);
		explicit Variant(float);
		explicit Variant(double);
		explicit Variant(const std::string&);
		explicit Variant(const std::vector<Variant>&);
		explicit Variant(const std::map<Variant,Variant>&);
		explicit Variant(const SerialData&);
		template <typename T>
		explicit Variant(T*);

		~Variant();
		int reset();

		E_DATA_TYPE get_type() const;
		std::type_index get_ptype() const;

		int interpret(const std::string&);
		std::string to_str(bool) const;
		std::string to_str() const;

		Variant& operator=(const Variant&);
		Variant& operator=(unsigned char);
		Variant& operator=(bool);
		Variant& operator=(int);
		Variant& operator=(long);
		Variant& operator=(float);
		Variant& operator=(double);
		Variant& operator=(const std::string&);
		Variant& operator=(const char*);
		Variant& operator=(const std::vector<Variant>&);
		Variant& operator=(const std::map<Variant,Variant>&);
		Variant& operator=(const SerialData&);
		template <typename T>
		Variant& operator=(T*);

		bool operator==(const Variant&) const;
		bool operator!=(const Variant&) const;

		friend bool operator<(const Variant&, const Variant&);

		friend std::ostream& operator<<(std::ostream&, const Variant&);
		friend std::istream& operator>>(std::istream&, Variant&);
	};
	bool operator<(const Variant&, const Variant&);

	std::ostream& operator<<(std::ostream&, const Variant&);
	std::istream& operator>>(std::istream&, Variant&);

	template <typename T>
	Variant::Variant(T* _p) :
		type(E_DATA_TYPE::NONE),
		ptype(std::type_index(typeid(_p))),
		p(_p)
	{}

	template <typename T>
	Variant& Variant::operator=(T* _p) {
		*this = Variant(_p);
		return *this;
	}
}

// Shorthand for easier use in Instances
#define _a(x) (*s)[x]
#define _c(x) (*s)[x].c
#define _i(x) (*s)[x].i
#define _f(x) (*s)[x].f
#define _s(x) (*s)[x].s
#define _v(x) (*s)[x].v
#define _m(x) (*s)[x].m
#define _sd(x) (*s)[x].sd
#define _p(x) (*s)[x].p

#endif // BEE_DATA_VARIANT_H
