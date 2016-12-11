/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

in vec4 f_position;
in vec2 f_texcoord;

out vec4 f_fragment;

uniform sampler2D f_texture;
uniform vec4 colorize = vec4(1.0, 1.0, 1.0, 1.0);
uniform int is_primitive = 0;

void main() {
	f_fragment = vec4(0.0, 0.0, 0.0, 0.0);
	if (is_primitive == 1) {
		f_fragment = colorize;
	} else {
		f_fragment = texture(f_texture, vec2(f_texcoord.x, f_texcoord.y));
		f_fragment *= colorize;
	}
}
