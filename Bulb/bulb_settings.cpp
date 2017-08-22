#include "bulb_settings.h"

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
		glColor3f(var_fg_color,var_fg_color,var_fg_color);
		int pos = (int)(((variable->value[0][i] - variable->value[1][i]) / variable->value[3][i]) * bar_width);
		drawing_tools->rectangle_filled(x + pos - 2, y, 4, bar_height);

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
		int actual_highlight_index = bulb_shader->shader_categories_indexes[shader_menu_category][shader_menu_item_highlight];
		BulbVariable *selected_variable = &bulb_shader->shader_variables[actual_highlight_index];

		if (gamepad_state->pressed(GamePad_Button_X) || keyboard_state->pressed_keyboard('x')) {
			selected_variable->animate_enable[shader_menu_item_sub_highlight] = !selected_variable->animate_enable[shader_menu_item_sub_highlight];
			shader_menu_item_sub_selected = selected_variable->animate_enable[shader_menu_item_sub_highlight]; // auto enter menu
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
		if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
			shader_menu_item_selected = true;
			shader_menu_item_sub_highlight = 0;
		}
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			menu_open = 0; // main menu
		}

		if (gamepad_state->pressed(GamePad_Button_RIGHT_SHOULDER) || keyboard_state->pressed_special(GLUT_KEY_RIGHT)) shader_menu_category++;
		if (gamepad_state->pressed(GamePad_Button_LEFT_SHOULDER) || keyboard_state->pressed_special(GLUT_KEY_LEFT)) shader_menu_category--;
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
	int menu_items_size = 4;

	if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
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
	if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
		settings_open = false;
	}
	
	if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) main_menu_item_highlight++;
	if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) main_menu_item_highlight--;
	main_menu_item_highlight = glm::clamp(main_menu_item_highlight, 0, menu_items_size - 1);
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
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size) * font_height + 5);

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
		drawing_tools->rectangle_filled(0, 0, 250, (menu_items_size) * font_height + 5);

		glColor4f(0.4f,0.4f,0.4f,1.0f);
		drawing_tools->rectangle_filled(0, load_menu_item_sub_highlight * font_height, 250, font_height);
	
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		for (int i = 0; i < menu_items_size; i++) {
			string file_text = save_files[i];
			drawing_tools->text(5, i * font_height + 5, settings_font, file_text);
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, save_tex_ids[load_menu_item_sub_highlight]);
		drawing_tools->rectangle_tex(250, 0, 256, 256);
		glDisable(GL_TEXTURE_2D);
	}
}

void BulbSettings::load_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state) {
	if (!load_menu_item_selected) {
		int menu_items_size = 2;
		if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
			load_menu_item_selected = true;
			load_menu_item_sub_highlight = 0;

			if (load_menu_item_highlight == 1) update_save_files();
		}
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			menu_open = 0; // main menu
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) load_menu_item_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) load_menu_item_highlight--;
		load_menu_item_highlight = glm::clamp(load_menu_item_highlight, 0, menu_items_size - 1);
	} else {
		if (gamepad_state->pressed(GamePad_Button_B) || keyboard_state->pressed_keyboard(27)) {
			load_menu_item_selected = false;
		}

		if (gamepad_state->pressed(GamePad_Button_DPAD_UP) || keyboard_state->pressed_special(GLUT_KEY_UP)) load_menu_item_sub_highlight++;
		if (gamepad_state->pressed(GamePad_Button_DPAD_DOWN) || keyboard_state->pressed_special(GLUT_KEY_DOWN)) load_menu_item_sub_highlight--;

		if (load_menu_item_highlight == 0) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)bulb_shader->fractal_files.size() - 1);
			
			if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
				bulb_shader->fractal_file = bulb_shader->fractal_files[load_menu_item_sub_highlight];
				bulb_shader->load();
			}
		} else if (load_menu_item_highlight == 1) {
			load_menu_item_sub_highlight = glm::clamp(load_menu_item_sub_highlight, 0, (int)save_files.size() - 1);

			if (gamepad_state->pressed(GamePad_Button_A) || keyboard_state->pressed_keyboard(13)) {
				load_save_file(save_files[load_menu_item_sub_highlight]);
			}
		}
	}
}


