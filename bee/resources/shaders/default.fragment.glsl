/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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
uniform int flip = 0;
uniform vec4 port;
uniform int is_lightable = 1;

struct Line {
	vec4 start;
	vec4 end;
};

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

// Shadows
const int BEE_MAX_LIGHTABLES = 96;
const int BEE_MAX_MASK_VERTICES = 8;

uniform int lightable_amount = 0;
struct Lightable {
	vec4 position;
	int vertex_amount;
	vec4 mask[BEE_MAX_MASK_VERTICES];
};
uniform Lightable lightables[BEE_MAX_LIGHTABLES];

float calc_attenuation_point(vec4 a, float d) {
	return 10000.0 / (a.x + a.y * d + a.z * d*d);
}
vec4 calc_light_ambient(Light l) {
	return l.color*l.color.w;
}
vec4 calc_light_diffuse(Light l) {
	return calc_light_ambient(l);
}
vec4 calc_light_point(Light l) {
	vec4 dir = f_position - l.position;
	float d = length(dir.xyz);

	float attenuation = 1.0;
	if (l.attenuation.xyz != vec3(0.0, 0.0, 0.0)) {
		attenuation = calc_attenuation_point(l.attenuation, d);
	}

	return calc_light_ambient(l) * attenuation;
}
vec4 calc_light_spot(Light l) {
	return calc_light_ambient(l);
}

float fast_length(vec4 a) {
	return a.x*a.x + a.y*a.y + a.z*a.z;
}
float distance_to_line(Line l, vec4 p) {
	vec4 dir = l.end - l.start;
	vec3 normal_dir = cross(dir.xyz, vec3(0.0, 0.0, 1.0));
	vec4 point_dir = l.start - p;
	return dot(normalize(normal_dir), point_dir.xyz);
}
bool check_is_inside(vec4 p, vec4 vertices[BEE_MAX_MASK_VERTICES]) {
	bool is_intersecting = false;
	int v = BEE_MAX_MASK_VERTICES;
	for (int i=0, e=v-1; i<v; e=i++) {
		if ((
			(vertices[i].y > p.y) != (vertices[e].y > p.y)
		)&&(
			p.x < (vertices[e].x - vertices[i].x) * (p.y - vertices[i].y) / (vertices[e].y - vertices[i].y) + vertices[i].x
		)) {
			is_intersecting = !is_intersecting;
		}
	}

	return is_intersecting;
}
float calc_shadow_point(Light l, Lightable s) {
	Line line = Line(l.position, f_position);

	int c = -1;
	float d = distance_to_line(line, s.position+s.mask[0]);
	for (int i=1; i<BEE_MAX_MASK_VERTICES; i++) {
		if (i >= s.vertex_amount) {
			break;
		}

		vec4 p = s.position+s.mask[i];
		if (fast_length(line.end-line.start) < fast_length(p-line.end)) {
			continue;
		}
		if (fast_length(line.end-line.start) < fast_length(line.start-p)) {
			return 0.0;
		}

		float dd = distance_to_line(line, p);
		if (dd * d < 0) { // If the sign changed then there is an intersection
			//c = i;
			//break;

			c = 1;
		}

		if (abs(dd) < abs(d)) {
			d = dd;
		}
	}

	/*if (c >= 0) {
		vec4 p = s.position+s.mask[c];
		return abs(distance_to_line(Line(line.start, p), line.end));
	}
	return 0.0;*/
	return abs(d)*c;
	//return 20.0*c;
}

void main() {
	f_fragment = vec4(0.0, 0.0, 0.0, 0.0);
	if (is_primitive == 1) {
		f_fragment = colorize;
	} else {
		if (flip == 1) {
			f_fragment = texture(f_texture, vec2(1.0-f_texcoord.x, f_texcoord.y));
		} else if (flip == 2) {
			f_fragment = texture(f_texture, vec2(f_texcoord.x, 1.0-f_texcoord.y));
		} else if (flip == 3) {
			f_fragment = texture(f_texture, 1.0-f_texcoord);
		} else {
			f_fragment = texture(f_texture, f_texcoord);
		}

		f_fragment *= colorize;

		// Lighting
		if (is_lightable > 0) {
			if (light_amount > 0) {
				vec4 f = vec4(0.0, 0.0, 0.0, 0.0);
				Light l;
				for (int i=0; i<light_amount; i++) {
					l = lighting[i];
					l.position = vec4(l.position.xy + port.xy, l.position.zw);
					float max_range_sqr = pow(4.0*50.0/l.attenuation.z, 2);

					vec4 ff = vec4(0.0, 0.0, 0.0, 0.0);
					switch (l.type) {
						case BEE_LIGHT_AMBIENT: {
							ff = calc_light_ambient(l);
							break;
						}
						case BEE_LIGHT_DIFFUSE: {
							ff = calc_light_diffuse(l);
							break;
						}
						case BEE_LIGHT_POINT: {
							/*if (fast_length(l.position - f_position) > max_range_sqr) {
								continue; // If the fragment is far outside of the range of the light, abort early
							}*/
							ff = calc_light_point(l);
							break;
						}
						case BEE_LIGHT_SPOT: {
							ff = calc_light_spot(l);
							break;
						}
					}

					if (ff != vec4(0.0, 0.0, 0.0, 0.0)) {
						switch (l.type) {
							case BEE_LIGHT_AMBIENT: {
								break;
							}
							case BEE_LIGHT_DIFFUSE: {
								break;
							}
							//case BEE_LIGHT_POINT:
							case 99: { // Temporarily disable shadows
								float shadow_amount = 0.0;
								for (int e=0; e<lightable_amount; e++) {
									/*if (fast_length(l.position - lightables[e].position) > max_range_sqr) {
										continue; // If the lightable is far outside the range of the light, abort early
									}*/

									float s = calc_shadow_point(l, lightables[e]);
									if (s > 0) {
										shadow_amount += s/50.0;
									}
								}

								float d = length(l.position - f_position) / 100.0;
								if (d < 1.0) {
									d = 1.0;
								}
								ff /= pow(d, shadow_amount);

								break;
							}
							case BEE_LIGHT_SPOT: {
								break;
							}
						}
						f += ff;
					}
				}
				f_fragment *= vec4(f.xyz, f_fragment.w);

				// Normalize the fragment color
				if (f_fragment.x > 1.0) {
					f_fragment /= vec4(f_fragment.xxx, 1.0);
				}
				if (f_fragment.y > 1.0) {
					f_fragment /= vec4(f_fragment.yyy, 1.0);
				}
				if (f_fragment.z > 1.0) {
					f_fragment /= vec4(f_fragment.zzz, 1.0);
				}
			}
		}
	}
}
