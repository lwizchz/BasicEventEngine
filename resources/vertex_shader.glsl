/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#version 330 core

in vec2 LVertexPos2D;
in vec2 LTexCoord;

out mat4 g_model;
out vec2 g_texcoord;
out vec2 g_should_draw;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 port;

bool is_on_screen(vec4 p, vec2 c) {
	if ((c.x >= p.x)&&(c.x <= p.x+p.z)) {
		if ((c.y >= p.y)&&(c.y <= p.y+p.w)) {
			return true;
		}
	}
	return false;
}

void main() {
	g_model = projection * view * model;
	gl_Position = vec4(LVertexPos2D.x+port.x, LVertexPos2D.y+port.y, 0.0, 1.0);

	g_texcoord = LTexCoord;

	g_should_draw = vec2(1, 0);
	if (!is_on_screen(port, vec2(LVertexPos2D.x+port.x, LVertexPos2D.y+port.y))) {
		g_should_draw = vec2(0, 0);
	}
}
