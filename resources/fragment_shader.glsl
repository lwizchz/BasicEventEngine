/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#version 330 core

in vec2 f_texcoord;
in vec2 f_should_draw;

out vec4 LFragment;

uniform sampler2D LTexture;
uniform vec4 colorize = vec4(1.0, 1.0, 1.0, 1.0);
uniform int is_primitive = 0;
uniform int flip = 0;
uniform vec4 viewport = vec4(0, 0, 1920, 1080);

void main() {
	LFragment = vec4(0.0, 0.0, 0.0, 0.0);
	if (f_should_draw.x == 1) {
		if (is_primitive == 1) {
			LFragment = colorize;
		} else {
			if (flip == 0) {
				LFragment = texture2D(LTexture, f_texcoord);
			} else if (flip == 1) {
				LFragment = texture2D(LTexture, vec2(1.0-f_texcoord.x, f_texcoord.y));
			} else if (flip == 2) {
				LFragment = texture2D(LTexture, vec2(f_texcoord.x, 1.0-f_texcoord.y));
			} else if (flip == 3) {
				LFragment = texture2D(LTexture, 1.0-f_texcoord);
			}

			LFragment *= colorize;
		}
	}
}
