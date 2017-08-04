#ifndef BULB_SETTINGS_H
#define BULB_SETTINGS_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <string>
#include <vector>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "bulb_shader.h"
#include "drawing_tools.h"
#include "gamepad_input.h"

class BulbSettings {
private:
	int item_highlight;
	bool item_selected;
	int sub_item_highlight;
public:
	DrawingTools *drawing_tools;

	vector<ShaderVariable> *shader_variables;

	bool menu_open;

	BulbSettings(vector<ShaderVariable> *shader_variables, DrawingTools *drawing_tools);

	void gamepad_update(GamePadState *state);

	void draw_bar(float x, float y, float bar_width, float bar_height, float current, float min, float max, string format, bool selected);
	void draw();
};


#endif