#include "bulb_settings.h"

BulbControlSettings::BulbControlSettings() {
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

BulbSettings::BulbSettings(vector<ShaderVariable> *shader_variables, DrawingTools *drawing_tools) {
	this->shader_variables = shader_variables;
	this->drawing_tools = drawing_tools;

	settings_open = false;
	menu_open = 0;

	settings_font = GLUT_BITMAP_HELVETICA_12;
	font_height = 20;

	shader_categories.clear();
	shader_categories_indexes.clear();

	control_menu_item_highlight = 0;
	control_menu_item_selected = false;
	control_menu_item_sub_highlight = 0;

	shader_menu_item_highlight = 0;
	shader_menu_item_selected = false;
	shader_menu_item_sub_highlight = 0;
	shader_menu_item_sub_selected = false;
	shader_menu_item_sub_animate_highlight = 0;
	shader_menu_category = 0;
}

void BulbSettings::update_shader_categories() {
	shader_categories.clear();
	shader_categories_indexes.clear();

	for (int i = 0; i < (int)(shader_variables->size()); i++) {
		if (std::find(shader_categories.begin(), shader_categories.end(), (*shader_variables)[i].category) == shader_categories.end()) {
			shader_categories.push_back((*shader_variables)[i].category);
			vector<int> new_category;
			shader_categories_indexes.push_back(new_category);
		}

		int found_index = std::find(shader_categories.begin(), shader_categories.end(), (*shader_variables)[i].category) - shader_categories.begin();
		shader_categories_indexes[found_index].push_back(i);
	}
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

		string variable_string = "Control : " + control_settings.get_variable_name(control_menu_item_highlight);
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 5, settings_font, variable_string);

		if (control_menu_item_highlight == 0) { // "camera_fov"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.camera_fov, "%f", true);
		}
		if (control_menu_item_highlight == 1) { // "control_expo_power"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_expo_power, "%0.0f", true);
		}
		if (control_menu_item_highlight == 2) { // "control_move_speed_forward"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_move_speed_forward, "%f", true);
		}
		if (control_menu_item_highlight == 3) { // "control_move_speed_lateral"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_move_speed_lateral, "%f", true);
		}
		if (control_menu_item_highlight == 4) { // "control_move_speed_vertical"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_move_speed_vertical, "%f", true);
		}
		if (control_menu_item_highlight == 5) { // "control_pitch_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_pitch_speed, "%f", true);
		}
		if (control_menu_item_highlight == 6) { // "control_roll_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_roll_speed, "%f", true);
		}
		if (control_menu_item_highlight == 7) { // "control_yaw_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_yaw_speed, "%f", true);
		}
		if (control_menu_item_highlight == 8) { // "control_vibrate"
			glColor4f(0.4f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, bar_height + 5, bar_width, bar_height);

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, bar_height + 10, settings_font, control_settings.get_variable_value(control_menu_item_highlight));
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
			string variable_string = "Control : " + control_settings.get_variable_name(i);
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, variable_string);
			drawing_tools->text(255, (i + 1) * font_height + 5, settings_font, control_settings.get_variable_value(i));
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

		control_settings.adjust_variable(dpad_step + trigger_step, control_menu_item_highlight);
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

