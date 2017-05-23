/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec2 g_texcoord;
out vec4 g_position;

uniform vec4 port;

void main() {
	gl_Position = vec4(v_position.xy + port.xy, v_position.z, 1.0);
	//gl_Position = vec4(v_position, 1.0);

	g_position = vec4(v_position, 1.0);
	g_texcoord = v_texcoord;
}
