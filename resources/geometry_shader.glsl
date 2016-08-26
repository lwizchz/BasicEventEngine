/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;

in vec4 g_position[3];
in vec2 g_texcoord[3];

out vec4 f_position;
out vec2 f_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 rotation;

void main() {
	mat4 vmr = view * model * rotation;
	for(int i=0; i<3; i++) {
		gl_Position = projection * vmr * gl_in[i].gl_Position;
		f_position = vmr * g_position[i];
		f_texcoord = g_texcoord[i];

		EmitVertex();
	}

	EndPrimitive();
}
