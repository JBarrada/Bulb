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

class BulbSettings {
private:
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

	bool settings_open;
	int menu_open;

	BulbSettings(vector<ShaderVariable> *shader_variables, DrawingTools *drawing_tools);

	void update_shader_variables();

	void draw_slider_bar(float x, float y, float bar_width, float bar_height, float current, float min, float max, string format, bool selected);
	
	void shader_menu_draw();
	void shader_menu_gamepad_update(GamePadState *state);

	void main_menu_draw();
	void main_menu_gamepad_update(GamePadState *state);
	
	void draw();

	void gamepad_update(GamePadState *state);
};


#endif