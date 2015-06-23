#include "resource_structures.hpp"
#include "../resources/resources.hpp"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

#include "../include/ogldev_math_3d.h"
#include "util.hpp"

GLuint VBO;
GLuint IBO;
GLuint gWorldLocation;
const char* vertex_shader_path = "ege/shader.vs";
const char* fragment_shader_path = "ege/shader.fs";

void idle() {
	static float Scale = 0.0;
	Scale += 0.01;
	
	Matrix4f World;
	World.m[0][0] = cosf(Scale);	World.m[0][1] = 0.0; World.m[0][2] = -sinf(Scale);	World.m[0][3] = 0.0;
	World.m[1][0] = 0.0;		World.m[1][1] = 1.0; World.m[1][2] = 0.0; 		World.m[1][3] = 0.0;
	World.m[2][0] = sinf(Scale);	World.m[2][1] = 0.0; World.m[2][2] = cosf(Scale);	World.m[2][3] = 0.0;
	World.m[3][0] = 0.0;		World.m[3][1] = 0.0; World.m[3][2] = 0.0;		World.m[3][3] = 1.0;
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);
	
	glutPostRedisplay();
}
void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	
	glutSwapBuffers();
}
int create_buffers() {
	// Vertex Buffer
	Vector3f vertices[4];
	vertices[0] = Vector3f(-1.0, -1.0, 0.0);
	vertices[1] = Vector3f(0.0, -1.0, 1.0);
	vertices[2] = Vector3f(1.0, -1.0, 0.0);
	vertices[3] = Vector3f(0.0, 1.0, 0.0);
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Index Buffer
	unsigned int indexes[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};
	
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
	
	return 0;
}
int add_shader(GLuint ShaderProgram, const char* shader_text, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);
	if (ShaderObj == 0) {
		std::cerr << "Error creating shader " << ShaderType << "\n";
		return 1;
	}
	
	const GLchar* p[1];
	p[0] = shader_text;
	GLint length[1];
	length[0] = strlen(shader_text);
	glShaderSource(ShaderObj, 1, p, length);
	glCompileShader(ShaderObj);
	GLint r;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &r);
	if (!r) {
		GLchar log[1024] = {0};
		glGetShaderInfoLog(ShaderObj, sizeof(log), NULL, log);
		std::cerr << "Error compiling shader " << ShaderType << ": '" << log << "'\n";
		return 1;
	}
	
	glAttachShader(ShaderProgram, ShaderObj);
	
	return 0;
}
int compile_shaders() {
	GLuint ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0) {
		std::cerr << "Error creating shader program\n";
		return 1;
	}
	
	std::string vs, fs;
	
	if (read_file(vertex_shader_path, vs)) {
		return 1;
	}
	if (read_file(fragment_shader_path, fs)) {
		return 1;
	}
	
	if (add_shader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER)) {
		return 1;
	}
	if (add_shader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER)) {
		return 1;
	}
	
	GLint r = 0;
	GLchar log[1024] = {0};
	
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &r);
	if (r == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(log), NULL, log);
		std::cerr << "Error linking shader program: '" << log << "'\n";
		return 1;
	}
	
	// Validation should only be done in development
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &r);
	if (!r) {
		glGetProgramInfoLog(ShaderProgram, sizeof(log), NULL, log);
		std::cerr << "Invalid shader program: '" << log << "'\n";
		return 1;
	}
	
	glUseProgram(ShaderProgram);
	
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	if (gWorldLocation == 0xFFFFFFFF) {
		std::cerr << "Invalid uniform location\n";
		return 1;
	}
	
	return 0;
}
int init_video(int argc, char* argv[]) {
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("EasyGameEngine");
	
	// GLUT callbacks
	glutDisplayFunc(render);
	glutIdleFunc(idle);
	
	// Initialize GLEW
	int r = glewInit();
	if (r != GLEW_OK) {
		std::cout << "GLEW Error: " << glewGetErrorString(r) << "\n";
		return 1;
	}
	
	std::cout << "GL version: " << glGetString(GL_VERSION) << "\n";
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	create_buffers();
	r = compile_shaders();
	if (r) {
		std::cerr << "Error compiling shaders\n";
		return r;
	}
	
	return 0;
}
void loop_video() {
	glutMainLoop();
}
int close_video() {
	return 0;
}

int init_audio(int argc, char* argv[]) {
	alutInit(&argc, argv);
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alGetError();
	return 0;
}
int close_audio() {
	alutExit();
	return 0;
}

int init_game(int argc, char* argv[]) {
	if (init_video(argc, argv)) {
		return 1;
	}
	init_audio(argc, argv);
	init_resources();
	return 0;
}
int loop_game() {
	loop_video();
	return 0;
}
int close_game() {
	close_resources();
	close_audio();
	close_video();
	return 0;
}
