/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_PHYSICS_BODY_H
#define BEE_PYTHON_PHYSICS_BODY_H 1

#include <string>
#include <memory>

#include <Python.h>

namespace bee {
	class PhysicsBody;
namespace python {
	PyObject* PhysicsBody_from(std::weak_ptr<PhysicsBody>);
	bool PhysicsBody_check(PyObject*);
namespace internal {
	typedef struct {
		PyObject_HEAD
		std::weak_ptr<PhysicsBody> body;
	} PhysicsBodyObject;

	extern PyTypeObject PhysicsBodyType;

	PyObject* PyInit_bee_physics_body(PyObject*);

	std::weak_ptr<PhysicsBody> as_physics_body(PhysicsBodyObject*);
	std::weak_ptr<PhysicsBody> as_physics_body(PyObject*);

	void PhysicsBody_dealloc(PhysicsBodyObject*);
	PyObject* PhysicsBody_new(PyTypeObject*, PyObject*, PyObject*);

	// PhysicsBody methods
	PyObject* PhysicsBody_attach(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_remove(PhysicsBodyObject*, PyObject*);

	PyObject* PhysicsBody_get_shape_type(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_mass(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_scale(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_inertia(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_world(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_instance(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_constraints(PhysicsBodyObject*, PyObject*);

	PyObject* PhysicsBody_get_pos(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_get_rotation(PhysicsBodyObject*, PyObject*);

	PyObject* PhysicsBody_set_shape(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_set_mass(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_set_friction(PhysicsBodyObject*, PyObject*);

	PyObject* PhysicsBody_add_constraint(PhysicsBodyObject*, PyObject*);
	PyObject* PhysicsBody_remove_constraints(PhysicsBodyObject*, PyObject*);

	PyObject* PhysicsBody_update_state(PhysicsBodyObject*, PyObject*);
}}}

#endif // BEE_PYTHON_PHYSICS_BODY_H
