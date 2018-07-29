/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DATA_STATEMACHINE
#define BEE_DATA_STATEMACHINE 1

#include <algorithm>

#include "statemachine.hpp" // Include the engine headers

#include "../engine.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	State::State() :
		State("", {})
	{}
	State::State(const std::string& _name, const std::set<std::string>& _outputs) :
		name(_name),
		outputs(_outputs),

		start_func(nullptr),
		update_func(nullptr),
		end_func(nullptr)
	{}

	void State::start() {
		if (start_func != nullptr) {
			start_func();
		}
	}
	void State::update(Uint32 ticks) {
		if (update_func != nullptr) {
			update_func(ticks);
		}
	}
	void State::end() {
		if (end_func != nullptr) {
			end_func();
		}
	}

	StateMachine::StateMachine() :
		stack(),
		graph()
	{
		init();
	}
	int StateMachine::init() {
		State none ("None", {"*"});
		add_state(none);
		stack.push_front(none.name);
		return 0;
	}
	int StateMachine::print_state() {
		messenger::send({"engine", "statemachine"}, E_MESSAGE::INFO, get_states());
		return 0;
	}
	int StateMachine::print_graph() {
		std::string graph_str;
		for (auto& s : graph) {
			graph_str += s.first + ": ";
			for (auto& o : s.second.outputs) {
				graph_str += o + ", ";
			}
			graph_str.pop_back();
			graph_str.pop_back();

			graph_str += "\n";
		}
		graph_str.pop_back();

		messenger::send({"engine", "statemachine"}, E_MESSAGE::INFO, graph_str);

		return 0;
	}

	int StateMachine::add_state(const State& state) {
		if (graph.find(state.name) != graph.end()) {
			return 1;
		}

		graph.emplace(state.name, state);

		return 0;
	}
	int StateMachine::remove_state(const std::string& state_name) {
		std::map<std::string,State>::iterator state (graph.find(state_name));
		if (state == graph.end()) {
			return 1;
		}

		graph.erase(state);

		return 0;
	}
	int StateMachine::clear() {
		stack.clear();
		graph.clear();
		init();
		return 0;
	}

	std::string StateMachine::get_state() const {
		return stack.front();
	}
	std::string StateMachine::get_state(int n) const {
		return (*std::next(stack.begin(), n));
	}
	std::string StateMachine::get_states() const {
		std::string states;
		for (auto& s : stack) {
			states += s + " ";
		}
		states.pop_back();

		return states;
	}
	bool StateMachine::has_state(const std::string& state_name) const {
		for (auto& s : stack) {
			if (s == state_name) {
				return true;
			}
		}
		return false;
	}

	int StateMachine::push_state(const std::string& state_name) {
		const std::set<std::string>& outputs = graph.at(get_state()).outputs;
		if (
			(outputs.find(state_name) == outputs.end())
			&&(outputs.find("*") == outputs.end())
		) {
			return 1;
		}

		if (get_state() == state_name) {
			return 2;
		}

		stack.push_front(state_name);
		graph.at(get_state()).start();

		return 0;
	}
	int StateMachine::pop_state() {
		const std::set<std::string>& outputs = graph.at(get_state()).outputs;
		if (outputs.find(get_state(1)) == outputs.end()) {
			return 1;
		}

		graph.at(get_state()).end();
		stack.pop_front();

		return 0;
	}
	int StateMachine::pop_state(const std::string& state_name) {
		if (get_state() != state_name) {
			return 2;
		}

		return pop_state();
	}
	int StateMachine::pop_state_all(const std::string& state_name) {
		graph.at(state_name).end();
		stack.erase(std::remove_if(stack.begin(), stack.end(), [&state_name] (const std::string& n) -> bool {
			return (n == state_name);
		}), stack.end());

		return 0;
	}
	int StateMachine::pop_all() {
		stack.clear();
		stack.push_front("None");
		return 0;
	}

	int StateMachine::update() {
		graph.at(get_state()).update(get_ticks());
		return 0;
	}
	int StateMachine::update_all() {
		Uint32 t = get_ticks();
		for (size_t i=0; i<stack.size(); ++i) { // Use a normal for loop to allow state changes during iteration
			graph.at(stack.at(i)).update(t);
		}

		return 0;
	}
}

#endif // BEE_DATA_STATEMACHINE
