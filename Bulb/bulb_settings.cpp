#include "bulb_settings.h"

BulbSave::BulbSave() {
	file_name = "";
	image = BMP(256, 256);
}

bool BulbSave::load(string save_file_name) {
	file_name = save_file_name;
	image.load(file_name);

	ifstream save_file(file_name);
	char verify_string[10];
	memset(verify_string, 0, 10);
	save_file.seekg(image.bmp_file_size, ios::beg);
	save_file.read(verify_string, 8);
	save_file.close();
	if (string(verify_string) == "BULBSAVE") {
		vector<string> file_name_split = split_string(file_name, "\\");
		clean_name = file_name_split.back().substr(0, file_name_split.back().size() - 4);

		glGenTextures(1, &tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.image_data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		return true;
	} else {
		return false;
	}
}


BulbSettings::BulbSettings(BulbShader *bulb_shader, BulbControlSettings *control_settings, DrawingTools *drawing_tools) {
	this->bulb_shader = bulb_shader;
	this->control_settings = control_settings;
	this->drawing_tools = drawing_tools;

	settings_open = false;
	menu_open = 0;

	info_text = "";

	settings_font = GLUT_BITMAP_HELVETICA_12;
	font_height = 20;


	load_menu_item_highlight = 0;
	load_menu_item_selected = false;
	load_menu_item_sub_highlight = 0; 
	load_menu_delete_hold = 0;

	save_menu_item_highlight = 0;
	save_menu_item_selected = false;
	save_menu_item_sub_highlight = 0; 
	save_menu_overwrite_hold = 0;
	save_menu_current_save_name = "";

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

void BulbSettings::draw_bulb_variable(BulbVariable *variable, int sub_highlight, int sub_animate_highlight, bool sub_selected) {
	int bar_width = 500;
	int bar_height = font_height;

	glColor3f(0.2f,0.2f,0.2f);
	drawing_tools->rectangle_filled(0, 0, bar_width, bar_height + 5);

	glColor3f(0.6f,0.6f,0.6f);
	drawing_tools->text(5, 5, settings_font, variable->category + " : " + variable->name);

	float var_bg_normal_color = (sub_selected) ? 0.1f : 0.3f;
	float var_bg_selected_color = (sub_selected) ? 0.2f : 0.4f;
	float var_fg_animate_color = (sub_selected) ? 0.3f : 0.5f;
	float var_fg_color = (sub_selected) ? 0.6f : 1.0f;

	for (int i = 0; i < BULB_VAR_SIZE[variable->var_type]; i++) {
		int x = 0;
		int y = (bar_height * (i+1)) + 5;

		float var_bg_color = (i == sub_highlight) ? var_bg_selected_color : var_bg_normal_color;
		glColor3f(var_bg_color,var_bg_color,var_bg_color);
		drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

		// draw animate stuff
		if (variable->animate_enable[i]) {
			float var_fg_anmiate_color = (i == sub_highlight) ? var_bg_selected_color : var_bg_normal_color;
			glColor3f(var_fg_animate_color,var_fg_animate_color,var_fg_animate_color);
			int a_scale = (int)(variable->animate_values[i][1] * bar_width);
			int a_offset = (int)(((variable->animate_values[i][2] + 1.0f) / 2.0f) * bar_width);
			drawing_tools->rectangle_filled(x + a_offset - (a_scale / 2), y + (bar_height / 2) - 2, a_scale, 4);
		}

		// draw slider
		if (variable->var_type != VAR_SAMP2D) {
			glColor3f(var_fg_color,var_fg_color,var_fg_color);
			int pos = (int)(((variable->value[0][i] - variable->value[1][i]) / variable->value[3][i]) * bar_width);
			drawing_tools->rectangle_filled(x + pos - 2, y, 4, bar_height);
		}

		// draw text
		glColor3f(var_fg_color,var_fg_color,var_fg_color);
		drawing_tools->text(x + 5, y + 5, settings_font, variable->get_string(i));
	}

	// draw color bar
	if (variable->is_color) {
		glm::vec4 variable_color = variable->get_color();
		glColor3f(variable_color.r, variable_color.g, variable_color.b);
		drawing_tools->rectangle_filled(0, (bar_height*(BULB_VAR_SIZE[variable->var_type] + 1)) + 5, bar_width, bar_height);
	}

	// animate
	if (variable->animate_enable[sub_highlight]) {
		int animate_sliders_offset = BULB_VAR_SIZE[variable->var_type] + ((variable->is_color) ? 1 : 0);
			
		float var_bg_normal_color = (!sub_selected) ? 0.1f : 0.3f;
		float var_bg_selected_color = (!sub_selected) ? 0.2f : 0.4f;
		float var_fg_color = (!sub_selected) ? 0.6f : 1.0f;

		float mins[3] = {0.0f, 0.0f, -1.0f};
		float ranges[3] = {1.0f, 1.0f, 2.0f};

		for (int i = 0; i < 3; i++) {
			int x = 0;
			int y = (bar_height * (i+1+animate_sliders_offset)) + 5;

			float var_bg_color = (i == sub_animate_highlight) ? var_bg_selected_color : var_bg_normal_color;
			glColor3f(var_bg_color,var_bg_color,var_bg_color);
			drawing_tools->rectangle_filled(x, y, bar_width, bar_height);

			// draw slider
			glColor3f(var_fg_color,var_fg_color,var_fg_color);
			int pos = (int)(((variable->animate_values[sub_highlight][i] - mins[i]) / ranges[i]) * bar_width);
			drawing_tools->rectangle_filled(x + pos - 2, y, 4, bar_height);

			// draw text
			glColor3f(var_fg_color,var_fg_color,var_fg_color);
			drawing_tools->text(x + 5, y + 5, settings_font, variable->get_string_animate(sub_highlight, i));
		}
	}
}


void BulbSettings::control_menu_draw() {
	if (control_menu_item_selected) {
		draw_bulb_variable(control_settings->control_variables[control_menu_item_highlight], control_menu_item_sub_highlight, 0, false);
	} else {
		int category_size = (int)control_settings->control_variables.size();

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (category_size + 1) * font_height + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (category_size + 1) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (control_menu_item_highlight + 1) * font_height, 250, font_height);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "Control Settings");

		for (int i = 0; i < category_size; i++) {
			BulbVariable *current_variable = control_settings->control_variables[i];

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

void BulbSettings::control_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state) {
	if (control_menu_item_selected) {
		info_text = "[TRIGGERS]/[ARROW KEYS] to change value.";
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			control_menu_item_selected = false;
		}

		int dpad_step = 0;
		dpad_step += (gamepad_state->pressed(GamePad_Button_DPAD_RIGHT) - gamepad_state->pressed(GamePad_Button_DPAD_LEFT));
		dpad_step += (keyboard_state->pressed_special(GLUT_KEY_RIGHT) - keyboard_state->pressed_special(GLUT_KEY_LEFT));

		float trigger_step = (settings_expo(gamepad_state->rt) - settings_expo(gamepad_state->lt));

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) control_menu_item_sub_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) control_menu_item_sub_highlight--;

		control_settings->control_variables[control_menu_item_highlight]->adjust_variable(trigger_step, dpad_step, control_menu_item_sub_highlight);
	} else {
		info_text = "[A]/[ENTER] to edit.";

		if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
			control_menu_item_selected = true;
			control_menu_item_sub_highlight = 0;
		}
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			menu_open = 0; // main menu
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) control_menu_item_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) control_menu_item_highlight--;
		int category_size = (int)control_settings->control_variables.size();
		control_menu_item_highlight = glm::clamp(control_menu_item_highlight, 0, category_size-1);
	}
}


