#include "bulb_settings.h"

BulbControlSettings::BulbControlSettings() {
	camera_fov = 1.5f;

	control_expo_power = 2;
	control_move_speed_forward = 0.015f;
	control_move_speed_lateral = 0.010f;
	control_move_speed_vertical = 0.010f;
	control_pitch_speed = 0.03f;
	control_roll_speed = 0.06f;
	control_yaw_speed = 0.03f;

	control_vibrate = true;
}

BulbSettings::BulbSettings(vector<ShaderVariable> *shader_variables, DrawingTools *drawing_tools) {
	this->shader_variables = shader_variables;
	this->drawing_tools = drawing_tools;

	settings_open = false;
	menu_open = 0;

	settings_font = GLUT_BITMAP_HELVETICA_10;
	font_height = 15;

	shader_categories.clear();
	shader_categories_indexes.clear();

	control_menu_item_highlight = 0;
	control_menu_item_selected = false;
	control_menu_item_sub_highlight = 0;

	shader_menu_item_highlight = 0;
	shader_menu_item_selected = false;
	shader_menu_item_sub_highlight = 0;
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

void BulbSettings::control_menu_draw() {
	string control_variable_names[] = {"camera_fov", "control_expo_power", "control_move_speed_forward", "control_move_speed_lateral", "control_move_speed_vertical", "control_pitch_speed", "control_roll_speed", "control_yaw_speed", "control_vibrate"};
	
	if (control_menu_item_selected) {
		float bar_width = 500;
		float bar_height = (float)font_height;

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

		string variable_string = "Control : " + control_variable_names[control_menu_item_highlight];
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 5, settings_font, variable_string);

		if (control_menu_item_highlight == 0) { // "camera_fov"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.camera_fov, 0.0f, 4.0f, "%f", true);
		}
		if (control_menu_item_highlight == 1) { // "control_expo_power"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, (float)control_settings.control_expo_power, 0.0f, 10.0f, "%0.0f", true);
		}
		if (control_menu_item_highlight == 2) { // "control_move_speed_forward"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_move_speed_forward, 0.0f, 0.1f, "%f", true);
		}
		if (control_menu_item_highlight == 3) { // "control_move_speed_lateral"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_move_speed_lateral, 0.0f, 0.1f, "%f", true);
		}
		if (control_menu_item_highlight == 4) { // "control_move_speed_vertical"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_move_speed_vertical, 0.0f, 0.1f, "%f", true);
		}
		if (control_menu_item_highlight == 5) { // "control_pitch_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_pitch_speed, 0.0f, 0.1f, "%f", true);
		}
		if (control_menu_item_highlight == 6) { // "control_roll_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_roll_speed, 0.0f, 0.1f, "%f", true);
		}
		if (control_menu_item_highlight == 7) { // "control_yaw_speed"
			draw_slider_bar(0, bar_height + 5, bar_width, bar_height, control_settings.control_yaw_speed, 0.0f, 0.1f, "%f", true);
		}
		if (control_menu_item_highlight == 8) { // "control_vibrate"
			glColor4f(0.4f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, bar_height + 5, bar_width, bar_height);

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, bar_height + 10, settings_font, (control_settings.control_vibrate) ? "true" : "false");
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

		for (int i = 0; i < items_size; i++) {
			string variable_string = "Control : " + control_variable_names[i];

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, variable_string);
			char text[50];

			if (i == 0) sprintf_s(text, "%f", control_settings.camera_fov);
			if (i == 1) sprintf_s(text, "%d", control_settings.control_expo_power);
			if (i == 2) sprintf_s(text, "%f", control_settings.control_move_speed_forward);
			if (i == 3) sprintf_s(text, "%f", control_settings.control_move_speed_lateral);
			if (i == 4) sprintf_s(text, "%f", control_settings.control_move_speed_vertical);
			if (i == 5) sprintf_s(text, "%f", control_settings.control_pitch_speed);
			if (i == 6) sprintf_s(text, "%f", control_settings.control_roll_speed);
			if (i == 7) sprintf_s(text, "%f", control_settings.control_yaw_speed);
			if (i == 8) sprintf_s(text, (control_settings.control_vibrate) ? "true" : "false");

			drawing_tools->text(255, (i + 1) * font_height + 5, settings_font, string(text));
		}
	}
}

