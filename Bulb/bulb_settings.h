#ifndef BULB_SETTINGS_H
#define BULB_SETTINGS_H

#include <fstream>
#include <time.h>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>

#include "bulb_shader.h"
#include "bulb_variable.h"
#include "bulb_control_settings.h"
#include "drawing_tools.h"
#include "gamepad_input.h"
#include "keyboard_input.h"

#include "string_tools.h"
#include "BMP.h"

class BulbSave {
public:
	string file_name;
	string clean_name;
	BMP image;
	GLuint tex_id;

	BulbSave();

	bool load(string save_file_name);
};

class BulbSettings {
private:
	int load_menu_item_highlight;
	bool load_menu_item_selected;
	int load_menu_item_sub_highlight;
	//vector<string> save_files;
	//vector<GLuint> save_tex_ids;

	vector<BulbSave> save_files;

	int control_menu_item_highlight;
	bool control_menu_item_selected;
	int control_menu_item_sub_highlight;

	int shader_menu_item_highlight;
	bool shader_menu_item_selected;
	int shader_menu_item_sub_highlight;
	bool shader_menu_item_sub_selected;
	int shader_menu_item_sub_animate_highlight;
	int shader_menu_category;

	int main_menu_item_highlight;
	bool main_menu_item_selected;

	void *settings_font;
	int font_height;
public:
	DrawingTools *drawing_tools;

	BulbShader *bulb_shader;
	BulbControlSettings *control_settings;

	bool settings_open;
	int menu_open;

	BulbSettings(BulbShader *bulb_shader, BulbControlSettings *control_settings, DrawingTools *drawing_tools);

	float settings_expo(float value);

	void draw_bulb_variable(BulbVariable *variable, int sub_highlight, int sub_animate_highlight, bool sub_selected);

	void control_menu_draw();
	void control_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state);

	void shader_menu_draw();
	void shader_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state);

	void main_menu_draw();
	void main_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state);

	void load_menu_draw();
	void load_menu_input_update(GamePadState *gamepad_state, KeyboardState *keyboard_state);

	void update_save_files();
	void write_save_image(ofstream &save_file);
	//void read_save_image(ifstream &save_file, GLuint &tex_id);

	void load_save_file(string save_file_name);
	void save_save_file(string save_file_name);

	void draw();

	void input_update(GamePadXbox *gamepad, KeyboardState *keyboard);
};

#endif