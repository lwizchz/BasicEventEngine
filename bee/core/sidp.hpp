/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_SIDP_H
#define BEE_CORE_SIDP_H 1

#include <map>
#include <vector>

namespace bee {
	class SIDP { // This class can hold a string, integer, double, or pointer and is meant to allow multiple types in the same container
		int type; // Possible types: 0=string, 1=int, 2=double, 3=pointer
		int container_type;

		std::string str;
		int integer;
		double floating;
		void* pointer;

	public:
		SIDP();
		SIDP(const SIDP&);
		SIDP(const std::string&, bool);
		SIDP(const std::string&);
		SIDP(const char*);
		SIDP(int);
		SIDP(double);
		SIDP(void*);
		SIDP(std::vector<SIDP>*);
		SIDP(std::map<SIDP,SIDP>*);
		~SIDP();
		int reset();

		int interpret(const std::string&);
		int vector(std::vector<SIDP>*);
		int map(std::map<SIDP,SIDP>*);
		std::string to_str() const;

		// Return the requested type
		std::string s(std::string, int) const;
		int i(std::string, int) const;
		double d(std::string, int) const;
		void* p(std::string, int) const;
		SIDP p(size_t, std::string, int) const;
		SIDP p(SIDP, std::string, int) const;
		std::vector<SIDP>& v(std::string, int) const;
		std::map<SIDP,SIDP>& m(std::string, int) const;

		SIDP& operator=(const SIDP&);
		SIDP& operator=(const std::string&);
		SIDP& operator=(const char*);
		SIDP& operator=(int);
		SIDP& operator=(double);
		SIDP& operator=(void*);
		SIDP& operator=(const std::vector<SIDP>&);
		SIDP& operator=(const std::map<SIDP,SIDP>&);

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

// Shorthand for easier SIDP debugging
#define SIDP_s(x) x.s(__FILE__, __LINE__)
#define SIDP_i(x) x.i(__FILE__, __LINE__)
#define SIDP_d(x) x.d(__FILE__, __LINE__)
#define SIDP_p(x) x.p(__FILE__, __LINE__)
#define SIDP_c(x,i) x.p(i, __FILE__, __LINE__)
#define SIDP_cs(x,i) x.p(i, __FILE__, __LINE__).s(__FILE__, __LINE__)
#define SIDP_ci(x,i) x.p(i, __FILE__, __LINE__).i(__FILE__, __LINE__)
#define SIDP_cd(x,i) x.p(i, __FILE__, __LINE__).d(__FILE__, __LINE__)
#define SIDP_cp(x,i) x.p(i, __FILE__, __LINE__).p(__FILE__, __LINE__)
#define SIDP_v(x) x.v(__FILE__, __LINE__)
#define SIDP_m(x) x.m(__FILE__, __LINE__)

// Shorthand for easier Instance data debugging
#define _a(x) (*s)[x]
#define _s(x) (*s)[x].s(__FILE__, __LINE__)
#define _i(x) (*s)[x].i(__FILE__, __LINE__)
#define _d(x) (*s)[x].d(__FILE__, __LINE__)
#define _p(x) (*s)[x].p(__FILE__, __LINE__)
#define _c(x,i) (*s)[x].p(i, __FILE__, __LINE__)
#define _cs(x,i) (*s)[x].p(i, __FILE__, __LINE__).s(__FILE__, __LINE__)
#define _ci(x,i) (*s)[x].p(i, __FILE__, __LINE__).i(__FILE__, __LINE__)
#define _cd(x,i) (*s)[x].p(i, __FILE__, __LINE__).d(__FILE__, __LINE__)
#define _cp(x,i) (*s)[x].p(i, __FILE__, __LINE__).p(__FILE__, __LINE__)

#endif // BEE_CORE_SIDP_H
