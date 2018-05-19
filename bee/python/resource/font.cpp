/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_FONT
#define BEE_PYTHON_FONT 1

#include <Python.h>
#include <structmember.h>

#include "font.hpp"

#include "../python.hpp"

#include "../../resource/font.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef FontMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Font_print), METH_NOARGS, "Print all relevant information about the Font"},

		{"get_font_size", reinterpret_cast<PyCFunction>(Font_get_font_size), METH_NOARGS, "Return the Font size"},
		{"get_style", reinterpret_cast<PyCFunction>(Font_get_style), METH_NOARGS, "Return the Font style"},
		{"get_lineskip", reinterpret_cast<PyCFunction>(Font_get_lineskip), METH_NOARGS, "Return the lineskip"},
		{"get_lineskip_default", reinterpret_cast<PyCFunction>(Font_get_lineskip_default), METH_NOARGS, "Return the default lineskip"},
		{"get_fontname", reinterpret_cast<PyCFunction>(Font_get_fontname), METH_NOARGS, "Return the Font's name"},

		{"set_font_size", reinterpret_cast<PyCFunction>(Font_set_font_size), METH_VARARGS, "Set the font size to use"},
		{"set_style", reinterpret_cast<PyCFunction>(Font_set_style), METH_VARARGS, "Set the font style to use"},
		{"set_lineskip", reinterpret_cast<PyCFunction>(Font_set_lineskip), METH_VARARGS, "Set the lineskip to use"},

		{"load", reinterpret_cast<PyCFunction>(Font_load), METH_NOARGS, "Load the Font from its path"},
		{"free", reinterpret_cast<PyCFunction>(Font_free), METH_NOARGS, "Free the Font"},

		//{"draw", reinterpret_cast<PyCFunction>(Font_draw), METH_VARARGS, "Draw the given text with the given attributes"},
		{"draw_fast", reinterpret_cast<PyCFunction>(Font_draw_fast), METH_VARARGS, "Draw the given text with the given attributes without storing the rendered text"},

		{"get_string_width", reinterpret_cast<PyCFunction>(Font_get_string_width), METH_VARARGS, "Return the width of the given text in the given font size"},
		{"get_string_height", reinterpret_cast<PyCFunction>(Font_get_string_height), METH_VARARGS, "Return the height of the given text in the given font size"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef FontMembers[] = {
		{"name", T_OBJECT_EX, offsetof(FontObject, name), 0, "The Font name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject FontType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Font",
		sizeof(FontObject), 0,
		reinterpret_cast<destructor>(Font_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Font_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Font_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Font objects",
		0,
		0,
		0,
		0,
		0, 0,
		FontMethods,
		FontMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Font_init),
		0, Font_new,
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

	PyObject* PyInit_bee_font(PyObject* module) {
		FontType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&FontType) < 0) {
			return nullptr;
		}

		Py_INCREF(&FontType);
		PyModule_AddObject(module, "Font", reinterpret_cast<PyObject*>(&FontType));

		return reinterpret_cast<PyObject*>(&FontType);
	}

	Font* as_font(FontObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Font::get_by_name(_name);
	}

	void Font_dealloc(FontObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Font_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		FontObject* self;

		self = reinterpret_cast<FontObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Font_init(FontObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Font_repr(FontObject* self) {
		std::string s = std::string("bee.Font(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Font_str(FontObject* self) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return Py_BuildValue("N", PyUnicode_FromString("Invalid Font name"));
		}

		Variant m (font->serialize());
		std::string s = "Font " + m.to_str(true);

		return Py_BuildValue("N", PyUnicode_FromString(s.c_str()));
	}
	PyObject* Font_print(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		font->print();

		Py_RETURN_NONE;
	}

	PyObject* Font_get_font_size(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->get_font_size());
	}
	PyObject* Font_get_style(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->get_style());
	}
	PyObject* Font_get_lineskip(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->get_lineskip());
	}
	PyObject* Font_get_lineskip_default(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->get_lineskip_default());
	}
	PyObject* Font_get_fontname(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("N", PyUnicode_FromString(font->get_fontname().c_str()));
	}

	PyObject* Font_set_font_size(FontObject* self, PyObject* args) {
		int size;

		if (!PyArg_ParseTuple(args, "i", &size)) {
			return nullptr;
		}

		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->set_font_size(size));
	}
	PyObject* Font_set_style(FontObject* self, PyObject* args) {
		int style;

		if (!PyArg_ParseTuple(args, "i", &style)) {
			return nullptr;
		}

		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		font->set_style(static_cast<E_FONT_STYLE>(style));

		Py_RETURN_NONE;
	}
	PyObject* Font_set_lineskip(FontObject* self, PyObject* args) {
		int lineskip;

		if (!PyArg_ParseTuple(args, "i", &lineskip)) {
			return nullptr;
		}

		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		font->set_lineskip(lineskip);

		Py_RETURN_NONE;
	}

	PyObject* Font_load(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->load());
	}
	PyObject* Font_free(FontObject* self, PyObject* args) {
		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->free());
	}

	PyObject* Font_draw(FontObject* self, PyObject* args) {
		Py_RETURN_NONE;
	}
	PyObject* Font_draw_fast(FontObject* self, PyObject* args) {
		int x, y;
		PyObject* text;
		RGBA color (0, 0, 0, 255);

		if (!PyArg_ParseTuple(args, "iiU|(bbbb)", &x, &y, &text, &color.r, &color.g, &color.b, &color.a)) {
			return nullptr;
		}

		std::string _text (PyUnicode_AsUTF8(text));

		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", font->draw_fast(x, y, _text, color));
	}

	PyObject* Font_get_string_width(FontObject* self, PyObject* args) {
		PyObject* text (PyUnicode_FromString("W"));
		int size = -1;

		if (!PyArg_ParseTuple(args, "Ui", &text, size)) {
			return nullptr;
		}

		std::string _text (PyUnicode_AsUTF8(text));

		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		if (size <= 0) {
			size = font->get_font_size();
		}

		return Py_BuildValue("i", font->get_string_width(_text, size));
	}
	PyObject* Font_get_string_height(FontObject* self, PyObject* args) {
		PyObject* text (PyUnicode_FromString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890`~!@#$%^&*(),./;'[]\\-='<>?:\"{}|_+"));
		int size = -1;

		if (!PyArg_ParseTuple(args, "Ui", &text, size)) {
			return nullptr;
		}

		std::string _text (PyUnicode_AsUTF8(text));

		Font* font = as_font(self);
		if (font == nullptr) {
			return nullptr;
		}

		if (size <= 0) {
			size = font->get_font_size();
		}

		return Py_BuildValue("i", font->get_string_height(_text, size));
	}
}}}

#endif // BEE_PYTHON_FONT
