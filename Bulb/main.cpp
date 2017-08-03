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

#define M_PI 3.14159265359f

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

	
	//GLint prog_de_iterations = glGetUniformLocation(program_fp32, "de_iterations");
	//glUniform1i(prog_de_iterations, de_iterations);
	//
	//GLint prog_max_ray_steps = glGetUniformLocation(program_fp32, "max_ray_steps");
	//glUniform1i(prog_max_ray_steps, max_ray_steps);
	//
	//GLint prog_min_distance = glGetUniformLocation(program_fp32, "min_distance");
	//glUniform1f(prog_min_distance, min_distance);
	
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

void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	update_program_variables();
	glUseProgram(program_fp32);

	glBegin(GL_QUADS);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();
	
	glutSwapBuffers();
	
	camera_prox_target = get_avg_dist();
	float prox_delta = (camera_prox_target - camera_prox);
	if (prox_delta > 0) {
		camera_prox +=  (prox_delta / 160.0f);
	} else {
		camera_prox +=  (prox_delta / 8.0f);
	}
	
	//printf("%f\n", camera_prox);

	frames++;
	float time_seconds = (float)(clock() - frame_time) / CLOCKS_PER_SEC;
	if (time_seconds > 0.25) {
		float fps = frames / time_seconds;
		printf("%0.1f\n", fps);

		frames = 0;
		frame_time = clock();
	}
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
	if (pad->is_connected()) {
		pad->update();

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

		//printf("%f\n", force);
	}
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	// update gamepad & stuff
	update_gamepad();

	glutTimerFunc(20, force_redraw, 0);
}

void reshape(int width, int height) {
	SCREEN_W = width;
	SCREEN_H = height;
	ASPECT = (float)SCREEN_W / (float)SCREEN_H;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

int main(int argc, const char * argv[]) {
	glutInit(&argc, const_cast<char**>(argv));
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("BULB");
	//glutFullScreen();
	glClearColor(1.0, 1.0, 1.0, 0.0);

	GLuint error = glewInit();

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	glutReshapeFunc(reshape);

	glEnable(GL_BLEND);

	BulbShader bs;
	bs.load("bulb.vert", "bulb.frag");
	load_shader("bulb.vert", "bulb_temp.frag", &program_fp32);

	glutMainLoop();

	return 0;
}
