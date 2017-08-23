#ifndef BULB_SHADER_H
#define BULB_SHADER_H

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include <glew.h>
#include <glut.h>
#include <glm\glm.hpp>

#include "shader_utils.h"
#include "string_tools.h"
#include "bulb_variable.h"
#include "bulb_control_settings.h"

using namespace std;

class BulbShader {
public:
	GLuint program_fp32;
	
	vector<BulbVariable> shader_variables;
	vector<string> shader_categories;
	vector<vector<int>> shader_categories_indexes;

	string fractal_file;

	vector<string> fractal_files;

	BulbShader();

	void write_to_save_file(ofstream &save_file);
	void read_from_save_file(ifstream &save_file, int data_start);

	void update_shader_variables();
	void update_control_variables(BulbControlSettings *control_settings, float camera_aspect);

	void draw();
	
	void load();
};

#endif