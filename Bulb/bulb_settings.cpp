#include "bulb_settings.h"

BulbControlSettings::BulbControlSettings() {
	camera_eye = glm::vec3(0.0, 4.0, 10.0);
	camera_target = glm::vec3(0.0, 0.0, 0.0);
	camera_up = glm::vec3(0, 0, 1);
	camera_orientation = glm::mat4(1.0);

	//camera_eye_prev = glm::vec3(camera_eye);
	//camera_velocity_prev = glm::vec3(0.0);

	camera_prox_target = 1.0f;
	camera_prox = 1.0f;

	camera_fov[0] = 1.5f;
	camera_fov[1] = 0.0f;
	camera_fov[2] = 4.0f;
	camera_fov[3] = camera_fov[2] - camera_fov[1];

	control_expo_power[0] = 2;
	control_expo_power[1] = 0;
	control_expo_power[2] = 10;
	control_expo_power[3] = control_expo_power[2] - control_expo_power[1];

	control_move_speed_forward[0] = 0.015f;
	control_move_speed_forward[1] = 0.0f;
	control_move_speed_forward[2] = 0.1f;
	control_move_speed_forward[3] = control_move_speed_forward[2] - control_move_speed_forward[1];

	control_move_speed_lateral[0] = 0.010f;
	control_move_speed_lateral[1] = 0.0f;
	control_move_speed_lateral[2] = 0.1f;
	control_move_speed_lateral[3] = control_move_speed_lateral[2] - control_move_speed_lateral[1];

	control_move_speed_vertical[0] = 0.010f;
	control_move_speed_vertical[1] = 0.0f;
	control_move_speed_vertical[2] = 0.1f;
	control_move_speed_vertical[3] = control_move_speed_vertical[2] - control_move_speed_vertical[1];

	control_pitch_speed[0] = 0.03f;
	control_pitch_speed[1] = 0.0f;
	control_pitch_speed[2] = 0.1f;
	control_pitch_speed[3] = control_pitch_speed[2] - control_pitch_speed[1];
	
	control_roll_speed[0] = 0.06f;
	control_roll_speed[1] = 0.0f;
	control_roll_speed[2] = 0.1f;
	control_roll_speed[3] = control_roll_speed[2] - control_roll_speed[1];
	
	control_yaw_speed[0] = 0.03f;
	control_yaw_speed[1] = 0.0f;
	control_yaw_speed[2] = 0.1f;
	control_yaw_speed[3] = control_yaw_speed[2] - control_yaw_speed[1];

	control_vibrate[0] = true;
	control_vibrate[1] = false;
	control_vibrate[2] = true;
	control_vibrate[3] = true;
}

void BulbControlSettings::write_to_save_file(ofstream &save_file) {
	char value_chars[100];

	string camera_vec3_names[] = {"CAMERA_EYE", "CAMERA_TARGET", "CAMERA_UP"};
	glm::vec3 *camera_vec3_vars[] = {&camera_eye, &camera_target, &camera_up};

	for (int i = 0; i < 3; i++) {
		memset(value_chars, 0, 100);
		sprintf_s(value_chars, "%f,%f,%f|", (*camera_vec3_vars[i])[0], (*camera_vec3_vars[i])[1], (*camera_vec3_vars[i])[2]);
		save_file << "CV|" << camera_vec3_names[i] << "|" << string(value_chars) << endl;
	}

	string mat_string = "";
	for (int i = 0; i < 4; i++) {
		memset(value_chars, 0, 100);
		sprintf_s(value_chars, "%f,%f,%f,%f|", camera_orientation[i][0], camera_orientation[i][1], camera_orientation[i][2], camera_orientation[i][3]);
		mat_string += string(value_chars);
	}
	save_file << "CV|" << "CAMERA_ORIENTATION" << "|" << mat_string << endl;

	// floats
	string float_names[] = {"CAMERA_FOV", "CONTROL_MOVE_SPEED_FORWARD", "CONTROL_MOVE_SPEED_LATERAL", "CONTROL_MOVE_SPEED_VERTICAL", "CONTROL_PITCH_SPEED", "CONTROL_ROLL_SPEED", "CONTROL_YAW_SPEED"};
	float* float_vars[] = {camera_fov, control_move_speed_forward, control_move_speed_lateral, control_move_speed_vertical, control_pitch_speed, control_roll_speed, control_yaw_speed};

	for (int i = 0; i < 7; i++) {
		memset(value_chars, 0, 100);
		sprintf_s(value_chars, "%f|%f|%f|%f|", float_vars[i][0], float_vars[i][1], float_vars[i][2], float_vars[i][3]);
		save_file << "CV|" << float_names[i] << "|" << string(value_chars) << endl;
	}

	// int
	memset(value_chars, 0, 100);
	sprintf_s(value_chars, "%d|%d|%d|%d|", control_expo_power[0], control_expo_power[1], control_expo_power[2], control_expo_power[3]);
	save_file << "CV|" << "CONTROL_EXPO_POWER" << "|" << string(value_chars) << endl;

	// bool
	memset(value_chars, 0, 100);
	sprintf_s(value_chars, "%d|%d|%d|%d|", control_vibrate[0], control_vibrate[1], control_vibrate[2], control_vibrate[3]);
	save_file << "CV|" << "CONTROL_VIBRATE" << "|" << string(value_chars) << endl;
}

