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
void game_mouse(int, int, int, int);

class Game {
		GLuint VBO;
		GLuint IBO;
		int window_width, window_height;
		long long m_currentTime;
		
		float m_scale = 0.0;
		Texture* pGroundTex;
		LightingTechnique* pLightingEffect;
		SpotLight spotLight;
		Mesh *pMesh, *pQuad;
		ShadowMapTechnique* pShadowMapEffect;
		ShadowMapFBO shadowMapFBO;
		SkyBox* pSkyBox;
		bool bumpMapEnabled = true;
		Texture* pNormalMap;
		Texture* pTrivialNormalMap;
		ParticleSystem particleSystem;
		
		SimpleColorTechnique simpleColorEffect;
		PickingTechnique pickingEffect;
		PickingTexture pickingTexture;
		struct {
			bool IsPressed;
			int x;
			int y;
		} m_leftMouseButton;
		Vector3f worldPos[2];
		
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
		void on_mouse(int, int, int, int);
		
		void picking_pass();
		void shadow_map_pass();
		void render_pass();
		
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
	// Do stuff
	
	glutPostRedisplay();
}
void Game::on_render() {
	pGameCamera->OnRender();
	m_scale += 0.1;
	
	picking_pass();
	shadow_map_pass();
	render_pass();
	
	glutSwapBuffers();
}
void Game::picking_pass() {
	Pipeline p;
	p.Scale(0.1, 0.1, 0.1);
	p.Rotate(0.0, 90.0, 0.0);
	p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
	p.SetPerspectiveProj(gPersProjInfo);
	
	pickingTexture.EnableWriting();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	pickingEffect.Enable();
	
	for (uint i = 0 ; i < (int)ARRAY_SIZE_IN_ELEMENTS(worldPos) ; i++) {
		p.WorldPos(worldPos[i]);
		pickingEffect.SetObjectIndex(i);
		pickingEffect.SetWVP(p.GetWVPTrans());    
		pMesh->Render(&pickingEffect);
	}
	
	pickingTexture.DisableWriting();
}
void Game::shadow_map_pass() {
	shadowMapFBO.BindForWriting();
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	pShadowMapEffect->Enable();
	
	Pipeline p;
	p.Scale(0.1, 0.1, 0.1);
	p.Rotate(0.0, m_scale, 0.0);
	p.WorldPos(0.0, 0.0, 5.0);
	p.SetCamera(spotLight.Position, spotLight.Direction, Vector3f(0.0, 1.0, 0.0));
	p.SetPerspectiveProj(gPersProjInfo);
	pShadowMapEffect->SetWVP(p.GetWVPTrans());
	
	pMesh->Render();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Game::render_pass() {
	long long timeNow = GetCurrentTimeMillis();
	assert(timeNow >= m_currentTime);
	unsigned int deltaTime = (unsigned int) (timeNow - m_currentTime);
	m_currentTime = timeNow;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, 90.0f, 0.0f);
	p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
	p.SetPerspectiveProj(gPersProjInfo);
	
	// If the left mouse button is clicked check if it hit a triangle
	// and color it red
	if (m_leftMouseButton.IsPressed) {
		PickingTexture::PixelInfo Pixel = pickingTexture.ReadPixel(m_leftMouseButton.x, window_height - m_leftMouseButton.y - 1);
		GLExitIfError;
		if (Pixel.PrimID != 0) {                
			simpleColorEffect.Enable();
			assert(Pixel.ObjectID < ARRAY_SIZE_IN_ELEMENTS(worldPos));
			p.WorldPos(worldPos[(uint)Pixel.ObjectID]);
			simpleColorEffect.SetWVP(p.GetWVPTrans());
			// Must compensate for the decrement in the FS!
			pMesh->Render((uint)Pixel.DrawID, (uint)Pixel.PrimID - 1); 
		}
	}
	
	pLightingEffect->Enable();
	pLightingEffect->SetEyeWorldPos(pGameCamera->GetPos());
	
	shadowMapFBO.BindForReading(SHADOW_TEXTURE_UNIT);
	
	for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(worldPos) ; i++) {
            p.WorldPos(worldPos[i]);
            pLightingEffect->SetWVP(p.GetWVPTrans());
            pLightingEffect->SetWorldMatrix(p.GetWorldTrans());                
            pMesh->Render(NULL);
        }
	
	// Render floor
	pGroundTex->Bind(COLOR_TEXTURE_UNIT);
	pTrivialNormalMap->Bind(NORMAL_TEXTURE_UNIT);
	
	//Pipeline p;
	p.Scale(10.0f, 10.0f, 10.0f);
	p.WorldPos(0.0f, 0.0f, 0.0f);
	p.Rotate(90.0f, 0.0f, 0.0f);
	p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
	p.SetPerspectiveProj(gPersProjInfo);
	
	pLightingEffect->SetWVP(p.GetWVPTrans());
	pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
	//p.SetCamera(spotLight.Position, spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
	//pLightingEffect->SetLightWVP(p.GetWVPTrans());
	pQuad->Render();
	
	// Render tank
	/*if (bumpMapEnabled) {
		pNormalMap->Bind(NORMAL_TEXTURE_UNIT);
	} else {
		pTrivialNormalMap->Bind(NORMAL_TEXTURE_UNIT);
	}
	
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, m_scale, 0.0f);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
	pLightingEffect->SetWVP(p.GetWVPTrans());
	pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
	p.SetCamera(spotLight.Position, spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
	pLightingEffect->SetLightWVP(p.GetWVPTrans());
	pMesh->Render();
	pTrivialNormalMap->Bind(NORMAL_TEXTURE_UNIT);*/
	
	particleSystem.Render(deltaTime, p.GetVPTrans(), pGameCamera->GetPos());
	
	pSkyBox->Render();
}
void Game::on_keyboard(unsigned char key, int x, int y) {
	std::pair<int, int> mouse (x, y);
	switch (key) {
		case 'q':
			glutLeaveMainLoop();
			break;
		case 'a':
			spotLight.AmbientIntensity += 0.05;
			break;
		case 's':
			spotLight.AmbientIntensity -= 0.05;
			break;
		case 'z':
			spotLight.DiffuseIntensity += 0.05;
			break;
		case 'x':
			spotLight.DiffuseIntensity -= 0.05;
			break;
		case 'b':
			bumpMapEnabled = !bumpMapEnabled;
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
void Game::on_mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		m_leftMouseButton.IsPressed = (state == GLUT_DOWN);
		m_leftMouseButton.x = x;
		m_leftMouseButton.y = y;
	}
}
int Game::init_video(int argc, char* argv[]) {
	m_currentTime = GetCurrentTimeMillis();
	
	// Initialize ImageMagick
	Magick::InitializeMagick(*argv);
	
	// Initialize GLUT
	window_width = DEFAULT_WINDOW_WIDTH;
	window_height = DEFAULT_WINDOW_HEIGHT;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("EasyGameEngine");
	
	// GLUT callbacks
	glutDisplayFunc(game_render);
	//glutIdleFunc(game_idle);
	glutIdleFunc(game_render);
	glutKeyboardFunc(game_keyboard);
	glutSpecialFunc(game_keyboard_special);
	glutPassiveMotionFunc(game_mouse_passive);
	glutMouseFunc(game_mouse);
	
	pGameCamera = new Camera(window_width, window_height, Vector3f(2.0, 2.0, -7.0), Vector3f(0.0, 0.0, 1.0), Vector3f(0.0, 1.0, 0.0));
	
	// Initialize GLEW
	int r = glewInit();
	if (r != GLEW_OK) {
		std::cout << "GLEW Error: " << glewGetErrorString(r) << "\n";
		return 1;
	}
	
	std::cout << "GL version: " << glGetString(GL_VERSION) << "\n";
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	spotLight.AmbientIntensity = 0.5f;
	spotLight.DiffuseIntensity = 0.9f;
	spotLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
	spotLight.Attenuation.Linear = 0.01f;
	spotLight.Position  = Vector3f(-20.0, 20.0, 5.0f);
	spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
	spotLight.Cutoff =  20.0f;
	
	pLightingEffect = new LightingTechnique();
	if (!pLightingEffect->Init()) {
		std::cerr << "Couldn't load lighting technique\n";
		return 1;
	}
	pLightingEffect->Enable();
	pLightingEffect->SetSpotLights(1, &spotLight);
	pLightingEffect->SetColorTextureUnit(0);
        pLightingEffect->SetNormalMapTextureUnit(2);
	
	pGroundTex = new Texture(GL_TEXTURE_2D, "resources/sprites/bricks.jpg");
	if (!pGroundTex->Load()) {
		std::cerr << "Couldn't load texture\n";
		return 1;
	}
	
	if (!shadowMapFBO.Init(window_width, window_height)) {
		std::cerr << "Couldn't init shadow map fbo\n";
		return 1;
	}
	pShadowMapEffect = new ShadowMapTechnique();
	if (!pShadowMapEffect->Init()) {
		std::cerr << "Couldn't init shadow map technique\n";
		return 1;
	}
	pShadowMapEffect->Enable();
	
	/*pMesh = new Mesh();
	if (!pMesh->LoadMesh("resources/meshes/phoenix_ugv.md2")) {
		std::cerr << "Couldn't load mesh\n";
		return 1;
	}*/
	
	pQuad = new Mesh();
	if (!pQuad->LoadMesh("resources/meshes/quad.obj")) {
		std::cerr << "Couldn't load mesh\n";
		return 1;
	}
	
	Vector3f ParticleSysPos = Vector3f(0.0, 0.0, 1.0);
	if (!particleSystem.InitParticleSystem(ParticleSysPos)) {
		std::cerr << "Couldn't init particle system\n";
		return 1;
	}
	
	gPersProjInfo.FOV = 60.0;
	gPersProjInfo.Width = window_width;
	gPersProjInfo.Height = window_height;
	gPersProjInfo.zNear = 1.0;
	gPersProjInfo.zFar = 100.0;
	
	pSkyBox = new SkyBox(pGameCamera, gPersProjInfo);
	if (!pSkyBox->Init("resources/skyboxes/",
		"sp3right.jpg",
		"sp3left.jpg",
		"sp3top.jpg",
		"sp3bottom.jpg",
		"sp3front.jpg",
		"sp3back.jpg"
	)) {
		std::cerr << "Couldn't load skybox\n";
		return 1;
	}
	
	pNormalMap = new Texture(GL_TEXTURE_2D, "resources/normal_maps/tank.jpg");
	if (!pNormalMap->Load()) {
		std::cerr << "Couldn't load normal map\n";
		return 1;
	}
	
	pTrivialNormalMap = new Texture(GL_TEXTURE_2D, "resources/normal_maps/normal_up.jpg");
	if (!pTrivialNormalMap->Load()) {
		std::cerr << "Couldn't load trivial normal map\n";
		return 1;
	}
	
	m_leftMouseButton.IsPressed = false;
	worldPos[0] = Vector3f(-10.0, 0.0, 5.0);
	worldPos[1] = Vector3f(10.0, 0.0, 5.0);
	
	if (!pickingTexture.Init(window_width, window_height)) {
		std::cerr << "Couldn't init picking texture\n";
		return 1;
	}
	if (!pickingEffect.Init()) {
		std::cerr << "Couldn't init picking effect\n";
		return 1;
	}
	if (!simpleColorEffect.Init()) {
		std::cerr << "Couldn't init simple color effect\n";
		return 1;
	}
	
	pMesh = new Mesh();
	if (!pMesh->LoadMesh("resources/meshes/spider.obj")) {
		std::cerr << "Couldn't load spider mesh\n";
		return 1;
	}
	
	return 0;
}
void Game::loop_video() {
	glutMainLoop();
}
int Game::close_video() {
	delete pLightingEffect;
	delete pShadowMapEffect;
	delete pMesh;
	delete pQuad;
	delete pGroundTex;
	delete pSkyBox;
	delete pNormalMap;
	delete pTrivialNormalMap;
	
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
void game_mouse(int b, int s, int x, int y) {game.on_mouse(b, s, x, y);}
