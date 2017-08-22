#include "bulb_control_settings.h"

BulbControlSettings::BulbControlSettings() {
	camera_eye.load_from_bulb_save_string("CV|camera_eye|Camera|DEFAULT|4|0|0|0000|0,8,0||||||||");
	camera_target.load_from_bulb_save_string("CV|camera_target|Camera|DEFAULT|4|0|0|0000|0,0,0||||||||");
	camera_up.load_from_bulb_save_string("CV|camera_up|Camera|DEFAULT|4|0|0|0000|0,0,1||||||||");
	camera_fov.load_from_bulb_save_string("CV|camera_fov|Camera|DEFAULT|2|0|0|0000|1.5|0|4||||||");
	camera_orientation = glm::mat4(1.0);

	camera_prox_target = 1.0f;
	camera_prox = 1.0f;

	//camera_eye_prev = glm::vec3(camera_eye);
	//camera_velocity_prev = glm::vec3(0.0);

	control_expo_power.load_from_bulb_save_string("CV|control_expo_power|Control|DEFAULT|1|0|0|0000|2|0|10|10|||||");
	control_move_speed_forward.load_from_bulb_save_string("CV|control_move_speed_forward|Control|DEFAULT|2|0|0|0000|0.015|0|0.1|0.1|||||");
	control_move_speed_lateral.load_from_bulb_save_string("CV|control_move_speed_lateral|Control|DEFAULT|2|0|0|0000|0.010|0|0.1|0.1|||||");
	control_move_speed_vertical.load_from_bulb_save_string("CV|control_move_speed_vertical|Control|DEFAULT|2|0|0|0000|0.010|0|0.1|0.1|||||");

	control_pitch_speed.load_from_bulb_save_string("CV|control_pitch_speed|Control|DEFAULT|2|0|0|0000|0.03|0|0.1|0.1|||||");
	control_roll_speed.load_from_bulb_save_string("CV|control_roll_speed|Control|DEFAULT|2|0|0|0000|0.06|0|0.1|0.1|||||");
	control_yaw_speed.load_from_bulb_save_string("CV|control_yaw_speed|Control|DEFAULT|2|0|0|0000|0.03|0|0.1|0.1|||||");

	control_vibrate.load_from_bulb_save_string("CV|control_vibrate|Control|DEFAULT|0|0|0|0000|1|0|1|1|||||");
	show_fps.load_from_bulb_save_string("CV|show_fps|Control|DEFAULT|0|0|0|0000|1|0|1|1|||||");

	control_variables.clear();
	control_variables.push_back(&camera_fov);
	control_variables.push_back(&control_expo_power);
	control_variables.push_back(&control_move_speed_forward);
	control_variables.push_back(&control_move_speed_lateral);
	control_variables.push_back(&control_move_speed_vertical);
	control_variables.push_back(&control_pitch_speed);
	control_variables.push_back(&control_roll_speed);
	control_variables.push_back(&control_yaw_speed);
	control_variables.push_back(&control_vibrate);
	control_variables.push_back(&show_fps);
}

void BulbControlSettings::write_to_save_file(ofstream &save_file) {
	save_file << camera_eye.get_bulb_save_string("CC") << endl;
	save_file << camera_target.get_bulb_save_string("CC") << endl;
	save_file << camera_up.get_bulb_save_string("CC") << endl;

	char value_chars[500];
	for (int i = 0, j = 0; i < 4; i++) {
		j += sprintf_s(value_chars+j, 500-j, "%f,%f,%f,%f|", camera_orientation[i][0], camera_orientation[i][1], camera_orientation[i][2], camera_orientation[i][3]);
	}
	save_file << "CM|" << "camera_orientation" << "|" << string(value_chars) << endl;

	for (int i = 0; i < (int)control_variables.size(); i++) {
		save_file << control_variables[i]->get_bulb_save_string("CV") << endl;
	}
}

void BulbControlSettings::read_from_save_file(ifstream &save_file) {
	save_file.clear();
	save_file.seekg(0, ios::beg);

	vector<BulbVariable> control_variables_new;

	string save_file_line;
	while (getline(save_file, save_file_line)) {
		if (save_file_line.substr(0, 2) == "CC") {
			BulbVariable current_variable(save_file_line);
			if (current_variable.name == "camera_eye") camera_eye = current_variable;
			if (current_variable.name == "camera_target") camera_target = current_variable;
			if (current_variable.name == "camera_up") camera_up = current_variable;
		}
		if (save_file_line.substr(0, 2) == "CM") {
			vector<string> line_split = split_string(save_file_line, "|");
			glm::vec4 value;
			for (int i = 0; i < 4; i++) {
				stovec(line_split[2+i], value);
				camera_orientation[i] = glm::vec4(value);
			}
		}
		if (save_file_line.substr(0, 2) == "CV") control_variables_new.push_back(BulbVariable(save_file_line));
	}

	for (int i = 0; i < (int)control_variables_new.size(); i++) {
		for (int j = 0; j < (int)control_variables.size(); j++) {
			if (control_variables[j]->name == control_variables_new[i].name) {
				*control_variables[j] = control_variables_new[i];
			}
		}
	}
}