void BulbSettings::update_save_files() {
	for (int i = 0; i < (int)save_tex_ids.size(); i++) {
		glDeleteTextures(1, &save_tex_ids[i]);
	}
	save_files.clear();
	save_tex_ids.clear();

	string directory = "BulbSaves\\*";
	WIN32_FIND_DATA fileData; 
	HANDLE hFind;
	if ( !((hFind = FindFirstFile(directory.c_str(), &fileData)) == INVALID_HANDLE_VALUE) ) {
		while(FindNextFile(hFind, &fileData)) {
			vector<string> file_name_split = split_string(fileData.cFileName, ".");
			if (file_name_split[1] == "BULBSAVE" || file_name_split[1] == "bulbsave") {
				string save_file_name = "BulbSaves\\" + string(fileData.cFileName);
				save_files.push_back(save_file_name);

				ifstream save_file(save_file_name, ios::binary);
				GLuint current_tex_id;
				read_save_image(save_file, current_tex_id);
				save_file.close();
				save_tex_ids.push_back(current_tex_id);
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

void BulbSettings::read_save_image(ifstream &save_file, GLuint &tex_id) {
	save_file.clear();
	save_file.seekg(0, ios::beg);

	char data[256*256*3];
	memset(data, 0, 256*256*3);

	int image_width=256, image_height=256;

	streamoff current_pos = 0;
	string save_file_line;
	while (getline(save_file, save_file_line)) {
		if (save_file_line.substr(0, 5) == "IMAGE") {
			vector<string> image_line = split_string(save_file_line, "|");
			image_width = stoi(image_line[1]);
			image_height = stoi(image_line[2]);

			save_file.clear();
			save_file.seekg(current_pos + 14, ios::beg);
			save_file.read(data, image_width*image_height*3);

			break;
		}
		current_pos = save_file.tellg();
	}
	
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void BulbSettings::write_save_image(ofstream &save_file) {
	save_file << "IMAGE|256|256|";
	int screen_w = (int)drawing_tools->SCREEN_W, screen_h = (int)drawing_tools->SCREEN_H;
	int capture_res = 256, screen_res = min(screen_w, screen_h);

	unsigned char *data = new unsigned char[screen_res * screen_res * 3];
	glReadPixels((screen_w / 2) - (screen_res / 2), (screen_h / 2) - (screen_res / 2), screen_res, screen_res, GL_RGB, GL_UNSIGNED_BYTE, data);

	for (int y = 0; y < capture_res; y++) {
		for (int x = 0; x < capture_res; x++) {
			int screen_x = (x * screen_res) / capture_res;
			int screen_y = (y * screen_res) / capture_res;
			int offset = (screen_y * 3 * screen_res) + (screen_x * 3);
			save_file << data[offset+0] << data[offset+1] << data[offset+2];
		}
	}
}

void BulbSettings::load_save_file(string save_file_name) {
	ifstream save_file;
	save_file.open(save_file_name, ios::in);

	bulb_shader->read_from_save_file(save_file);
	control_settings->read_from_save_file(save_file);

	save_file.close();
}

void BulbSettings::save_save_file(string save_file_name) {
	ofstream save_file;
	save_file.open(save_file_name);

	bulb_shader->write_to_save_file(save_file);
	control_settings->write_to_save_file(save_file);
	
	write_save_image(save_file);

	save_file.close();
}

void BulbSettings::input_update(GamePadXbox *gamepad, KeyboardState *keyboard) {
	if (menu_open == 0) {
		main_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 1) {
		shader_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 2) {
		control_menu_input_update(&gamepad->State, keyboard);
	} else if (menu_open == 3) {
		load_menu_input_update(&gamepad->State, keyboard);
	}
}