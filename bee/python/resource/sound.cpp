/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_SOUND
#define BEE_PYTHON_SOUND 1

#include <Python.h>
#include <structmember.h>

#include "sound.hpp"

#include "../python.hpp"

#include "../../resource/sound.hpp"

#include "../../util/string.hpp"

namespace bee { namespace python {
	PyObject* Sound_from(const Sound* sound) {
		PyObject* py_sound = internal::Sound_new(&internal::SoundType, nullptr, nullptr);
		internal::SoundObject* _py_sound = reinterpret_cast<internal::SoundObject*>(py_sound);

		if (Sound_init(_py_sound, Py_BuildValue("(N)", PyUnicode_FromString(sound->get_name().c_str())), nullptr)) {
			return nullptr;
		}

		return py_sound;
	}
	bool Sound_check(PyObject* obj) {
		return PyObject_TypeCheck(obj, &internal::SoundType);
	}
namespace internal {
	PyMethodDef SoundMethods[] = {
		{"print", reinterpret_cast<PyCFunction>(Sound_print), METH_NOARGS, "Print all relevant information about the Sound"},

		{"get_volume", reinterpret_cast<PyCFunction>(Sound_get_volume), METH_NOARGS, "Return the relative volume"},
		{"get_pan", reinterpret_cast<PyCFunction>(Sound_get_pan), METH_NOARGS, "Return the panning"},
		{"get_is_loaded", reinterpret_cast<PyCFunction>(Sound_get_is_loaded), METH_NOARGS, "Return whether the Sound is loaded or not"},
		{"get_is_music", reinterpret_cast<PyCFunction>(Sound_get_is_music), METH_NOARGS, "Return whether the Sound was loaded as music or chunk data"},
		{"get_is_playing", reinterpret_cast<PyCFunction>(Sound_get_is_playing), METH_NOARGS, "Return whether the Sound is playing or not"},
		{"get_is_looping", reinterpret_cast<PyCFunction>(Sound_get_is_looping), METH_NOARGS, "Return whether the Sound is looping or not"},

		{"set_volume", reinterpret_cast<PyCFunction>(Sound_set_volume), METH_VARARGS, "Set the relative volume"},
		{"set_pan", reinterpret_cast<PyCFunction>(Sound_set_pan), METH_VARARGS, "Set the panning"},
		{"set_is_music", reinterpret_cast<PyCFunction>(Sound_set_is_music), METH_VARARGS, "Set whether to treat the Sound as music or chunk data"},
		{"set_finish", reinterpret_cast<PyCFunction>(Sound_set_finish), METH_VARARGS, "Set the finish callback"},

		{"load", reinterpret_cast<PyCFunction>(Sound_load), METH_NOARGS, "Load the Sound from its path"},
		{"free", reinterpret_cast<PyCFunction>(Sound_free), METH_NOARGS, "Free the Sound and delete all of its effect buffers"},

		{"play", reinterpret_cast<PyCFunction>(Sound_play), METH_VARARGS, "Play the sound with optional looping and fading"},
		{"play_once", reinterpret_cast<PyCFunction>(Sound_play_once), METH_NOARGS, "Play the sound but only if it is not already playing"},
		{"stop", reinterpret_cast<PyCFunction>(Sound_stop), METH_VARARGS, "Stop playing all instances of the sound on every channel with optional fading"},
		{"rewind", reinterpret_cast<PyCFunction>(Sound_rewind), METH_NOARGS, "Play the sound from the beginning on every current channel"},
		{"pause", reinterpret_cast<PyCFunction>(Sound_pause), METH_NOARGS, "Pause the sound on every channel"},
		{"resume", reinterpret_cast<PyCFunction>(Sound_resume), METH_NOARGS, "Resume all paused instances of the sound"},
		{"toggle", reinterpret_cast<PyCFunction>(Sound_toggle), METH_NOARGS, "Toggle the sound between playing and paused"},
		{"loop", reinterpret_cast<PyCFunction>(Sound_loop), METH_NOARGS, "Play the sound in a continuous loop"},

		{"effect_add", reinterpret_cast<PyCFunction>(Sound_effect_add), METH_VARARGS, "Add a SoundEffect and apply it to any playing channels"},
		{"effect_remove", reinterpret_cast<PyCFunction>(Sound_effect_remove), METH_VARARGS, "Remove a SoundEffect"},
		{"effect_remove_all", reinterpret_cast<PyCFunction>(Sound_effect_remove_all), METH_NOARGS, "Remove all SoundEffects"},

		{nullptr, nullptr, 0, nullptr}
	};

