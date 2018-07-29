/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

in ivec4 v_bone_indices;
in vec4 v_bone_weights;

out vec2 g_texcoord;
out vec4 g_position;

out ivec4 g_bone_indices;
out vec4 g_bone_weights;

uniform vec4 port;

void main() {
	gl_Position = vec4(v_position.xy + port.xy, v_position.z, 1.0);
	g_position = vec4(v_position, 1.0);

	g_texcoord = v_texcoord;
	g_bone_indices = v_bone_indices;
	g_bone_weights = v_bone_weights;
}