void BulbControlSettings::update_camera_prox(int SCREEN_W,  int SCREEN_H) {
	float depth_total = 0.0f;
	int depth_samples = 0;
	GLfloat depth_comp;
	for (int i = 0; i < 100; i++) {
		glReadPixels(rand() % SCREEN_W, rand() % SCREEN_H, 1, 1, GL_ALPHA, GL_FLOAT, &depth_comp);
		depth_total += depth_comp;
		depth_samples++;
	}

	camera_prox_target = depth_total / (float)depth_samples;

	float prox_delta = (camera_prox_target - camera_prox);
	if (prox_delta > 0.0f) {
		camera_prox +=  (prox_delta / 160.0f);
	} else {
		camera_prox +=  (prox_delta / 8.0f);
	}
}

void BulbControlSettings::camera_gamepad_update(GamePadState *state, bool sticks_only) {
	glm::vec4 forward_direction = camera_orientation * glm::vec4(0, 1, 0, 0);
	glm::vec4 left_direction = camera_orientation * glm::vec4(1, 0, 0, 0);

	float prox_speed_forward = control_move_speed_forward.value[0][0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_lateral = control_move_speed_lateral.value[0][0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_vertical = control_move_speed_vertical.value[0][0] * glm::max(camera_prox, 0.0001f);

	// forward
	if (!sticks_only) {
		camera_eye.value[0] -= (prox_speed_forward * (expo(state->rt) - expo(state->lt))) * forward_direction;
	}

	// vertical
	camera_eye.value[0] += (prox_speed_vertical * expo(state->lstick_y)) * camera_up.value[0];

	// lateral & yaw
	if (state->buttons[GamePad_Button_LEFT_SHOULDER] || state->buttons[GamePad_Button_RIGHT_SHOULDER]) {
		camera_eye.value[0] -= (prox_speed_lateral * expo(state->lstick_x)) * left_direction;
	} else {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_yaw_speed.value[0][0] * expo(state->lstick_x), glm::vec3(0, 0, -1));
	}

	// pitch
	camera_orientation *= glm::rotate(glm::mat4(1.0), control_pitch_speed.value[0][0] * expo(state->rstick_y), glm::vec3(1, 0, 0));

	// roll
	camera_orientation *= glm::rotate(glm::mat4(1.0), control_roll_speed.value[0][0] * expo(state->rstick_x), glm::vec3(0, -1, 0));

	// update
	camera_target.value[0] = camera_eye.value[0] + (camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up.value[0] = camera_orientation * glm::vec4(0, 0, 1, 0);

	/*
	// velocity & accel
	glm::vec3 velocity = camera_eye - camera_eye_prev;
	camera_eye_prev = glm::vec3(camera_eye);

	float accel = length(velocity - camera_velocity_prev);
	camera_velocity_prev = glm::vec3(velocity);

	// vibrate
	if (control_vibrate) {
		pad->vibrate(accel * 100.0f, accel * 100.0f);
	}
	*/
}

void BulbControlSettings::camera_keyboard_update(KeyboardState *keyboard_state) {
	glm::vec4 forward_direction = camera_orientation * glm::vec4(0, 1, 0, 0);
	glm::vec4 left_direction = camera_orientation * glm::vec4(1, 0, 0, 0);

	float prox_speed_forward = control_move_speed_forward.value[0][0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_lateral = control_move_speed_lateral.value[0][0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_vertical = control_move_speed_vertical.value[0][0] * glm::max(camera_prox, 0.0001f);

	if (keyboard_state->keyboard['w']) {
		camera_eye.value[0] -= prox_speed_forward * forward_direction;
	}
	if (keyboard_state->keyboard['s']) {
		camera_eye.value[0] += prox_speed_forward * forward_direction;
	}
	if (keyboard_state->keyboard['a']) {
		camera_eye.value[0] += prox_speed_lateral * left_direction;
	}
	if (keyboard_state->keyboard['d']) {
		camera_eye.value[0] -= prox_speed_lateral * left_direction;
	}
	
	if (keyboard_state->keyboard['q']) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_roll_speed.value[0][0] * 1.0f, glm::vec3(0, -1, 0));
	}
	if (keyboard_state->keyboard['e']) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_roll_speed.value[0][0] * -1.0f, glm::vec3(0, -1, 0));
	}

	if (keyboard_state->keyboard['r']) {
		camera_eye.value[0] += prox_speed_vertical * camera_up.value[0];
	}
	if (keyboard_state->keyboard['f']) {
		camera_eye.value[0] -= prox_speed_vertical * camera_up.value[0];
	}
	
	if (keyboard_state->special[GLUT_KEY_UP]) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_pitch_speed.value[0][0] * 1.0f, glm::vec3(1, 0, 0));
	}
	if (keyboard_state->special[GLUT_KEY_DOWN]) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_pitch_speed.value[0][0] * -1.0f, glm::vec3(1, 0, 0));
	}
	if (keyboard_state->special[GLUT_KEY_LEFT]) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_yaw_speed.value[0][0] * 1.0f, glm::vec3(0, 0, -1));
	}
	if (keyboard_state->special[GLUT_KEY_RIGHT]) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_yaw_speed.value[0][0] * -1.0f, glm::vec3(0, 0, -1));
	}

	camera_target.value[0] = camera_eye.value[0] + (camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up.value[0] = camera_orientation * glm::vec4(0, 0, 1, 0);
}

float BulbControlSettings::expo(float value) {
	if (value < 0 && ((int)control_expo_power.value[0][0] % 2 == 0)) {
		return -1.0f * pow(value, control_expo_power.value[0][0]);
	} else {
		return pow(value, control_expo_power.value[0][0]);
	}
}
