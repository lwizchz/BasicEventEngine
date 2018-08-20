/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_PHYSICS_DRAW
#define BEE_PHYSICS_DRAW 1

#include "draw.hpp"

#include "world.hpp"

#include "../engine.hpp"

#include "../util/real.hpp"

#include "../core/rooms.hpp"
#include "../core/enginestate.hpp"

#include "../messenger/messenger.hpp"

#include "../render/drawing.hpp"
#include "../render/rgba.hpp"

#include "../resource/font.hpp"
#include "../resource/room.hpp"

namespace bee { namespace internal {
	/**
	* Default construct the debug drawer.
	*/
	PhysicsDraw::PhysicsDraw() :
		debug_mode(btIDebugDraw::DBG_NoDebug)
	{}

	/**
	* Draw a line with the given colors.
	* @note The colors should be blended between the endpoints but currently color2 is just being ignored.
	* @param v1 the first endpoint
	* @param v2 the second endpoint
	* @param color1 the color near the first endpoint
	* @param color2 the color near the second endpoint
	*/
	void PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color1, const btVector3& color2) {
		if (auto world = get_current_room()->get_phys_world()) {
			btScalar s = btScalar(world->get_scale());
			RGBA c (
				static_cast<int>(color1.x()),
				static_cast<int>(color1.y()),
				static_cast<int>(color1.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), c);
		}
	}
	/**
	* Draw a line with the given color.
	* @param v1 the first endpoint
	* @param v2 the second endpoint
	* @param color the color to draw with
	*/
	void PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color) {
		if (auto world = get_current_room()->get_phys_world()) {
			btScalar s = btScalar(world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), c);
		}
	}
	/**
	* Draw a sphere with the given color.
	* @param center the center point of the sphere
	* @param radius a vector with the magnitude of the sphere radius
	* @param color the color to draw with
	*/
	void PhysicsDraw::drawSphere(const btVector3& center, const btVector3& radius, const btVector3& color) {
		if (auto world = get_current_room()->get_phys_world()) {
			btScalar s = btScalar(world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_circle(util::bt_to_glm_v3(center*s), (radius*s).length(), 1, c);
		}
	}
	/**
	* Draw a triangle with the given color.
	* @param v1 a point of the triangle
	* @param v2 a point of the triangle
	* @param v3 a point of the triangle
	* @param color the color to draw with
	* @param alpha the opacity to draw the color with
	*/
	void PhysicsDraw::drawTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3, const btVector3& color, btScalar alpha) {
		if (auto world = get_current_room()->get_phys_world()) {
			btScalar s = btScalar(world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				alpha
			);
			draw_triangle(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), util::bt_to_glm_v3(v3*s), c, false);
		}
	}
	/**
	* Draw a line at the given contact point with the given color.
	* @param v1 the contact point
	* @param normal the contact direction
	* @param distance the contact distance
	* @param lifetime the contact lifetime
	* @param color the color to draw with
	*/
	void PhysicsDraw::drawContactPoint(const btVector3& v1, const btVector3& normal, btScalar distance, int lifetime, const btVector3& color) {
		if (auto world = get_current_room()->get_phys_world()) {
			btScalar s = btScalar(world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(distance*normal*s), c);
		}
	}

	/**
	* Log the given warning message.
	* @param str the message to log
	*/
	void PhysicsDraw::reportErrorWarning(const char* str) {
		messenger::send({"engine", "physics", "draw"}, E_MESSAGE::WARNING, str);
	}

	/**
	* Draw the given text.
	* @param center the point at which to draw the text
	* @param str the string to draw
	*/
	void PhysicsDraw::draw3dText(const btVector3& center, const char* str) {
		if (auto world = get_current_room()->get_phys_world()) {
			btScalar s = btScalar(world->get_scale());
			RGBA c (0, 0, 0, 255);
			engine->font_default->draw_fast(center.x()*s, center.y()*s, str, c);
		}
	}

	/**
	* Set the debug mode.
	* @param _debug_mode the new debug mode
	* @see http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html for details of DebugDrawModes
	*/
	void PhysicsDraw::setDebugMode(int _debug_mode) {
		debug_mode = _debug_mode;
	}
	/**
	* @returns the current debug mode
	* @see http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html for details of DebugDrawModes
	*/
	int PhysicsDraw::getDebugMode() const {
		return debug_mode;
	}
}}

#endif // BEE_PHYSICS_DRAW
