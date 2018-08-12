/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_LOADER
#define BEE_PYTHON_LOADER 1

#include "loader.hpp"

#include "../core/loader.hpp"

#include "python.hpp"

#include "resource/texture.hpp"
#include "resource/sound.hpp"
#include "resource/font.hpp"
#include "resource/path.hpp"
#include "resource/timeline.hpp"
#include "resource/mesh.hpp"
#include "resource/light.hpp"
#include "resource/script.hpp"
#include "resource/object.hpp"
#include "resource/room.hpp"

#include "../resource/texture.hpp"
#include "../resource/sound.hpp"
#include "../resource/font.hpp"
#include "../resource/path.hpp"
#include "../resource/timeline.hpp"
#include "../resource/mesh.hpp"
#include "../resource/light.hpp"
#include "../resource/script.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

namespace bee { namespace python { namespace internal {
	PyMethodDef BEELoaderMethods[] = {
		{"queue", loader_queue, METH_VARARGS, "Add the given Resource to the loader queue"},
		{"clear", loader_clear, METH_NOARGS, "Clear the queue"},

		{"load", loader_load, METH_NOARGS, "Immediately load the entire queue"},
		{"load_lazy", loader_load_lazy, METH_VARARGS, "Load the given number of Resources per frame from the queue"},

		{"get_amount_loaded", loader_get_amount_loaded, METH_NOARGS, "Return the amount of Resources that have been loaded"},
		{"get_total", loader_get_total, METH_NOARGS, "Return the total amount of queued Resources"},

		{nullptr, nullptr, 0, nullptr}
	};
	PyModuleDef BEELoaderModule = {
		PyModuleDef_HEAD_INIT, "loader", nullptr, -1, BEELoaderMethods,
		nullptr, nullptr, nullptr, nullptr
	};

	PyObject* PyInit_bee_loader() {
		return PyModule_Create(&BEELoaderModule);
	}

	PyObject* loader_queue(PyObject* self, PyObject* args) {
		PyObject* res;
		PyObject* loadfunc = nullptr;

		if (!PyArg_ParseTuple(args, "O|O", &res, &loadfunc)) {
			return nullptr;
		}

		Resource* _res = nullptr;
		if (Texture_check(res)) {
			_res = as_texture(res);
		} else if (Sound_check(res)) {
			_res = as_sound(res);
		} else if (Font_check(res)) {
			_res = as_font(res);
		} else if (Path_check(res)) {
			_res = as_path(res);
		} else if (Timeline_check(res)) {
			_res = as_timeline(res);
		} else if (Mesh_check(res)) {
			_res = as_mesh(res);
		} else if (Light_check(res)) {
			_res = as_light(res);
		} else if (Script_check(res)) {
			_res = as_script(res);
		} else if (Object_check(res)) {
			_res = as_object(res);
		} else if (Room_check(res)) {
			_res = as_room(res);
		}

		if (_res == nullptr) {
			PyErr_SetString(PyExc_ValueError, "the provided argument is not a valid Resource");
			return nullptr;
		}

		std::function<int (Resource*)> _loadfunc = nullptr;
		if ((loadfunc != nullptr)&&(loadfunc != Py_None)) {
			if (!PyCallable_Check(loadfunc)) {
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				return nullptr;
			}

			Py_INCREF(loadfunc);
			_loadfunc = [loadfunc] (Resource* r) -> int {
				PyObject* _r = nullptr;
				if (dynamic_cast<Texture*>(r) != nullptr) {
					_r = Texture_from(static_cast<Texture*>(r));
				} else if (dynamic_cast<Sound*>(r) != nullptr) {
					_r = Sound_from(static_cast<Sound*>(r));
				} else if (dynamic_cast<Font*>(r) != nullptr) {
					_r = Font_from(static_cast<Font*>(r));
				} else if (dynamic_cast<Path*>(r) != nullptr) {
					_r = Path_from(static_cast<Path*>(r));
				} else if (dynamic_cast<Timeline*>(r) != nullptr) {
					_r = Timeline_from(static_cast<Timeline*>(r));
				} else if (dynamic_cast<Mesh*>(r) != nullptr) {
					_r = Mesh_from(static_cast<Mesh*>(r));
				} else if (dynamic_cast<Light*>(r) != nullptr) {
					_r = Light_from(static_cast<Light*>(r));
				} else if (dynamic_cast<Script*>(r) != nullptr) {
					_r = Script_from(static_cast<Script*>(r));
				} else if (dynamic_cast<Object*>(r) != nullptr) {
					_r = Object_from(static_cast<Object*>(r));
				} else if (dynamic_cast<Room*>(r) != nullptr) {
					_r = Room_from(static_cast<Room*>(r));
				} else {
					messenger::send({"engine", "python", "loader"}, E_MESSAGE::ERROR, "Failed to load Resource \"" + r->get_name() + "\": could not cast to derived type");
					return -1;
				}

				PyObject* arg_tup = Py_BuildValue("(N)", _r);
				PyObject* ret = PyEval_CallObject(loadfunc, arg_tup);
				if (ret == nullptr) {
					Py_DECREF(arg_tup);
					PyErr_Print();
					return -2;
				}

				Py_DECREF(arg_tup);

				return pyobj_to_variant(ret).i;
			};
		}

		if (_loadfunc == nullptr) {
			loader::queue(_res);
		} else {
			loader::queue(_res, _loadfunc);
		}

		Py_RETURN_NONE;
	}
	PyObject* loader_clear(PyObject* self, PyObject* args) {
		loader::clear();

		Py_RETURN_NONE;
	}

	PyObject* loader_load(PyObject* self, PyObject* args) {
		loader::load();

		Py_RETURN_NONE;
	}
	PyObject* loader_load_lazy(PyObject* self, PyObject* args) {
		Py_ssize_t amount = 1;

		if (!PyArg_ParseTuple(args, "n", &amount)) {
			return nullptr;
		}

		size_t _amount = amount;

		loader::load_lazy(_amount);

		Py_RETURN_NONE;
	}

	PyObject* loader_get_amount_loaded(PyObject* self, PyObject* args) {
		return Py_BuildValue("n", loader::get_amount_loaded());
	}
	PyObject* loader_get_total(PyObject* self, PyObject* args) {
		return Py_BuildValue("n", loader::get_total());
	}
}}}

#endif // BEE_PYTHON_LOADER
