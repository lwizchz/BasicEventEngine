/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
uniform int time;

void main() {
	if (f_fragment == vec4(16.0/255.0, 16.0/255.0, 28.0/255.0, 1.0)) {
		vec2 pos = vec2(f_position.x, f_position.y);
		vec2 posp = vec2(f_position.x/1920.0, f_position.y/1080.0);

		//f_fragment = vec4(f_position.x/1920.0, f_position.y/1080.0, time/10000.0, 1.0);

		//f_fragment = vec4(0.2 * pos.y, -0.4 * pos.y, 0.1, 1.0);
		//f_fragment = vec4(0.2 * pos.y * time/1000.0, -0.4 * pos.y * time/1000.0, 0.1, 1.0);

		//f_fragment = vec4(0.2 * pos.y * (sin(time/1000.0)+1.0), -0.4 * pos.y * cos(time/1000.0), 0.1, 1.0);

		//if (mod(f_position.x-time/50.0, 5.0) <= 2.0) {
		//if (sin(f_position.x/5.0 * sqrt(time)) + cos(f_position.y/5.0) <= 0.0) {
		//if (sin(time/f_position.x)/2.0 + cos(f_position.y/5.0 * time) <= 0.0) {
		//if (sin(f_position.x/5.0) + log(cos(f_position.y/5.0 * time)) <= 0.0) {
		//if (sin(f_position.x/5.0) + sqrt(cos(f_position.y/5.0 * time)) <= 0.0) {
		//if (floor(sqrt(pos.x) - sqrt(pos.y)) == floor(sqrt(pos.x) - sqrt(pos.y) + 0.3)) {
		//if (cos(floor(sqrt(pos.x) - sqrt(pos.y) + sqrt(time))) <= 0.0) {
		//if (cos(8.0*pos.y + time/1000.0) <= 0.0) {
		if (cos(8.0*pos.y + time/1000.0) <= 0.0) {
			f_fragment = vec4(0.2 * posp.y * (sin(time/1000.0)+1.0), 0.0, 0.1 - 0.4 * posp.x * cos(time/1000.0), 1.0);
			f_fragment += vec4(0.25);
		}
	}
}
