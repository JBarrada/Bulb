#include <iostream>
#include <cmath>
#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shader_utils.h"
#include "gamepad_input.h"

#define M_PI 3.14159265359f

static GLuint program_fp32;

int SCREEN_W = 100;
int SCREEN_H = 100;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

//GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);

int de_iterations = 10;
int max_ray_steps = 50;
float min_distance = 0.001f;

glm::vec3 camera_eye = glm::vec3(0.0, 2.0, 0.0);
glm::vec3 camera_target = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 camera_up = glm::vec3(0, 0, 1);

float camera_orbit = 0.0;

float camera_fov = 90.0;

glm::mat4 camera_orientation = glm::mat4(1.0);

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

	
	GLint prog_de_iterations = glGetUniformLocation(program_fp32, "de_iterations");
	glUniform1i(prog_de_iterations, de_iterations);

	GLint prog_max_ray_steps = glGetUniformLocation(program_fp32, "max_ray_steps");
	glUniform1i(prog_max_ray_steps, max_ray_steps);
	
	GLint prog_min_distance = glGetUniformLocation(program_fp32, "min_distance");
	glUniform1f(prog_min_distance, min_distance);
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
}

float get_avg_dist() {
	float depth_total = 0.0;
	int depth_samples = 0;
	for (int x = 0; x < SCREEN_W; x += (SCREEN_W / 10)) {
		for (int y = 0; y < SCREEN_H; y += (SCREEN_H / 10)) {
			GLfloat depth_comp;
			glReadPixels(x, y, 1, 1, GL_ALPHA, GL_FLOAT, &depth_comp);
			depth_total += depth_comp;
			depth_samples++;
		}
	}

	printf("%f\n", depth_total / (float)depth_samples);
	return (depth_total / (float)depth_samples);
}

void on_click(int button, int state, int x, int y) {
	/*
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			move_drag = true;
			move_drag_x = x;
			move_drag_y = y;
		}
		else if (state == GLUT_UP) {
			move_drag = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			c_value_drag = true;
			c_value_drag_x = x;
			c_value_drag_y = y;
		}
		else if (state == GLUT_UP) {
			c_value_drag = false;
		}
	}

	if (button == 3) {
		julia_state.zoom.add(-0.5);
	}
	if (button == 4) {
		julia_state.zoom.add(0.5);
	}
	*/
}

void on_move(int x, int y) {
	/*
	if (move_drag) {
		double fx = (x - move_drag_x) / (SCREEN_W / 2.0) * julia_state.zoom.x_exp;
		double fy = (y - move_drag_y) / (SCREEN_H / 2.0) * julia_state.zoom.x_exp;

		move_drag_x = x;
		move_drag_y = y;

		julia_state.center.x += -fx;
		julia_state.center.y += fy;
	}

	if (c_value_drag) {
		double fx = ((x - c_value_drag_x) / (SCREEN_W / 2.0) * julia_state.zoom.x_exp) / 500.0;
		double fy = ((y - c_value_drag_y) / (SCREEN_H / 2.0) * julia_state.zoom.x_exp) / 500.0;

		c_value_drag_x = x;
		c_value_drag_y = y;

		julia_state.c.x += -fx;
		julia_state.c.y += fy;
	}
	*/
}

void keyboard_down(unsigned char key, int x, int y) {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

		
	float avg_dist = glm::max(get_avg_dist(), 0.0001f);

	float move_amount = 0.01 * avg_dist;

	if (key == '1') 
		de_iterations++;
	if (key == '2')
		max_ray_steps++;
	if (key == '3')
		min_distance /= 2.0f;


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
		camera_orientation *= glm::rotate(glm::mat4(1.0), 0.02f, glm::vec3(0, 1, 0));
	}
	if (key == 'f') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), -0.02f, glm::vec3(0, 1, 0));
	}

	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));
}

void force_redraw(int value) {
	glutPostRedisplay();
	
	// update gamepad & stuff

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
	
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("BULB");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	GLuint error = glewInit();

	glutDisplayFunc(render);
	glutTimerFunc(20, force_redraw, 0);
	glutKeyboardFunc(keyboard_down);
	//glutMouseFunc(on_click);
	//glutMotionFunc(on_move);
	glutReshapeFunc(reshape);

	load_shader("bulb.vert", "bulb.frag", &program_fp32);

	glutMainLoop();

	return 0;
}