void BulbControlSettings::read_from_save_file(ifstream &save_file) {
	save_file.clear();
	save_file.seekg(0, ios::beg);

	string save_file_line;
	while (getline(save_file, save_file_line)) {
		if (save_file_line.substr(0, 2) == "CV") {
			vector<string> line_split = split_string(save_file_line, "|");
			
			glm::vec4 value;

			if (line_split[1] == "CAMERA_EYE") {
				stovec(line_split[2], value);
				camera_eye = glm::vec3(value.x, value.y, value.z);
			}

			if (line_split[1] == "CAMERA_TARGET") {
				stovec(line_split[2], value);
				camera_target = glm::vec3(value.x, value.y, value.z);
			}

			if (line_split[1] == "CAMERA_UP") {
				stovec(line_split[2], value);
				camera_up = glm::vec3(value.x, value.y, value.z);
			}

			if (line_split[1] == "CAMERA_ORIENTATION") {
				for (int i = 0; i < 4; i++) {
					stovec(line_split[2+i], value);
					camera_orientation[i] = glm::vec4(value);
				}
			}

			if (line_split[1] == "CAMERA_FOV") {
				for (int i = 0; i < 4; i++) {
					camera_fov[i] = stof(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_MOVE_SPEED_FORWARD") {
				for (int i = 0; i < 4; i++) {
					control_move_speed_forward[i] = stof(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_MOVE_SPEED_LATERAL") {
				for (int i = 0; i < 4; i++) {
					control_move_speed_lateral[i] = stof(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_MOVE_SPEED_VERTICAL") {
				for (int i = 0; i < 4; i++) {
					control_move_speed_vertical[i] = stof(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_PITCH_SPEED") {
				for (int i = 0; i < 4; i++) {
					control_pitch_speed[i] = stof(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_ROLL_SPEED") {
				for (int i = 0; i < 4; i++) {
					control_roll_speed[i] = stof(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_YAW_SPEED") {
				for (int i = 0; i < 4; i++) {
					control_yaw_speed[i] = stof(line_split[2+i]);
				}
			}
			
			if (line_split[1] == "CONTROL_EXPO_POWER") {
				for (int i = 0; i < 4; i++) {
					control_expo_power[i] = stoi(line_split[2+i]);
				}
			}
			if (line_split[1] == "CONTROL_VIBRATE") {
				for (int i = 0; i < 4; i++) {
					control_vibrate[i] = (stoi(line_split[2+i]) == 1);
				}
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
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

	float prox_speed_forward = control_move_speed_forward[0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_lateral = control_move_speed_lateral[0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_vertical = control_move_speed_vertical[0] * glm::max(camera_prox, 0.0001f);

	// forward
	if (!sticks_only) {
		camera_eye -= (prox_speed_forward * (expo(state->rt) - expo(state->lt))) * forward_direction;
	}

	// vertical
	camera_eye += (prox_speed_vertical * expo(state->lstick_y)) * camera_up;

	// lateral & yaw
	if (state->buttons[GamePad_Button_LEFT_SHOULDER] || state->buttons[GamePad_Button_RIGHT_SHOULDER]) {
		camera_eye -= (prox_speed_lateral * expo(state->lstick_x)) * left_direction;
	} else {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_yaw_speed[0] * expo(state->lstick_x), glm::vec3(0, 0, -1));
	}

	// pitch
	camera_orientation *= glm::rotate(glm::mat4(1.0), control_pitch_speed[0] * expo(state->rstick_y), glm::vec3(1, 0, 0));

	// roll
	camera_orientation *= glm::rotate(glm::mat4(1.0), control_roll_speed[0] * expo(state->rstick_x), glm::vec3(0, -1, 0));

	// update
	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));

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

void BulbControlSettings::camera_keyboard_update(int key) {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

	float prox_speed_forward = control_move_speed_forward[0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_lateral = control_move_speed_lateral[0] * glm::max(camera_prox, 0.0001f);
	float prox_speed_vertical = control_move_speed_vertical[0] * glm::max(camera_prox, 0.0001f);

	if (key == 'w') {
		camera_eye -= prox_speed_forward * forward_direction;
	}
	if (key == 's') {
		camera_eye += prox_speed_forward * forward_direction;
	}
	if (key == 'a') {
		camera_eye += prox_speed_lateral * left_direction;
	}
	if (key == 'd') {
		camera_eye -= prox_speed_lateral * left_direction;
	}
	
	if (key == 'q') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_roll_speed[0] * 1.0f, glm::vec3(0, -1, 0));
	}
	if (key == 'e') {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_roll_speed[0] * -1.0f, glm::vec3(0, -1, 0));
	}

	if (key == 'r') {
		camera_eye += prox_speed_vertical * camera_up;
	}
	if (key == 'f') {
		camera_eye -= prox_speed_vertical * camera_up;
	}
	
	if (key == GLUT_KEY_UP) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_pitch_speed[0] * 1.0f, glm::vec3(1, 0, 0));
	}
	if (key == GLUT_KEY_DOWN) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_pitch_speed[0] * -1.0f, glm::vec3(1, 0, 0));
	}
	if (key == GLUT_KEY_LEFT) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_yaw_speed[0] * 1.0f, glm::vec3(0, 0, -1));
	}
	if (key == GLUT_KEY_RIGHT) {
		camera_orientation *= glm::rotate(glm::mat4(1.0), control_yaw_speed[0] * -1.0f, glm::vec3(0, 0, -1));
	}

	camera_target = camera_eye + glm::vec3(camera_orientation * glm::vec4(0, -1, 0, 0));
	camera_up = glm::vec3(camera_orientation * glm::vec4(0, 0, 1, 0));
}

float BulbControlSettings::expo(float value) {
	if (value < 0 && (control_expo_power[0] % 2 == 0)) {
		return -1.0f * pow(value, control_expo_power[0]);
	} else {
		return pow(value, control_expo_power[0]);
	}
}

void BulbControlSettings::adjust_variable(float normalized_amount, int variable) {
	float step = 0.0f;
	float resolution = 100.0f;

	if (variable == 0) { // "camera_fov"'
		step = camera_fov[3] / resolution;
		camera_fov[0] = glm::clamp(camera_fov[0] + (normalized_amount * step), camera_fov[1], camera_fov[2]);
	}
	if (variable == 1) { // "control_expo_power"
		step = control_expo_power[3] / resolution;
		control_expo_power[0] = glm::clamp(control_expo_power[0] + (int)(normalized_amount * step), control_expo_power[1], control_expo_power[2]);
	}
	if (variable == 2) { // "control_move_speed_forward"
		step = control_move_speed_forward[3] / resolution;
		control_move_speed_forward[0] = glm::clamp(control_move_speed_forward[0] + (normalized_amount * step), control_move_speed_forward[1], control_move_speed_forward[2]);
	}
	if (variable == 3) { // "control_move_speed_lateral"
		step = control_move_speed_lateral[3] / resolution;
		control_move_speed_lateral[0] = glm::clamp(control_move_speed_lateral[0] + (normalized_amount * step), control_move_speed_lateral[1], control_move_speed_lateral[2]);
	}
	if (variable == 4) { // "control_move_speed_vertical"
		step = control_move_speed_vertical[3] / resolution;
		control_move_speed_vertical[0] = glm::clamp(control_move_speed_vertical[0] + (normalized_amount * step), control_move_speed_vertical[1], control_move_speed_vertical[2]);
	}
	if (variable == 5) { // "control_pitch_speed"
		step = control_pitch_speed[3] / resolution;
		control_pitch_speed[0] = glm::clamp(control_pitch_speed[0] + (normalized_amount * step), control_pitch_speed[1], control_pitch_speed[2]);
	}
	if (variable == 6) { // "control_roll_speed"
		step = control_roll_speed[3] / resolution;
		control_roll_speed[0] = glm::clamp(control_roll_speed[0] + (normalized_amount * step), control_roll_speed[1], control_roll_speed[2]);
	}
	if (variable == 7) { // "control_yaw_speed"
		step = control_yaw_speed[3] / resolution;
		control_yaw_speed[0] = glm::clamp(control_yaw_speed[0] + (normalized_amount * step), control_yaw_speed[1], control_yaw_speed[2]);
	}
	if (variable == 8) { // "control_vibrate"
		if (abs(normalized_amount) == 1.0f) control_vibrate[0] = !control_vibrate[0];
	}
}

string BulbControlSettings::get_variable_name(int variable) {
	string control_variable_names[] = {"camera_fov", "control_expo_power", "control_move_speed_forward", "control_move_speed_lateral", "control_move_speed_vertical", "control_pitch_speed", "control_roll_speed", "control_yaw_speed", "control_vibrate"};
	return control_variable_names[variable];
}

string BulbControlSettings::get_variable_value(int variable) {
	char text[50];

	if (variable == 0) sprintf_s(text, "%f", camera_fov[0]);
	if (variable == 1) sprintf_s(text, "%d", control_expo_power[0]);
	if (variable == 2) sprintf_s(text, "%f", control_move_speed_forward[0]);
	if (variable == 3) sprintf_s(text, "%f", control_move_speed_lateral[0]);
	if (variable == 4) sprintf_s(text, "%f", control_move_speed_vertical[0]);
	if (variable == 5) sprintf_s(text, "%f", control_pitch_speed[0]);
	if (variable == 6) sprintf_s(text, "%f", control_roll_speed[0]);
	if (variable == 7) sprintf_s(text, "%f", control_yaw_speed[0]);
	if (variable == 8) sprintf_s(text, (control_vibrate[0]) ? "true" : "false");

	return string(text);
}

BulbSettings::BulbSettings(BulbShader *bulb_shader, BulbControlSettings *control_settings, DrawingTools *drawing_tools) {
	this->bulb_shader = bulb_shader;
	this->control_settings = control_settings;
	this->drawing_tools = drawing_tools;

	settings_open = false;
	menu_open = 0;

	settings_font = GLUT_BITMAP_HELVETICA_12;
	font_height = 20;


	load_menu_item_highlight = 0;
	load_menu_item_selected = false;
	load_menu_item_sub_highlight = 0; 

	control_menu_item_highlight = 0;
	control_menu_item_selected = false;
	control_menu_item_sub_highlight = 0;

	shader_menu_item_highlight = 0;
	shader_menu_item_selected = false;
	shader_menu_item_sub_highlight = 0;
	shader_menu_item_sub_selected = false;
	shader_menu_item_sub_animate_highlight = 0;
	shader_menu_category = 0;

	main_menu_item_highlight = 0;
	main_menu_item_selected = false;
}

float BulbSettings::settings_expo(float value) {
	int power = 2;

	if (value < 0 && (power % 2 == 0)) {
		return -1.0f * pow(value, power);
	} else {
		return pow(value, power);
	}
}

void BulbSettings::draw_slider_bar(float x, float y, float bar_width, float bar_height, float current, float min, float max, string format, bool selected) {
	float range = max - min;
	float pos = (current - min) / range;
	pos *= bar_width;

	if (selected)
		glColor4f(0.4f,0.4f,0.4f,1.0f);
	else
		glColor4f(0.2f,0.2f,0.2f,1.0f);

	drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

	glColor4f(0.6f,0.6f,0.6f,1.0f);
	drawing_tools->rectangle_filled(x + pos, y, 4, bar_height);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	char text[50];
	sprintf_s(text, format.c_str(), current);
	drawing_tools->text(x + 5, y + 5, settings_font, string(text));
}

void BulbSettings::draw_slider_bar(float x, float y, float bar_width, float bar_height, float *values, string format, bool selected) {
	draw_slider_bar(x, y, bar_width, bar_height, values[0], values[1], values[2], format, selected);
}

void BulbSettings::draw_slider_bar(float x, float y, float bar_width, float bar_height, int *values, string format, bool selected) {
	draw_slider_bar(x, y, bar_width, bar_height, (float)values[0], (float)values[1], (float)values[2], format, selected);
}

void BulbSettings::control_menu_draw() {
	if (control_menu_item_selected) {
		float bar_width = 500;
		float bar_height = (float)font_height;

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

		string variable_string = "Control : " + control_settings->get_variable_name(control_menu_item_highlight);
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 5, settings_font, variable_string);

		if (control_menu_item_highlight == 0) { // "camera_fov"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->camera_fov, "%f", true);
		}
		if (control_menu_item_highlight == 1) { // "control_expo_power"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_expo_power, "%0.0f", true);
		}
		if (control_menu_item_highlight == 2) { // "control_move_speed_forward"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_move_speed_forward, "%f", true);
		}
		if (control_menu_item_highlight == 3) { // "control_move_speed_lateral"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_move_speed_lateral, "%f", true);
		}
		if (control_menu_item_highlight == 4) { // "control_move_speed_vertical"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_move_speed_vertical, "%f", true);
		}
		if (control_menu_item_highlight == 5) { // "control_pitch_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_pitch_speed, "%f", true);
		}
		if (control_menu_item_highlight == 6) { // "control_roll_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_roll_speed, "%f", true);
		}
		if (control_menu_item_highlight == 7) { // "control_yaw_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings->control_yaw_speed, "%f", true);
		}
		if (control_menu_item_highlight == 8) { // "control_vibrate"
			glColor4f(0.4f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, bar_height + 5, bar_width, bar_height);

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, bar_height + 10, settings_font, control_settings->get_variable_value(control_menu_item_highlight));
		}
	} else {
		int items_size = 9;

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(items_size + 1.0f) * font_height + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (float)(items_size + 1.0f) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (control_menu_item_highlight + 1) * font_height, 250, font_height);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "Control Settings");

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < items_size; i++) {
			string variable_string = "Control : " + control_settings->get_variable_name(i);
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, variable_string);
			drawing_tools->text(255, (i + 1) * font_height + 5, settings_font, control_settings->get_variable_value(i));
		}
	}
}

void BulbSettings::control_menu_gamepad_update(GamePadState *state) {
	if (control_menu_item_selected) {
		if (state->pressed(GamePad_Button_B)) {
			control_menu_item_selected = false;
		}

		float dpad_step = (state->pressed(GamePad_Button_DPAD_RIGHT) - state->pressed(GamePad_Button_DPAD_LEFT));
		float trigger_step = (settings_expo(state->rt) - settings_expo(state->lt));

		control_settings->adjust_variable(dpad_step + trigger_step, control_menu_item_highlight);
	} else {
		if (state->pressed(GamePad_Button_A)) {
			control_menu_item_selected = true;
			control_menu_item_sub_highlight = 0;
		}
		if (state->pressed(GamePad_Button_B)) {
			menu_open = 0; // main menu
		}

		if (state->pressed(GamePad_Button_DPAD_UP)) control_menu_item_highlight++;
		if (state->pressed(GamePad_Button_DPAD_DOWN)) control_menu_item_highlight--;
		int items_size = 9;
		control_menu_item_highlight = glm::clamp(control_menu_item_highlight, 0, items_size-1);
	}
}

void BulbSettings::control_menu_keyboard_update(int key) {
	if (control_menu_item_selected) {
		if (key == 27) {
			control_menu_item_selected = false;
		}

		float dpad_step = ((key == GLUT_KEY_RIGHT) - (key == GLUT_KEY_LEFT));
		float trigger_step = 0.0f; // (settings_expo(state->rt) - settings_expo(state->lt));

		control_settings->adjust_variable(dpad_step + trigger_step, control_menu_item_highlight);
	} else {
		if (key == 13) {
			control_menu_item_selected = true;
			control_menu_item_sub_highlight = 0;
		}
		if (key == 27) {
			menu_open = 0; // main menu
		}

		if (key == GLUT_KEY_UP) control_menu_item_highlight++;
		if (key == GLUT_KEY_DOWN) control_menu_item_highlight--;
		int items_size = 9;
		control_menu_item_highlight = glm::clamp(control_menu_item_highlight, 0, items_size-1);
	}
}

void BulbSettings::shader_menu_draw() {
	if (shader_menu_item_selected) {
		int actual_highlight_index = bulb_shader->shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];

		ShaderVariable selected_variable = bulb_shader->shader_variables[actual_highlight_index];
		SHADER_VAR_TYPE selected_type = selected_variable.var_type;

		float bar_width = 500;
		float bar_height = (float)font_height;

		glColor3f(0.2f,0.2f,0.2f);
		drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

		glColor3f(0.6f,0.6f,0.6f);
		drawing_tools->text(5, 5, settings_font, selected_variable.category + " : " + selected_variable.name);

		int sub_var_count[6] = {1, 1, 1, 2, 3, 4};

		float var_bg_normal_color = (shader_menu_item_sub_selected) ? 0.1f : 0.3f;
		float var_bg_selected_color = (shader_menu_item_sub_selected) ? 0.2f : 0.4f;
		float var_fg_animate_color = (shader_menu_item_sub_selected) ? 0.3f : 0.5f;
		float var_fg_color = (shader_menu_item_sub_selected) ? 0.6f : 1.0f;

		for (int i = 0; i < sub_var_count[selected_variable.var_type]; i++) {
			float x = 0;
			float y = (bar_height * (i+1)) + 5;

			float var_bg_color = (i == shader_menu_item_sub_highlight) ? var_bg_selected_color : var_bg_normal_color;
			glColor3f(var_bg_color,var_bg_color,var_bg_color);
			drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

			// draw animate stuff
			if (selected_variable.animate_enable[i]) {
				float var_fg_anmiate_color = (i == shader_menu_item_sub_highlight) ? var_bg_selected_color : var_bg_normal_color;
				glColor3f(var_fg_animate_color,var_fg_animate_color,var_fg_animate_color);
				float a_scale = selected_variable.animate_values[i][1] * bar_width;
				float a_offset = ((selected_variable.animate_values[i][2] + 1.0f) / 2.0f) * bar_width;
				drawing_tools->rectangle_filled(x + a_offset - (a_scale / 2.0f), y + (bar_height / 2.0f) - 2.0f, a_scale, 4.0f);
			}

			// draw slider
			glColor3f(var_fg_color,var_fg_color,var_fg_color);
			float pos = ((selected_variable.value[0][i] - selected_variable.value[1][i]) / selected_variable.value[3][i]) * bar_width;
			drawing_tools->rectangle_filled(x + pos - 2.0f, y, 4.0f, bar_height);

			// draw text
			glColor3f(var_fg_color,var_fg_color,var_fg_color);
			drawing_tools->text(x + 5, y + 5, settings_font, selected_variable.get_string(i));
		}

		// draw color bar
		if (selected_variable.is_color) {
			glm::vec4 variable_color = selected_variable.get_color();
			glColor3f(variable_color.r, variable_color.g, variable_color.b);
			drawing_tools->rectangle_filled(0, (bar_height*(sub_var_count[selected_variable.var_type] + 1)) + 5, bar_width, bar_height);
		}

		// animate
		if (selected_variable.animate_enable[shader_menu_item_sub_highlight]) {
			float animate_sliders_offset = sub_var_count[selected_variable.var_type] + ((selected_variable.is_color) ? 1 : 0);
			
			float var_bg_normal_color = (!shader_menu_item_sub_selected) ? 0.1f : 0.3f;
			float var_bg_selected_color = (!shader_menu_item_sub_selected) ? 0.2f : 0.4f;
			float var_fg_color = (!shader_menu_item_sub_selected) ? 0.6f : 1.0f;

			float mins[3] = {0.0f, 0.0f, -1.0f};
			float ranges[3] = {1.0f, 1.0f, 2.0f};

			for (int i = 0; i < 3; i++) {
				float x = 0;
				float y = (bar_height * (i+1+animate_sliders_offset)) + 5;

				float var_bg_color = (i == shader_menu_item_sub_animate_highlight) ? var_bg_selected_color : var_bg_normal_color;
				glColor3f(var_bg_color,var_bg_color,var_bg_color);
				drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

				// draw slider
				glColor3f(var_fg_color,var_fg_color,var_fg_color);
				float pos = ((selected_variable.animate_values[shader_menu_item_sub_highlight][i] - mins[i]) / ranges[i]) * bar_width;
				drawing_tools->rectangle_filled(x + pos - 2.0f, y, 4.0f, bar_height);

				// draw text
				glColor3f(var_fg_color,var_fg_color,var_fg_color);
				drawing_tools->text(x + 5, y + 5, settings_font, selected_variable.get_string_animate(shader_menu_item_sub_highlight, i));
			}
		}
	} else {
		int category_size = (int)bulb_shader->shader_categories_indexes[shader_menu_category].size();

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(category_size + 1.0f) * font_height + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (float)(category_size + 1.0f) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (shader_menu_item_highlight + 1) * font_height, 250, font_height);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "< " + bulb_shader->shader_categories[shader_menu_category] + " >");

		for (int i = 0; i < category_size; i++) {
			int actual_index = bulb_shader->shader_categories_indexes[shader_menu_category][i];
			ShaderVariable *current_variable = &bulb_shader->shader_variables[actual_index];

			string variable_string = current_variable->category + " : " + current_variable->name;

			if (current_variable->is_color) {
				glm::vec4 variable_color = current_variable->get_color();
				glColor3f(variable_color.r, variable_color.g, variable_color.b);
				drawing_tools->rectangle_filled(250, (i + 1) * font_height, 200, font_height);
			}

			glColor3f(1.0f,1.0f,1.0f);
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, variable_string);
			if (current_variable->is_bright()) {
				glColor3f(0.0f,0.0f,0.0f);
			}
			drawing_tools->text(255, (i + 1) * font_height + 5, settings_font, current_variable->get_string());
		}
	}
}

