/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_ROOM
#define BEE_PYTHON_ROOM 1

#include <Python.h>
#include <structmember.h>

#include "room.hpp"

#include "../python.hpp"

#include "../../resource/room.hpp"

#include "../instance.hpp"

#include "../physics/world.hpp"
#include "../physics/body.hpp"

#include "path.hpp"
#include "timeline.hpp"
#include "object.hpp"
#include "../structs.hpp"

#include "../../render/background.hpp"
#include "../../render/viewport.hpp"

#include "../../resource/path.hpp"
#include "../../resource/timeline.hpp"

namespace bee { namespace python {
	PyObject* Room_from(const Room* room) {
		if (room == nullptr) {
			return nullptr;
		}

		PyObject* py_room = internal::Room_new(&internal::RoomType, nullptr, nullptr);
		internal::RoomObject* _py_room = reinterpret_cast<internal::RoomObject*>(py_room);

		if (Room_init(_py_room, Py_BuildValue("(N)", PyUnicode_FromString(room->get_name().c_str())), nullptr)) {
			return nullptr;
		}

		return py_room;
	}
	bool Room_check(PyObject* room) {
		return PyObject_TypeCheck(room, &internal::RoomType);
	}
namespace internal {
	PyMethodDef RoomMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Room_print), METH_NOARGS, "Print all relevant information about the Room"},

		{"get_width", reinterpret_cast<PyCFunction>(Room_get_width), METH_NOARGS, "Return the Room width"},
		{"get_height", reinterpret_cast<PyCFunction>(Room_get_height), METH_NOARGS, "Return the Room height"},
		{"get_is_persistent", reinterpret_cast<PyCFunction>(Room_get_is_persistent), METH_NOARGS, "Return whether the Room's Instances persist to the next Room"},
		{"get_backgrounds", reinterpret_cast<PyCFunction>(Room_get_backgrounds), METH_NOARGS, "Return the Room's named Backgrounds"},
		{"get_viewports", reinterpret_cast<PyCFunction>(Room_get_viewports), METH_NOARGS, "Return the Room's named ViewPorts"},
		{"get_instances", reinterpret_cast<PyCFunction>(Room_get_instances), METH_NOARGS, "Return the Room's Instances"},
		{"get_current_viewport", reinterpret_cast<PyCFunction>(Room_get_current_viewport), METH_NOARGS, "Return the ViewPort that is currently being drawn"},
		{"get_phys_world", reinterpret_cast<PyCFunction>(Room_get_phys_world), METH_NOARGS, "Return the Room's PhysicsWorld"},
		{"get_paths", reinterpret_cast<PyCFunction>(Room_get_paths), METH_NOARGS, "Return the Paths which are being automatically updated"},
		{"get_timelines", reinterpret_cast<PyCFunction>(Room_get_timelines), METH_NOARGS, "Return the Timelines which are being automatically stepped"},

		{"set_width", reinterpret_cast<PyCFunction>(Room_set_width), METH_VARARGS, "Set the Room width"},
		{"set_height", reinterpret_cast<PyCFunction>(Room_set_height), METH_VARARGS, "Set the Room height"},
		{"set_is_persistent", reinterpret_cast<PyCFunction>(Room_set_is_persistent), METH_VARARGS, "Set whether the Room's Instances persist to the next Room"},

		{"add_background", reinterpret_cast<PyCFunction>(Room_add_background), METH_VARARGS, "Add the Background to the Room using the given name"},
		{"remove_background", reinterpret_cast<PyCFunction>(Room_remove_background), METH_VARARGS, "Remove the Background with the given name from the Room"},
		{"add_viewport", reinterpret_cast<PyCFunction>(Room_add_viewport), METH_VARARGS, "Add the ViewPort to the Room using the given name"},
		{"remove_viewport", reinterpret_cast<PyCFunction>(Room_remove_viewport), METH_VARARGS, "Remove the ViewPort with the given name from the Room"},
		{"add_instance", reinterpret_cast<PyCFunction>(Room_add_instance), METH_VARARGS, "Add an Instance of the given Object at the given position"},
		{"destroy", reinterpret_cast<PyCFunction>(Room_destroy), METH_VARARGS, "Destroy the given Instance at the end of the frame"},
		{"destroy_all", reinterpret_cast<PyCFunction>(Room_destroy_all), METH_VARARGS, "Destroy all Instances of a given Object"},
		{"automate_path", reinterpret_cast<PyCFunction>(Room_automate_path), METH_VARARGS, "Automatically advance the given Instance with the given PathFollower"},
		{"automate_timeline", reinterpret_cast<PyCFunction>(Room_automate_timeline), METH_VARARGS, "Automatically advance the given TimelineIterator"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef RoomMembers[] = {
		{"name", T_OBJECT_EX, offsetof(RoomObject, name), 0, "The Room name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject RoomType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Room",
		sizeof(RoomObject), 0,
		reinterpret_cast<destructor>(Room_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Room_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Room_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Room objects",
		0,
		0,
		0,
		0,
		0, 0,
		RoomMethods,
		RoomMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Room_init),
		0, Room_new,
		0, 0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};

	PyObject* PyInit_bee_room(PyObject* module) {
		RoomType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&RoomType) < 0) {
			return nullptr;
		}

		Py_INCREF(&RoomType);
		PyModule_AddObject(module, "Room", reinterpret_cast<PyObject*>(&RoomType));

		return reinterpret_cast<PyObject*>(&RoomType);
	}