void BulbSettings::shader_menu_draw() {
	if (shader_menu_item_selected) {
		int actual_highlight_index = bulb_shader->shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];
		BulbVariable *selected_variable = &bulb_shader->shader_variables[actual_highlight_index];
		draw_bulb_variable(selected_variable, shader_menu_item_sub_highlight, shader_menu_item_sub_animate_highlight, shader_menu_item_sub_selected);
	} else {
		int category_size = (int)bulb_shader->shader_categories_indexes[shader_menu_category].size();

		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (category_size + 1) * font_height + 5);

		glColor4f(0.3f,0.3f,0.3f,1.0f);
		drawing_tools->rectangle_filled(250, 0, 200, (category_size + 1) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (shader_menu_item_highlight + 1) * font_height, 250, font_height);

		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "< " + bulb_shader->shader_categories[shader_menu_category] + " >");

		for (int i = 0; i < category_size; i++) {
			int actual_index = bulb_shader->shader_categories_indexes[shader_menu_category][i];
			BulbVariable *current_variable = &bulb_shader->shader_variables[actual_index];

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

void BulbSettings::shader_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state) {
	if (shader_menu_item_selected) {
		info_text = "[TRIGGERS]/[ARROW KEYS] to change value. [X] to toggle animate. [Y] to toggle HSV on colors.";

		int actual_highlight_index = bulb_shader->shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];
		BulbVariable *selected_variable = &bulb_shader->shader_variables[actual_highlight_index];

		if (gamepad_state->pressed(GamePad_Button_X) || keyboard_state->pressed_keyboard('x')) {
			if (selected_variable->var_type != VAR_SAMP2D) {
				selected_variable->animate_enable[shader_menu_item_sub_highlight] = !selected_variable->animate_enable[shader_menu_item_sub_highlight];
				shader_menu_item_sub_selected = selected_variable->animate_enable[shader_menu_item_sub_highlight]; // auto enter menu
			}
		}

		int dpad_step = 0;
		dpad_step += (gamepad_state->pressed(GamePad_Button_DPAD_RIGHT) - gamepad_state->pressed(GamePad_Button_DPAD_LEFT));
		dpad_step += (keyboard_state->pressed_special(GLUT_KEY_RIGHT) - keyboard_state->pressed_special(GLUT_KEY_LEFT));

		float trigger_step = (settings_expo(gamepad_state->rt) - settings_expo(gamepad_state->lt));

		if (shader_menu_item_sub_selected) {
			if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
				shader_menu_item_sub_selected = false;
			}

			if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) shader_menu_item_sub_animate_highlight++;
			if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) shader_menu_item_sub_animate_highlight--;

			selected_variable->adjust_animate(dpad_step + trigger_step, shader_menu_item_sub_highlight, shader_menu_item_sub_animate_highlight);
		} else {
			if ((gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) && selected_variable->animate_enable[shader_menu_item_sub_highlight]) {
				shader_menu_item_sub_selected = true;
			}
			if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
				shader_menu_item_selected = false;
			}

			if (gamepad_state->pressed(GamePad_Button_Y) || keyboard_state->pressed_keyboard('y')) {
				selected_variable->set_hsv_mode(!selected_variable->hsv_mode);
			}

			if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) shader_menu_item_sub_highlight++;
			if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) shader_menu_item_sub_highlight--;

			selected_variable->adjust_variable(trigger_step, dpad_step, shader_menu_item_sub_highlight);
		}
	} else {
		info_text = "[DPAD]/[ARROW KEYS] to change category. [A]/[ENTER] to edit.";
		if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
			shader_menu_item_selected = true;
			shader_menu_item_sub_highlight = 0;
		}
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			menu_open = 0; // main menu
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_RIGHT) || keyboard_state->pressed_special(GLUT_KEY_RIGHT)) shader_menu_category++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_LEFT) || keyboard_state->pressed_special(GLUT_KEY_LEFT)) shader_menu_category--;
		shader_menu_category = glm::clamp(shader_menu_category, 0, (int)bulb_shader->shader_categories.size() - 1);

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) shader_menu_item_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) shader_menu_item_highlight--;
		int category_size = (int)bulb_shader->shader_categories_indexes[shader_menu_category].size();
		shader_menu_item_highlight = glm::clamp(shader_menu_item_highlight, 0, category_size-1);
	}
}