void BulbSettings::shader_menu_gamepad_update(GamePadState *state) {
	if (shader_menu_item_selected) {
		int actual_highlight_index = bulb_shader->shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];
		ShaderVariable *selected_variable = &bulb_shader->shader_variables[actual_highlight_index];

		if (state->pressed(GamePad_Button_X)) {
			selected_variable->animate_enable[shader_menu_item_sub_highlight] = !selected_variable->animate_enable[shader_menu_item_sub_highlight];
			shader_menu_item_sub_selected = selected_variable->animate_enable[shader_menu_item_sub_highlight]; // auto enter menu
		}

		float dpad_step = (state->pressed(GamePad_Button_DPAD_RIGHT) - state->pressed(GamePad_Button_DPAD_LEFT));
		float trigger_step = (settings_expo(state->rt) - settings_expo(state->lt));

		if (shader_menu_item_sub_selected) {
			if (state->pressed(GamePad_Button_B)) {
				shader_menu_item_sub_selected = false;
			}

			if (state->pressed(GamePad_Button_DPAD_UP)) shader_menu_item_sub_animate_highlight++;
			if (state->pressed(GamePad_Button_DPAD_DOWN)) shader_menu_item_sub_animate_highlight--;

			selected_variable->adjust_animate(dpad_step + trigger_step, shader_menu_item_sub_highlight, shader_menu_item_sub_animate_highlight);
		} else {
			if (state->pressed(GamePad_Button_A) && selected_variable->animate_enable[shader_menu_item_sub_highlight]) {
				shader_menu_item_sub_selected = true;
			}
			if (state->pressed(GamePad_Button_B)) {
				shader_menu_item_selected = false;
			}

			if (state->pressed(GamePad_Button_Y)) {
				selected_variable->set_hsv_mode(!selected_variable->hsv_mode);
			}

			if (state->pressed(GamePad_Button_DPAD_UP)) shader_menu_item_sub_highlight++;
			if (state->pressed(GamePad_Button_DPAD_DOWN)) shader_menu_item_sub_highlight--;

			selected_variable->adjust_variable(dpad_step + trigger_step, shader_menu_item_sub_highlight);
		}
	} else {
		if (state->pressed(GamePad_Button_A)) {
			shader_menu_item_selected = true;
			shader_menu_item_sub_highlight = 0;
		}
		if (state->pressed(GamePad_Button_B)) {
			menu_open = 0; // main menu
		}

		if (state->pressed(GamePad_Button_RIGHT_SHOULDER)) shader_menu_category++;
		if (state->pressed(GamePad_Button_LEFT_SHOULDER)) shader_menu_category--;
		shader_menu_category = glm::clamp(shader_menu_category, 0, (int)bulb_shader->shader_categories.size() - 1);

		if (state->pressed(GamePad_Button_DPAD_UP)) shader_menu_item_highlight++;
		if (state->pressed(GamePad_Button_DPAD_DOWN)) shader_menu_item_highlight--;
		int category_size = (int)bulb_shader->shader_categories_indexes[shader_menu_category].size();
		shader_menu_item_highlight = glm::clamp(shader_menu_item_highlight, 0, category_size-1);
	}
}

