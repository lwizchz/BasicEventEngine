/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_TIMELINE_H
#define BEE_PYTHON_TIMELINE_H 1

#include <string>

#include <Python.h>

namespace bee {
	class Timeline;
namespace python {
	PyObject* Timeline_from(const Timeline*);
	bool Timeline_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} TimelineObject;

	extern PyTypeObject TimelineType;

	PyObject* PyInit_bee_timeline(PyObject*);

	Timeline* as_timeline(TimelineObject*);
	Timeline* as_timeline(PyObject*);

	void Timeline_dealloc(TimelineObject*);
	PyObject* Timeline_new(PyTypeObject*, PyObject*, PyObject*);
	int Timeline_init(TimelineObject*, PyObject*, PyObject*);

	// Timeline methods
	PyObject* Timeline_repr(TimelineObject*);
	PyObject* Timeline_str(TimelineObject*);
	PyObject* Timeline_print(TimelineObject*, PyObject*);

	PyObject* Timeline_get_actions(TimelineObject*, PyObject*);

	PyObject* Timeline_add_action(TimelineObject*, PyObject*);
	PyObject* Timeline_remove_actions(TimelineObject*, PyObject*);
	PyObject* Timeline_set_ending(TimelineObject*, PyObject*);

	PyObject* Timeline_start(TimelineObject*, PyObject*);
	PyObject* Timeline_step_to(TimelineObject*, PyObject*);
	PyObject* Timeline_end(TimelineObject*, PyObject*);
}}}

#endif // BEE_PYTHON_TIMELINE_H
