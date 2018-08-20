/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_ROOM_H
#define BEE_PYTHON_ROOM_H 1

#include <Python.h>

namespace bee {
	class Room;
namespace python {
	PyObject* Room_from(const Room*);
	bool Room_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} RoomObject;

	extern PyTypeObject RoomType;

	PyObject* PyInit_bee_room(PyObject*);

	Room* as_room(RoomObject*);
	Room* as_room(PyObject*);

	void Room_dealloc(RoomObject*);
	PyObject* Room_new(PyTypeObject*, PyObject*, PyObject*);
	int Room_init(RoomObject*, PyObject*, PyObject*);

	// Room methods
	PyObject* Room_repr(RoomObject*);
	PyObject* Room_str(RoomObject*);
	PyObject* Room_print(RoomObject*, PyObject*);

	PyObject* Room_get_width(RoomObject*, PyObject*);
	PyObject* Room_get_height(RoomObject*, PyObject*);
	PyObject* Room_get_is_persistent(RoomObject*, PyObject*);
	PyObject* Room_get_backgrounds(RoomObject*, PyObject*);
	PyObject* Room_get_viewports(RoomObject*, PyObject*);
	PyObject* Room_get_instances(RoomObject*, PyObject*);
	PyObject* Room_get_current_viewport(RoomObject*, PyObject*);
	PyObject* Room_get_phys_world(RoomObject*, PyObject*);
	PyObject* Room_get_paths(RoomObject*, PyObject*);
	PyObject* Room_get_timelines(RoomObject*, PyObject*);

	PyObject* Room_set_width(RoomObject*, PyObject*);
	PyObject* Room_set_height(RoomObject*, PyObject*);
	PyObject* Room_set_is_persistent(RoomObject*, PyObject*);

	PyObject* Room_add_background(RoomObject*, PyObject*);
	PyObject* Room_remove_background(RoomObject*, PyObject*);
	PyObject* Room_add_viewport(RoomObject*, PyObject*);
	PyObject* Room_remove_viewport(RoomObject*, PyObject*);
	PyObject* Room_add_instance(RoomObject*, PyObject*);
	PyObject* Room_destroy(RoomObject*, PyObject*);
	PyObject* Room_destroy_all(RoomObject*, PyObject*);
	PyObject* Room_automate_path(RoomObject*, PyObject*);
	PyObject* Room_automate_timeline(RoomObject*, PyObject*);
}}}

#endif // BEE_PYTHON_ROOM_H
