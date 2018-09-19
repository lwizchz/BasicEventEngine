/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_RENDER
#define BEE_PYTHON_RENDER 1

#include "render.hpp"

#include "../messenger/messenger.hpp"

#include "../render/render.hpp"
#include "../render/camera.hpp"
#include "../render/transition.hpp"

#include "resource/texture.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEERenderMethods[] = {
		{"set_is_lightable", render_set_is_lightable, METH_VARARGS, "Set whether to enable lighting or not"},

		{"set_3d", render_set_3d, METH_VARARGS, "Set whether 3D mode is enabled or not"},
		{"set_camera", render_set_camera, METH_VARARGS, "Set the camera position and angle for 3D mode"},
		{"get_3d", render_get_3d, METH_NOARGS, "Return whether 3D mode is enabled or not"},
		{"get_camera", render_get_camera, METH_NOARGS, "Get a copy of the camera values"},

		{"get_transition_type", render_get_transition_type, METH_NOARGS, "Return the transition type used when changing Rooms"},
		{"set_transition_type", render_set_transition_type, METH_VARARGS, "Set the transition type used when changing Rooms"},
		{"set_transition_custom", render_set_transition_custom, METH_VARARGS, "Set the transition type to a custom function"},
		{"get_transition_speed", render_get_transition_speed, METH_NOARGS, "Return the transition speed when drawing Room transitions"},
		{"set_transition_speed", render_set_transition_speed, METH_VARARGS, "Set the transition speed when drawing Room transitions"},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEERenderModule = {
		PyModuleDef_HEAD_INIT, "render", nullptr, -1, BEERenderMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_render() {
		return PyModule_Create(&BEERenderModule);
	}

	PyObject* render_set_is_lightable(PyObject* self, PyObject* args) {
		int is_lightable;

		if (!PyArg_ParseTuple(args, "p", &is_lightable)) {
			return nullptr;
		}

		bool _is_lightable = is_lightable;

		return PyLong_FromLong(render::set_is_lightable(_is_lightable));
	}

	PyObject* render_set_3d(PyObject* self, PyObject* args) {
		int is_3d;

		if (!PyArg_ParseTuple(args, "p", &is_3d)) {
			return nullptr;
		}

		bool _is_3d = is_3d;

		return PyLong_FromLong(render::set_3d(_is_3d));
	}
	PyObject* render_set_camera(PyObject* self, PyObject* args) {
		double px, py, pz;
		double dx, dy, dz;
		double ox, oy, oz;

		if (!PyArg_ParseTuple(
			args, "((ddd)(ddd)(ddd))",
			&px, &py, &pz,
			&dx, &dy, &dz,
			&ox, &oy, &oz
		)) {
			return nullptr;
		}

		glm::vec3 pos (px, py, pz);
		glm::vec3 dir (dx, dy, dz);
		glm::vec3 ori (ox, oy, oz);

		Camera old_camera = render::get_camera();
		if (
			(pos == old_camera.position)
			&&(dir == old_camera.direction)
			&&(ori == old_camera.orientation)
		) {
			return PyLong_FromLong(1);
		}

		Camera* _camera = new Camera(pos, dir, ori);

		return PyLong_FromLong(render::set_camera(_camera));
	}
	PyObject* render_get_3d(PyObject* self, PyObject* args) {
		return PyBool_FromLong(render::get_3d());
	}
	PyObject* render_get_camera(PyObject* self, PyObject* args) {
		Camera c = render::get_camera();

		return Py_BuildValue("((ddd)(ddd)(ddd))",
			c.position.x, c.position.y, c.position.z,
			c.direction.x, c.direction.y, c.direction.z,
			c.orientation.x, c.orientation.y, c.orientation.z
		);
	}

	PyObject* render_get_transition_type(PyObject* self, PyObject* args) {
		return Py_BuildValue("i", render::get_transition_type());
	}
	PyObject* render_set_transition_type(PyObject* self, PyObject* args) {
		int type;

		if (!PyArg_ParseTuple(args, "i", &type)) {
			return nullptr;
		}

		E_TRANSITION _type = static_cast<E_TRANSITION>(type);

		render::set_transition_type(_type);

		Py_RETURN_NONE;
	}
	PyObject* render_set_transition_custom(PyObject* self, PyObject* args) {
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "O", &callback)) {
			return nullptr;
		}

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		render::set_transition_custom([callback] (Texture* before, Texture* after) {
			PyObject* arg_tup = Py_BuildValue("(NN)", Texture_from(before), Texture_from(after));
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}
			Py_DECREF(arg_tup);
		});

		Py_RETURN_NONE;
	}
	PyObject* render_get_transition_speed(PyObject* self, PyObject* args) {
		return PyFloat_FromDouble(render::get_transition_speed());
	}
	PyObject* render_set_transition_speed(PyObject* self, PyObject* args) {
		double speed;

		if (!PyArg_ParseTuple(args, "d", &speed)) {
			return nullptr;
		}

		render::set_transition_speed(speed);

		Py_RETURN_NONE;
	}
}}}

#endif // BEE_PYTHON_RENDER
