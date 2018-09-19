/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_UI
#define BEE_PYTHON_UI 1

#include "ui.hpp"

#include "python.hpp"

#include "../ui/ui.hpp"

#include "resource/font.hpp"
#include "instance.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEEUIMethods[] = {
		{"load", ui_load, METH_NOARGS, ""},
		{"free", ui_free, METH_NOARGS, ""},

		{"destroy_parent", ui_destroy_parent, METH_VARARGS, ""},

		{"create_button", ui_create_button, METH_VARARGS, ""},
		{"button_callback", ui_button_callback, METH_VARARGS, ""},

		{"create_handle", ui_create_handle, METH_VARARGS, ""},
		{"destroy_handle", ui_destroy_handle, METH_VARARGS, ""},

		{"create_text_entry", ui_create_text_entry, METH_VARARGS, ""},
		{"add_text_entry_completor", ui_add_text_entry_completor, METH_VARARGS, ""},
		{"add_text_entry_handler", ui_add_text_entry_handler, METH_VARARGS, ""},
		{"text_entry_callback", ui_text_entry_callback, METH_VARARGS, ""},
		{"text_entry_completor", ui_text_entry_completor, METH_VARARGS, ""},
		{"text_entry_handler", ui_text_entry_handler, METH_VARARGS, ""},

		{"create_gauge", ui_create_gauge, METH_VARARGS, ""},

		{"create_slider", ui_create_slider, METH_VARARGS, ""},
		{"slider_callback", ui_slider_callback, METH_VARARGS, ""},

		{"create_optionbox", ui_create_optionbox, METH_VARARGS, ""},
		{"push_optionbox_option", ui_push_optionbox_option, METH_VARARGS, ""},
		{"pop_optionbox_option", ui_pop_optionbox_option, METH_VARARGS, ""},
		{"reset_optionbox_options", ui_reset_optionbox_options, METH_VARARGS, ""},
		{"optionbox_callback", ui_optionbox_callback, METH_VARARGS, ""},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEEUIModule = {
		PyModuleDef_HEAD_INIT, "ui", nullptr, -1, BEEUIMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_ui() {
		return PyModule_Create(&BEEUIModule);
	}

	PyObject* ui_load(PyObject* self, PyObject* args) {
		return PyLong_FromLong(ui::load());
	}
	PyObject* ui_free(PyObject* self, PyObject* args) {
		return PyLong_FromLong(ui::free());
	}

	PyObject* ui_destroy_parent(PyObject* self, PyObject* args) {
		PyObject* parent;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &parent)) {
			return nullptr;
		}

		Instance* _parent = as_instance(parent);
		if (_parent == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(ui::destroy_parent(_parent));
	}

	PyObject* ui_create_button(PyObject* self, PyObject* args) {
		int x, y;
		PyObject* font;
		PyObject* text;
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "iiO!UO", &x, &y, &FontType, &font, &text, &callback)) {
			return nullptr;
		}

		Font* _font = as_font(font);
		if (_font == nullptr) {
			return nullptr;
		}

		std::string _text (PyUnicode_AsUTF8(text));

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		return Instance_from(ui::create_button(x, y, _font, _text, [callback] (Instance* button) {
			PyObject* arg_tup = Py_BuildValue("(N)", Instance_from(button));
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}
			Py_DECREF(arg_tup);
		}));
	}
	PyObject* ui_button_callback(PyObject* self, PyObject* args) {
		PyObject* button;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &button)) {
			return nullptr;
		}

		Instance* _button = as_instance(button);
		if (_button == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(ui::button_callback(_button));
	}

	PyObject* ui_create_handle(PyObject* self, PyObject* args) {
		int x, y;
		int w, h;
		PyObject* parent;

		if (!PyArg_ParseTuple(args, "iiiiO!", &x, &y, &w, &h, &InstanceType, &parent)) {
			return nullptr;
		}

		Instance* _parent = as_instance(parent);
		if (_parent == nullptr) {
			return nullptr;
		}

		return Instance_from(ui::create_handle(x, y, w, h, _parent));
	}
	PyObject* ui_destroy_handle(PyObject* self, PyObject* args) {
		PyObject* handle;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &handle)) {
			return nullptr;
		}

		Instance* _handle = as_instance(handle);
		if (_handle == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(ui::destroy_handle(_handle));
	}

	PyObject* ui_create_text_entry(PyObject* self, PyObject* args) {
		int x, y;
		int rows, cols;
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "iiiiO", &x, &y, &rows, &cols, &callback)) {
			return nullptr;
		}

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		return Instance_from(ui::create_text_entry(x, y, rows, cols, [callback] (Instance* text_entry, const std::string& input) {
			PyObject* arg_tup = Py_BuildValue("(NN)", Instance_from(text_entry), PyUnicode_FromString(input.c_str()));
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}
			Py_DECREF(arg_tup);
		}));
	}
	PyObject* ui_add_text_entry_completor(PyObject* self, PyObject* args) {
		PyObject* text_entry;
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "O!O", &InstanceType, &text_entry, &callback)) {
			return nullptr;
		}

		Instance* _text_entry = as_instance(text_entry);
		if (_text_entry) {
			return nullptr;
		}

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		return PyLong_FromLong(ui::add_text_entry_completor(_text_entry, [callback] (Instance* __text_entry, const std::string& input) -> std::vector<Variant> {
			PyObject* arg_tup = Py_BuildValue("(NN)", Instance_from(__text_entry), PyUnicode_FromString(input.c_str()));
			PyObject* r = PyEval_CallObject(callback, arg_tup);
			if (r == nullptr) {
				PyErr_Print();
			}
			Py_DECREF(arg_tup);

			return pyobj_to_variant(r).v;
		}));
	}
	PyObject* ui_add_text_entry_handler(PyObject* self, PyObject* args) {
		PyObject* text_entry;
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "O!O", &InstanceType, &text_entry, &callback)) {
			return nullptr;
		}

		Instance* _text_entry = as_instance(text_entry);
		if (_text_entry) {
			return nullptr;
		}

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		return PyLong_FromLong(ui::add_text_entry_handler(_text_entry, [callback] (Instance* __text_entry, const std::string& input, const SDL_Event* e) {
			PyObject* keysym = PyDict_New();
			PyDict_SetItemString(keysym, "scancode", PyLong_FromLong(e->key.keysym.scancode));
			PyDict_SetItemString(keysym, "sym", PyLong_FromLong(e->key.keysym.sym));
			PyDict_SetItemString(keysym, "mode", PyLong_FromLong(e->key.keysym.mod));

			PyObject* event = PyDict_New();
			PyDict_SetItemString(event, "type", PyLong_FromLong(e->key.type));
			PyDict_SetItemString(event, "timestamp", PyLong_FromLong(e->key.timestamp));
			PyDict_SetItemString(event, "windowID", PyLong_FromLong(e->key.windowID));
			PyDict_SetItemString(event, "state", PyLong_FromLong(e->key.state));
			PyDict_SetItemString(event, "repeat", PyLong_FromLong(e->key.repeat));
			PyDict_SetItemString(event, "keysym", keysym);

			PyObject* arg_tup = Py_BuildValue("(NNN)", Instance_from(__text_entry), PyUnicode_FromString(input.c_str()), event);
			PyObject* r = PyEval_CallObject(callback, arg_tup);
			if (r == nullptr) {
				PyErr_Print();
			}

			Py_DECREF(arg_tup);
			Py_DECREF(event);
		}));
	}
	PyObject* ui_text_entry_callback(PyObject* self, PyObject* args) {
		PyObject* text_entry;
		PyObject* input;

		if (!PyArg_ParseTuple(args, "O!U", &InstanceType, &text_entry, &input)) {
			return nullptr;
		}

		Instance* _text_entry = as_instance(text_entry);
		if (_text_entry == nullptr) {
			return nullptr;
		}

		std::string _input (PyUnicode_AsUTF8(input));

		return PyLong_FromLong(ui::text_entry_callback(_text_entry, _input));
	}
	PyObject* ui_text_entry_completor(PyObject* self, PyObject* args) {
		PyObject* text_entry;
		PyObject* input;

		if (!PyArg_ParseTuple(args, "O!U", &InstanceType, &text_entry, &input)) {
			return nullptr;
		}

		Instance* _text_entry = as_instance(text_entry);
		if (_text_entry == nullptr) {
			return nullptr;
		}

		std::string _input (PyUnicode_AsUTF8(input));

		return variant_to_pyobj(Variant(ui::text_entry_completor(_text_entry, _input)));
	}
	PyObject* ui_text_entry_handler(PyObject* self, PyObject* args) {
		PyObject* text_entry;
		PyObject* input;
		PyObject* event;

		if (!PyArg_ParseTuple(args, "O!UO!", &InstanceType, &text_entry, &input, &PyDict_Type, &event)) {
			return nullptr;
		}

		Instance* _text_entry = as_instance(text_entry);
		if (_text_entry == nullptr) {
			return nullptr;
		}

		std::string _input (PyUnicode_AsUTF8(input));

		PyObject* keysym = PyDict_GetItemString(event, "keysym");
		SDL_Keysym _keysym = {
			static_cast<SDL_Scancode>(PyLong_AsLong(PyDict_GetItemString(keysym, "scancode"))),
			static_cast<SDL_Keycode>(PyLong_AsLong(PyDict_GetItemString(keysym, "sym"))),
			static_cast<Uint16>(PyLong_AsLong(PyDict_GetItemString(keysym, "mod"))),
			0u
		};

		SDL_Event _event;
		_event.key = {
			static_cast<Uint32>(PyLong_AsLong(PyDict_GetItemString(event, "type"))),
			static_cast<Uint32>(PyLong_AsLong(PyDict_GetItemString(event, "timestamp"))),
			static_cast<Uint32>(PyLong_AsLong(PyDict_GetItemString(event, "windowID"))),
			static_cast<Uint8>(PyLong_AsLong(PyDict_GetItemString(event, "state"))),
			static_cast<Uint8>(PyLong_AsLong(PyDict_GetItemString(event, "repeat"))),
			0u,
			0u,
			_keysym
		};

		return PyLong_FromLong(ui::text_entry_handler(_text_entry, _input, &_event));
	}

	PyObject* ui_create_gauge(PyObject* self, PyObject* args) {
		int x, y;
		int w, h;
		int range;

		if (!PyArg_ParseTuple(args, "iiiii", &x, &y, &w, &h, &range)) {
			return nullptr;
		}

		return Instance_from(ui::create_gauge(x, y, w, h, range));
	}

	PyObject* ui_create_slider(PyObject* self, PyObject* args) {
		int x, y;
		int w, h;
		int range, value;
		int is_continuous_callback;
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "iiiiiipO", &x, &y, &w, &h, &range, &value, &is_continuous_callback, &callback)) {
			return nullptr;
		}

		bool _is_continuous_callback = is_continuous_callback;

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		return Instance_from(ui::create_slider(x, y, w, h, range, value, _is_continuous_callback, [callback] (Instance* slider, int _value) {
			PyObject* arg_tup = Py_BuildValue("(Ni)", Instance_from(slider), _value);
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}
			Py_DECREF(arg_tup);
		}));
	}
	PyObject* ui_slider_callback(PyObject* self, PyObject* args) {
		PyObject* slider;
		int value;

		if (!PyArg_ParseTuple(args, "O!i", &InstanceType, &slider, &value)) {
			return nullptr;
		}

		Instance* _slider = as_instance(slider);
		if (_slider == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(ui::slider_callback(_slider, value));
	}

	PyObject* ui_create_optionbox(PyObject* self, PyObject* args) {
		int x, y;
		int w, h;

		if (!PyArg_ParseTuple(args, "iiii", &x, &y, &w, &h)) {
			return nullptr;
		}

		return Instance_from(ui::create_optionbox(x, y, w, h));
	}
	PyObject* ui_push_optionbox_option(PyObject* self, PyObject* args) {
		PyObject* optionbox;
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "O!O", &InstanceType, &optionbox, &callback)) {
			return nullptr;
		}

		Instance* _optionbox = as_instance(optionbox);
		if (_optionbox == nullptr) {
			return nullptr;
		}

		if (!PyCallable_Check(callback)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Py_INCREF(callback);
		return PyLong_FromLong(ui::push_optionbox_option(_optionbox, [callback] (Instance* __optionbox, bool state) {
			PyObject* arg_tup = Py_BuildValue("(NN)", Instance_from(__optionbox), PyBool_FromLong(state));
			if (PyEval_CallObject(callback, arg_tup) == nullptr) {
				PyErr_Print();
			}
			Py_DECREF(arg_tup);
		}));
	}
	PyObject* ui_pop_optionbox_option(PyObject* self, PyObject* args) {
		PyObject* optionbox;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &optionbox)) {
			return nullptr;
		}

		Instance* _optionbox = as_instance(optionbox);
		if (_optionbox == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(ui::pop_optionbox_option(_optionbox));
	}
	PyObject* ui_reset_optionbox_options(PyObject* self, PyObject* args) {
		PyObject* optionbox;

		if (!PyArg_ParseTuple(args, "O!", &InstanceType, &optionbox)) {
			return nullptr;
		}

		Instance* _optionbox = as_instance(optionbox);
		if (_optionbox == nullptr) {
			return nullptr;
		}

		return PyLong_FromLong(ui::reset_optionbox_options(_optionbox));
	}
	PyObject* ui_optionbox_callback(PyObject* self, PyObject* args) {
		PyObject* optionbox;
		int option_index;
		int state;

		if (!PyArg_ParseTuple(args, "O!ip", &InstanceType, &optionbox, &option_index, &state)) {
			return nullptr;
		}

		Instance* _optionbox = as_instance(optionbox);
		if (_optionbox == nullptr) {
			return nullptr;
		}

		size_t _option_index = option_index;

		bool _state = state;

		return PyLong_FromLong(ui::optionbox_callback(_optionbox, _option_index, _state));
	}
}}}

#endif // BEE_PYTHON_UI
