#ifndef BULB_SHADER_H
#define BULB_SHADER_H

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

#include "shader_utils.h"

using namespace std;

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
	int var_int[3];
	float var_float[3];
	glm::vec2 var_vec2[3];
	glm::vec3 var_vec3[3];
	glm::vec4 var_vec4[3];

	ShaderVariable(string code);

	void update_program_variable(GLuint *program);
};


class BulbShader {
public:
	GLuint program_fp32;
	
	vector<ShaderVariable> shader_variables;
	// stuct for camera pos/orient variables

	

	void load(char *vertex, char *fragment);
};


#endif