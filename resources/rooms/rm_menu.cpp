/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_RM_MENU
#define RES_RM_MENU 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "rm_menu.hpp"

RmMenu::RmMenu() : Room("rm_menu", "rm_menu.cpp") {}

void RmMenu::init() {
	Room::init();

	set_width(1920);
	set_height(1080);

	get_phys_world()->set_gravity(btVector3(0.0, 0.0, 0.0));
	get_phys_world()->set_scale(100.0);

	set_background_color({16, 16, 28, 255});
	//add_background(bk_vortex, true, false, 0, 0, false, false, 0, 0, false);

	bee::set_window_title(
		std::string(MACRO_TO_STR(GAME_NAME)) + " - v" +
		std::to_string(GAME_VERSION_MAJOR) + "." + std::to_string(GAME_VERSION_MINOR) + "." + std::to_string(GAME_VERSION_RELEASE)
	);

	bee::render::set_3d(true);

	add_instance(-1, obj_menu, 0, 0, 0);

	// Add vortex shader
	bee::ShaderProgram* sp = new bee::ShaderProgram();

	bee::Shader vs ("bee/render/shader/default.vertex.glsl", GL_VERTEX_SHADER);
	sp->add_shader(vs);

	sp->add_attrib("v_position", true);
	sp->add_attrib("v_texcoord", true);

	sp->add_uniform("projection", true);
	sp->add_uniform("view", true);
	sp->add_uniform("model", true);
	sp->add_uniform("port", true);

	bee::Shader gs ("bee/render/shader/default.geometry.glsl", GL_GEOMETRY_SHADER);
	sp->add_shader(gs);

	sp->add_uniform("rotation", true);

	bee::Shader fs ("resources/vortex.glsl", GL_FRAGMENT_SHADER);
	sp->add_shader(fs);

	sp->add_uniform("f_texture", true);
	sp->add_uniform("flip", true);
	sp->add_uniform("time", true);

	sp->link();

	bee::ViewPort* v = bee::get_current_room()->get_views().at(0);
	v->update_func = [] (bee::ViewPort* vp) {
		glUniform1i(vp->shader->get_location("time"), bee::get_ticks());
	};
	v->shader = sp;
}
void RmMenu::end() {
	delete bee::get_current_room()->get_views().at(0)->shader;
	bee::get_current_room()->get_views().at(0)->shader = nullptr;
}

#endif // RES_RM_MENU
