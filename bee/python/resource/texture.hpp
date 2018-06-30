/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_TEXTURE_H
#define BEE_PYTHON_TEXTURE_H 1

#include <string>

#include <Python.h>

namespace bee {
	class Texture;
namespace python { namespace internal {
	typedef struct {
		PyObject_HEAD
		PyObject* name;
	} TextureObject;

	extern PyTypeObject TextureType;

	PyObject* PyInit_bee_texture(PyObject*);

	Texture* as_texture(TextureObject*);

	void Texture_dealloc(TextureObject*);
	PyObject* Texture_new(PyTypeObject*, PyObject*, PyObject*);
	int Texture_init(TextureObject*, PyObject*, PyObject*);

	// Texture methods
	PyObject* Texture_repr(TextureObject*);
	PyObject* Texture_str(TextureObject*);
	PyObject* Texture_print(TextureObject*, PyObject*);

	PyObject* Texture_get_size(TextureObject*, PyObject*);
	PyObject* Texture_get_subimage_amount(TextureObject*, PyObject*);
	PyObject* Texture_get_subimage_width(TextureObject*, PyObject*);
	PyObject* Texture_get_speed(TextureObject*, PyObject*);
	PyObject* Texture_get_is_animated(TextureObject*, PyObject*);
	PyObject* Texture_get_origin(TextureObject*, PyObject*);
	PyObject* Texture_get_rotate(TextureObject*, PyObject*);
	PyObject* Texture_get_is_loaded(TextureObject*, PyObject*);

	PyObject* Texture_set_speed(TextureObject*, PyObject*);
	PyObject* Texture_set_origin(TextureObject*, PyObject*);
	PyObject* Texture_set_rotate(TextureObject*, PyObject*);
	PyObject* Texture_set_subimage_amount(TextureObject*, PyObject*);
	PyObject* Texture_crop_image(TextureObject*, PyObject*);

	PyObject* Texture_load(TextureObject*, PyObject*);
	PyObject* Texture_load_as_target(TextureObject*, PyObject*);
	PyObject* Texture_free(TextureObject*, PyObject*);

	PyObject* Texture_draw(TextureObject*, PyObject*);
	PyObject* Texture_set_as_target(TextureObject*, PyObject*);
}}}

#endif // BEE_PYTHON_TEXTURE_H
