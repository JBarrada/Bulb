#ifndef BULB_SHADER_H
#define BULB_SHADER_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shader_utils.h"

using namespace std;

#define M_PI 3.14159265359f

enum SHADER_VAR_TYPE {
	VAR_BOOL,
	VAR_INT,
	VAR_FLOAT,
	VAR_VEC2,
	VAR_VEC3,
	VAR_VEC4
};

class ShaderVariable {
public:
	string name;
	string category;
	string type;

	SHADER_VAR_TYPE var_type;

	bool hsv_mode;
	bool is_color;

	glm::vec4 value[4];

	bool animate_enable[4];
	glm::vec4 animate_values[4];

	bool update;


	ShaderVariable();

	void load_from_shader_comment(string code);
	void load_from_bulb_save_string(string code);

	string get_bulb_save_string();

	bool needs_update();
	void update_program_variable(GLuint program);

	void adjust_variable(float normalized_amount, int &sub_variable);
	void adjust_animate(float normalized_amount, int &sub_variable, int &animate_variable);

	void set_hsv_mode(bool enable);

	string get_string();
	string get_string(int &sub_variable);
	string get_string_animate(int &sub_variable, int &animate_variable);

	bool is_bright();

	bool operator==(const ShaderVariable& rhs);
};


class BulbShader {
public:
	GLuint program_fp32;
	
	vector<ShaderVariable> shader_variables;
	vector<string> shader_categories;
	vector<vector<int>> shader_categories_indexes;

	string fractal_file;

	vector<string> fractal_files;

	BulbShader();

	void write_to_save_file(ofstream &save_file);
	void read_from_save_file(ifstream &save_file);

	void update_shader_variables();
	void update_control_variables(glm::vec3 camera_eye, glm::vec3 camera_target, glm::vec3 camera_up, float camera_fov, float camera_aspect);

	void draw();
	
	void load();
};


#endif