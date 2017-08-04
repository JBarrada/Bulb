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
	glm::vec3 camera_eye;
	glm::vec3 camera_target;
	glm::vec3 camera_up;

	glm::mat4 camera_orientation;

	float camera_fov;

	glm::vec3 camera_eye_prev;
	glm::vec3 camera_velocity_prev;

	BulbControl();

	void update_program_variables();

	float expo(float value);
	void update_gamepad();
	void keyboard_down(unsigned char key, int x, int y);

};


#endif