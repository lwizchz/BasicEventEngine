/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PHYSICS_WORLD_H
#define BEE_PYTHON_PHYSICS_WORLD_H 1

#include <string>
#include <memory>

#include <Python.h>

namespace bee {
	class PhysicsWorld;
namespace python {
	PyObject* PhysicsWorld_from(std::weak_ptr<PhysicsWorld>);
	bool PhysicsWorld_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		std::weak_ptr<PhysicsWorld> world;
	} PhysicsWorldObject;

	extern PyTypeObject PhysicsWorldType;

	PyObject* PyInit_bee_physics_world(PyObject*);

	std::weak_ptr<PhysicsWorld> as_physics_world(PhysicsWorldObject*);
	std::weak_ptr<PhysicsWorld> as_physics_world(PyObject*);

	void PhysicsWorld_dealloc(PhysicsWorldObject*);
	PyObject* PhysicsWorld_new(PyTypeObject*, PyObject*, PyObject*);

	// PhysicsWorld methods
	PyObject* PhysicsWorld_get_gravity(PhysicsWorldObject*, PyObject*);
	PyObject* PhysicsWorld_get_scale(PhysicsWorldObject*, PyObject*);

	PyObject* PhysicsWorld_set_gravity(PhysicsWorldObject*, PyObject*);
	PyObject* PhysicsWorld_set_scale(PhysicsWorldObject*, PyObject*);

	PyObject* PhysicsWorld_add_body(PhysicsWorldObject*, PyObject*);
	PyObject* PhysicsWorld_add_constraint(PhysicsWorldObject*, PyObject*);

	PyObject* PhysicsWorld_remove_body(PhysicsWorldObject*, PyObject*);

	PyObject* PhysicsWorld_step(PhysicsWorldObject*, PyObject*);

	PyObject* PhysicsWorld_draw_debug(PhysicsWorldObject*, PyObject*);
}}}

#endif // BEE_PYTHON_PHYSICS_WORLD_H
