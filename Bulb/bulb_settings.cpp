#include "bulb_settings.h"

BulbSettings::BulbSettings(vector<ShaderVariable> *shader_variables, DrawingTools *drawing_tools) {
	this->shader_variables = shader_variables;
	this->drawing_tools = drawing_tools;

	item_highlight = 0;
	item_selected = false;
	menu_open = false;
}

void BulbSettings::draw_bar(float x, float y, float bar_width, float bar_height, float current, float min, float max, string format, bool selected) {
	float range = max - min;
	float pos = (current - min) / range;
	pos *= bar_width;

	if (selected)
		glColor4f(0.4f,0.4f,0.4f,1.0f);
	else
		glColor4f(0.2f,0.2f,0.2f,1.0f);

	drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

	glColor4f(0.6f,0.6f,0.6f,1.0f);
	drawing_tools->rectangle_filled(x + pos, y, 2, bar_height);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	char text[50];
	sprintf_s(text, format.c_str(), current);
	drawing_tools->text(x + 5, y + 5, GLUT_BITMAP_HELVETICA_18, string(text));
}

void BulbSettings::draw() {
	glUseProgram(0);

	if (item_selected == false) {
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (float)shader_variables->size() * 20 + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (float)shader_variables->size() * 20 + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, item_highlight * 20, 250, 20);

		for (int i = 0; i < (int)shader_variables->size(); i++) {
			string variable_string = (*shader_variables)[i].category + " : " + (*shader_variables)[i].name;

			if ((*shader_variables)[i].type == "color") {
				glm::vec3 color = (*shader_variables)[i].var_vec3[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(250, i * 20, 200, 20);
			} else if ((*shader_variables)[i].type == "color4") {
				glm::vec4 color = (*shader_variables)[i].var_vec4[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(250, i * 20, 200, 20);
			}

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, i * 20 + 5, GLUT_BITMAP_HELVETICA_18, variable_string);
			drawing_tools->text(255, i * 20 + 5, GLUT_BITMAP_HELVETICA_18, (*shader_variables)[i].get_string());
		}
	} else {
		ShaderVariable selected_variable = (*shader_variables)[item_highlight];
		SHADER_VAR_TYPE selected_type = selected_variable.var_type;

		float bar_width = 500;
		float bar_height = 20;

		int animate_bar_offset = 0;

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

		string variable_string = selected_variable.category + " : " + selected_variable.name;
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		drawing_tools->text(5, 5, GLUT_BITMAP_HELVETICA_18, variable_string);

		if (selected_type == VAR_BOOL) {
			glColor4f(0.2f,0.2f,0.2f,1.0f);
			drawing_tools->rectangle_filled(0, bar_height + 5, bar_width, bar_height);

			glColor4f(1.0f,1.0f,1.0f,1.0f);
			drawing_tools->text(5, bar_height + 5, GLUT_BITMAP_HELVETICA_18, selected_variable.get_string());
		}
		if (selected_type == VAR_INT) {
			draw_bar(0, bar_height + 5, bar_width, bar_height, selected_variable.var_int[0], selected_variable.var_int[1], selected_variable.var_int[2], "%0.0f", !selected_variable.animate);
		}
		if (selected_type == VAR_FLOAT) {
			draw_bar(0, bar_height + 5, bar_width, bar_height, selected_variable.var_float[0], selected_variable.var_float[1], selected_variable.var_float[2], "%f", !selected_variable.animate);
		}
		if (selected_type == VAR_VEC2) {
			animate_bar_offset = 1;
			for (int i = 0; i < 2; i++) {
				draw_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec2[0][i], selected_variable.var_vec2[1][i], selected_variable.var_vec2[2][i], "%f", (i == sub_item_highlight) && !selected_variable.animate);
			}
		}
		if (selected_type == VAR_VEC3) {
			animate_bar_offset = 2;
			for (int i = 0; i < 3; i++) {
				draw_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec3[0][i], selected_variable.var_vec3[1][i], selected_variable.var_vec3[2][i], "%f", (i == sub_item_highlight) && !selected_variable.animate);
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
				draw_bar(0, (bar_height*(i+1)) + 5, bar_width, bar_height, selected_variable.var_vec4[0][i], selected_variable.var_vec4[1][i], selected_variable.var_vec4[2][i], "%f", (i == sub_item_highlight) && !selected_variable.animate);
			}

			if (selected_variable.type == "color4") {
				animate_bar_offset++;
				glm::vec4 color = selected_variable.var_vec4[0];
				glColor4f(color.r, color.g, color.b,1.0f);
				drawing_tools->rectangle_filled(0, (bar_height*5) + 5, bar_width, bar_height);
			}
		}
		if (selected_variable.animate) {
			draw_bar(0, (bar_height*(1+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_speed, 0, 1, "Animate Speed: %f", sub_item_highlight == 0);
			draw_bar(0, (bar_height*(2+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_scale, 0, 1, "Animate Scale: %f", sub_item_highlight == 1);
			draw_bar(0, (bar_height*(3+1+animate_bar_offset)) + 5, bar_width, bar_height, selected_variable.animate_offset, -1, 1, "Animate Offset: %f", sub_item_highlight == 2);
		}
	}
}

void BulbSettings::gamepad_update(GamePadState *state) {
	if (item_selected) {
		ShaderVariable *selected_variable = &(*shader_variables)[item_highlight];
		SHADER_VAR_TYPE selected_type = selected_variable->var_type;
		selected_variable->update = true;

		if (!state->buttons_last[GamePad_Button_X] && state->buttons[GamePad_Button_X]) {
			selected_variable->animate = !selected_variable->animate;
		}

		float dpad_step = 0.0f;
		if (!state->buttons_last[GamePad_Button_DPAD_RIGHT] && state->buttons[GamePad_Button_DPAD_RIGHT]) {
			dpad_step += 1.0f;
		}
		if (!state->buttons_last[GamePad_Button_DPAD_LEFT] && state->buttons[GamePad_Button_DPAD_LEFT]) {
			dpad_step -= 1.0f;
		}

		if (!state->buttons_last[GamePad_Button_DPAD_UP] && state->buttons[GamePad_Button_DPAD_UP]) {
			sub_item_highlight++;
		}
		if (!state->buttons_last[GamePad_Button_DPAD_DOWN] && state->buttons[GamePad_Button_DPAD_DOWN]) {
			sub_item_highlight--;
		}

		if (!selected_variable->animate) {
			if (selected_type == VAR_BOOL) {
				if (dpad_step) {
					selected_variable->var_bool[0] = !selected_variable->var_bool[0];
				}
			}
			if (selected_type == VAR_INT) {
				float range = selected_variable->var_int[2] - selected_variable->var_int[1];
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
				sub_item_highlight = glm::clamp(sub_item_highlight, 0, 1);

				float range = selected_variable->var_vec2[2][sub_item_highlight] - selected_variable->var_vec2[1][sub_item_highlight];
				float step = range / 100.0f;

				selected_variable->var_vec2[0][sub_item_highlight] += dpad_step;
				selected_variable->var_vec2[0][sub_item_highlight] += (state->rt - state->lt) * step;

				selected_variable->var_vec2[0][sub_item_highlight] = glm::clamp(selected_variable->var_vec2[0][sub_item_highlight], selected_variable->var_vec2[1][sub_item_highlight], selected_variable->var_vec2[2][sub_item_highlight]);
			}
			if (selected_type == VAR_VEC3) {
				sub_item_highlight = glm::clamp(sub_item_highlight, 0, 2);

				float range = selected_variable->var_vec3[2][sub_item_highlight] - selected_variable->var_vec3[1][sub_item_highlight];
				float step = range / 100.0f;

				selected_variable->var_vec3[0][sub_item_highlight] += dpad_step;
				selected_variable->var_vec3[0][sub_item_highlight] += (state->rt - state->lt) * step;

				selected_variable->var_vec3[0][sub_item_highlight] = glm::clamp(selected_variable->var_vec3[0][sub_item_highlight], selected_variable->var_vec3[1][sub_item_highlight], selected_variable->var_vec3[2][sub_item_highlight]);
			}
			if (selected_type == VAR_VEC4) {
				sub_item_highlight = glm::clamp(sub_item_highlight, 0, 3);
			
				float range = selected_variable->var_vec4[2][sub_item_highlight] - selected_variable->var_vec4[1][sub_item_highlight];
				float step = range / 100.0f;

				selected_variable->var_vec4[0][sub_item_highlight] += dpad_step;
				selected_variable->var_vec4[0][sub_item_highlight] += (state->rt - state->lt) * step;

				selected_variable->var_vec4[0][sub_item_highlight] = glm::clamp(selected_variable->var_vec4[0][sub_item_highlight], selected_variable->var_vec4[1][sub_item_highlight], selected_variable->var_vec4[2][sub_item_highlight]);
			}
		} else {
			sub_item_highlight = glm::clamp(sub_item_highlight, 0, 2);

			float step = 0.01f;

			if (sub_item_highlight == 0) {
				selected_variable->animate_speed += (state->rt - state->lt) * step;
				selected_variable->animate_speed = glm::clamp(selected_variable->animate_speed, 0.0f, 1.0f);
			}
			if (sub_item_highlight == 1) {
				selected_variable->animate_scale += (state->rt - state->lt) * step;
				selected_variable->animate_scale = glm::clamp(selected_variable->animate_scale, 0.0f, 1.0f);
			}
			if (sub_item_highlight == 2) {
				selected_variable->animate_offset += (state->rt - state->lt) * 2.0 / 100.0f;
				selected_variable->animate_offset = glm::clamp(selected_variable->animate_offset, -1.0f, 1.0f);
			}
		}


		if (!state->buttons_last[GamePad_Button_B] && state->buttons[GamePad_Button_B]) {
			item_selected = false;
		}
	} else {
		if (!state->buttons_last[GamePad_Button_A] && state->buttons[GamePad_Button_A]) {
			item_selected = true;
			sub_item_highlight = 0;
		}
		if (!state->buttons_last[GamePad_Button_B] && state->buttons[GamePad_Button_B]) {
			menu_open = false;
		}

		// menu movement
		if (!state->buttons_last[GamePad_Button_DPAD_UP] && state->buttons[GamePad_Button_DPAD_UP]) {
			item_highlight = glm::min(item_highlight + 1, (int)shader_variables->size() - 1);
		}
		if (!state->buttons_last[GamePad_Button_DPAD_DOWN] && state->buttons[GamePad_Button_DPAD_DOWN]) {
			item_highlight = glm::max(item_highlight - 1, 0);
		}
	}

}