/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#version 140
in vec2 LVertexPos2D;
in vec2 LTexCoord;
uniform mat4 model;
uniform mat4 mvp;
out vec2 f_texcoord;

void main() {
	gl_Position = mvp * model * vec4(LVertexPos2D.x, LVertexPos2D.y, 0.0, 1.0);
	f_texcoord = LTexCoord;
}
