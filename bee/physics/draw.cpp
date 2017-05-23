/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PHYSICS_DRAW
#define _BEE_PHYSICS_DRAW 1

#include "draw.hpp"

BEE::PhysicsDraw::PhysicsDraw() {
	debug_mode = 0;
}
BEE::PhysicsDraw::~PhysicsDraw() {}
int BEE::PhysicsDraw::attach(BEE* new_game, PhysicsWorld* new_world) {
	attached_game = new_game;
	attached_world = new_world;
	return 0;
}

void BEE::PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color1, const btVector3& color2) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color1.x(), color1.y(), color1.z(), 255);
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
	}
}
void BEE::PhysicsDraw::drawLine(const btVector3& v1, const btVector3& v2, const btVector3& color) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color.x(), color.y(), color.z(), 255);
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
	}
}
void BEE::PhysicsDraw::drawSphere(const btVector3& center, const btVector3& radius, const btVector3& color) {
	if (attached_game != nullptr) {
		/*
		double s = attached_world->get_scale();
		RGBA c (color.x, color.y, color.z, 255)
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
		*/
	}
}
void BEE::PhysicsDraw::drawTriangle(const btVector3& v1, const btVector3& v2, const btVector3& v3, const btVector3& color, btScalar alpha) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color.x(), color.y(), color.z(), alpha);
		attached_game->draw_triangle(bt_to_glm3(v1*s), bt_to_glm3(v2*s), bt_to_glm3(v3*s), c, false);
	}
}
void BEE::PhysicsDraw::drawContactPoint(const btVector3& v1, const btVector3& normal, btScalar distance, int lifetime, const btVector3& color) {
	if (attached_game != nullptr) {
		double s = attached_world->get_scale();
		RGBA c (color.x(), color.y(), color.z(), 255);
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(distance*normal*s), c);
	}
}

void BEE::PhysicsDraw::reportErrorWarning(const char* str) {
	if (attached_game != nullptr) {
		attached_game->messenger_send({"engine", "physics", "draw"}, BEE_MESSAGE_WARNING, std::string(str));
	} else {
		std::cerr << "PHYS WARN: " << str << "\n";
	}
}

void BEE::PhysicsDraw::draw3dText(const btVector3& center, const char* str) {
	if (attached_game != nullptr) {
		/*
		double s = attached_world->get_scale();
		RGBA c (color.x, color.y, color.z, 255)
		attached_game->draw_line(bt_to_glm3(v1*s), bt_to_glm3(v2*s), c);
		*/
	}
}

void BEE::PhysicsDraw::setDebugMode(int new_debug_mode) {
	debug_mode = new_debug_mode;
}
int BEE::PhysicsDraw::getDebugMode() const {
	return debug_mode;
}

#endif // _BEE_PHYSICS_DRAW