void BulbSettings::shader_menu_keyboard_update(int key) {
	if (shader_menu_item_selected) {
		int actual_highlight_index = bulb_shader->shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];
		ShaderVariable *selected_variable = &bulb_shader->shader_variables[actual_highlight_index];

		if (key == 'x') {
			selected_variable->animate_enable[shader_menu_item_sub_highlight] = !selected_variable->animate_enable[shader_menu_item_sub_highlight];
			shader_menu_item_sub_selected = selected_variable->animate_enable[shader_menu_item_sub_highlight]; // auto enter menu
		}

		float dpad_step = ((key == GLUT_KEY_RIGHT) - (key == GLUT_KEY_LEFT));
		float trigger_step = 0.0f; //(settings_expo(state->rt) - settings_expo(state->lt));

		if (shader_menu_item_sub_selected) {
			if (key == 27) {
				shader_menu_item_sub_selected = false;
			}

			if (key == GLUT_KEY_UP) shader_menu_item_sub_animate_highlight++;
			if (key == GLUT_KEY_DOWN) shader_menu_item_sub_animate_highlight--;

			selected_variable->adjust_animate(dpad_step + trigger_step, shader_menu_item_sub_highlight, shader_menu_item_sub_animate_highlight);
		} else {
			if ((key == 13) && selected_variable->animate_enable[shader_menu_item_sub_highlight]) {
				shader_menu_item_sub_selected = true;
			}
			if (key == 27) {
				shader_menu_item_selected = false;
			}

			if (key == 'y') {
				selected_variable->set_hsv_mode(!selected_variable->hsv_mode);
			}

			if (key == GLUT_KEY_UP) shader_menu_item_sub_highlight++;
			if (key == GLUT_KEY_DOWN) shader_menu_item_sub_highlight--;

			selected_variable->adjust_variable(dpad_step + trigger_step, shader_menu_item_sub_highlight);
		}
	} else {
		if (key == 13) {
			shader_menu_item_selected = true;
			shader_menu_item_sub_highlight = 0;
		}
		if (key == 27) {
			menu_open = 0; // main menu
		}

		if (key == GLUT_KEY_RIGHT) shader_menu_category++;
		if (key == GLUT_KEY_LEFT) shader_menu_category--;
		shader_menu_category = glm::clamp(shader_menu_category, 0, (int)bulb_shader->shader_categories.size() - 1);

		if (key == GLUT_KEY_UP) shader_menu_item_highlight++;
		if (key == GLUT_KEY_DOWN) shader_menu_item_highlight--;
		int category_size = (int)bulb_shader->shader_categories_indexes[shader_menu_category].size();
		shader_menu_item_highlight = glm::clamp(shader_menu_item_highlight, 0, category_size-1);
	}
}

