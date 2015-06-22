#include <GL/freeglut.h>

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

int init_video(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("EasyGameEngine");
	
	glutDisplayFunc(render);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	glutMainLoop();
	
	return 0;
}
int close_video() {
	return 0;
}

int init_sound() {
	alutInit(0, NULL);
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alGetError();
	return 0;
}
int close_sound() {
	alutExit();
	return 0;
}