	PyMemberDef SoundMembers[] = {
		{"name", T_OBJECT_EX, offsetof(SoundObject, name), 0, "The Sound name"},
		{nullptr, 0, 0, 0, nullptr}
	};

	PyTypeObject SoundType = {
		PyVarObject_HEAD_INIT(NULL, 0)
		"bee.Sound",
		sizeof(SoundObject), 0,
		reinterpret_cast<destructor>(Sound_dealloc),
		0,
		0, 0,
		0,
		reinterpret_cast<reprfunc>(Sound_repr),
		0, 0, 0,
		0,
		0,
		reinterpret_cast<reprfunc>(Sound_str),
		0, 0,
		0,
		Py_TPFLAGS_DEFAULT,
		"Sound objects",
		0,
		0,
		0,
		0,
		0, 0,
		SoundMethods,
		SoundMembers,
		0,
		0,
		0,
		0, 0,
		0,
		reinterpret_cast<initproc>(Sound_init),
		0, Sound_new,
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

	PyObject* PyInit_bee_sound(PyObject* module) {
		SoundType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&SoundType) < 0) {
			return nullptr;
		}

		Py_INCREF(&SoundType);
		PyModule_AddObject(module, "Sound", reinterpret_cast<PyObject*>(&SoundType));

		return reinterpret_cast<PyObject*>(&SoundType);
	}

	Sound* as_sound(SoundObject* self) {
		if (self->name == nullptr) {
			PyErr_SetString(PyExc_AttributeError, "name");
			return nullptr;
		}
		std::string _name (PyUnicode_AsUTF8(self->name));

		return Sound::get_by_name(_name);
	}
	Sound* as_sound(PyObject* self) {
		if (Sound_check(self)) {
			return as_sound(reinterpret_cast<SoundObject*>(self));
		}
		return nullptr;
	}

	void Sound_dealloc(SoundObject* self) {
		Py_XDECREF(self->name);
		Py_TYPE(self)->tp_free(self);
	}
	PyObject* Sound_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
		SoundObject* self;

		self = reinterpret_cast<SoundObject*>(type->tp_alloc(type, 0));
		if (self != nullptr) {
			self->name = PyUnicode_FromString("");
			if (self->name == nullptr) {
				Py_DECREF(self);
				return nullptr;
			}
		}

		return reinterpret_cast<PyObject*>(self);
	}
	int Sound_init(SoundObject* self, PyObject* args, PyObject* kwds) {
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

	PyObject* Sound_repr(SoundObject* self) {
		std::string s = std::string("bee.Sound(\"") + PyUnicode_AsUTF8(self->name) + "\")";
		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Sound_str(SoundObject* self) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return PyUnicode_FromString("Invalid Sound name");
		}

		Variant m (snd->serialize());
		std::string s = "Sound " + m.to_str(true);

		return PyUnicode_FromString(s.c_str());
	}
	PyObject* Sound_print(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		snd->print();

		Py_RETURN_NONE;
	}

	PyObject* Sound_get_volume(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", snd->get_volume());
	}
	PyObject* Sound_get_pan(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("d", snd->get_pan());
	}
	PyObject* Sound_get_is_loaded(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(snd->get_is_loaded());
	}
	PyObject* Sound_get_is_music(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(snd->get_is_music());
	}
	PyObject* Sound_get_is_playing(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(snd->get_is_playing());
	}
	PyObject* Sound_get_is_looping(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return PyBool_FromLong(snd->get_is_looping());
	}