void BulbSettings::main_menu_draw() {
	string menu_items[] = {"Shader Settings", "Control Settings", "Load", "Save"};
	int menu_items_size = 4;
		
	glColor4f(0.2f,0.2f,0.2f,1.0f);
	drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size) * font_height + 5);

	glColor4f(0.4f,0.4f,0.4f,1.0f);
	drawing_tools->rectangle_filled(0, main_menu_item_highlight * font_height, 250, font_height);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	for (int i = 0; i < menu_items_size; i++) {	
		drawing_tools->text(5, i * font_height + 5, settings_font, menu_items[i]);
	}
}

void BulbSettings::main_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state) {
	info_text = "";
	int menu_items_size = 4;

	if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
		main_menu_item_selected = true;
		menu_open = main_menu_item_highlight + 1;

		if (menu_open == 4) save_menu_current_save_name = random_name(8);
	}
	if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
		settings_open = false;
	}
	
	if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) main_menu_item_highlight++;
	if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) main_menu_item_highlight--;
	main_menu_item_highlight = glm::clamp(main_menu_item_highlight, 0, menu_items_size - 1);
}


void BulbSettings::save_menu_draw() {
	if (!save_menu_item_selected) {
		string menu_items[] = {"New \"" + save_menu_current_save_name + "\"", "Overwrite"};
		int menu_items_size = 2;
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, save_menu_item_highlight * font_height, 250, font_height);

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {	
			drawing_tools->text(5, i * font_height + 5, settings_font, menu_items[i]);
		}
	} else if (save_menu_item_highlight == 0) {
		// new save should be handled in input update
	} else if (save_menu_item_highlight == 1) {
		int menu_items_size = (int)save_files.size();
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size + 1) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (save_menu_item_sub_highlight + 1) * font_height, 250, font_height);

		if (save_menu_overwrite_hold > 0) {
			glColor4f(0.6f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, (save_menu_item_sub_highlight + 1) * font_height, (250 * save_menu_overwrite_hold) / 100, font_height);
		}
	
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "Save\\Overwrite");

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {
			string file_text = save_files[i].clean_name;
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, file_text);
		}

		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, save_files[save_menu_item_sub_highlight].tex_id);
		drawing_tools->rectangle_tex(250, 0, 256, 256);
		glDisable(GL_TEXTURE_2D);
	}
}

