/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_TIMELINE
#define BEE_PYTHON_TIMELINE 1

#include <Python.h>
#include <structmember.h>

#include "timeline.hpp"

#include "../python.hpp"

#include "../../resource/timeline.hpp"

namespace bee { namespace python {
	PyObject* Timeline_from(Timeline* timeline) {
		PyObject* py_timeline = internal::Timeline_new(&internal::TimelineType, nullptr, nullptr);
		internal::TimelineObject* _py_timeline = reinterpret_cast<internal::TimelineObject*>(py_timeline);
		_py_timeline->name = PyUnicode_FromString(timeline->get_name().c_str());
		return py_timeline;
	}
	bool Timeline_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::TimelineType);
	}
namespace internal {
	std::map<Timeline*,std::map<int,TimelineIterator*>> tlits;

	PyMethodDef TimelineMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Timeline_print), METH_NOARGS, "Print all relevant information about the Timeline"},

		{"get_actions", reinterpret_cast<PyCFunction>(Timeline_get_actions), METH_NOARGS, "Return the list of action frames and names"},

		{"add_action", reinterpret_cast<PyCFunction>(Timeline_add_action), METH_VARARGS, "Add the given callback to the action list"},
		{"remove_actions", reinterpret_cast<PyCFunction>(Timeline_remove_actions), METH_VARARGS, "Remove all actions from the given frames"},
		{"set_ending", reinterpret_cast<PyCFunction>(Timeline_set_ending), METH_VARARGS, "Set the end action"},

		{"step_to", reinterpret_cast<PyCFunction>(Timeline_step_to), METH_VARARGS, "Execute all actions up to the given frame"},
		{"end", reinterpret_cast<PyCFunction>(Timeline_end), METH_VARARGS, "End execution whether the iterator's finished or not"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef TimelineMembers[] = {
		{"name", T_OBJECT_EX, offsetof(TimelineObject, name), 0, "The Timeline name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject TimelineType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Timeline",
		sizeof(TimelineObject), 0,
		reinterpret_cast<destructor>(Timeline_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Timeline_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Timeline_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Timeline objects",
		0,
		0,
		0,
		0,
		0, 0,
		TimelineMethods,
		TimelineMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Timeline_init),
		0, Timeline_new,
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

	PyObject* PyInit_bee_timeline(PyObject* module) {
		TimelineType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&TimelineType) < 0) {
			return nullptr;
		}

		Py_INCREF(&TimelineType);
		PyModule_AddObject(module, "Timeline", reinterpret_cast<PyObject*>(&TimelineType));

		return reinterpret_cast<PyObject*>(&TimelineType);
	}

	Timeline* as_timeline(TimelineObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Timeline::get_by_name(_name);
	}
	Timeline* as_timeline(PyObject* self) {
		if (Timeline_check(self)) {
			return as_timeline(reinterpret_cast<TimelineObject*>(self));
		}
		return nullptr;
	}

	void Timeline_dealloc(TimelineObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Timeline_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		TimelineObject* self;

		self = reinterpret_cast<TimelineObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Timeline_init(TimelineObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Timeline_repr(TimelineObject* self) {
		std::string s = std::string("bee.Timeline(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Timeline_str(TimelineObject* self) {
		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return Py_BuildValue("N", PyUnicode_FromString("Invalid Timeline name"));
		}

		Variant m (tl->serialize());
		std::string s = "Timeline " + m.to_str(true);

		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Timeline_print(TimelineObject* self, PyObject* args) {
		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		tl->print();

		Py_RETURN_NONE;
	}

	PyObject* Timeline_get_actions(TimelineObject* self, PyObject* args) {
		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		auto _actions = tl->get_actions();
		PyObject* actions = PyList_New(_actions.size());

		Py_ssize_t i = 0;
		for (auto& a : _actions) {
			PyList_SetItem(actions, i++, Py_BuildValue("(iN)", a.first, PyUnicode_FromString(a.second.name.c_str())));
		}

		return actions;
	}

	PyObject* Timeline_add_action(TimelineObject* self, PyObject* args) {
		Uint32 frame = -1;
		PyObject* name = nullptr;
		PyObject* callback = nullptr;

		if (!PyArg_ParseTuple(args, "IUO", &frame, &name, &callback)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		Py_INCREF(callback);
		tl->add_action(frame, _name, [callback] (TimelineIterator* tlit, TimelineAction* action) {
			PyObject* arg_tup = Py_BuildValue("(N)", PyUnicode_FromString(action->name.c_str()));
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}

			Py_DECREF(arg_tup);
		});

		Py_RETURN_NONE;
	}
	PyObject* Timeline_remove_actions(TimelineObject* self, PyObject* args) {
		Uint32 frame_start = -1;
		Uint32 frame_end = -1;

		if (!PyArg_ParseTuple(args, "|II", &frame_start, &frame_end)) {
			return nullptr;
		}

		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		if ((frame_start != static_cast<Uint32>(-1))&&(frame_end != static_cast<Uint32>(-1))) {
			return Py_BuildValue("i", tl->remove_actions_range(frame_start, frame_end));
		} else if (frame_start != static_cast<Uint32>(-1)) {
			return Py_BuildValue("i", tl->remove_actions(frame_start));
		} else {
			return Py_BuildValue("i", tl->remove_actions_all());
		}
	}
	PyObject* Timeline_set_ending(TimelineObject* self, PyObject* args) {
		PyObject* callback = nullptr;

		if (!PyArg_ParseTuple(args, "O", &callback)) {
			return nullptr;
		}

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		Py_INCREF(callback);
		tl->set_ending(TimelineAction("end", [callback] (TimelineIterator* tlit, TimelineAction* action) {
			PyObject* arg_tup = Py_BuildValue("(N)", PyUnicode_FromString(action->name.c_str()));
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}

			Py_DECREF(arg_tup);
		}));

		Py_RETURN_NONE;
	}

	PyObject* Timeline_start(TimelineObject* self, PyObject* args) {
		Uint32 start_offset = 0;
		int is_looping = false;
		int is_pausable = false;

		if (!PyArg_ParseTuple(args, "|Ipp", &start_offset, &is_looping, &is_pausable)) {
			return nullptr;
		}

		bool _is_looping = is_looping;
		bool _is_pausable = is_pausable;

		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		TimelineIterator* tlit = new TimelineIterator(tl, start_offset, _is_looping, _is_pausable);

		int index = 0;
		if (!tlits[tl].empty()) {
			index = tlits[tl].end()->first+1;
		}
		tlits[tl].emplace(index, tlit);

		return Py_BuildValue("(Ni)", tl->get_name(), index);
	}
	PyObject* Timeline_step_to(TimelineObject* self, PyObject* args) {
		PyObject* name = nullptr;
		int index = -1;
		Uint32 frame = -1;

		if (!PyArg_ParseTuple(args, "(Ui)I", &name, &index, &frame)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		if ((tl->get_name() != _name)||(index < 0)) {
			PyErr_SetString(PyExc_ValueError, "parameter must be valid TimelineIterator");
			return nullptr;
		}

		int r = tl->step_to(tlits[tl][index], frame);
		if (r == 2) {
			tlits[tl].erase(index);
			if (tlits[tl].empty()) {
				tlits.erase(tl);
			}
		}

		return Py_BuildValue("i", r);
	}
	PyObject* Timeline_end(TimelineObject* self, PyObject* args) {
		PyObject* name = nullptr;
		int index = -1;

		if (!PyArg_ParseTuple(args, "(Ui)", &name, &index)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Timeline* tl = as_timeline(self);
		if (tl == nullptr) {
			return nullptr;
		}

		if ((tl->get_name() != _name)||(index < 0)) {
			PyErr_SetString(PyExc_ValueError, "parameter must be valid TimelineIterator");
			return nullptr;
		}

		TimelineIterator* tlit = tlits[tl][index];
		tl->end(tlit);

		if (!tlit->is_looping) {
			tlits[tl].erase(index);
			if (tlits[tl].empty()) {
				tlits.erase(tl);
			}
		}

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_TIMELINE
