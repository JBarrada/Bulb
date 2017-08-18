#ifndef BULB_CONTROL_SETTINGS_H
#define BULB_CONTROL_SETTINGS_H

#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "bulb_variable.h"
#include "gamepad_input.h"
#include "keyboard_input.h"

#include "string_tools.h"

class BulbControlSettings {
public:
	//glm::vec3 camera_eye_prev;
	//glm::vec3 camera_velocity_prev;

	float camera_prox_target;
	float camera_prox;

	BulbVariable camera_eye;
	BulbVariable camera_target;
	BulbVariable camera_up;
	BulbVariable camera_fov;
	glm::mat4 camera_orientation;

	BulbVariable control_expo_power;
	BulbVariable control_move_speed_forward;
	BulbVariable control_move_speed_lateral;
	BulbVariable control_move_speed_vertical;
	BulbVariable control_pitch_speed;
	BulbVariable control_roll_speed;
	BulbVariable control_yaw_speed;
	BulbVariable control_vibrate;

	vector<BulbVariable*> control_variables;

	BulbControlSettings();

	void write_to_save_file(ofstream &save_file);
	void read_from_save_file(ifstream &save_file);

	void update_camera_prox(int SCREEN_W,  int SCREEN_H);
	void camera_gamepad_update(GamePadState *state, bool sticks_only);
	void camera_keyboard_update(int key);
	float expo(float value);
};

#endif