void BulbSettings::save_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state) {
	if (!save_menu_item_selected) {
		info_text = "";
		int menu_items_size = 2;
		if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
			save_menu_item_selected = true;
			save_menu_item_sub_highlight = 0;

			if (save_menu_item_highlight == 0) {
				save_menu_item_selected = false;

				save_save_file("BulbSaves\\" + save_menu_current_save_name + ".bmp");
				save_menu_current_save_name = random_name(8);

			} else if (save_menu_item_highlight == 1) { 
				update_save_files();
			}
		}
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			menu_open = 0; // main menu
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) save_menu_item_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) save_menu_item_highlight--;
		save_menu_item_highlight = glm::clamp(save_menu_item_highlight, 0, menu_items_size - 1);
	} else {
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			save_menu_item_selected = false;
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) {save_menu_item_sub_highlight++; save_menu_overwrite_hold = -1;}
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) {save_menu_item_sub_highlight--; save_menu_overwrite_hold = -1;}

		if (save_menu_item_highlight == 0) {
			// should not happen
		} else if (save_menu_item_highlight == 1) {
			info_text = "Hold [A]/[ENTER] to overwrite with new save.";
			save_menu_item_sub_highlight = glm::clamp(save_menu_item_sub_highlight, 0, (int)save_files.size() - 1);

			if (gamepad_state->buttons[GamePad_Button_A] || keyboard_state->keyboard[13]) {
				if (save_menu_overwrite_hold != -1) {
					save_menu_overwrite_hold += 1;
					if (save_menu_overwrite_hold >= 100) {
						save_menu_overwrite_hold = -1;

						save_save_file(save_files[save_menu_item_sub_highlight].file_name);
						update_save_files();
					}
				}
			} else {
				save_menu_overwrite_hold = 0;
			}
		}
	}
}