void BulbSettings::main_menu_draw() {
	string menu_items[] = {"Shader Settings", "Control Settings", "Load", "Save"};
	int menu_items_size = 4;
		
	glColor4f(0.2f,0.2f,0.2f,1.0f);
	drawing_tools->rectangle_filled(0, 0, 250, (float)(menu_items_size) * font_height + 5);

	glColor4f(0.4f,0.4f,0.4f,1.0f);
	drawing_tools->rectangle_filled(0, main_menu_item_highlight * font_height, 250, font_height);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	for (int i = 0; i < menu_items_size; i++) {	
		drawing_tools->text(5, i * font_height + 5, settings_font, menu_items[i]);
	}
}

void BulbSettings::main_menu_gamepad_update(GamePadState *state) {
	int menu_items_size = 4;

	if (state->pressed(GamePad_Button_A)) {
		main_menu_item_selected = true;
		menu_open = main_menu_item_highlight + 1;

		if (main_menu_item_highlight == 3) {
			main_menu_item_selected = false;
			menu_open = 0;

			// save (todo: make this better)

			time_t rawtime;
			struct tm * timeinfo;
			char buffer[100];

			time(&rawtime);
			timeinfo = localtime(&rawtime);

			strftime(buffer, sizeof(buffer), "%j %H_%M_%S", timeinfo);

			save_save_file("BulbSaves\\" + string(buffer) + ".bulbsave");
		}
	}
	if (state->pressed(GamePad_Button_B)) {
		settings_open = false;
	}
	
	if (state->pressed(GamePad_Button_DPAD_UP)) main_menu_item_highlight++;
	if (state->pressed(GamePad_Button_DPAD_DOWN)) main_menu_item_highlight--;
	main_menu_item_highlight = glm::clamp(main_menu_item_highlight, 0, menu_items_size - 1);
}