	PyObject* Sound_set_volume(SoundObject* self, PyObject* args) {
		double volume;

		if (!PyArg_ParseTuple(args, "d", &volume)) {
			return nullptr;
		}

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		snd->set_volume(volume);

		Py_RETURN_NONE;
	}
	PyObject* Sound_set_pan(SoundObject* self, PyObject* args) {
		double pan;

		if (!PyArg_ParseTuple(args, "d", &pan)) {
			return nullptr;
		}

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->set_pan(pan));
	}
	PyObject* Sound_set_is_music(SoundObject* self, PyObject* args) {
		int is_music;

		if (!PyArg_ParseTuple(args, "p", &is_music)) {
			return nullptr;
		}

		bool _is_music = is_music;

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->set_is_music(_is_music));
	}
	PyObject* Sound_set_finish(SoundObject* self, PyObject* args) {
		PyObject* callback;

		if (!PyArg_ParseTuple(args, "O", &callback)) {
			return nullptr;
		}

		if ((!PyCallable_Check(callback))&&(callback != Py_None)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return nullptr;
		}

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		if (callback != Py_None) {
			Py_INCREF(callback);
			Py_INCREF(self);
			snd->set_finish([callback, self] (Sound* s) {
				PyObject* arg_tup = Py_BuildValue("(N)", self);
				if (PyEval_CallObject(callback, arg_tup) == nullptr) {
					PyErr_Print();
				}

				Py_DECREF(arg_tup);
			});
		} else {
			snd->set_finish(nullptr);
		}

		Py_RETURN_NONE;
	}

	PyObject* Sound_load(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->load());
	}
	PyObject* Sound_free(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->free());
	}

	PyObject* Sound_play(SoundObject* self, PyObject* args) {
		int loop_amount = 0;
		int fade_in = 0;

		if (!PyArg_ParseTuple(args, "|ii", &loop_amount, &fade_in)) {
			return nullptr;
		}

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->play(loop_amount, fade_in));
	}
	PyObject* Sound_play_once(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->play_once());
	}
	PyObject* Sound_stop(SoundObject* self, PyObject* args) {
		int fade_out = 0;

		if (!PyArg_ParseTuple(args, "|i", &fade_out)) {
			return nullptr;
		}

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->stop(fade_out));
	}
	PyObject* Sound_rewind(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->rewind());
	}
	PyObject* Sound_pause(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->pause());
	}
	PyObject* Sound_resume(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->resume());
	}
	PyObject* Sound_toggle(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->toggle());
	}
	PyObject* Sound_loop(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->loop());
	}

	PyObject* Sound_effect_add(SoundObject* self, PyObject* args) {
		PyObject* name;
		PyObject* type_name;
		PyObject* params;

		if (!PyArg_ParseTuple(args, "UUO!", &name, &type_name, &PyDict_Type, &params)) {
			return nullptr;
		}

		name = PyTuple_GetItem(args, 0);
		std::string _name (PyUnicode_AsUTF8(name));

		std::string _type_name (PyUnicode_AsUTF8(type_name));
		_type_name = util::string::lower(_type_name);

		E_SOUNDEFFECT type;
		if (_type_name == "chorus") {
			type = E_SOUNDEFFECT::CHORUS;
		} else if (_type_name == "echo") {
			type = E_SOUNDEFFECT::ECHO;
		} else if (_type_name == "flanger") {
			type = E_SOUNDEFFECT::FLANGER;
		} else if (_type_name == "gargle") {
			type = E_SOUNDEFFECT::GARGLE;
		} else if (_type_name == "reverb") {
			type = E_SOUNDEFFECT::REVERB;
		} else if (_type_name == "compressor") {
			type = E_SOUNDEFFECT::COMPRESSOR;
		} else if (_type_name == "equalizer") {
			type = E_SOUNDEFFECT::EQUALIZER;
		} else {
			PyErr_SetString(PyExc_TypeError, "parameter must be valid effect type");
			return nullptr;
		}

		Variant _params (pyobj_to_variant(params));

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->effect_add(SoundEffect(_name, type, _params.m)));
	}
	PyObject* Sound_effect_remove(SoundObject* self, PyObject* args) {
		PyObject* name;

		if (!PyArg_ParseTuple(args, "U", &name)) {
			return nullptr;
		}

		std::string _name (PyUnicode_AsUTF8(name));

		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->effect_remove(_name));
	}
	PyObject* Sound_effect_remove_all(SoundObject* self, PyObject* args) {
		Sound* snd = as_sound(self);
		if (snd == nullptr) {
			return nullptr;
		}

		return Py_BuildValue("i", snd->effect_remove_all());
	}
}}}

#endif // BEE_PYTHON_SOUND
