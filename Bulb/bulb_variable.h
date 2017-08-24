#ifndef BULB_VARIABLE_H
#define BULB_VARIABLE_H

#define M_PI 3.14159265359f

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <Windows.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtx\color_space.hpp>

#include "string_tools.h"
#include "BMP.h"

using namespace std;

enum BULB_VAR_TYPE {
	VAR_BOOL,
	VAR_INT,
	VAR_FLOAT,
	VAR_VEC2,
	VAR_VEC3,
	VAR_VEC4,
	VAR_SAMP2D
};

extern int BULB_VAR_SIZE[];

class BulbVariable {
public:
	string name;
	string category;
	string type;
	BULB_VAR_TYPE var_type;

	bool is_color;
	bool hsv_mode;
	bool no_clamp;
	bool update;

	glm::vec4 value[4];

	bool animate_enable[4];
	glm::vec4 animate_values[4];

	vector<string> image_files;
	string value_image;
	int image_files_index;
	GLuint value_image_tex_id;


	BulbVariable();
	BulbVariable(string code);

	void update_image_files();
	void update_value_image();

	void load_from_shader_comment(string code);
	void load_from_bulb_save_string(string code);
	string get_bulb_save_string(string prefix);

	bool needs_update();
	void update_program_variable(GLuint program);

	void adjust_variable(float analog, int digital, int &sub_variable);
	void adjust_animate(float normalized_amount, int &sub_variable, int &animate_variable);

	void set_hsv_mode(bool enable);
	glm::vec4 get_color();

	string get_string();
	string get_string(int &sub_variable);
	string get_string_animate(int &sub_variable, int &animate_variable);

	bool is_bright();

	bool operator==(const BulbVariable& rhs);
};


#endif
