/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_TEXTURE
#define BEE_PYTHON_TEXTURE 1

#include <Python.h>
#include <structmember.h>

#include "texture.hpp"

#include "../python.hpp"

#include "../../resource/texture.hpp"

namespace bee { namespace python {
	PyObject* Texture_from(const Texture* texture) {
		if (texture == nullptr) {
			return nullptr;
		}

		PyObject* py_texture = internal::Texture_new(&internal::TextureType, nullptr, nullptr);
		internal::TextureObject* _py_texture = reinterpret_cast<internal::TextureObject*>(py_texture);

		if (Texture_init(_py_texture, Py_BuildValue("(N)", PyUnicode_FromString(texture->get_name().c_str())), nullptr)) {
			return nullptr;
		}

		return py_texture;
	}
	bool Texture_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::TextureType);
	}
namespace internal {
	PyMethodDef TextureMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Texture_print), METH_NOARGS, "Print all relevant information about the Texture"},

		{"get_size", reinterpret_cast<PyCFunction>(Texture_get_size), METH_NOARGS, "Return the image dimensions"},
		{"get_subimage_amount", reinterpret_cast<PyCFunction>(Texture_get_subimage_amount), METH_NOARGS, "Return the amount of subimages"},
		{"get_subimage_width", reinterpret_cast<PyCFunction>(Texture_get_subimage_width), METH_NOARGS, "Return the width of each subimage"},
		{"get_speed", reinterpret_cast<PyCFunction>(Texture_get_speed), METH_NOARGS, "Return the animation speed"},
		{"get_is_animated", reinterpret_cast<PyCFunction>(Texture_get_is_animated), METH_NOARGS, "Return whether the image is animating or not"},
		{"get_origin", reinterpret_cast<PyCFunction>(Texture_get_origin), METH_NOARGS, "Return the drawing origin"},
		{"get_rotate", reinterpret_cast<PyCFunction>(Texture_get_rotate), METH_NOARGS, "Return the rotation origin"},
		{"get_is_loaded", reinterpret_cast<PyCFunction>(Texture_get_is_loaded), METH_NOARGS, "Return whether the Texture is loaded or not"},

		{"set_speed", reinterpret_cast<PyCFunction>(Texture_set_speed), METH_VARARGS, "Set the animation speed"},
		{"set_origin", reinterpret_cast<PyCFunction>(Texture_set_origin), METH_VARARGS, "Set the drawing origin"},
		{"set_rotate", reinterpret_cast<PyCFunction>(Texture_set_rotate), METH_VARARGS, "Set the rotation origin"},
		{"set_subimage_amount", reinterpret_cast<PyCFunction>(Texture_set_subimage_amount), METH_VARARGS, "Set the amount and width of subimages"},
		{"crop_image", reinterpret_cast<PyCFunction>(Texture_crop_image), METH_VARARGS, "Set the cropping border for the image"},

		{"load", reinterpret_cast<PyCFunction>(Texture_load), METH_NOARGS, "Load the Texture from its path"},
		{"load_as_target", reinterpret_cast<PyCFunction>(Texture_load_as_target), METH_VARARGS, "Setup the texture for use as a render target"},
		{"free", reinterpret_cast<PyCFunction>(Texture_free), METH_NOARGS, "Free the Texture and delete all of its buffers"},

		{"draw", reinterpret_cast<PyCFunction>(Texture_draw), METH_VARARGS, "Draw the Texture using the given attributes"},
		{"set_as_target", reinterpret_cast<PyCFunction>(Texture_set_as_target), METH_NOARGS, "Set the Texture as the render target"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef TextureMembers[] = {
		{"name", T_OBJECT_EX, offsetof(TextureObject, name), 0, "The Texture name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject TextureType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Texture",
		sizeof(TextureObject), 0,
		reinterpret_cast<destructor>(Texture_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Texture_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Texture_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Texture objects",
		0,
		0,
		0,
		0,
		0, 0,
		TextureMethods,
		TextureMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Texture_init),
		0, Texture_new,
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

	PyObject* PyInit_bee_texture(PyObject* module) {
		TextureType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&TextureType) < 0) {
			return nullptr;
		}

		Py_INCREF(&TextureType);
		PyModule_AddObject(module, "Texture", reinterpret_cast<PyObject*>(&TextureType));

		return reinterpret_cast<PyObject*>(&TextureType);
	}

	Texture* as_texture(TextureObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Texture::get_by_name(_name);
	}
	Texture* as_texture(PyObject* self) {
		if (Texture_check(self)) {
			return as_texture(reinterpret_cast<TextureObject*>(self));
		}
		return nullptr;
	}

	void Texture_dealloc(TextureObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Texture_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		TextureObject* self;

		self = reinterpret_cast<TextureObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Texture_init(TextureObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Texture_repr(TextureObject* self) {
		std::string s = std::string("bee.Texture(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Texture_str(TextureObject* self) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return PyUnicode_FromString("Invalid Texture name");
		}

		Variant m (tex->serialize());
		std::string s = "Texture " + m.to_str(true);

		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Texture_print(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		tex->print();

		Py_RETURN_NONE;
	}

	PyObject* Texture_get_size(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		std::pair<int,int> size (tex->get_size());

		return Py_BuildValue("(ii)", size.first, size.second);
	}
	PyObject* Texture_get_subimage_amount(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", tex->get_subimage_amount());
	}
	PyObject* Texture_get_subimage_width(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", tex->get_subimage_width());
	}
	PyObject* Texture_get_speed(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", tex->get_speed());
	}
	PyObject* Texture_get_is_animated(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(tex->get_is_animated());
	}
	PyObject* Texture_get_origin(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		std::pair<int,int> origin (tex->get_origin());

		return Py_BuildValue("(ii)", origin.first, origin.second);
	}
	PyObject* Texture_get_rotate(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		std::pair<double,double> rotate (tex->get_rotate());

		return Py_BuildValue("(dd)", rotate.first, rotate.second);
	}
	PyObject* Texture_get_is_loaded(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(tex->get_is_loaded());
	}

	PyObject* Texture_set_speed(TextureObject* self, PyObject* args) {
		double speed;

		if (!PyArg_ParseTuple(args, "d", &speed)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		tex->set_speed(speed);

		Py_RETURN_NONE;
	}
	PyObject* Texture_set_origin(TextureObject* self, PyObject* args) {
		int ox, oy;

		if (!PyArg_ParseTuple(args, "(ii)", &ox, &oy)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		tex->set_origin(ox, oy);

		Py_RETURN_NONE;
	}
	PyObject* Texture_set_rotate(TextureObject* self, PyObject* args) {
		double rx, ry;

		if (!PyArg_ParseTuple(args, "(dd)", &rx, &ry)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		tex->set_rotate(rx, ry);

		Py_RETURN_NONE;
	}
	PyObject* Texture_set_subimage_amount(TextureObject* self, PyObject* args) {
		int amount;
		int width;

		if (!PyArg_ParseTuple(args, "ii", &amount, &width)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		tex->set_subimage_amount(amount, width);

		Py_RETURN_NONE;
	}
	PyObject* Texture_crop_image(TextureObject* self, PyObject* args) {
		SDL_Rect crop;

		if (!PyArg_ParseTuple(args, "(iiii)", &crop.x, &crop.y, &crop.w, &crop.h)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		tex->crop_image(crop);

		Py_RETURN_NONE;
	}

	PyObject* Texture_load(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", tex->load());
	}
	PyObject* Texture_load_as_target(TextureObject* self, PyObject* args) {
		int w, h;

		if (!PyArg_ParseTuple(args, "ii", &w, &h)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", tex->load_as_target(w, h));
	}
	PyObject* Texture_free(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", tex->free());
	}

	PyObject* Texture_draw(TextureObject* self, PyObject* args) {
		int x, y;
		Uint32 t;
		int w = -1, h = -1;
		double angle = 0.0;
		RGBA color (255, 255, 255, 255);
		if (!PyArg_ParseTuple(args, "iiI|iid(iiii)", &x, &y, &t, &w, &h, &angle, &color.r, &color.g, &color.b, &color.a)) {
			return nullptr;
		}

		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", tex->draw(x, y, t, w, h, angle, color));
	}
	PyObject* Texture_set_as_target(TextureObject* self, PyObject* args) {
		Texture* tex = as_texture(self);
		if (tex == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("I", tex->set_as_target());
	}
}}}

#endif // BEE_PYTHON_TEXTURE