void BulbSettings::load_menu_draw() {
	if (!load_menu_item_selected) {
		string menu_items[] = {"Fractals", "Saves"};
		int menu_items_size = 2;
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, load_menu_item_highlight * font_height, 250, font_height);

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {	
			drawing_tools->text(5, i * font_height + 5, settings_font, menu_items[i]);
		}
	} else if (load_menu_item_highlight == 0) {
		int menu_items_size = (int)bulb_shader->fractal_files.size();
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size + 1) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (load_menu_item_sub_highlight + 1) * font_height, 250, font_height);
	
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "Load\\Fractals");

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {
			string file_text = bulb_shader->fractal_files[i];
			if (bulb_shader->fractal_files[i] == bulb_shader->fractal_file) {
				file_text += " (Loaded)";
			}
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, file_text);
		}
	} else if (load_menu_item_highlight == 1) {
		int menu_items_size = (int)save_files.size();
		
		glColor4f(0.2f,0.2f,0.2f,1.0f);
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size + 1) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, (load_menu_item_sub_highlight + 1) * font_height, 250, font_height);

		if (load_menu_delete_hold > 0) {
			glColor4f(0.6f,0.4f,0.4f,1.0f);
			drawing_tools->rectangle_filled(0, (load_menu_item_sub_highlight + 1) * font_height, (250 * load_menu_delete_hold) / 100, font_height);
		}
	
		glColor4f(0.6f,0.6f,0.6f,1.0f);
		drawing_tools->text(5, 0 * font_height + 5, settings_font, "Load\\Saves");

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {
			string file_text = save_files[i].clean_name;
			drawing_tools->text(5, (i + 1) * font_height + 5, settings_font, file_text);
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, save_files[load_menu_item_sub_highlight].tex_id);
		drawing_tools->rectangle_tex(250, 0, 256, 256);
		glDisable(GL_TEXTURE_2D);
	}
}

void BulbSettings::load_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state) {
	if (!load_menu_item_selected) {
		info_text = "";
		int menu_items_size = 2;
		if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
			load_menu_item_selected = true;
			load_menu_item_sub_highlight = 0;

			if (load_menu_item_highlight == 1) { 
				update_save_files();
			}
		}
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			menu_open = 0; // main menu
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) load_menu_item_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) load_menu_item_highlight--;
		load_menu_item_highlight = glm::clamp(load_menu_item_highlight, 0, menu_items_size - 1);
	} else {
		info_text = "[A]/[ENTER] to load.";
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			load_menu_item_selected = false;
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) {load_menu_item_sub_highlight++; load_menu_delete_hold = -1;}
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) {load_menu_item_sub_highlight--; load_menu_delete_hold = -1;}

		if (load_menu_item_highlight == 0) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)bulb_shader->fractal_files.size() - 1);
			
			if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
				bulb_shader->fractal_file = bulb_shader->fractal_files[load_menu_item_sub_highlight];
				bulb_shader->load();
			}
		} else if (load_menu_item_highlight == 1) {
			info_text = "[A]/[ENTER] to load. Hold [X] to delete a save.";
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)save_files.size() - 1);

			if (gamepad_state->buttons[GamePad_Button_X] || keyboard_state->keyboard['x']) {
				if (load_menu_delete_hold != -1) {
					load_menu_delete_hold += 1;
					if (load_menu_delete_hold >= 100) {
						load_menu_delete_hold = -1;

						remove(save_files[load_menu_item_sub_highlight].file_name.c_str());
						update_save_files();
						load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)save_files.size() - 1);
						return;
					}
				}
			} else {
				load_menu_delete_hold = 0;
			}

			if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
				load_save_file(save_files[load_menu_item_sub_highlight].file_name);
			}
		}
	}
}


