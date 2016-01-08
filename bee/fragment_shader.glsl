/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#version 140
in vec2 f_texcoord;
uniform sampler2D LTexture;
out vec4 LFragment;

void main() {
	LFragment = texture2D(LTexture, f_texcoord);
}
