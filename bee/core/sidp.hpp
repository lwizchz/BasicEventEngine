/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_SIDP_H
#define _BEE_CORE_SIDP_H 1

#include "../engine.hpp" // Include the engine headers

namespace bee {
	struct SIDP { // This class can hold a string, integer, double, or pointer and is meant to allow multiple types in the same container
		int type; // Possible types: 0=string, 1=int, 2=double, 3=pointer
		int container_type;

		std::string str;
		int integer;
		double floating;
		void* pointer;

		// Initialize the variable
		SIDP();
		SIDP(const SIDP&);
		SIDP(const std::string&);
		SIDP(const std::string&, bool);
		SIDP(int);
		SIDP(double);
		SIDP(void*);
		~SIDP();
		int reset();

		int interpret(const std::string&);
		int vector(std::vector<SIDP>*);
		int map(std::map<SIDP,SIDP>*);
		std::string to_str();

		// Return the requested type
		std::string s(std::string, int);
		int i(std::string, int);
		double d(std::string, int);
		void* p(std::string, int);
		SIDP p(size_t, std::string, int);
		SIDP p(SIDP, std::string, int);
		std::string s();
		int i();
		double d();
		void* p();
		SIDP p(size_t);
		SIDP p(SIDP);

		SIDP& operator=(const SIDP&);
		SIDP& operator=(const std::string&);
		SIDP& operator=(int);
		SIDP& operator=(double);
		SIDP& operator=(void*);

		SIDP& operator+=(const SIDP&);
		SIDP& operator+=(int);

		SIDP& operator-=(const SIDP&);
		SIDP& operator-=(int);

		friend bool operator<(const SIDP&, const SIDP&);

		friend std::ostream& operator<<(std::ostream&, const SIDP&);
		friend std::istream& operator>>(std::istream&, SIDP&);
	};
	bool operator<(const SIDP&, const SIDP&);

	std::ostream& operator<<(std::ostream&, const SIDP&);
	std::istream& operator>>(std::istream&, SIDP&);
}

#define SIDP_s(x) x.s(__FILE__, __LINE__)
#define SIDP_i(x) x.i(__FILE__, __LINE__)
#define SIDP_d(x) x.d(__FILE__, __LINE__)
#define SIDP_p(x) x.p(__FILE__, __LINE__)
#define SIDP_c(x,i) x.p(i, __FILE__, __LINE__)
#define SIDP_cs(x,i) x.p(i, __FILE__, __LINE__).s(__FILE__, __LINE__)
#define SIDP_ci(x,i) x.p(i, __FILE__, __LINE__).i(__FILE__, __LINE__)
#define SIDP_cd(x,i) x.p(i, __FILE__, __LINE__).d(__FILE__, __LINE__)
#define SIDP_cp(x,i) x.p(i, __FILE__, __LINE__).p(__FILE__, __LINE__)

#endif // _BEE_CORE_SIDP_H
