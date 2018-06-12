/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

layout (triangles) in;

layout (triangle_strip, max_vertices = 3) out;
const int VERTICES_PER_PRIMITIVE = 3;

in vec4 g_position[3];
in vec2 g_texcoord[3];

in ivec4 g_bone_indices[3];
in vec4 g_bone_weights[3];

out vec4 f_position;
out vec2 f_texcoord;

const int BEE_MAX_BONES = 100;
const int BEE_BONES_PER_VERTEX = 4;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 rotation;

uniform int has_bones = 0;
uniform mat4 bone_transforms[BEE_MAX_BONES];

void main() {
	mat4 mr = model * rotation;
	mat4 pv = projection * view;
	for(int i=0; i<VERTICES_PER_PRIMITIVE; ++i) {
		mat4 bone_transform = mat4(1.0);
		if (has_bones == 1) {
			bone_transform = mat4(0.0);
			for (int e=0; e<BEE_BONES_PER_VERTEX; ++e) {
				bone_transform += bone_transforms[g_bone_indices[i][e]] * g_bone_weights[i][e];
			}
		}

		mat4 mrb = mr * bone_transform;
		gl_Position = pv * mrb * gl_in[i].gl_Position;
		f_position = mrb * g_position[i];
		f_texcoord = g_texcoord[i];

		EmitVertex();
	}

	EndPrimitive();
}
