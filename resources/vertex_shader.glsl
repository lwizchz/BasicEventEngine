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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 port;

out vec2 f_texcoord;
out vec2 should_draw;

bool is_on_screen(vec4 p, vec2 c) {
	if ((c.x >= p.x)&&(c.x <= p.x+p.z)) {
		if ((c.y >= p.y)&&(c.y <= p.y+p.w)) {
			return true;
		}
	}
	return false;
}

void main() {
	gl_Position = projection * view * model * vec4(LVertexPos2D.x+port.x, LVertexPos2D.y+port.y, 0.0, 1.0);

	f_texcoord = LTexCoord;

	should_draw = vec2(1, 0);
	if (!is_on_screen(port, vec2(LVertexPos2D.x+port.x, LVertexPos2D.y+port.y))) {
		should_draw = vec2(0, 0);
	}
}
