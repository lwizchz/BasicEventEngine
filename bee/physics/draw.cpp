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

#include "../messenger/messenger.hpp"

#include "../render/drawing.hpp"
#include "../render/rgba.hpp"

namespace bee {
	PhysicsDraw::PhysicsDraw(PhysicsWorld* _world) :
		debug_mode(0),

		attached_world(_world)
	{}

	void PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color1, const btVector3& color2) {
		if (attached_world != nullptr) {
			btScalar s = btScalar(attached_world->get_scale());
			RGBA c (
				static_cast<int>(color1.x()),
				static_cast<int>(color1.y()),
				static_cast<int>(color1.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), c);
		}
	}
	void PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color) {
		if (attached_world != nullptr) {
			btScalar s = btScalar(attached_world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), c);
		}
	}
	void PhysicsDraw::drawSphere(const btVector3& center, const btVector3& radius, const btVector3& color) {
		if (attached_world != nullptr) {
			/*
			btScalar s = btScalar(attached_world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), c);
			*/
		}
	}
	void PhysicsDraw::drawTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3, const btVector3& color, btScalar alpha) {
		if (attached_world != nullptr) {
			btScalar s = btScalar(attached_world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_triangle(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), util::bt_to_glm_v3(v3*s), c, false);
		}
	}
	void PhysicsDraw::drawContactPoint(const btVector3& v1, const btVector3& normal, btScalar distance, int lifetime, const btVector3& color) {
		if (attached_world != nullptr) {
			btScalar s = btScalar(attached_world->get_scale());
			RGBA c (
				static_cast<int>(color.x()),
				static_cast<int>(color.y()),
				static_cast<int>(color.z()),
				255
			);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(distance*normal*s), c);
		}
	}

	void PhysicsDraw::reportErrorWarning(const char* str) {
		messenger::send({"engine", "physics", "draw"}, E_MESSAGE::WARNING, str);
	}

	void PhysicsDraw::draw3dText(const btVector3& center, const char* str) {
		if (attached_world != nullptr) {
			/*
			btScalar s = btScalar(attached_world->get_scale());
			RGBA c (0, 0, 0, 255);
			draw_line(util::bt_to_glm_v3(v1*s), util::bt_to_glm_v3(v2*s), c);
			*/
		}
	}

	void PhysicsDraw::setDebugMode(int _debug_mode) {
		debug_mode = _debug_mode;
	}
	int PhysicsDraw::getDebugMode() const {
		return debug_mode;
	}
}

#endif // BEE_PHYSICS_DRAW
