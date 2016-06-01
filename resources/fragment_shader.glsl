/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

// Version is included before OpenGL initialization

in vec2 f_texcoord;

out vec4 LFragment;

uniform sampler2D LTexture;
uniform vec4 colorize = vec4(1.0, 1.0, 1.0, 1.0);
uniform int is_primitive = 0;
uniform int flip = 0;
uniform vec4 port;
uniform int is_lightable = 1;

// Lighting
const int BEE_LIGHT_AMBIENT = 1;
const int BEE_LIGHT_DIFFUSE = 2;
const int BEE_LIGHT_POINT = 3;
const int BEE_LIGHT_SPOT = 4;

const int BEE_MAX_LIGHTS = 8;

uniform int light_amount = 0;
struct Light {
	int type;
	vec4 position;
	vec4 direction;
	vec4 attenuation;
	vec4 color;
};
uniform Light lighting[BEE_MAX_LIGHTS];

vec4 calc_light_ambient(Light l) {
	return l.color*l.color.w;
}
vec4 calc_light_diffuse(Light l) {
	return calc_light_ambient(l);
}
vec4 calc_light_point(Light l) {
	vec4 dir = vec4(gl_FragCoord.x, -gl_FragCoord.y + port.w, 0.0, 1.0) - l.position; // Note that port.w is actually the port height
	float d = length(dir.xy);

	float attenuation = 1.0;
	if (l.attenuation.xyz != vec3(0.0, 0.0, 0.0)) {
		attenuation = 10000.0 / (l.attenuation.x + l.attenuation.y * d + l.attenuation.z * d*d);
	}

	return calc_light_ambient(l) * attenuation;
}
vec4 calc_light_spot(Light l) {
	return calc_light_ambient(l);
}

void main() {
	LFragment = vec4(0.0, 0.0, 0.0, 0.0);
	if (is_primitive == 1) {
		LFragment = colorize;
	} else {
		if (flip == 1) {
			LFragment = texture(LTexture, vec2(1.0-f_texcoord.x, f_texcoord.y));
		} else if (flip == 2) {
			LFragment = texture(LTexture, vec2(f_texcoord.x, 1.0-f_texcoord.y));
		} else if (flip == 3) {
			LFragment = texture(LTexture, 1.0-f_texcoord);
		} else {
			LFragment = texture(LTexture, f_texcoord);
		}

		LFragment *= colorize;

		// Lighting
		if (is_lightable > 0) {
			if (light_amount > 0) {
				vec4 f = vec4(0.0, 0.0, 0.0, 0.0);
				Light l;
				for (int i=0; i<light_amount; i++) {
					l = lighting[i];
					l.position = vec4(l.position.xy + port.xy, l.position.zw);
					switch (l.type) {
						case BEE_LIGHT_AMBIENT: {
							f += calc_light_ambient(l);
							break;
						}
						case BEE_LIGHT_DIFFUSE: {
							f += calc_light_diffuse(l);
							break;
						}
						case BEE_LIGHT_POINT: {
							f += calc_light_point(l);
							break;
						}
						case BEE_LIGHT_SPOT: {
							f += calc_light_spot(l);
							break;
						}
					}
				}
				LFragment *= vec4(f.xyz, LFragment.w);

				// Normalize(?) the fragment color
				if (LFragment.x > 1.0) {
					LFragment /= vec4(LFragment.xxx, 1.0);
				}
				if (LFragment.y > 1.0) {
					LFragment /= vec4(LFragment.yyy, 1.0);
				}
				if (LFragment.z > 1.0) {
					LFragment /= vec4(LFragment.zzz, 1.0);
				}
			}
		}
	}
}