void BulbSettings::update_save_files() {
	for (int i = 0; i < (int)save_files.size(); i++) {
		glDeleteTextures(1, &save_files[i].tex_id);
	}
	save_files.clear();

	string directory = "BulbSaves\\*";
	WIN32_FIND_DATA fileData; 
	HANDLE hFind;
	if ( !((hFind = FindFirstFile(directory.c_str(), &fileData)) == INVALID_HANDLE_VALUE) ) {
		while(FindNextFile(hFind, &fileData)) {
			vector<string> file_name_split = split_string(fileData.cFileName, ".");
			if (to_upper(file_name_split[1]) == "BMP") {
				string save_file_name = "BulbSaves\\" + string(fileData.cFileName);
				BulbSave current_save;	
			
				if (current_save.load(save_file_name)) {
					save_files.push_back(current_save);
				}
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
	} else if (menu_open == 4) {
		save_menu_draw();
	}

	if (info_text.length() != 0) {
		int text_width = drawing_tools->text_width(settings_font, info_text);
		
		glColor3f(0.2f,0.2f,0.2f);
		drawing_tools->rectangle_filled(0, (int)drawing_tools->SCREEN_H - font_height, text_width + 10, font_height);
		glColor3f(1,1,1);
		drawing_tools->text(5, (int)drawing_tools->SCREEN_H - font_height + 5, settings_font, info_text);
	}
}

void BulbSettings::load_save_file(string save_file_name) {
	ifstream save_file;
	save_file.open(save_file_name, ios::in);

	BMP save_image;
	save_image.load(save_file_name);

	bulb_shader->read_from_save_file(save_file, save_image.bmp_file_size);
	control_settings->read_from_save_file(save_file, save_image.bmp_file_size);

	save_file.close();
}

void BulbSettings::save_save_file(string save_file_name) {
	// capture and save thumbnail
	glClear(GL_COLOR_BUFFER_BIT);
	bulb_shader->draw();
	glutSwapBuffers();

	int screen_w = (int)drawing_tools->SCREEN_W, screen_h = (int)drawing_tools->SCREEN_H;
	int capture_res = 256, screen_res = min(screen_w, screen_h);
	screen_res -= (screen_res % 4);
	unsigned char *data = new unsigned char[screen_res * screen_res * 3];
	glReadPixels((screen_w / 2) - (screen_res / 2), (screen_h / 2) - (screen_res / 2), screen_res, screen_res, GL_RGB, GL_UNSIGNED_BYTE, data);

	BMP save_image(capture_res, capture_res);
	for (int y = 0; y < capture_res; y++) {
		for (int x = 0; x < capture_res; x++) {
			int screen_x = (x * screen_res) / capture_res;
			int screen_y = ((capture_res - y - 1) * screen_res) / capture_res;
			save_image.set_pixel(x, y, &data[(screen_y * 3 * screen_res) + (screen_x * 3)]);
		}
	}
	save_image.save(save_file_name);

	ofstream save_file;
	save_file.open(save_file_name, ios::app);
	save_file.seekp(save_image.bmp_file_size, ios::beg);

	save_file << "BULBSAVE" << endl;

	bulb_shader->write_to_save_file(save_file);
	control_settings->write_to_save_file(save_file);

	save_file.close();
}

void BulbSettings::input_update(GamePadXbox *gamepad, KeyboardState *keyboard) {
	if (gamepad->State.pressed(GamePad_Button_START)) {
		settings_open = false;
		info_text = "";
	}

	if (menu_open == 0) {
		main_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 1) {
		shader_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 2) {
		control_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 3) {
		load_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 4) {
		save_menu_input_update(&gamepad->State, keyboard);
	}
}