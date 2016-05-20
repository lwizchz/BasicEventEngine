/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#version 330 core
#extension GL_EXT_geometry_shader4 : enable

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

in mat4 g_model[3];
in vec2 g_texcoord[3];
in vec2 g_should_draw[3];

out vec2 f_texcoord;
out vec2 f_should_draw;

uniform mat4 rotation;

void main() {
	for(int i=0; i<gl_VerticesIn; i++) {
		gl_Position = g_model[i] * rotation * gl_PositionIn[i];
		f_texcoord = g_texcoord[i];
		f_should_draw = g_should_draw[i];

		EmitVertex();
	}
	EndPrimitive();
}
