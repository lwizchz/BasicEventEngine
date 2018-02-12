/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_INSTANCE
#define BEE_PYTHON_INSTANCE 1

#include <Python.h>
#include <structmember.h>

#include "instance.hpp"

#include "python.hpp"

#include "../core/instance.hpp"

#include "../resource/texture.hpp"
#include "../resource/object.hpp"

namespace bee { namespace python { namespace internal {
        PyMethodDef InstanceMethods[] = {
                {"print", reinterpret_cast<PyCFunction>(Instance_print), METH_NOARGS, "Print info about the instance"},

                {"set_alarm", reinterpret_cast<PyCFunction>(Instance_set_alarm), METH_VARARGS, ""},

                {"set_sprite", reinterpret_cast<PyCFunction>(Instance_set_sprite), METH_VARARGS, ""},
                {"set_computation_type", reinterpret_cast<PyCFunction>(Instance_set_computation_type), METH_VARARGS, ""},
                {"set_is_persistent", reinterpret_cast<PyCFunction>(Instance_set_is_persistent), METH_VARARGS, ""},

                {"get_data", reinterpret_cast<PyCFunction>(Instance_get_data), METH_VARARGS, ""},
                {"set_data", reinterpret_cast<PyCFunction>(Instance_set_data), METH_VARARGS, ""},

                {"get_pos", reinterpret_cast<PyCFunction>(Instance_get_pos), METH_NOARGS, ""},
                {"get_corner", reinterpret_cast<PyCFunction>(Instance_get_corner), METH_NOARGS, ""},
                {"get_start", reinterpret_cast<PyCFunction>(Instance_get_start), METH_NOARGS, ""},

                {"get_sprite", reinterpret_cast<PyCFunction>(Instance_get_sprite), METH_NOARGS, ""},
                {"get_mass", reinterpret_cast<PyCFunction>(Instance_get_mass), METH_NOARGS, ""},
                {"get_computation_type", reinterpret_cast<PyCFunction>(Instance_get_computation_type), METH_NOARGS, ""},
                {"get_is_persistent", reinterpret_cast<PyCFunction>(Instance_get_is_persistent), METH_NOARGS, ""},
                {"get_aabb", reinterpret_cast<PyCFunction>(Instance_get_aabb), METH_NOARGS, ""},

                {"set_pos", reinterpret_cast<PyCFunction>(Instance_set_pos), METH_VARARGS, ""},
                {"set_to_start", reinterpret_cast<PyCFunction>(Instance_set_to_start), METH_NOARGS, ""},
                {"set_corner", reinterpret_cast<PyCFunction>(Instance_set_corner), METH_VARARGS, ""},
                {"set_mass", reinterpret_cast<PyCFunction>(Instance_set_mass), METH_VARARGS, ""},
                {"move", reinterpret_cast<PyCFunction>(Instance_move), METH_VARARGS, ""},
                {"set_friction", reinterpret_cast<PyCFunction>(Instance_set_friction), METH_VARARGS, ""},
                {"set_gravity", reinterpret_cast<PyCFunction>(Instance_set_gravity), METH_VARARGS, ""},
                {"set_velocity", reinterpret_cast<PyCFunction>(Instance_set_velocity), METH_VARARGS, ""},
                {"add_velocity", reinterpret_cast<PyCFunction>(Instance_add_velocity), METH_VARARGS, ""},
                {"limit_velocity", reinterpret_cast<PyCFunction>(Instance_limit_velocity), METH_VARARGS | METH_KEYWORDS, ""},

                {"get_speed", reinterpret_cast<PyCFunction>(Instance_get_speed), METH_NOARGS, ""},
                {"get_velocity", reinterpret_cast<PyCFunction>(Instance_get_velocity), METH_NOARGS, ""},
                {"get_velocity_ang", reinterpret_cast<PyCFunction>(Instance_get_velocity_ang), METH_NOARGS, ""},
                {"get_friction", reinterpret_cast<PyCFunction>(Instance_get_friction), METH_NOARGS, ""},
                {"get_gravity", reinterpret_cast<PyCFunction>(Instance_get_gravity), METH_NOARGS, ""},

                {nullptr, nullptr, 0, nullptr}
        };

        PyMemberDef InstanceMembers[] = {
                {"object", T_OBJECT_EX, offsetof(InstanceObject, object), 0, "The object name"},
                {"num", T_INT, offsetof(InstanceObject, num), 0, "The instance number"},
                {nullptr, 0, 0, 0, nullptr}
        };

