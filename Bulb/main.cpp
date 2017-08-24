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
#include "keyboard_input.h"

int SCREEN_W = 320;
int SCREEN_H = 240;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

bool is_fullscreen = true;

GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);
KeyboardState keyboard_state;

float frames = 0;
clock_t frame_time;
float current_fps = 0;

DrawingTools drawing_tools;
BulbShader bulb_shader;
BulbControlSettings control_settings;
BulbSettings bulb_settings(&bulb_shader, &control_settings, &drawing_tools);


void frame_counter() {
	frames++;
	float time_seconds = (float)(clock() - frame_time) / CLOCKS_PER_SEC;
	if (time_seconds > 0.25) {
		current_fps = frames / time_seconds;

		frames = 0;
		frame_time = clock();
	}
}

void draw_fps() {
		char fps_string[10];
		sprintf_s(fps_string, "%0.1f", current_fps);
		glUseProgram(0);
		glColor3f(1,1,1);
		drawing_tools.rectangle_filled(SCREEN_W - 35, SCREEN_H - 20, 35, 20);
		glColor3f(0,0,0);
		drawing_tools.text(SCREEN_W - 30, SCREEN_H - 15, GLUT_BITMAP_HELVETICA_12, string(fps_string));
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	bulb_shader.draw();

	bulb_shader.update_control_variables(&control_settings, ASPECT);
	bulb_shader.update_shader_variables();

	if (bulb_settings.settings_open) bulb_settings.draw();
	if (control_settings.show_fps.value[0][0] == 1.0f) draw_fps();

	glutSwapBuffers();
	
	control_settings.update_camera_prox(SCREEN_W, SCREEN_H);

	frame_counter();
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
	keyboard_state.special_down(key);
}

void keyboard_up_special(int key, int x, int y) {
	keyboard_state.special_up(key);
}

void keyboard_down(unsigned char key, int x, int y) {
	keyboard_state.keyboard_down(key);
}

void keyboard_up(unsigned char key, int x, int y) {
	keyboard_state.keyboard_up(key);
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	if (pad->is_connected()) {
		pad->update();
	}

	if (bulb_settings.settings_open) {
		bulb_settings.input_update(pad, &keyboard_state);
		control_settings.camera_gamepad_update(&pad->State, true);
	} else {
		if (pad->State.pressed(GamePad_Button_START) || keyboard_state.pressed_keyboard(27)) {
			bulb_settings.settings_open = true;
		} else if (pad->State.pressed(GamePad_Button_BACK) || keyboard_state.pressed_special(GLUT_KEY_F11)) {
			set_fullscreen(!is_fullscreen);
		} else {
			control_settings.camera_gamepad_update(&pad->State, false);
			control_settings.camera_keyboard_update(&keyboard_state);
		}
	}
	
	keyboard_state.next();

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
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialFunc(keyboard_down_special);
	glutSpecialUpFunc(keyboard_up_special);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);

	bulb_shader.load();

	keyboard_state.reset();

	glutMainLoop();

	return 0;
}