void BulbSettings::control_menu_gamepad_update(GamePadState *state) {
	if (control_menu_item_selected) {
		if (state->pressed(GamePad_Button_B)) {
			shader_menu_item_selected = false;
		}

		float dpad_step = 0.0f;
		if (state->pressed(GamePad_Button_DPAD_RIGHT)) {
			dpad_step += 1.0f;
		}
		if (state->pressed(GamePad_Button_DPAD_LEFT)) {
			dpad_step -= 1.0f;
		}

		/*
		if (state->pressed(GamePad_Button_DPAD_UP)) {
			control_menu_item_sub_highlight++;
		}
		if (state->pressed(GamePad_Button_DPAD_DOWN)) {
			control_menu_item_sub_highlight--;
		}
		*/

		if (control_menu_item_highlight == 0) { // "camera_fov"
			control_settings.camera_fov += dpad_step;
			control_settings.camera_fov += (state->rt - state->lt) * (4.0f / 100.0f);
			control_settings.camera_fov = glm::clamp(control_settings.camera_fov, 0.0f, 4.0f);
		}
		if (control_menu_item_highlight == 1) { // "control_expo_power"
			control_settings.control_expo_power += (int)dpad_step;
			control_settings.control_expo_power += (int)((state->rt - state->lt) * (10.0f / 100.0f));
			control_settings.control_expo_power = glm::clamp(control_settings.control_expo_power, 0, 10);
		}
		if (control_menu_item_highlight == 2) { // "control_move_speed_forward"
			control_settings.control_move_speed_forward += (state->rt - state->lt) * (0.1f / 100.0f);
			control_settings.control_move_speed_forward = glm::clamp(control_settings.control_move_speed_forward, 0.0f, 0.1f);
		}
		if (control_menu_item_highlight == 3) { // "control_move_speed_lateral"
			control_settings.control_move_speed_lateral += (state->rt - state->lt) * (0.1f / 100.0f);
			control_settings.control_move_speed_lateral = glm::clamp(control_settings.control_move_speed_lateral, 0.0f, 0.1f);
		}
		if (control_menu_item_highlight == 4) { // "control_move_speed_vertical"
			control_settings.control_move_speed_vertical += (state->rt - state->lt) * (0.1f / 100.0f);
			control_settings.control_move_speed_vertical = glm::clamp(control_settings.control_move_speed_vertical, 0.0f, 0.1f);
		}
		if (control_menu_item_highlight == 5) { // "control_pitch_speed"
			control_settings.control_pitch_speed += (state->rt - state->lt) * (0.1f / 100.0f);
			control_settings.control_pitch_speed = glm::clamp(control_settings.control_pitch_speed, 0.0f, 0.1f);
		}
		if (control_menu_item_highlight == 6) { // "control_roll_speed"
			control_settings.control_roll_speed += (state->rt - state->lt) * (0.1f / 100.0f);
			control_settings.control_roll_speed = glm::clamp(control_settings.control_roll_speed, 0.0f, 0.1f);
		}
		if (control_menu_item_highlight == 7) { // "control_yaw_speed"
			control_settings.control_yaw_speed += (state->rt - state->lt) * (0.1f / 100.0f);
			control_settings.control_yaw_speed = glm::clamp(control_settings.control_yaw_speed, 0.0f, 0.1f);
		}
		if (control_menu_item_highlight == 8) { // "control_vibrate"
			if (dpad_step) {
				control_settings.control_vibrate = !control_settings.control_vibrate;
			}
		}
	} else {
		if (state->pressed(GamePad_Button_A)) {
			control_menu_item_selected = true;
			control_menu_item_sub_highlight = 0;
		}
		if (state->pressed(GamePad_Button_B)) {
			menu_open = 0; // main menu
		}

		int items_size = 9;
		if (state->pressed(GamePad_Button_DPAD_UP)) {
			control_menu_item_highlight++;
		}
		if (state->pressed(GamePad_Button_DPAD_DOWN)) {
			control_menu_item_highlight--;
		}
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

void BulbSettings::shader_menu_gamepad_update(GamePadState *state) {
	if (shader_menu_item_selected) {
		int actual_highlight_index = shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];

		ShaderVariable *selected_variable = &(*shader_variables)[actual_highlight_index];
		SHADER_VAR_TYPE selected_type = selected_variable->var_type;
		selected_variable->update = true;

		if (state->pressed(GamePad_Button_X)) {
			selected_variable->animate = !selected_variable->animate;
		}
		if (state->pressed(GamePad_Button_B)) {
			shader_menu_item_selected = false;
		}

		float dpad_step = 0.0f;
		if (state->pressed(GamePad_Button_DPAD_RIGHT)) {
			dpad_step += 1.0f;
		}
		if (state->pressed(GamePad_Button_DPAD_LEFT)) {
			dpad_step -= 1.0f;
		}

		if (state->pressed(GamePad_Button_DPAD_UP)) {
			shader_menu_item_sub_highlight++;
		}
		if (state->pressed(GamePad_Button_DPAD_DOWN)) {
			shader_menu_item_sub_highlight--;
		}

		if (!selected_variable->animate) {
			if (selected_type == VAR_BOOL) {
				if (dpad_step) {
					selected_variable->var_bool[0] = !selected_variable->var_bool[0];
				}
			}
			if (selected_type == VAR_INT) {
				float range = (float)(selected_variable->var_int[2] - selected_variable->var_int[1]);
				float step = range / 100.0f;

				selected_variable->var_int[0] += (int)dpad_step;
				selected_variable->var_int[0] += (int)((state->rt - state->lt) * step);

				selected_variable->var_int[0] = glm::clamp(selected_variable->var_int[0], selected_variable->var_int[1], selected_variable->var_int[2]);
			}
			if (selected_type == VAR_FLOAT) {
				float range = selected_variable->var_float[2] - selected_variable->var_float[1];
				float step = range / 100.0f;

				selected_variable->var_float[0] += dpad_step;
				selected_variable->var_float[0] += (state->rt - state->lt) * step;

				selected_variable->var_float[0] = glm::clamp(selected_variable->var_float[0], selected_variable->var_float[1], selected_variable->var_float[2]);
			}
			if (selected_type == VAR_VEC2) {
				shader_menu_item_sub_highlight = glm::clamp(shader_menu_item_sub_highlight, 0, 1);

				float range = selected_variable->var_vec2[2][shader_menu_item_sub_highlight] - selected_variable->var_vec2[1][shader_menu_item_sub_highlight];
				float step = range / 100.0f;

				selected_variable->var_vec2[0][shader_menu_item_sub_highlight] += dpad_step;
				selected_variable->var_vec2[0][shader_menu_item_sub_highlight] += (state->rt - state->lt) * step;

				selected_variable->var_vec2[0][shader_menu_item_sub_highlight] = glm::clamp(selected_variable->var_vec2[0][shader_menu_item_sub_highlight], selected_variable->var_vec2[1][shader_menu_item_sub_highlight], selected_variable->var_vec2[2][shader_menu_item_sub_highlight]);
			}
			if (selected_type == VAR_VEC3) {
				shader_menu_item_sub_highlight = glm::clamp(shader_menu_item_sub_highlight, 0, 2);

				float range = selected_variable->var_vec3[2][shader_menu_item_sub_highlight] - selected_variable->var_vec3[1][shader_menu_item_sub_highlight];
				float step = range / 100.0f;

				selected_variable->var_vec3[0][shader_menu_item_sub_highlight] += dpad_step;
				selected_variable->var_vec3[0][shader_menu_item_sub_highlight] += (state->rt - state->lt) * step;

				selected_variable->var_vec3[0][shader_menu_item_sub_highlight] = glm::clamp(selected_variable->var_vec3[0][shader_menu_item_sub_highlight], selected_variable->var_vec3[1][shader_menu_item_sub_highlight], selected_variable->var_vec3[2][shader_menu_item_sub_highlight]);
			}
			if (selected_type == VAR_VEC4) {
				shader_menu_item_sub_highlight = glm::clamp(shader_menu_item_sub_highlight, 0, 3);
			
				float range = selected_variable->var_vec4[2][shader_menu_item_sub_highlight] - selected_variable->var_vec4[1][shader_menu_item_sub_highlight];
				float step = range / 100.0f;

				selected_variable->var_vec4[0][shader_menu_item_sub_highlight] += dpad_step;
				selected_variable->var_vec4[0][shader_menu_item_sub_highlight] += (state->rt - state->lt) * step;

				selected_variable->var_vec4[0][shader_menu_item_sub_highlight] = glm::clamp(selected_variable->var_vec4[0][shader_menu_item_sub_highlight], selected_variable->var_vec4[1][shader_menu_item_sub_highlight], selected_variable->var_vec4[2][shader_menu_item_sub_highlight]);
			}
		} else {
			shader_menu_item_sub_highlight = glm::clamp(shader_menu_item_sub_highlight, 0, 2);

			if (shader_menu_item_sub_highlight == 0) {
				selected_variable->animate_speed += (state->rt - state->lt) * 0.01f;
				selected_variable->animate_speed = glm::clamp(selected_variable->animate_speed, 0.0f, 1.0f);
			}
			if (shader_menu_item_sub_highlight == 1) {
				selected_variable->animate_scale += (state->rt - state->lt) * 0.01f;
				selected_variable->animate_scale = glm::clamp(selected_variable->animate_scale, 0.0f, 1.0f);
			}
			if (shader_menu_item_sub_highlight == 2) {
				selected_variable->animate_offset += (state->rt - state->lt) * 0.02f;
				selected_variable->animate_offset = glm::clamp(selected_variable->animate_offset, -1.0f, 1.0f);
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

		if (state->pressed(GamePad_Button_RIGHT_SHOULDER)) {
			shader_menu_category++;
		}
		if (state->pressed(GamePad_Button_LEFT_SHOULDER)) {
			shader_menu_category--;
		}
		shader_menu_category = glm::clamp(shader_menu_category, 0, (int)shader_categories.size() - 1);

		int category_size = (int)shader_categories_indexes[shader_menu_category].size();
		if (state->pressed(GamePad_Button_DPAD_UP)) {
			shader_menu_item_highlight++;
		}
		if (state->pressed(GamePad_Button_DPAD_DOWN)) {
			shader_menu_item_highlight--;
		}
		shader_menu_item_highlight = glm::clamp(shader_menu_item_highlight, 0, category_size-1);
	}
}

void BulbSettings::main_menu_draw() {
	string menu_items[] = {"Shader Settings", "Control Settings", "Load", "Save"};
	int menu_items_size = 4;
		
	glColor4f(0.2f,0.2f,0.2f,1.0f);
	drawing_tools->rectangle_filled(0, 0, 250, (float)(menu_items_size) * font_height + 5);

	glColor4f(0.4f,0.4f,0.4f,1.0f);
	drawing_tools->rectangle_filled(0, (main_menu_item_highlight) * font_height, 250, font_height);

	for (int i = 0; i < menu_items_size; i++) {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
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

	if (state->pressed(GamePad_Button_DPAD_UP)) {
		main_menu_item_highlight++;
	}
	if (state->pressed(GamePad_Button_DPAD_DOWN)) {
		main_menu_item_highlight--;
	}
	shader_menu_item_highlight = glm::clamp(shader_menu_item_highlight, 0, menu_items_size-1);
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
	}
	if (menu_open == 1) {
		shader_menu_gamepad_update(state);
	}
}