        PyTypeObject InstanceType = {
                PyVarObject_HEAD_INIT(NULL, 0)
                "bee.Instance",
                sizeof(InstanceObject), 0,
                reinterpret_cast<destructor>(Instance_dealloc),
                0,
                0, 0,
                0,
                reinterpret_cast<reprfunc>(Instance_repr),
                0, 0, 0,
                0,
                0,
                reinterpret_cast<reprfunc>(Instance_str),
                0, 0,
                0,
                Py_TPFLAGS_DEFAULT,
                "Instance objects",
                0,
                0,
                0,
                0,
                0, 0,
                InstanceMethods,
                InstanceMembers,
                0,
                0,
                0,
                0, 0,
                0,
                reinterpret_cast<initproc>(Instance_init),
                0, Instance_new,
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

        PyObject* PyInit_bee_instance(PyObject* module) {
                InstanceType.tp_new = PyType_GenericNew;
                if (PyType_Ready(&InstanceType) < 0) {
                        return nullptr;
                }

                Py_INCREF(&InstanceType);
                PyModule_AddObject(module, "Instance", reinterpret_cast<PyObject*>(&InstanceType));

                return reinterpret_cast<PyObject*>(&InstanceType);
        }

        Instance* as_instance(InstanceObject* self) {
                if (self->object == nullptr) {
                        PyErr_SetString(PyExc_AttributeError, "object");
                        return nullptr;
                }
                std::string _object (PyUnicode_AsUTF8(self->object));

                return Object::get_by_name(_object)->get_instance(self->num);
        }

        void Instance_dealloc(InstanceObject* self) {
                Py_XDECREF(self->object);
                Py_TYPE(self)->tp_free(self);
        }
        PyObject* Instance_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
                InstanceObject* self;

                self = reinterpret_cast<InstanceObject*>(type->tp_alloc(type, 0));
                if (self != nullptr) {
                        self->object = PyUnicode_FromString("");
                        if (self->object == nullptr) {
                                Py_DECREF(self);
                                return nullptr;
                        }

                        self->num = 0;
                }

                return reinterpret_cast<PyObject*>(self);
        }
        int Instance_init(InstanceObject* self, PyObject* args, PyObject* kwds) {
                PyObject* object = nullptr;

                const char* kwlist[] = {"object", "num", nullptr};
                if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oi", const_cast<char**>(kwlist), &object, &self->num)) {
                        return -1;
                }

                if (object != nullptr) {
                        PyObject* tmp = self->object;
                        Py_INCREF(object);
                        self->object = object;
                        Py_XDECREF(tmp);
                }

                return 0;
        }

        PyObject* Instance_repr(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                std::string s = std::string("bee.Instance(\"") + PyUnicode_AsUTF8(self->object) + "\", " + std::to_string(self->num) + ")";

                return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
        }
        PyObject* Instance_str(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                Variant m (inst->serialize());
                std::string s = "Instance " + m.to_str(true);

                return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
        }
        PyObject* Instance_print(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->print();

                Py_RETURN_NONE;
        }

        PyObject* Instance_set_alarm(InstanceObject* self, PyObject* args) {
                PyObject* name;
                int ticks;
                if (!PyArg_ParseTuple(args, "Ui", &name, &ticks)) {
                        return nullptr;
                }

                std::string _name (PyUnicode_AsUTF8(name));

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_alarm(_name, ticks);

                Py_RETURN_NONE;
        }

