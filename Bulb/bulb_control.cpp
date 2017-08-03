#include "bulb_control.h"

BulbControl::BulbControl(int screen_w, int screen_h) {
	reshape(screen_w, screen_h);

	camera_eye = glm::vec3(0.0, 4.0, 10.0);
	camera_target = glm::vec3(0.0, 0.0, 0.0);
	camera_up = glm::vec3(0, 0, 1);
	camera_fov = 1.5;
	camera_orientation = glm::mat4(1.0);

	camera_eye_prev = glm::vec3(camera_eye);
	camera_velocity_prev = glm::vec3(0.0);

	pad = new GamePadXbox(GamePadIndex_One);
}

void BulbControl::reshape(int screen_w, int screen_h) {
	SCREEN_W = screen_w;
	SCREEN_H = screen_h;
	ASPECT = (float)SCREEN_W / (float)SCREEN_H;
}

void BulbControl::load_shader_ex(char *vtxShdName, char *frgShdName) {
	// read through frag shader and concatenate includes

	ofstream frag_temp_file;
	frag_temp_file.open("bulb_temp.frag", ios::out);

	ifstream frag_orig_file;
	frag_orig_file.open(frgShdName, ios::in);

	string current_line;
	while (getline(frag_orig_file, current_line)) {
		if (current_line.substr(0, 8) == "#include") {
			string include_file_name = current_line.substr(9);
			include_file_name.erase(0, 1);
			include_file_name.erase(include_file_name.size() - 1);

			ifstream include_file;
			include_file.open(include_file_name, ios::in);

			string include_file_line;
			while (getline(include_file, include_file_line)) {
				frag_temp_file << include_file_line << endl;
				// check for uniforms
			}

			include_file.close();

			int test = 0;
		} else {
			frag_temp_file << current_line << endl;
			// check for uniforms
		}
	}

	frag_orig_file.close();
	frag_temp_file.close();

	load_shader(vtxShdName, "bulb_temp.frag", &program_fp32);
}

void BulbControl::update_program_variables() {
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

float BulbControl::expo(float value) {
	if (value < 0)
		return -1.0f * pow(value, 2);
	return pow(value, 2);
}


void BulbControl::update_gamepad() {
	if (pad->is_connected()) {
		pad->update();

		glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
		glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

		float avg_dist = 0.01f; //glm::max(get_avg_dist(), 0.0001f);
		float move_amount = 0.02f * avg_dist;
		
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

void BulbControl::keyboard_down(unsigned char key, int x, int y) {
	glm::vec3 forward_direction = glm::vec3(camera_orientation * glm::vec4(0, 1, 0, 0));
	glm::vec3 left_direction = glm::vec3(camera_orientation * glm::vec4(1, 0, 0, 0));

		
	float avg_dist = 0.01f; //glm::max(get_avg_dist(), 0.0001f);

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