void BulbSettings::main_menu_keyboard_update(int key) {
	int menu_items_size = 4;

	if (key == 13) {
		main_menu_item_selected = true;
		menu_open = main_menu_item_highlight + 1;

		if (main_menu_item_highlight == 3) {
			main_menu_item_selected = false;
			menu_open = 0;

			// save (todo: make this better)

			time_t rawtime;
			struct tm * timeinfo;
			char buffer[100];

			time(&rawtime);
			timeinfo = localtime(&rawtime);

			strftime(buffer, sizeof(buffer), "%j %H_%M_%S", timeinfo);

			save_save_file("BulbSaves\\" + string(buffer) + ".bulbsave");
		}
	}
	if (key == 27) {
		settings_open = false;
	}

	if (key == GLUT_KEY_UP) main_menu_item_highlight++;
	if (key == GLUT_KEY_DOWN) main_menu_item_highlight--;
	main_menu_item_highlight = glm::clamp(main_menu_item_highlight, 0, menu_items_size - 1);
}

void BulbSettings::load_menu_draw() {
	if (!load_menu_item_selected) {
		string menu_items[] = {"Fractals", "Saves"};
		int menu_items_size = 2;
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(menu_items_size) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, load_menu_item_highlight * font_height, 250, font_height);
	
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {	
			drawing_tools->text(5, i * font_height + 5, settings_font, menu_items[i]);
		}
	} else if (load_menu_item_highlight == 0) {
		int menu_items_size = (int)bulb_shader->fractal_files.size();
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(menu_items_size) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, load_menu_item_sub_highlight * font_height, 250, font_height);
	
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {
			string file_text = bulb_shader->fractal_files[i];
			if (bulb_shader->fractal_files[i] == bulb_shader->fractal_file) {
				file_text += " (Loaded)";
			}
			drawing_tools->text(5, i * font_height + 5, settings_font, file_text);
		}
	} else if (load_menu_item_highlight == 1) {
		int menu_items_size = (int)save_files.size();
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(menu_items_size) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, load_menu_item_sub_highlight * font_height, 250, font_height);
	
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {
			string file_text = save_files[i];
			drawing_tools->text(5, i * font_height + 5, settings_font, file_text);
		}
	}
}

