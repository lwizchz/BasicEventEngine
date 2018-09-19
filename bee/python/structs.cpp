/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PYTHON_STRUCTS
#define BEE_PYTHON_STRUCTS 1

#include "structs.hpp"

#include "../render/background.hpp"
#include "../render/viewport.hpp"

#include "../resource/texture.hpp"
#include "../resource/path.hpp"
#include "../resource/timeline.hpp"

#include "resource/path.hpp"
#include "resource/timeline.hpp"

namespace bee { namespace python {
	PyObject* Background_from(const Background& bg) {
		PyObject* b = PyDict_New();
		PyDict_SetItemString(b, "texture", PyUnicode_FromString(bg.texture->get_name().c_str()));
		PyDict_SetItemString(b, "is_visible", PyBool_FromLong(bg.is_visible));
		PyDict_SetItemString(b, "is_foreground", PyBool_FromLong(bg.is_foreground));
		PyDict_SetItemString(b, "transform", TextureTransform_from(bg.transform));

		return b;
	}
	PyObject* SDL_Rect_from(const SDL_Rect& r) {
		PyObject* _r = PyDict_New();
		PyDict_SetItemString(_r, "x", Py_BuildValue("i", r.x));
		PyDict_SetItemString(_r, "y", Py_BuildValue("i", r.y));
		PyDict_SetItemString(_r, "w", Py_BuildValue("i", r.w));
		PyDict_SetItemString(_r, "h", Py_BuildValue("i", r.h));

		return _r;
	}
	PyObject* ViewPort_from(const ViewPort& vp) {
		PyObject* _vp = PyDict_New();
		PyDict_SetItemString(_vp, "is_active", PyBool_FromLong(vp.is_active));
		PyDict_SetItemString(_vp, "view", SDL_Rect_from(vp.view));
		PyDict_SetItemString(_vp, "port", SDL_Rect_from(vp.port));

		return _vp;
	}

	PyObject* TextureTransform_from(const TextureTransform& tr) {
		PyObject* t = PyDict_New();
		PyDict_SetItemString(t, "x", Py_BuildValue("i", tr.x));
		PyDict_SetItemString(t, "y", Py_BuildValue("i", tr.y));
		PyDict_SetItemString(t, "is_horizontal_tile", PyBool_FromLong(tr.is_horizontal_tile));
		PyDict_SetItemString(t, "is_vertical_tile", PyBool_FromLong(tr.is_vertical_tile));
		PyDict_SetItemString(t, "horizontal_speed", Py_BuildValue("i", tr.horizontal_speed));
		PyDict_SetItemString(t, "vertical_speed", Py_BuildValue("i", tr.vertical_speed));
		PyDict_SetItemString(t, "is_stretched", PyBool_FromLong(tr.is_stretched));

		return t;
	}
	PyObject* PathFollower_from(const PathFollower& pf) {
		PyObject* _pf = PyDict_New();
		PyDict_SetItemString(_pf, "path", Path_from(pf.path));
		PyDict_SetItemString(_pf, "offset", Py_BuildValue("(fff)", pf.offset.x(), pf.offset.y(), pf.offset.z()));
		PyDict_SetItemString(_pf, "node", Py_BuildValue("I", pf.node));
		PyDict_SetItemString(_pf, "progress", Py_BuildValue("I", pf.progress));
		PyDict_SetItemString(_pf, "speed", Py_BuildValue("I", pf.speed));
		PyDict_SetItemString(_pf, "direction", PyBool_FromLong(pf.direction));
		PyDict_SetItemString(_pf, "is_curved", PyBool_FromLong(pf.is_curved));
		PyDict_SetItemString(_pf, "is_closed", PyBool_FromLong(pf.is_closed));
		PyDict_SetItemString(_pf, "is_pausable", PyBool_FromLong(pf.is_pausable));

		return _pf;
	}
	PyObject* TimelineIterator_from(const TimelineIterator& tlit) {
		PyObject* _tlit = PyDict_New();
		PyDict_SetItemString(_tlit, "timeline", Timeline_from(tlit.tl));
		PyDict_SetItemString(_tlit, "start_frame", Py_BuildValue("I", tlit.start_frame));
		PyDict_SetItemString(_tlit, "position_frame", Py_BuildValue("I", tlit.position_frame));
		PyDict_SetItemString(_tlit, "start_offset", Py_BuildValue("I", tlit.start_offset));
		PyDict_SetItemString(_tlit, "pause_offset", Py_BuildValue("I", tlit.pause_offset));
		PyDict_SetItemString(_tlit, "is_looping", PyBool_FromLong(tlit.is_looping));
		PyDict_SetItemString(_tlit, "is_pausable", PyBool_FromLong(tlit.is_pausable));

		return _tlit;
	}

