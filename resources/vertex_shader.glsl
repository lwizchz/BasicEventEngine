/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

in vec2 LVertexPos2D;
in vec2 LTexCoord;

out mat4 g_model;
out vec2 g_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 port;

void main() {
	g_model = projection * view * model;
	gl_Position = vec4(LVertexPos2D.x+port.x, LVertexPos2D.y+port.y, 0.0, 1.0);

	g_texcoord = LTexCoord;
}
