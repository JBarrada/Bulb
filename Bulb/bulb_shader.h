#ifndef BULB_SHADER_H
#define BULB_SHADER_H

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

	bool var_bool[3];
	int var_int[4];
	float var_float[4];
	glm::vec2 var_vec2[4];
	glm::vec3 var_vec3[4];
	glm::vec4 var_vec4[4];

	bool update;

	bool animate;
	float animate_scale;
	float animate_offset;
	float animate_speed;
	float animate_pos;

	ShaderVariable(string code);

	void update_program_variable(GLuint program);

	void adjust_variable(float normalized_amount, int &sub_variable);
	void adjust_animate(float normalized_amount, int &sub_variable);

	string get_string();
};


class BulbShader {
public:
	GLuint program_fp32;
	
	vector<ShaderVariable> shader_variables;
	// stuct for camera pos/orient variables

	void update_shader_variables();
	void update_control_variables(glm::vec3 camera_eye, glm::vec3 camera_target, glm::vec3 camera_up, float camera_fov, float camera_aspect);

	void draw();
	
	void load(char *vertex, char *fragment);
};


#endif