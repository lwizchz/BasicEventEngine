/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_NET
#define BEE_PYTHON_NET 1

#include "net.hpp"

#include "../network/network.hpp"

#include "instance.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEENetMethods[] = {
		{"init", net_init, METH_NOARGS, "Initialize the networking utilities"},
		{"get_is_initialized", net_get_is_initialized, METH_NOARGS, "Return whether networking has been initialized or not"},
		{"get_time", net_get_time, METH_NOARGS, "Return the network time"},
		{"close", net_close, METH_NOARGS, "Close and clean up the networking utilities"},

		{"handle_events", net_handle_events, METH_NOARGS, "Handle network sending and receiving during the event loop"},

		{"session_start", net_session_start, METH_VARARGS, "Begin hosting a network session for others to join over UDP"},
		{"session_find", net_session_find, METH_NOARGS, "Query the local network for available sessions"},
		{"session_join", net_session_join, METH_VARARGS, "Attempt to join a session at the given IP address"},
		{"get_is_connected", net_get_is_connected, METH_NOARGS, "Return whether the network is currently connected to a session"},
		{"session_end", net_session_end, METH_NOARGS, "End the session connection"},

		{"session_sync_data", net_session_sync_data, METH_VARARGS, "Sync the given data to the session"},
		{"session_sync_instance", net_session_sync_instance, METH_VARARGS, "Sync the given Instance to the session"},
		{"session_sync_player", net_session_sync_player, METH_VARARGS, "Sync the given player Instance"},

		{"get_print", net_get_print, METH_NOARGS, "Return an info string about the network session"},
		{"get_players", net_get_players, METH_NOARGS, "Return the map of connected players"},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEENetModule = {
		PyModuleDef_HEAD_INIT, "net", nullptr, -1, BEENetMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_net() {
		return PyModule_Create(&BEENetModule);
	}

	PyObject* net_init(PyObject* self, PyObject* args) {
		return PyLong_FromLong(net::init());
	}
	PyObject* net_get_is_initialized(PyObject* self, PyObject* args) {
		return PyBool_FromLong(net::get_is_initialized());
	}
	PyObject* net_get_time(PyObject* self, PyObject* args) {
		return Py_BuildValue("I", net::get_time());
	}
	PyObject* net_close(PyObject* self, PyObject* args) {
		return PyLong_FromLong(net::close());
	}

	PyObject* net_handle_events(PyObject* self, PyObject* args) {
		return PyLong_FromLong(net::handle_events());
	}

	PyObject* net_session_start(PyObject* self, PyObject* args) {
		PyObject* session_name;
		int max_players;
		PyObject* player_name;

		if (!PyArg_ParseTuple(args, "UiU", &session_name, &max_players, &player_name)) {
			return nullptr;
		}

		std::string _session_name (PyUnicode_AsUTF8(session_name));
		std::string _player_name (PyUnicode_AsUTF8(player_name));

		return PyLong_FromLong(net::session_start(_session_name, max_players, _player_name));
	}
	PyObject* net_session_find(PyObject* self, PyObject* args) {
		const std::map<std::string,std::string>& sessions = net::session_find();
		PyObject* _sessions = PyDict_New();

		for (auto& s : sessions) {
			PyDict_SetItemString(_sessions, s.first.c_str(), PyUnicode_FromString(s.second.c_str()));
		}

		return _sessions;
	}
	PyObject* net_session_join(PyObject* self, PyObject* args) {
		PyObject* ip;
		PyObject* player_name;

		if (!PyArg_ParseTuple(args, "UU", &ip, &player_name)) {
			return nullptr;
		}

		std::string _ip (PyUnicode_AsUTF8(ip));
		std::string _player_name (PyUnicode_AsUTF8(player_name));

		return PyLong_FromLong(net::session_join(_ip, _player_name));
	}
	PyObject* net_get_is_connected(PyObject* self, PyObject* args) {
		return PyBool_FromLong(net::get_is_connected());
	}
	PyObject* net_session_end(PyObject* self, PyObject* args) {
		return PyLong_FromLong(net::session_end());
	}

	PyObject* net_session_sync_data(PyObject* self, PyObject* args) {
		PyObject* key;
		PyObject* value;

		if (!PyArg_ParseTuple(args, "UU", &key, &value)) {
			return nullptr;
		}

		std::string _key (PyUnicode_AsUTF8(key));
		std::string _value (PyUnicode_AsUTF8(value));

		return PyLong_FromLong(net::session_sync_data(_key, _value));
	}
	PyObject* net_session_sync_instance(PyObject* self, PyObject* args) {
		InstanceObject* inst;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &inst)) {
			return nullptr;
		}

		Instance* _inst = as_instance(inst);
		if (_inst == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(net::session_sync_instance(_inst));
	}
	PyObject* net_session_sync_player(PyObject* self, PyObject* args) {
		int id;
		InstanceObject* inst;

		if (!PyArg_ParseTuple(args, "iO!", &id, &InstanceType, &inst)) {
			return nullptr;
		}

		Instance* _inst = as_instance(inst);
		if (_inst == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(net::session_sync_player(id, _inst));
	}

	PyObject* net_get_print(PyObject* self, PyObject* args) {
		return PyUnicode_FromString(net::get_print().c_str());
	}
	PyObject* net_get_players(PyObject* self, PyObject* args) {
		const std::map<int,NetworkClient>& players = net::get_players();
		PyObject* _players = PyDict_New();

		for (auto& p : players) {
			PyObject* player = PyDict_New();
			PyDict_SetItemString(player, "channel", PyLong_FromLong(p.second.channel));
			PyDict_SetItemString(player, "last_recv", Py_BuildValue("I", p.second.last_recv));
			PyDict_SetItemString(player, "id", PyLong_FromLong(p.second.id));
			PyDict_SetItemString(player, "name", PyUnicode_FromString(p.second.name.c_str()));

			PyDict_SetItem(_players, PyLong_FromLong(p.first), player);
		}

		return _players;
	}
}}}

#endif // BEE_PYTHON_NET