void BulbSettings::load_menu_gamepad_update(GamePadState *state) {
	if (!load_menu_item_selected) {
		int menu_items_size = 2;
		if (state->pressed(GamePad_Button_A)) {
			load_menu_item_selected = true;
			load_menu_item_sub_highlight = 0;

			if (load_menu_item_highlight == 1) update_save_files();
		}
		if (state->pressed(GamePad_Button_B)) {
			menu_open = 0; // main menu
		}

		if (state->pressed(GamePad_Button_DPAD_UP)) load_menu_item_highlight++;
		if (state->pressed(GamePad_Button_DPAD_DOWN)) load_menu_item_highlight--;
		load_menu_item_highlight = glm::clamp(load_menu_item_highlight, 0, menu_items_size - 1);
	} else {
		if (state->pressed(GamePad_Button_B)) {
			load_menu_item_selected = false;
		}

		if (state->pressed(GamePad_Button_DPAD_UP)) load_menu_item_sub_highlight++;
		if (state->pressed(GamePad_Button_DPAD_DOWN)) load_menu_item_sub_highlight--;

		if (load_menu_item_highlight == 0) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)bulb_shader->fractal_files.size() - 1);
			
			if (state->pressed(GamePad_Button_A)) {
				bulb_shader->fractal_file = bulb_shader->fractal_files[load_menu_item_sub_highlight];
				bulb_shader->load();
			}
		} else if (load_menu_item_highlight == 1) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)save_files.size() - 1);

			if (state->pressed(GamePad_Button_A)) {
				// load save file
			}
		}
	}
}

