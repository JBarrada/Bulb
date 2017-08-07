#ifndef BULB_SETTINGS_H
#define BULB_SETTINGS_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <string>
#include <vector>
#include <algorithm>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "bulb_shader.h"
#include "drawing_tools.h"
#include "gamepad_input.h"

class BulbControlSettings {
public:
	float camera_fov[4];
	int control_expo_power[4];
	float control_move_speed_forward[4];
	float control_move_speed_lateral[4];
	float control_move_speed_vertical[4];
	float control_pitch_speed[4];
	float control_roll_speed[4];
	float control_yaw_speed[4];
	bool control_vibrate[4];

	BulbControlSettings();

	void adjust_variable(float normalized_amount, int variable);
	string get_variable_name(int variable);
	string get_variable_value(int variable);
};

class BulbSettings {
private:
	int control_menu_item_highlight;
	bool control_menu_item_selected;
	int control_menu_item_sub_highlight;

	int shader_menu_item_highlight;
	bool shader_menu_item_selected;
	int shader_menu_item_sub_highlight;
	int shader_menu_category;

	vector<string> shader_categories;
	vector<vector<int>> shader_categories_indexes;

	int main_menu_item_highlight;
	bool main_menu_item_selected;
	int main_menu_item_sub_highlight; // maybe for loading fractal

	void *settings_font;
	int font_height;
public:
	DrawingTools *drawing_tools;

	vector<ShaderVariable> *shader_variables;
	BulbControlSettings control_settings;

	bool settings_open;
	int menu_open;

	BulbSettings(vector<ShaderVariable> *shader_variables, DrawingTools *drawing_tools);

	void update_shader_categories();

	float settings_expo(float value);

	void draw_slider_bar(float x, float y, float bar_width, float bar_height, float current, float min, float max, string format, bool selected);
	void draw_slider_bar(float x, float y, float bar_width, float bar_height, float *values, string format, bool selected);
	void draw_slider_bar(float x, float y, float bar_width, float bar_height, int *values, string format, bool selected);

	void control_menu_draw();
	void control_menu_gamepad_update(GamePadState *state);

	void shader_menu_draw();
	void shader_menu_gamepad_update(GamePadState *state);

	void main_menu_draw();
	void main_menu_gamepad_update(GamePadState *state);
	
	void draw();

	void gamepad_update(GamePadState *state);
};


#endif