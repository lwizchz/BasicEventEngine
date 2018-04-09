/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_SOUND_H
#define BEE_PYTHON_SOUND_H 1

#include <string>

#include <Python.h>

namespace bee { namespace python { namespace internal {
        typedef struct {
                PyObject_HEAD
                PyObject* name;
        } SoundObject;

        PyObject* PyInit_bee_sound(PyObject*);

        void Sound_dealloc(SoundObject*);
        PyObject* Sound_new(PyTypeObject*, PyObject*, PyObject*);
        int Sound_init(SoundObject*, PyObject*, PyObject*);

        // Sound methods
        PyObject* Sound_repr(SoundObject*, PyObject*);
        PyObject* Sound_str(SoundObject*, PyObject*);
        PyObject* Sound_print(SoundObject*, PyObject*);

        PyObject* Sound_get_volume(SoundObject*, PyObject*);
        PyObject* Sound_get_pan(SoundObject*, PyObject*);
        PyObject* Sound_get_is_loaded(SoundObject*, PyObject*);
        PyObject* Sound_get_is_music(SoundObject*, PyObject*);
        PyObject* Sound_get_is_playing(SoundObject*, PyObject*);
        PyObject* Sound_get_is_looping(SoundObject*, PyObject*);

        PyObject* Sound_set_volume(SoundObject*, PyObject*);
        PyObject* Sound_set_pan(SoundObject*, PyObject*);
        PyObject* Sound_set_is_music(SoundObject*, PyObject*);
        PyObject* Sound_set_finish(SoundObject*, PyObject*);

        PyObject* Sound_load(SoundObject*, PyObject*);
        PyObject* Sound_free(SoundObject*, PyObject*);

        PyObject* Sound_play(SoundObject*, PyObject*);
        PyObject* Sound_play_once(SoundObject*, PyObject*);
        PyObject* Sound_stop(SoundObject*, PyObject*);
        PyObject* Sound_rewind(SoundObject*, PyObject*);
        PyObject* Sound_pause(SoundObject*, PyObject*);
        PyObject* Sound_resume(SoundObject*, PyObject*);
        PyObject* Sound_toggle(SoundObject*, PyObject*);
        PyObject* Sound_loop(SoundObject*, PyObject*);

        PyObject* Sound_effect_add(SoundObject*, PyObject*);
        PyObject* Sound_effect_remove(SoundObject*, PyObject*);
        PyObject* Sound_effect_remove_all(SoundObject*, PyObject*);
}}}

#endif // BEE_PYTHON_SOUND_H