	Room* as_room(RoomObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Room::get_by_name(_name);
	}
	Room* as_room(PyObject* self) {
		if (Room_check(self)) {
			return as_room(reinterpret_cast<RoomObject*>(self));
		}
		return nullptr;
	}

	void Room_dealloc(RoomObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Room_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		RoomObject* self;

		self = reinterpret_cast<RoomObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Room_init(RoomObject* self, PyObject* args, PyObject* kwds) {
		PyObject* name = nullptr;

		const char* kwlist[] = {"name", nullptr};
		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", const_cast<char**>(kwlist), &name)) {
			return -1;
		}

		if (name != nullptr) {
			PyObject* tmp = self->name;
			Py_INCREF(name);
			self->name = name;
			Py_XDECREF(tmp);
		}

		if (self->name == nullptr) {
            self->name = PyUnicode_FromString("");
		}

		return 0;
	}

	PyObject* Room_repr(RoomObject* self) {
		std::string s = std::string("bee.Room(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Room_str(RoomObject* self) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return PyUnicode_FromString("Invalid Room name");
		}

		Variant m (room->serialize());
		std::string s = "Room " + m.to_str(true);

		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Room_print(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->print();

		Py_RETURN_NONE;
	}

	PyObject* Room_get_width(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", room->get_width());
	}
	PyObject* Room_get_height(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", room->get_height());
	}
	PyObject* Room_get_is_persistent(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(room->get_is_persistent());
	}
	PyObject* Room_get_backgrounds(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		const std::map<std::string,Background>& backgrounds = room->get_backgrounds();
		PyObject* bgs = PyDict_New();
		for (auto& bg : backgrounds) {
			PyDict_SetItemString(bgs, bg.first.c_str(), Background_from(bg.second));
		}

		return bgs;
	}
	PyObject* Room_get_viewports(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		const std::map<std::string,ViewPort>& viewports = room->get_viewports();
		PyObject* vps = PyDict_New();
		for (auto& vp : viewports) {
			PyDict_SetItemString(vps, vp.first.c_str(), ViewPort_from(vp.second));
		}

		return vps;
	}
	PyObject* Room_get_current_viewport(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		std::pair<const std::string,ViewPort>* vp = room->get_current_viewport();
		if (vp == nullptr) {
			Py_RETURN_NONE;
		}

		return Py_BuildValue("(NN)", PyUnicode_FromString(vp->first.c_str()), ViewPort_from(vp->second));
	}
	PyObject* Room_get_instances(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		const std::map<size_t,Instance*>& instances = room->get_instances();
		PyObject* insts = PyDict_New();
		for (auto& inst : instances) {
			PyDict_SetItem(insts, Py_BuildValue("I", inst.first), Instance_from(inst.second));
		}

		return insts;
	}
	PyObject* Room_get_phys_world(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		return PhysicsWorld_from(room->get_phys_world());
	}
	PyObject* Room_get_paths(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		const std::map<Instance*,PathFollower>& paths = room->get_paths();
		PyObject* _paths = PyDict_New();
		for (auto& p : paths) {
			PyDict_SetItem(_paths, Instance_from(p.first), PathFollower_from(p.second));
		}

		return _paths;
	}
	PyObject* Room_get_timelines(RoomObject* self, PyObject* args) {
		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		const std::vector<TimelineIterator>& timelines = room->get_timelines();
		PyObject* tls = PyList_New(timelines.size());
		Py_ssize_t i = 0;
		for (auto& tlit : timelines) {
			PyList_SetItem(tls, i++, TimelineIterator_from(tlit));
		}

		return tls;
	}

	PyObject* Room_set_width(RoomObject* self, PyObject* args) {
		int width;

		if (!PyArg_ParseTuple(args, "i", &width)) {
			return nullptr;
		}

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->set_width(width);

		Py_RETURN_NONE;
	}
	PyObject* Room_set_height(RoomObject* self, PyObject* args) {
		int height;

		if (!PyArg_ParseTuple(args, "i", &height)) {
			return nullptr;
		}

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->set_height(height);

		Py_RETURN_NONE;
	}
	PyObject* Room_set_is_persistent(RoomObject* self, PyObject* args) {
		int is_persistent;

		if (!PyArg_ParseTuple(args, "p", &is_persistent)) {
			return nullptr;
		}

		bool _is_persistent = is_persistent;

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->set_is_persistent(_is_persistent);

		Py_RETURN_NONE;
	}

	PyObject* Room_add_background(RoomObject* self, PyObject* args) {
		PyObject* name;
		PyDictObject* bg;

		if (!PyArg_ParseTuple(args, "UO!", &name, &PyDict_Type, &bg)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		Background _bg;
		if (as_background(bg, &_bg)) {
			PyErr_SetString(PyExc_ValueError, "the provided Background dict is not valid");
			return nullptr;
		}

		return Py_BuildValue("i", room->add_background(_name, _bg));
	}
	PyObject* Room_remove_background(RoomObject* self, PyObject* args) {
		PyObject* name;

		if (!PyArg_ParseTuple(args, "U", &name)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->remove_background(_name);

		Py_RETURN_NONE;
	}
	PyObject* Room_add_viewport(RoomObject* self, PyObject* args) {
		PyObject* name;
		PyDictObject* vp;

		if (!PyArg_ParseTuple(args, "UO!", &name, &PyDict_Type, &vp)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		ViewPort _vp;
		if (as_viewport(vp, &_vp)) {
			PyErr_SetString(PyExc_ValueError, "the provided ViewPort dict is not valid");
			return nullptr;
		}

		return Py_BuildValue("i", room->add_viewport(_name, _vp));
	}
	PyObject* Room_remove_viewport(RoomObject* self, PyObject* args) {
		PyObject* name;

		if (!PyArg_ParseTuple(args, "U", &name)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->remove_viewport(_name);

		Py_RETURN_NONE;
	}
	PyObject* Room_add_instance(RoomObject* self, PyObject* args) {
		ObjectObject* obj;
		double x, y, z;

		if (!PyArg_ParseTuple(args, "O!(ddd)", &ObjectType, &obj, &x, &y, &z)) {
			return nullptr;
		}

		btScalar _x (x), _y (y), _z (z);

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		return Instance_from(room->add_instance(as_object(obj), {_x, _y, _z}));
	}
	PyObject* Room_destroy(RoomObject* self, PyObject* args) {
		InstanceObject* inst;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &inst)) {
			return nullptr;
		}

		Instance* _inst = as_instance(inst);
		if (_inst == nullptr) {
			return nullptr;
		}

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->destroy(_inst);

		Py_RETURN_NONE;
	}
	PyObject* Room_destroy_all(RoomObject* self, PyObject* args) {
		ObjectObject* obj;

		if (!PyArg_ParseTuple(args, "O!", &ObjectType, &obj)) {
			return nullptr;
		}

		Object* _obj = as_object(obj);
		if (_obj == nullptr) {
			return nullptr;
		}

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		room->destroy_all(_obj);

		Py_RETURN_NONE;
	}
	PyObject* Room_automate_path(RoomObject* self, PyObject* args) {
		PyObject* inst;
		PyObject* pf;

		if (!PyArg_ParseTuple(args, "O!O", &InstanceType, &inst, &pf)) {
			return nullptr;
		}

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		Instance* _inst = as_instance(inst);
		if (inst == nullptr) {
			return nullptr;
		}

		if (pf == Py_None) {
			room->automate_path(_inst, PathFollower());
		} else {
			PathFollower _pf;
			if (as_path_follower(reinterpret_cast<PyDictObject*>(pf), &_pf)) {
				PyErr_SetString(PyExc_ValueError, "the provided PathFollower dict is not valid");
				return nullptr;
			}

			room->automate_path(_inst, _pf);
		}

		Py_RETURN_NONE;
	}
	PyObject* Room_automate_timeline(RoomObject* self, PyObject* args) {
		PyDictObject* tlit;

		if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &tlit)) {
			return nullptr;
		}

		Room* room = as_room(self);
		if (room == nullptr) {
			return nullptr;
		}

		TimelineIterator _tlit;
		if (as_timeline_iterator(tlit, &_tlit)) {
			PyErr_SetString(PyExc_ValueError, "the provided TimelineIterator dict is not valid");
			return nullptr;
		}

		room->automate_timeline(_tlit);

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_ROOM
