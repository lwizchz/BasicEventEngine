#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#include "resource_structures.hpp"
#include "../resources/resources.hpp"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

#include "../include/ogldev.hpp"
#include "util.hpp"

static const float FieldDepth = 20.0;
static const float FieldWidth = 10.0;

void game_render();
void game_idle();
void game_keyboard(unsigned char, int, int);
void game_keyboard_special(int, int, int);
void game_mouse_passive(int, int);

class Game {
		GLuint VBO;
		GLuint IBO;
		int window_width, window_height;
		
		Texture* pTexture;
		LightingTechnique* pEffect;
		DirectionalLight directionalLight;
		
		Camera* pGameCamera;
		PersProjInfo gPersProjInfo;
		const char* vertex_shader_path = "ege/lighting.vs";
		const char* fragment_shader_path = "ege/lighting.fs";
	public:
		void on_idle();
		void on_render();
		void on_keyboard(unsigned char, int, int);
		void on_keyboard_special(int, int, int);
		void on_mouse_passive(int, int);
		
		int create_buffers();
		int add_shader(GLuint, const char*, GLenum, GLuint*);
		int compile_shaders();
		int init_video(int, char**);
		void loop_video();
		int close_video();
		int init_audio(int, char**);
		int close_audio();
		int init(int, char**);
		int loop();
		int close();
} game;
void Game::on_idle() {
	static float Scale = 0.0;
	Scale += 0.1;
	
	PointLight pl[2];
	pl[0].DiffuseIntensity = 0.5;
	pl[0].Color = Vector3f(1.0, 0.5, 0.0);
	pl[0].Position = Vector3f(3.0, 1.0, FieldDepth*(cosf(Scale)+1.0)/2.0);
	pl[0].Attenuation.Linear = 0.1;
	pl[1].DiffuseIntensity = 0.5;
	pl[1].Color = Vector3f(0.0, 0.5, 1.0);
	pl[1].Position = Vector3f(7.0, 1.0, FieldDepth*(sinf(Scale)+1.0)/2.0);
	pl[1].Attenuation.Linear = 0.1;
	pEffect->SetPointLights(2, pl);
	
	SpotLight sl[2];
	sl[0].DiffuseIntensity = 0.9;
	sl[0].Color = Vector3f(0.0, 1.0, 1.0);
	sl[0].Position = pGameCamera->GetPos();
	sl[0].Direction = pGameCamera->GetTarget();
	sl[0].Attenuation.Linear = 0.1;
	sl[0].Cutoff = 10.0;
	
	sl[1].DiffuseIntensity = 0.9;
	sl[1].Color = Vector3f(1.0, 1.0, 1.0);
	sl[1].Position = Vector3f(5.0, 3.0, 10.0);
	sl[1].Direction = Vector3f(0.0, -1.0, 0.0);
	sl[1].Attenuation.Linear = 0.1;
	sl[1].Cutoff = 20.0;
	pEffect->SetSpotLights(2, sl);
	
	Pipeline p;
	p.WorldPos(0.0, 0.0, 1.0);
	p.SetCamera(*pGameCamera);
	p.SetPerspectiveProj(gPersProjInfo);
	
	pEffect->SetWVP(p.GetWVPTrans());
	const Matrix4f& WorldTransformation = p.GetWorldTrans();
	pEffect->SetWorldMatrix(WorldTransformation);
	pEffect->SetDirectionalLight(directionalLight);
	pEffect->SetEyeWorldPos(pGameCamera->GetPos());
	pEffect->SetMatSpecularIntensity(0.0);
	pEffect->SetMatSpecularPower(0.0);
	
	glutPostRedisplay();
}
void Game::on_render() {
	pGameCamera->OnRender();
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	pTexture->Bind(GL_TEXTURE0);
	
	//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glutSwapBuffers();
}
void Game::on_keyboard(unsigned char key, int x, int y) {
	std::pair<int, int> mouse (x, y);
	switch (key) {
		case 'q':
			glutLeaveMainLoop();
			break;
		case 'a':
			directionalLight.AmbientIntensity += 0.05;
			break;
		case 's':
			directionalLight.AmbientIntensity -= 0.05;
			break;
		case 'z':
			directionalLight.DiffuseIntensity += 0.05;
			break;
		case 'x':
			directionalLight.DiffuseIntensity -= 0.05;
			break;
		default:
			OGLDEV_KEY OgldevKey = GLUTKeyToOGLDEVKey(key);
			pGameCamera->OnKeyboard(OgldevKey);
	}
}
void Game::on_keyboard_special(int key, int x, int y) {
	std::pair<int, int> mouse(x, y);
	
	OGLDEV_KEY OgldevKey = GLUTKeyToOGLDEVKey(key);
	pGameCamera->OnKeyboard(OgldevKey);
}
void Game::on_mouse_passive(int x, int y) {
	pGameCamera->OnMouse(x, y);
}
int Game::create_buffers() {
	/*// Define indexes
	unsigned int indexes[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};
	
	// Create Vertex Buffer
	Vertex vertices[4] = {
		Vertex(Vector3f(-1.0, -1.0, 0.5773), Vector2f(0.0, 0.0)),
		Vertex(Vector3f(0.0, -1.0, -1.15475), Vector2f(0.5, 0.0)),
		Vertex(Vector3f(1.0, -1.0, 0.5773), Vector2f(1.0, 0.0)),
		Vertex(Vector3f(0.0, 1.0, 0.0), Vector2f(0.5, 1.0)),
	};
	
	unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS(vertices);
	calculate_normals(indexes, sizeof(indexes)/4, vertices, VertexCount);
		
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Create Index Buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);*/
	
	const Vector3f Normal = Vector3f(0.0, 1.0, 0.0);
	Vertex vertices[6] = {
		Vertex(Vector3f(0.0, 0.0, 0.0),			Vector2f(0.0, 0.0), Normal),
		Vertex(Vector3f(0.0, 0.0, FieldDepth),		Vector2f(0.0, 1.0), Normal),
		Vertex(Vector3f(FieldWidth, 0.0, 0.0),		Vector2f(1.0, 0.0), Normal),
		
		Vertex(Vector3f(FieldWidth, 0.0, 0.0),		Vector2f(1.0, 0.0), Normal),
		Vertex(Vector3f(0.0, 0.0, FieldDepth),		Vector2f(0.0, 1.0), Normal),
		Vertex(Vector3f(FieldWidth, 0.0, FieldDepth),	Vector2f(1.0, 1.0), Normal)
	};
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	return 0;
}
int Game::add_shader(GLuint ShaderProgram, const char* shader_text, GLenum ShaderType, GLuint* ShaderObj) {
	if (*ShaderObj == 0) {
		std::cerr << "Error creating shader " << ShaderType << "\n";
		return 1;
	}
	
	const GLchar* p[1];
	p[0] = shader_text;
	GLint length[1];
	length[0] = strlen(shader_text);
	glShaderSource(*ShaderObj, 1, p, length);
	glCompileShader(*ShaderObj);
	GLint r;
	glGetShaderiv(*ShaderObj, GL_COMPILE_STATUS, &r);
	if (!r) {
		GLchar log[1024] = {0};
		glGetShaderInfoLog(*ShaderObj, sizeof(log), NULL, log);
		std::cerr << "Error compiling shader " << ShaderType << ": '" << log << "'\n";
		return 1;
	}
	
	glAttachShader(ShaderProgram, *ShaderObj);
	
	return 0;
}
int Game::compile_shaders() {
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
	
	GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	if (add_shader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER, &VertexShaderObj)) {
		return 1;
	}
	if (add_shader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER, &FragmentShaderObj)) {
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
	
	// Validation should only be done in development (?)
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &r);
	if (!r) {
		glGetProgramInfoLog(ShaderProgram, sizeof(log), NULL, log);
		std::cerr << "Invalid shader program: '" << log << "'\n";
		return 1;
	}
	
	glUseProgram(ShaderProgram);
	glDeleteShader(VertexShaderObj); // Delete shaders after linking to use less memory
	glDeleteShader(FragmentShaderObj);
	
	return 0;
}
int Game::init_video(int argc, char* argv[]) {
	// Initialize ImageMagick
	Magick::InitializeMagick(*argv);
	
	// Initialize GLUT
	window_width = DEFAULT_WINDOW_WIDTH;
	window_height = DEFAULT_WINDOW_HEIGHT;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("EasyGameEngine");
	
	// GLUT callbacks
	glutDisplayFunc(game_render);
	glutIdleFunc(game_idle);
	glutKeyboardFunc(game_keyboard);
	glutSpecialFunc(game_keyboard_special);
	glutPassiveMotionFunc(game_mouse_passive);
	
	pGameCamera = new Camera(window_width, window_height, Vector3f(5.0, 1.0, -3.0), Vector3f(0.0, 0.0, 1.0), Vector3f(0.0, 1.0, 0.0));
	
	// Initialize GLEW
	int r = glewInit();
	if (r != GLEW_OK) {
		std::cout << "GLEW Error: " << glewGetErrorString(r) << "\n";
		return 1;
	}
	
	std::cout << "GL version: " << glGetString(GL_VERSION) << "\n";
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	/*glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);*/
	
	create_buffers();
	r = compile_shaders();
	if (r) {
		std::cerr << "Error compiling shaders\n";
		return r;
	}
	
	pEffect = new LightingTechnique();
	if (!pEffect->Init()) {
		std::cerr << "Couldn't load lighting technique\n";
		return 1;
	}
	pEffect->Enable();
	pEffect->SetTextureUnit(0);
	pTexture = new Texture(GL_TEXTURE_2D, "resources/sprites/bricks.jpg");
	if (!pTexture->Load()) {
		std::cerr << "Couldn't load texture\n";
		return 1;
	}
	
	directionalLight.Color = Vector3f(1.0, 1.0, 1.0);
	directionalLight.AmbientIntensity = 0.5;
	directionalLight.DiffuseIntensity = 0.75;
	directionalLight.Direction = Vector3f(1.0, 0.0, 0.0);
	
	gPersProjInfo.FOV = 60.0;
	gPersProjInfo.Width = window_width;
	gPersProjInfo.Height = window_height;
	gPersProjInfo.zNear = 1.0;
	gPersProjInfo.zFar = 100.0;
	
	return 0;
}
void Game::loop_video() {
	glutMainLoop();
}
int Game::close_video() {
	delete pGameCamera;
	delete pTexture;
	delete pEffect;
	
	return 0;
}

int Game::init_audio(int argc, char* argv[]) {
	alutInit(&argc, argv);
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alGetError();
	return 0;
}
int Game::close_audio() {
	alutExit();
	return 0;
}

int Game::init(int argc, char* argv[]) {
	if (init_video(argc, argv)) {
		return 1;
	}
	init_audio(argc, argv);
	init_resources();
	return 0;
}
int Game::loop() {
	loop_video();
	return 0;
}
int Game::close() {
	close_resources();
	close_audio();
	close_video();
	return 0;
}

void game_render() {game.on_render();}
void game_idle() {game.on_idle();}
void game_keyboard(unsigned char k, int x, int y) {game.on_keyboard(k, x, y);}
void game_keyboard_special(int k, int x, int y) {game.on_keyboard_special(k, x, y);}
void game_mouse_passive(int x, int y) {game.on_mouse_passive(x, y);}