        PyObject* Instance_set_sprite(InstanceObject* self, PyObject* args) {
                PyObject* texture;
                if (!PyArg_ParseTuple(args, "U", &texture)) {
                        return nullptr;
                }

                std::string _texture (PyUnicode_AsUTF8(texture));

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_sprite(Texture::get_by_name(_texture));

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_computation_type(InstanceObject* self, PyObject* args) {
                unsigned long comptype;
                if (!PyArg_ParseTuple(args, "k", &comptype)) {
                        return nullptr;
                }

                E_COMPUTATION _comptype (static_cast<E_COMPUTATION>(comptype));

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_computation_type(_comptype);

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_is_persistent(InstanceObject* self, PyObject* args) {
                bool is_persistent;
                if (!PyArg_ParseTuple(args, "p", &is_persistent)) {
                        return nullptr;
                }

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_is_persistent(is_persistent);

                Py_RETURN_NONE;
        }

        PyObject* Instance_get_data(InstanceObject* self, PyObject* args) {
                PyObject* field;
                if (!PyArg_ParseTuple(args, "U", &field)) {
                        return nullptr;
                }

                std::string _field (PyUnicode_AsUTF8(field));

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                Variant data (inst->get_data(_field));

                return Py_BuildValue("N", variant_to_pyobj(data));
        }
        PyObject* Instance_set_data(InstanceObject* self, PyObject* args) {
                PyObject* field;
                PyObject* data;
                if (!PyArg_ParseTuple(args, "UO", &field, &data)) {
                        return nullptr;
                }

                std::string _field (PyUnicode_AsUTF8(field));

                Variant _data (pyobj_to_variant(data));

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_data(_field, _data);

                Py_RETURN_NONE;
        }

        PyObject* Instance_get_pos(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                btVector3 pos (inst->get_pos());

                return Py_BuildValue("(ddd)", pos.x(), pos.y(), pos.z());
        }
        PyObject* Instance_get_corner(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                double cx, cy;
                std::tie(cx, cy) = inst->get_corner();

                return Py_BuildValue("(dd)", cx, cy);
        }
        PyObject* Instance_get_start(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                btVector3 start (inst->get_start());

                return Py_BuildValue("(ddd)", start.x(), start.y(), start.z());
        }

        PyObject* Instance_get_sprite(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                return Py_BuildValue("N", PyUnicode_FromString(inst->get_sprite()->get_name().c_str()));
        }
        PyObject* Instance_get_mass(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                return Py_BuildValue("d", inst->get_mass());
        }
        PyObject* Instance_get_computation_type(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                return Py_BuildValue("k", inst->get_computation_type());
        }
        PyObject* Instance_get_is_persistent(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                return Py_BuildValue("p", inst->get_is_persistent());
        }
        PyObject* Instance_get_aabb(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }
                SDL_Rect aabb = inst->get_aabb();

                return Py_BuildValue("(iiii)", aabb.x, aabb.y, aabb.w, aabb.h);
        }

        PyObject* Instance_set_pos(InstanceObject* self, PyObject* args) {
                double x, y, z;
                if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
                        return nullptr;
                }

                btScalar _x (x), _y (y), _z (z);

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_pos({_x, _y, _z});

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_to_start(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_to_start();

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_corner(InstanceObject* self, PyObject* args) {
                double cx, cy;
                if (!PyArg_ParseTuple(args, "dd", &cx, &cy)) {
                        return nullptr;
                }

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_corner(cx, cy);

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_mass(InstanceObject* self, PyObject* args) {
                double mass;
                if (!PyArg_ParseTuple(args, "d", &mass)) {
                        return nullptr;
                }

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_mass(mass);

                Py_RETURN_NONE;
        }
        PyObject* Instance_move(InstanceObject* self, PyObject* args) {
                double x, y, z;
                if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
                        return nullptr;
                }

                btScalar _x (x), _y (y), _z (z);

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->move({_x, _y, _z});

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_friction(InstanceObject* self, PyObject* args) {
                double friction;
                if (!PyArg_ParseTuple(args, "d", &friction)) {
                        return nullptr;
                }

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_friction(friction);

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_gravity(InstanceObject* self, PyObject* args) {
                double x, y, z;
                if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
                        return nullptr;
                }

                btScalar _x (x), _y (y), _z (z);

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_gravity({_x, _y, _z});

                Py_RETURN_NONE;
        }
        PyObject* Instance_set_velocity(InstanceObject* self, PyObject* args) {
                double x, y, z;
                if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
                        return nullptr;
                }

                btScalar _x (x), _y (y), _z (z);

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->set_velocity({_x, _y, _z});

                Py_RETURN_NONE;
        }
        PyObject* Instance_add_velocity(InstanceObject* self, PyObject* args) {
                double x, y, z;
                if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
                        return nullptr;
                }

                btScalar _x (x), _y (y), _z (z);

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                inst->add_velocity({_x, _y, _z});

                Py_RETURN_NONE;
        }
        PyObject* Instance_limit_velocity(InstanceObject* self, PyObject* args, PyObject* kwds) {
                const char* kwlist[] = {"", "x", "y", "z", nullptr};

                double vel = nan(""), x = nan(""), y = nan(""), z = nan("");
                if (!PyArg_ParseTupleAndKeywords(args, kwds, "|d$ddd", const_cast<char**>(kwlist), &vel, &x, &y, &z)) {
                        return nullptr;
                }

                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                if (!isnan(vel)) {
                        inst->limit_velocity(vel);
                } else {
                        bool has_param = false;
                        btVector3 limit (-1.0, -1.0, -1.0);
                        if (!isnan(x)) {
                                has_param = true;
                                limit.setX(x);
                        }
                        if (!isnan(y)) {
                                has_param = true;
                                limit.setY(y);
                        }
                        if (!isnan(z)) {
                                has_param = true;
                                limit.setZ(z);
                        }

                        if (limit != btVector3(-1.0, -1.0, -1.0)) {
                                inst->limit_velocity(limit);
                        } else if (!has_param) {
                                PyErr_SetString(PyExc_RuntimeError, "a parameter is required");
                                return nullptr;
                        }
                }

                Py_RETURN_NONE;
        }

        PyObject* Instance_get_speed(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                return Py_BuildValue("d", inst->get_speed());
        }
        PyObject* Instance_get_velocity(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                btVector3 v (inst->get_velocity());

                return Py_BuildValue("(ddd)", v.x(), v.y(), v.z());
        }
        PyObject* Instance_get_velocity_ang(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                btVector3 v (inst->get_velocity_ang());

                return Py_BuildValue("(ddd)", v.x(), v.y(), v.z());
        }
        PyObject* Instance_get_friction(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                return Py_BuildValue("d", inst->get_friction());
        }
        PyObject* Instance_get_gravity(InstanceObject* self, PyObject* args) {
                Instance* inst = as_instance(self);
                if (inst == nullptr) {
                        return nullptr;
                }

                btVector3 g (inst->get_gravity());

                return Py_BuildValue("(ddd)", g.x(), g.y(), g.z());
        }
}}}

#endif // BEE_PYTHON_INSTANCE