	int as_background(PyDictObject* dict, Background* bg) {
		PyObject* obj = reinterpret_cast<PyObject*>(dict);

		PyObject* texture_name = PyDict_GetItemString(obj, "texture");
		if (texture_name == nullptr) {
			return 1;
		}
		std::string _texture_name (PyUnicode_AsUTF8(texture_name));

		PyObject* is_visible = PyDict_GetItemString(obj, "is_visible");
		if (is_visible == nullptr) {
			return 2;
		}

		PyObject* is_foreground = PyDict_GetItemString(obj, "is_foreground");
		if (is_foreground == nullptr) {
			return 3;
		}

		TextureTransform tr;
		if (as_texture_transform(reinterpret_cast<PyDictObject*>(PyDict_GetItemString(obj, "transform")), &tr)) {
			return 4;
		}

		*bg = Background(
			Texture::get_by_name(_texture_name),
			PyObject_IsTrue(is_visible),
			PyObject_IsTrue(is_foreground),
			tr
		);

		return 0;
	}
	int as_rect(PyDictObject* dict, SDL_Rect* rect) {
		PyObject* obj = reinterpret_cast<PyObject*>(dict);

		PyObject* x = PyDict_GetItemString(obj, "x");
		if (x == nullptr) {
			return 1;
		}
		PyObject* y = PyDict_GetItemString(obj, "y");
		if (y == nullptr) {
			return 2;
		}
		PyObject* w = PyDict_GetItemString(obj, "w");
		if (w == nullptr) {
			return 3;
		}
		PyObject* h = PyDict_GetItemString(obj, "h");
		if (h == nullptr) {
			return 4;
		}

		*rect = {
			static_cast<int>(PyLong_AsLong(x)),
			static_cast<int>(PyLong_AsLong(y)),
			static_cast<int>(PyLong_AsLong(w)),
			static_cast<int>(PyLong_AsLong(h))
		};

		return 0;
	}
	int as_viewport(PyDictObject* dict, ViewPort* vp) {
		PyObject* obj = reinterpret_cast<PyObject*>(dict);

		PyObject* is_active = PyDict_GetItemString(obj, "is_active");
		if (is_active == nullptr) {
			return 1;
		}

		SDL_Rect v, p;
		if (as_rect(reinterpret_cast<PyDictObject*>(PyDict_GetItemString(obj, "view")), &v)) {
			return 2;
		}
		if (as_rect(reinterpret_cast<PyDictObject*>(PyDict_GetItemString(obj, "port")), &p)) {
			return 3;
		}

		*vp = ViewPort(
			PyObject_IsTrue(is_active),
			v,
			p
		);

		return 0;
	}

