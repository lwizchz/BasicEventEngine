/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_STRUCTS_H
#define BEE_PYTHON_STRUCTS_H 1

#include <Python.h>

struct SDL_Rect;

namespace bee {
	struct Background;
	struct ViewPort;

	struct TextureTransform;
	struct PathFollower;
	struct TimelineIterator;
namespace python {
	PyObject* Background_from(const Background&);
	PyObject* SDL_Rect_from(const SDL_Rect&);
	PyObject* ViewPort_from(const ViewPort&);

	PyObject* TextureTransform_from(const TextureTransform&);
	PyObject* PathFollower_from(const PathFollower&);
	PyObject* TimelineIterator_from(const TimelineIterator&);

	int as_background(PyDictObject*, Background*);
	int as_rect(PyDictObject*, SDL_Rect*);
	int as_viewport(PyDictObject*, ViewPort*);

	int as_texture_transform(PyDictObject*, TextureTransform*);
	int as_path_follower(PyDictObject*, PathFollower*);
	int as_timeline_iterator(PyDictObject*, TimelineIterator*);
}}

#endif // BEE_PYTHON_STRUCTS_H
