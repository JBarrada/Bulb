#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shader_utils.h"
#include "gamepad_input.h"

#include "drawing_tools.h"
#include "bulb_shader.h"
#include "bulb_settings.h"

int SCREEN_W = 640;
int SCREEN_H = 480;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

bool is_fullscreen = false;

GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);

float frames = 0;
clock_t frame_time;

DrawingTools drawing_tools;
BulbShader bulb_shader;
BulbControlSettings control_settings;
BulbSettings bulb_settings(&bulb_shader, &control_settings, &drawing_tools);


void frame_counter() {
	frames++;
	float time_seconds = (float)(clock() - frame_time) / CLOCKS_PER_SEC;
	if (time_seconds > 0.25) {
		float fps = frames / time_seconds;
		printf("%0.1f\n", fps);

		frames = 0;
		frame_time = clock();
	}
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	bulb_shader.draw();

	bulb_shader.update_control_variables(control_settings.camera_eye, control_settings.camera_target, control_settings.camera_up, control_settings.camera_fov[0], ASPECT);
	bulb_shader.update_shader_variables();

	if (bulb_settings.settings_open) bulb_settings.draw();

	glutSwapBuffers();
	
	control_settings.update_camera_prox(SCREEN_W, SCREEN_H);

	//frame_counter();
}

void set_fullscreen(bool fullscreen) {
	if (fullscreen != is_fullscreen) {
		is_fullscreen = fullscreen;
		if (is_fullscreen) {
			glutFullScreen();
		} else {
			glutReshapeWindow(800, 600);
			glutPositionWindow(0, 0);
		}
	}
}

void keyboard_down_special(int key, int x, int y) {
	if (bulb_settings.settings_open) {
		bulb_settings.keyboard_update(key);
		//control_settings.camera_keyboard_update(key);
	} else {
		if (key == 27) {
			bulb_settings.settings_open = true;
		} else if (key == GLUT_KEY_F11) {
			set_fullscreen(!is_fullscreen);
		} else {
			control_settings.camera_keyboard_update(key);
		}
	}
}

void keyboard_down(unsigned char key, int x, int y) {
	keyboard_down_special((int)key, x, y);
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	if (pad->is_connected()) {
		pad->update();

		if (bulb_settings.settings_open) {
			bulb_settings.gamepad_update(&pad->State);
			control_settings.camera_gamepad_update(&pad->State, true);
		} else {
			if (pad->State.pressed(GamePad_Button_START)) {
				bulb_settings.settings_open = true;
			} else if (pad->State.pressed(GamePad_Button_BACK)) {
				set_fullscreen(!is_fullscreen);
			} else {
				control_settings.camera_gamepad_update(&pad->State, false);
			}
		}
	}

	glutTimerFunc(20, force_redraw, 0);
}

void reshape(int width, int height) {
	SCREEN_W = width;
	SCREEN_H = height;
	ASPECT = (float)SCREEN_W / (float)SCREEN_H;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	drawing_tools.reshape((float)SCREEN_W, (float)SCREEN_H);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("BULB");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	glutSpecialFunc(keyboard_down_special);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);

	bulb_shader.load();

	/*
	ofstream save_file;
	save_file.open("testsave.bulbsave");

	bulb_shader.write_to_save_file(save_file);

	save_file.close();
	*/

	/*
	ifstream save_file;
	save_file.open("testsave.bulbsave", ios::in);

	bulb_shader.read_from_save_file(save_file);

	save_file.close();
	*/

	glutMainLoop();

	return 0;
}