	int as_texture_transform(PyDictObject* dict, TextureTransform* tr) {
		PyObject* obj = reinterpret_cast<PyObject*>(dict);

		PyObject* x = PyDict_GetItemString(obj, "x");
		if (x == nullptr) {
			return 1;
		}
		PyObject* y = PyDict_GetItemString(obj, "y");
		if (y == nullptr) {
			return 2;
		}

		PyObject* is_horizontal_tile = PyDict_GetItemString(obj, "is_horizontal_tile");
		if (is_horizontal_tile == nullptr) {
			return 3;
		}
		PyObject* is_vertical_tile = PyDict_GetItemString(obj, "is_vertical_tile");
		if (is_vertical_tile == nullptr) {
			return 4;
		}

		PyObject* horizontal_speed = PyDict_GetItemString(obj, "horizontal_speed");
		if (horizontal_speed == nullptr) {
			return 5;
		}
		PyObject* vertical_speed = PyDict_GetItemString(obj, "vertical_speed");
		if (vertical_speed == nullptr) {
			return 6;
		}

		PyObject* is_stretched = PyDict_GetItemString(obj, "is_stretched");
		if (is_stretched == nullptr) {
			return 7;
		}

		*tr = TextureTransform(
			PyLong_AsLong(x),
			PyLong_AsLong(y),
			PyObject_IsTrue(is_horizontal_tile),
			PyObject_IsTrue(is_vertical_tile),
			PyLong_AsLong(horizontal_speed),
			PyLong_AsLong(vertical_speed),
			PyObject_IsTrue(is_stretched)
		);

		return 0;
	}
	int as_path_follower(PyDictObject* dict, PathFollower* pf) {
		PyObject* obj = reinterpret_cast<PyObject*>(dict);

		PyObject* path = PyDict_GetItemString(obj, "path");
		if (path == nullptr) {
			return 1;
		}
		Path* _path = internal::as_path(path);

		PyObject* offset = PyDict_GetItemString(obj, "offset");
		if (offset == nullptr) {
			return 2;
		}
		btVector3 _offset (
			PyFloat_AsDouble(PyTuple_GetItem(offset, 0)),
			PyFloat_AsDouble(PyTuple_GetItem(offset, 1)),
			PyFloat_AsDouble(PyTuple_GetItem(offset, 2))
		);

		PyObject* speed = PyDict_GetItemString(obj, "speed");
		if (speed == nullptr) {
			return 3;
		}

		*pf = PathFollower(
			_path,
			_offset,
			static_cast<unsigned int>(PyLong_AsUnsignedLong(speed))
		);

		PyObject* node = PyDict_GetItemString(obj, "node");
		if (node != nullptr) {
			pf->node = static_cast<unsigned int>(PyLong_AsUnsignedLong(node));
		}
		PyObject* progress = PyDict_GetItemString(obj, "progress");
		if (progress != nullptr) {
			pf->progress = static_cast<unsigned int>(PyLong_AsUnsignedLong(progress));
		}


		PyObject* direction = PyDict_GetItemString(obj, "direction");
		if (direction != nullptr) {
			pf->direction = PyObject_IsTrue(direction);
		}
		PyObject* is_curved = PyDict_GetItemString(obj, "is_curved");
		if (is_curved != nullptr) {
			pf->is_curved = PyObject_IsTrue(is_curved);
		}
		PyObject* is_closed = PyDict_GetItemString(obj, "is_closed");
		if (is_closed != nullptr) {
			pf->is_closed = PyObject_IsTrue(is_closed);
		}
		PyObject* is_pausable = PyDict_GetItemString(obj, "is_pausable");
		if (is_pausable != nullptr) {
			pf->is_pausable = PyObject_IsTrue(is_pausable);
		}

		return 0;
	}
	int as_timeline_iterator(PyDictObject* dict, TimelineIterator* tlit) {
		PyObject* obj = reinterpret_cast<PyObject*>(dict);

		PyObject* timeline = PyDict_GetItemString(obj, "timeline");
		if (timeline == nullptr) {
			return 1;
		}
		Timeline* _timeline = internal::as_timeline(timeline);
		if (_timeline == nullptr) {
			return 2;
		}

		PyObject* start_offset = PyDict_GetItemString(obj, "start_offset");
		if (start_offset == nullptr) {
			return 3;
		}

		PyObject* is_looping = PyDict_GetItemString(obj, "is_looping");
		if (is_looping == nullptr) {
			return 4;
		}
		PyObject* is_pausable = PyDict_GetItemString(obj, "is_pausable");
		if (is_pausable == nullptr) {
			return 5;
		}

		*tlit = TimelineIterator(
			_timeline,
			static_cast<unsigned int>(PyLong_AsUnsignedLong(start_offset)),
			PyObject_IsTrue(is_looping),
			PyObject_IsTrue(is_pausable)
		);

		PyObject* start_frame = PyDict_GetItemString(obj, "start_frame");
		if (start_frame != nullptr) {
			tlit->start_frame = static_cast<unsigned int>(PyLong_AsUnsignedLong(start_frame));
		}
		PyObject* position_frame = PyDict_GetItemString(obj, "position_frame");
		if (position_frame != nullptr) {
			tlit->start_frame = static_cast<unsigned int>(PyLong_AsUnsignedLong(position_frame));
		}
		PyObject* pause_offset = PyDict_GetItemString(obj, "pause_offset");
		if (pause_offset != nullptr) {
			tlit->start_frame = static_cast<unsigned int>(PyLong_AsUnsignedLong(pause_offset));
		}

		return 0;
	}
}}

#endif // BEE_PYTHON_STRUCTS