void BulbSettings::load_menu_keyboard_update(int key) {
	if (!load_menu_item_selected) {
		int menu_items_size = 2;
		if (key == 13) {
			load_menu_item_selected = true;
			load_menu_item_sub_highlight = 0;

			if (load_menu_item_highlight == 1) update_save_files();
		}
		if (key == 27) {
			menu_open = 0; // main menu
		}

		if (key == GLUT_KEY_UP) load_menu_item_highlight++;
		if (key == GLUT_KEY_DOWN) load_menu_item_highlight--;
		load_menu_item_highlight = glm::clamp(load_menu_item_highlight, 0, menu_items_size - 1);
	} else {
		if (key == 27) {
			load_menu_item_selected = false;
		}

		if (key == GLUT_KEY_UP) load_menu_item_sub_highlight++;
		if (key == GLUT_KEY_DOWN) load_menu_item_sub_highlight--;

		if (load_menu_item_highlight == 0) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)bulb_shader->fractal_files.size() - 1);
			
			if (key == 13) {
				bulb_shader->fractal_file = bulb_shader->fractal_files[load_menu_item_sub_highlight];
				bulb_shader->load();
			}
		} else if (load_menu_item_highlight == 1) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)save_files.size() - 1);

			if (key == 13) {
				// load save file
			}
		}
	}
}

void BulbSettings::update_save_files() {
	save_files.clear();

	string directory = "BulbSaves\\*";
	WIN32_FIND_DATA fileData; 
	HANDLE hFind;
	if ( !((hFind = FindFirstFile(directory.c_str(), &fileData)) == INVALID_HANDLE_VALUE) ) {
		while(FindNextFile(hFind, &fileData)) {
			vector<string> file_name_split = split_string(fileData.cFileName, ".");
			if (file_name_split[1] == "BULBSAVE" || file_name_split[1] == "bulbsave") {
				save_files.push_back("BulbSaves\\" + string(fileData.cFileName));
			}
		}
	}
	
	FindClose(hFind);
}

void BulbSettings::draw() {
	glUseProgram(0);

	if (menu_open == 0) {
		main_menu_draw();
	} else if (menu_open == 1) {
		shader_menu_draw();
	} else if (menu_open == 2) {
		control_menu_draw();
	} else if (menu_open == 3) {
		load_menu_draw();
	}
}

void BulbSettings::gamepad_update(GamePadState *state) {
	if (menu_open == 0) {
		main_menu_gamepad_update(state);
	} else if (menu_open == 1) {
		shader_menu_gamepad_update(state);
	} else if (menu_open == 2) {
		control_menu_gamepad_update(state);
	} else if (menu_open == 3) {
		load_menu_gamepad_update(state);
	}
}

void BulbSettings::load_save_file(string save_file_name) {
	ifstream save_file;
	save_file.open(save_file_name, ios::in);

	control_settings->read_from_save_file(save_file);
	bulb_shader->read_from_save_file(save_file);

	save_file.close();
}

void BulbSettings::save_save_file(string save_file_name) {
	ofstream save_file;
	save_file.open(save_file_name);

	control_settings->write_to_save_file(save_file);
	bulb_shader->write_to_save_file(save_file);

	save_file.close();
}

void BulbSettings::keyboard_update(int key) {
	if (menu_open == 0) {
		main_menu_keyboard_update(key);
	} else if (menu_open == 1) {
		shader_menu_keyboard_update(key);
	} else if (menu_open == 2) {
		control_menu_keyboard_update(key);
	} else if (menu_open == 3) {
		load_menu_keyboard_update(key);
	}
}