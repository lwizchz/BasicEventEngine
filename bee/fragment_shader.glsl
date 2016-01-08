#version 140
in vec2 f_texcoord;
uniform sampler2D LTexture;
out vec4 LFragment;

void main() {
	LFragment = texture2D(LTexture, f_texcoord);
}
