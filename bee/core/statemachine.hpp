/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_STATEMACHINE_H
#define BEE_CORE_STATEMACHINE_H 1

#include <functional>
#include <deque>
#include <set>
#include <map>
#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers for Uint32

namespace bee {
	struct State {
		std::string name;
		std::set<std::string> outputs;

		std::function<void ()> start_func;
		std::function<void (Uint32)> update_func;
		std::function<void ()> end_func;

		State();
		State(const std::string&, const std::set<std::string>&);

		void start();
		void update(Uint32);
		void end();
	};

	class StateMachine {
			std::deque<std::string> stack;
			std::map<std::string,State> graph;
		public:
			StateMachine();
			int init();
			int print_state();
			int print_graph();

			int add_state(const State&);
			int remove_state(const std::string&);
			int clear();

			std::string get_state() const;
			std::string get_state(int) const;
			std::string get_states() const;
			bool has_state(const std::string&) const;

			int push_state(const std::string&);
			int pop_state();
			int pop_state(const std::string&);
			int pop_state_all(const std::string&);
			int pop_all();

			int update();
			int update_all();
	};
}

#endif // BEE_CORE_STATEMACHINE_H
