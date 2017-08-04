#include <iostream>
#include <cmath>
#include <time.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shader_utils.h"
#include "gamepad_input.h"

//#include "bulb_control.h"
#include "bulb_shader.h"
#include "drawing_tools.h"
#include "bulb_settings.h"

static GLuint program_fp32;

int SCREEN_W = 100;
int SCREEN_H = 100;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);

glm::vec3 camera_eye = glm::vec3(0.0, 4.0, 10.0);
glm::vec3 camera_target = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 camera_up = glm::vec3(0, 0, 1);

glm::vec3 camera_eye_prev = glm::vec3(camera_eye);
glm::vec3 camera_velocity_prev = glm::vec3(0.0);

float camera_fov = 1.5;

glm::mat4 camera_orientation = glm::mat4(1.0);

float camera_prox = 1.0;
float camera_prox_target = 1.0;

float frames = 0;
clock_t frame_time;

DrawingTools drawing_tools;
BulbShader bulb_shader;
BulbSettings bulb_settings(&bulb_shader.shader_variables, &drawing_tools);

void update_program_variables() {
	GLint prog_camera_pos = glGetUniformLocation(program_fp32, "camera_eye");
	glUniform3fv(prog_camera_pos, 1, (float*)&camera_eye);
	
	GLint prog_camera_target = glGetUniformLocation(program_fp32, "camera_target");
	glUniform3fv(prog_camera_target, 1, (float*)&camera_target);

	GLint prog_camera_up = glGetUniformLocation(program_fp32, "camera_up");
	glUniform3fv(prog_camera_up, 1, (float*)&camera_up);

	GLint prog_camera_fov = glGetUniformLocation(program_fp32, "camera_fov");
	glUniform1f(prog_camera_fov, camera_fov);
	
	GLint prog_camera_aspect = glGetUniformLocation(program_fp32, "camera_aspect");
	glUniform1f(prog_camera_aspect, ASPECT);
}

float get_avg_dist() {
	float depth_total = 0.0;
	int depth_samples = 0;
	GLfloat depth_comp;
	for (int i = 0; i < 100; i++) {
		glReadPixels(rand() % SCREEN_W, rand() % SCREEN_H, 1, 1, GL_ALPHA, GL_FLOAT, &depth_comp);
		depth_total += depth_comp;
		depth_samples++;
	}

	if (depth_samples != 0) {
		return (depth_total / (float)depth_samples);
	} else {
		return 1.0;
	}
}

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
	update_program_variables();
	bulb_shader.update_control_variables(camera_eye, camera_target, camera_up, camera_fov, ASPECT);
	bulb_shader.update_shader_variables();

	if (bulb_settings.settings_open) bulb_settings.draw();

	glutSwapBuffers();
	
	camera_prox_target = get_avg_dist();
	float prox_delta = (camera_prox_target - camera_prox);
	if (prox_delta > 0) {
		camera_prox +=  (prox_delta / 160.0f);
	} else {
		camera_prox +=  (prox_delta / 8.0f);
	}

	//frame_counter();
}

void keyboard_down(unsigned char key, int x, int y) {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

		
	float avg_dist = glm::max(camera_prox, 0.0001f);

	float move_amount = 0.02f * avg_dist;

	if (key == 'w') {
		camera_eye -= move_amount * forward_direction;
	}
	if (key == 's') {
		camera_eye += move_amount * forward_direction;
	}
	if (key == 'a') {
		camera_eye += move_amount * left_direction;
	}
	if (key == 'd') {
		camera_eye -= move_amount * left_direction;
	}
	
	if (key == ' ') {
		camera_eye += move_amount * camera_up;
	}

	if (key == 'q') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), 0.02f, glm::vec3(0, 0, 1));
	}
	if (key == 'e') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), -0.02f, glm::vec3(0, 0, 1));
	}

	if (key == 'r') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), 0.02f, glm::vec3(-1, 0, 0));
	}
	if (key == 'f') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), -0.02f, glm::vec3(-1, 0, 0));
	}

	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));
}

float expo(float value) {
	if (value < 0)
		return -1.0f * pow(value, 2);
	return pow(value, 2);
}

void update_gamepad() {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

	float avg_dist = glm::max(camera_prox, 0.0001f);
	float move_amount = 0.015f * avg_dist;
		
	// triggers
	float trigger_sum = expo(pad->State.rt) - expo(pad->State.lt);
	camera_eye -= (move_amount * trigger_sum) * forward_direction;

	camera_eye += (move_amount * expo(pad->State.lstick_y)) * camera_up;

	// rstick y
	camera_orientation *= glm::rotate(glm::mat4(1.0), 0.03f * expo(pad->State.rstick_y), glm::vec3(1, 0, 0));

	// rstick x
	camera_orientation *= glm::rotate(glm::mat4(1.0), 0.06f * expo(pad->State.rstick_x), glm::vec3(0, -1, 0));

	// lstick x
	camera_orientation *= glm::rotate(glm::mat4(1.0), 0.03f * expo(pad->State.lstick_x), glm::vec3(0, 0, -1));


	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));

	glm::vec3 velocity = camera_eye - camera_eye_prev;
	camera_eye_prev = glm::vec3(camera_eye);

	float accel = length(velocity - camera_velocity_prev);
	camera_velocity_prev = glm::vec3(velocity);

	pad->vibrate(accel * 100.0f, accel * 100.0f);
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	if (pad->is_connected()) {
		pad->update();

		if (bulb_settings.settings_open) {
			bulb_settings.gamepad_update(&pad->State);
		} else {
			if (pad->State.buttons_last[GamePad_Button_START] && pad->State.buttons[GamePad_Button_START]) {
				bulb_settings.settings_open = true;
			} else {
				update_gamepad();
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

	drawing_tools.reshape(SCREEN_W, SCREEN_H);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("BULB");
	//glutFullScreen();
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);

	bulb_shader.load("bulb.vert", "bulb.frag");
	program_fp32 = bulb_shader.program_fp32;

	bulb_settings.update_shader_variables();
	//load_shader("bulb.vert", "bulb_temp.frag", &program_fp32);

	glutMainLoop();

	return 0;
}
