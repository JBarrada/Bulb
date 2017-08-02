#ifndef BULB_CONTROL_H
#define BULB_CONTROL_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <string>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shader_utils.h"
#include "gamepad_input.h"

using namespace std;


class BulbControl {
public:
	GamePadXbox* pad;

	GLuint program_fp32;

	int SCREEN_W;
	int SCREEN_H;
	float ASPECT;

	glm::vec3 camera_eye;
	glm::vec3 camera_target;
	glm::vec3 camera_up;
	float camera_fov;
	glm::mat4 camera_orientation;

	glm::vec3 camera_eye_prev;
	glm::vec3 camera_velocity_prev;

	BulbControl(int screen_w, int screen_h);

	void reshape(int screen_w, int screen_h);

	void load_shader_ex(char *vtxShdName, char *frgShdName);

	void update_program_variables();

	float expo(float value);
	void update_gamepad();
	void keyboard_down(unsigned char key, int x, int y);

};


#endif