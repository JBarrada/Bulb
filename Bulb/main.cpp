#include <iostream>
#include <cmath>
#include <glew.h>
#include <glut.h>

#include "shader_utils.h"
#include "gamepad_input.h"

#define M_PI 3.14159265359f

static GLuint program_fp32;

int SCREEN_W = 600;
int SCREEN_H = 600;
float ASPECT = (float)SCREEN_W / (float)SCREEN_H;

//GamePadXbox* pad = new GamePadXbox(GamePadIndex_One);

int de_iterations = 100;
int max_ray_steps = 100;
float min_distance = 0.001f;

GLfloat camera_eye[] = {00.0, 0, 0};
GLfloat camera_target[] = {0.0, 0.0, 0.0};
GLfloat camera_up[] = {0, 0, 1};

float camera_orbit = 0.0;

float camera_fov = 90.0;

void update_program_variables() {
	GLint prog_camera_pos = glGetUniformLocation(program_fp32, "camera_eye");
	glUniform3fv(prog_camera_pos, 1, camera_eye);
	
	GLint prog_camera_target = glGetUniformLocation(program_fp32, "camera_target");
	glUniform3fv(prog_camera_target, 1, camera_target);

	GLint prog_camera_up = glGetUniformLocation(program_fp32, "camera_up");
	glUniform3fv(prog_camera_up, 1, camera_up);

	GLint prog_camera_fov = glGetUniformLocation(program_fp32, "camera_fov");
	glUniform1f(prog_camera_fov, camera_fov);
	
	GLint prog_camera_aspect = glGetUniformLocation(program_fp32, "camera_aspect");
	glUniform1f(prog_camera_aspect, ASPECT);

	/*
	GLint prog_de_iterations = glGetUniformLocation(program_fp32, "de_iterations");
	glUniform1i(prog_de_iterations, de_iterations);

	GLint prog_max_ray_steps = glGetUniformLocation(program_fp32, "max_ray_steps");
	glUniform1i(prog_max_ray_steps, max_ray_steps);
	
	GLint prog_min_distance = glGetUniformLocation(program_fp32, "min_distance");
	glUniform1f(prog_min_distance, min_distance);
	*/
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
	/*
	if (key == 'g')
		explore = !explore;
	//julia_explorer.explore(SCREEN_W, SCREEN_H);
	if (key == 'h')
		julia_state.zoom.cruise = !julia_state.zoom.cruise;
	
	*/

	if (key == 'w') 
		camera_eye[1] += 0.1;
	if (key == 's') 
		camera_eye[1] -= 0.1;
	if (key == 'a') 
		camera_eye[0] += 0.1;
	if (key == 'd') 
		camera_eye[0] -= 0.1;
	
	if (key == ' ') {
		camera_orbit += 0.01;

		camera_eye[0] = 3.0 * sin(camera_orbit);
		camera_eye[2] = 3.0 * cos(camera_orbit);

	}
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

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//glLineWidth(2.0);

	load_shader("bulb.vert", "bulb.frag", &program_fp32);


	glutMainLoop();

	return 0;
}