void BulbSettings::shader_menu_draw() {
	if (shader_menu_item_selected) {
		int actual_highlight_index = shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];

		ShaderVariable selected_variable = (*shader_variables)[actual_highlight_index];
		SHADER_VAR_TYPE selected_type = selected_variable.var_type;

		float bar_width = 500;
		float bar_height = (float)font_height;

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 5, settings_font, selected_variable.category + " : " + selected_variable.name);

		int sub_var_count[6] = {1, 1, 1, 2, 3, 4};
		bool is_color = (shader_variable.type == "color" || shader_variable.type == "color4");

		float var_bg_normal_color = (shader_menu_item_sub_selected) ? 0.1f : 0.2f;
		float var_bg_selected_color = (shader_menu_item_sub_selected) ? 0.2f : 0.4f;
		float var_fg_color = (shader_menu_item_sub_selected) ? 0.6f : 1.0f;

		for (int i = 0; i < sub_var_count[shader_variable.var_type]; i++) {
			float x = 0;
			float y = (bar_height * (i+1)) + 5;

			float var_bg_color = (i == shader_menu_item_sub_highlight) ? var_bg_selected_color : var_bg_normal_color;
			glColor3f(var_bg_color,var_bg_color,var_bg_color);
			drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

			
			// draw animate stuff
			if (selected_variable.animate_enable[i]) {
				glColor3f(var_fg_color,var_fg_color,var_fg_color);
				float a_scale = selected_variable.animate_values[i][1] * bar_width;
				float a_offset = ((selected_variable.animate_values[i][2] + 1.0f) / 2.0f) * bar_width;
				drawing_tools->rectangle_filled(x + a_offset - (a_scale / 2.0f), y + (bar_height / 2.0f) - 2.0f, a_scale, 4.0f);
			}

			// draw slider
			glColor3f(var_fg_color,var_fg_color,var_fg_color);
			float pos = ((selected_variable.value[0][i] - selected_variable.value[1][i]) / selected_variable.value[3][i]) * bar_width;
			drawing_tools->rectangle_filled(x + pos - 2.0f, y, 4.0f, bar_height);
		}


		if (selected_type == VAR_BOOL) {
			glColor4f(0.4f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, bar_height + 5, bar_width, bar_height);

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, bar_height + 10, settings_font, selected_variable.get_string());
		}
		if (selected_type == VAR_INT) {
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, selected_variable.var_int[0], selected_variable.var_int[1], selected_variable.var_int[2], "%0.0f", !selected_variable.animate);
		}
		if (selected_type == VAR_FLOAT) {
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, selected_variable.var_float[0], selected_variable.var_float[1], selected_variable.var_float[2], "%f", !selected_variable.animate);
		}
		if (selected_type == VAR_VEC2) {
			animate_bar_offset = 1;
			for (int i = 0; i < 2; i++) {
				draw_slider_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec2[0][i], selected_variable.var_vec2[1][i], selected_variable.var_vec2[2][i], "%f", (i == shader_menu_item_sub_highlight) && !selected_variable.animate);
			}
		}
		if (selected_type == VAR_VEC3) {
			animate_bar_offset = 2;
			for (int i = 0; i < 3; i++) {
				draw_slider_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec3[0][i], selected_variable.var_vec3[1][i], selected_variable.var_vec3[2][i], "%f", (i == shader_menu_item_sub_highlight) && !selected_variable.animate);
			}
			
			if (selected_variable.type == "color") {
				animate_bar_offset++;
				glm::vec3 color = selected_variable.var_vec3[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(0, (bar_height*4) + 5, bar_width, bar_height);
			}
		}
		if (selected_type == VAR_VEC4) {
			animate_bar_offset = 3;
			for (int i = 0; i < 4; i++) {
				draw_slider_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec4[0][i], selected_variable.var_vec4[1][i], selected_variable.var_vec4[2][i], "%f", (i == shader_menu_item_sub_highlight) && !selected_variable.animate);
			}

			if (selected_variable.type == "color4") {
				animate_bar_offset++;
				glm::vec4 color = selected_variable.var_vec4[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(0, (bar_height*5) + 5, bar_width, bar_height);
			}
		}
		if (selected_variable.animate) {
			draw_slider_bar(0, (bar_height*(1+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_speed, 0, 1, "Animate Speed: %f", shader_menu_item_sub_highlight == 0);
			draw_slider_bar(0, (bar_height*(2+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_scale, 0, 1, "Animate Scale: %f", shader_menu_item_sub_highlight == 1);
			draw_slider_bar(0, (bar_height*(3+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_offset, -1, 1, "Animate Offset: %f", shader_menu_item_sub_highlight == 2);
		}
	} else {
		int category_size = (int)shader_categories_indexes[shader_menu_category].size();

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(category_size + 1.0f) * font_height + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (float)(category_size + 1.0f) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (shader_menu_item_highlight + 1) * font_height, 250, font_height);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "< " + shader_categories[shader_menu_category] + " >");

		for (int i = 0; i < category_size; i++) {
			int actual_index = shader_categories_indexes[shader_menu_category][i];

			string variable_string = (*shader_variables)[actual_index].category + " : " + (*shader_variables)[actual_index].name;

			if ((*shader_variables)[actual_index].type == "color") {
				glm::vec3 color = (*shader_variables)[actual_index].var_vec3[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(250, (i + 1) * font_height, 200, font_height);
			} else if ((*shader_variables)[actual_index].type == "color4") {
				glm::vec4 color = (*shader_variables)[actual_index].var_vec4[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(250, (i + 1) * font_height, 200, font_height);
			}

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, variable_string);
			drawing_tools->text(255, (i + 1) * font_height + 5, settings_font, (*shader_variables)[actual_index].get_string());
		}
	}
}

/*
void BulbSettings::shader_menu_draw() {
	if (shader_menu_item_selected) {
		int actual_highlight_index = shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];

		ShaderVariable selected_variable = (*shader_variables)[actual_highlight_index];
		SHADER_VAR_TYPE selected_type = selected_variable.var_type;

		float bar_width = 500;
		float bar_height = (float)font_height;

		int animate_bar_offset = 0;

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

		string variable_string = selected_variable.category + " : " + selected_variable.name;
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 5, settings_font, variable_string);

		if (selected_type == VAR_BOOL) {
			glColor4f(0.4f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, bar_height + 5, bar_width, bar_height);

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, bar_height + 10, settings_font, selected_variable.get_string());
		}
		if (selected_type == VAR_INT) {
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, selected_variable.var_int[0], selected_variable.var_int[1], selected_variable.var_int[2], "%0.0f", !selected_variable.animate);
		}
		if (selected_type == VAR_FLOAT) {
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, selected_variable.var_float[0], selected_variable.var_float[1], selected_variable.var_float[2], "%f", !selected_variable.animate);
		}
		if (selected_type == VAR_VEC2) {
			animate_bar_offset = 1;
			for (int i = 0; i < 2; i++) {
				draw_slider_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec2[0][i], selected_variable.var_vec2[1][i], selected_variable.var_vec2[2][i], "%f", (i == shader_menu_item_sub_highlight) && !selected_variable.animate);
			}
		}
		if (selected_type == VAR_VEC3) {
			animate_bar_offset = 2;
			for (int i = 0; i < 3; i++) {
				draw_slider_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec3[0][i], selected_variable.var_vec3[1][i], selected_variable.var_vec3[2][i], "%f", (i == shader_menu_item_sub_highlight) && !selected_variable.animate);
			}
			
			if (selected_variable.type == "color") {
				animate_bar_offset++;
				glm::vec3 color = selected_variable.var_vec3[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(0, (bar_height*4) + 5, bar_width, bar_height);
			}
		}
		if (selected_type == VAR_VEC4) {
			animate_bar_offset = 3;
			for (int i = 0; i < 4; i++) {
				draw_slider_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec4[0][i], selected_variable.var_vec4[1][i], selected_variable.var_vec4[2][i], "%f", (i == shader_menu_item_sub_highlight) && !selected_variable.animate);
			}

			if (selected_variable.type == "color4") {
				animate_bar_offset++;
				glm::vec4 color = selected_variable.var_vec4[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(0, (bar_height*5) + 5, bar_width, bar_height);
			}
		}
		if (selected_variable.animate) {
			draw_slider_bar(0, (bar_height*(1+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_speed, 0, 1, "Animate Speed: %f", shader_menu_item_sub_highlight == 0);
			draw_slider_bar(0, (bar_height*(2+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_scale, 0, 1, "Animate Scale: %f", shader_menu_item_sub_highlight == 1);
			draw_slider_bar(0, (bar_height*(3+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_offset, -1, 1, "Animate Offset: %f", shader_menu_item_sub_highlight == 2);
		}
	} else {
		int category_size = (int)shader_categories_indexes[shader_menu_category].size();

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)(category_size + 1.0f) * font_height + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (float)(category_size + 1.0f) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (shader_menu_item_highlight + 1) * font_height, 250, font_height);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "< " + shader_categories[shader_menu_category] + " >");

		for (int i = 0; i < category_size; i++) {
			int actual_index = shader_categories_indexes[shader_menu_category][i];

			string variable_string = (*shader_variables)[actual_index].category + " : " + (*shader_variables)[actual_index].name;

			if ((*shader_variables)[actual_index].type == "color") {
				glm::vec3 color = (*shader_variables)[actual_index].var_vec3[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(250, (i + 1) * font_height, 200, font_height);
			} else if ((*shader_variables)[actual_index].type == "color4") {
				glm::vec4 color = (*shader_variables)[actual_index].var_vec4[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(250, (i + 1) * font_height, 200, font_height);
			}

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, variable_string);
			drawing_tools->text(255, (i + 1) * font_height + 5, settings_font, (*shader_variables)[actual_index].get_string());
		}
	}
}
*/

void BulbSettings::shader_menu_gamepad_update(GamePadState *state) {
	if (shader_menu_item_selected) {
		int actual_highlight_index = shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];
		ShaderVariable *selected_variable = &(*shader_variables)[actual_highlight_index];

		if (state->pressed(GamePad_Button_X)) {
			selected_variable->animate_enable[shader_menu_item_sub_highlight] = !selected_variable->animate_enable[shader_menu_item_sub_highlight];
			// shader_menu_item_sub_selected = selected_variable->animate_enable[shader_menu_item_sub_highlight]; // auto enter menu
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

			if (state->pressed(GamePad_Button_DPAD_UP)) shader_menu_item_sub_highlight++;
			if (state->pressed(GamePad_Button_DPAD_DOWN)) shader_menu_item_sub_highlight--;

			if (!selected_variable->animate_enable[shader_menu_item_sub_highlight]) {
				selected_variable->adjust_variable(dpad_step + trigger_step, shader_menu_item_sub_highlight);
			}
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
		shader_menu_category = glm::clamp(shader_menu_category, 0, (int)shader_categories.size() - 1);

		if (state->pressed(GamePad_Button_DPAD_UP)) shader_menu_item_highlight++;
		if (state->pressed(GamePad_Button_DPAD_DOWN)) shader_menu_item_highlight--;
		int category_size = (int)shader_categories_indexes[shader_menu_category].size();
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
		main_menu_item_sub_highlight = 0;

		menu_open = main_menu_item_highlight + 1;
	}
	if (state->pressed(GamePad_Button_B)) {
		settings_open = false;
	}

	if (state->pressed(GamePad_Button_DPAD_UP)) main_menu_item_highlight++;
	if (state->pressed(GamePad_Button_DPAD_DOWN)) main_menu_item_highlight--;
	main_menu_item_highlight = glm::clamp(main_menu_item_highlight, 0, menu_items_size - 1 - 2);
}

void BulbSettings::draw() {
	glUseProgram(0);

	if (menu_open == 0) {
		main_menu_draw();
	}
	if (menu_open == 1) {
		shader_menu_draw();
	}
	if (menu_open == 2) {
		control_menu_draw();
	}
}

void BulbSettings::gamepad_update(GamePadState *state) {
	if (menu_open == 0) {
		main_menu_gamepad_update(state);
	} else if (menu_open == 1) {
		shader_menu_gamepad_update(state);
	} else if (menu_open == 2) {
		control_menu_gamepad_update(state);
	}
}