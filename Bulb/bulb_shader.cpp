#include "bulb_shader.h"

string parse_to_next(string input, string target, int &pos) {
	string output = "";

	while (pos < (int)input.size()) {
		for (int i = 0; i < (int)target.size(); i++) {
			if (input[pos] == target[i]) {
				return output;
			}
		}
		output += input[pos++];
	}

	return output;
}

void stovec(string values_string, glm::vec4 &value) {
	int values_string_pos = 0;

	for (int i = 0; i < 4; i++) {
		string current_value = parse_to_next(values_string, ",", values_string_pos); values_string_pos++;

		try {
			value[i] = stof(current_value);
		} catch (std::invalid_argument) {
			value[i] = 0.0f;
			return;
		}
	}
}

ShaderVariable::ShaderVariable(string code) {
	name = "";
	category = "";
	type = "";

	is_color = false;

	update = true;

	for (int i = 0; i < 4; i++) {
		animate_enable[i] = false;
		animate_values[i][0] = 0.0f; // speed
		animate_values[i][1] = 1.0f; // scale
		animate_values[i][2] = 0.0f; // offset
		animate_values[i][3] = 0.0f; // pos
	}

	if (code.substr(0, 7) == "uniform") {
		int code_pos = 8;

		string type_string = parse_to_next(code, " ", code_pos); code_pos++;
		if (type_string == "bool") var_type = VAR_BOOL;
		if (type_string == "int") var_type = VAR_INT;
		if (type_string == "float") var_type = VAR_FLOAT;
		if (type_string == "vec2") var_type = VAR_VEC2;
		if (type_string == "vec3") var_type = VAR_VEC3;
		if (type_string == "vec4") var_type = VAR_VEC4;

		name = parse_to_next(code, " =;(", code_pos); code_pos++;

		int tilde_pos = code.find("~");
		code_pos = tilde_pos + 1;
		category = parse_to_next(code, ",", code_pos); code_pos++;
		
		type = parse_to_next(code, ",", code_pos); code_pos++;

		
		string var_default = parse_to_next(code, "|", code_pos); code_pos++;
		string var_min = parse_to_next(code, "|", code_pos); code_pos++;
		string var_max = parse_to_next(code, "|", code_pos); code_pos++;

		if (var_type == VAR_BOOL) {
			value[0][0] = (float)(var_default[0] == 't');
			value[1][0] = (float)(var_min[0] == 't');
			value[2][0] = (float)(var_max[0] == 't');
		} else {
			stovec(var_default, value[0]);
			stovec(var_min, value[1]);
			stovec(var_max, value[2]);
		}
		value[3] = value[2] - value[1];
	}

	is_color = (type == "color" || type == "color4");
}

bool ShaderVariable::needs_update() {
	return (update + animate_enable[0] + animate_enable[1] + animate_enable[2] + animate_enable[3]) != 0;
}

void ShaderVariable::update_program_variable(GLuint program) {
	for (int i = 0; i < 4; i++) {
		if (animate_enable[i]) {
			animate_values[i][3] += 0.025f * animate_values[i][0];
			if (animate_values[i][3] > M_PI*2.0f) animate_values[i][3] -= M_PI*2.0f; 

			float animate_wave = (((sin(animate_values[i][3]) * animate_values[i][1]) + 1.0f) / 2.0f) + (animate_values[i][2] / 2.0f);
			value[0][i] = (animate_wave * value[3][i]) + value[1][i];
		}
	}

	const char *var_name = name.c_str();
	GLint var_pointer = glGetUniformLocation(program, var_name);
	
	if (var_type == VAR_BOOL) {
		glUniform1i(var_pointer, (GLint)value[0][0]);
	}
	if (var_type == VAR_INT) {
		glUniform1i(var_pointer, (GLint)value[0][0]);
	}
	if (var_type == VAR_FLOAT) {
		glUniform1f(var_pointer, (GLfloat)value[0][0]);
	}
	if (var_type == VAR_VEC2) {
		glUniform2fv(var_pointer, 1, (float*)&value[0]);
	}
	if (var_type == VAR_VEC3) {
		glUniform3fv(var_pointer, 1, (float*)&value[0]);
	}
	if (var_type == VAR_VEC4) {
		glUniform4fv(var_pointer, 1, (float*)&value[0]);
	}

	update = false;
}

void ShaderVariable::adjust_variable(float normalized_amount, int &sub_variable) {
	int sub_var_count[6] = {1, 1, 1, 2, 3, 4};
	sub_variable = glm::clamp(sub_variable, 0, sub_var_count[var_type] - 1);
	
	if (!animate_enable[sub_variable]) {
		float resolution = 100.0f;
		if (var_type == VAR_BOOL) {
			if (abs(normalized_amount) == 1.0f) {
				value[0][sub_variable] = (value[0][sub_variable] == 1.0f) ? 0.0f : 1.0f;
			}
		} else {
			float step = value[3][sub_variable] / resolution;
			value[0][sub_variable] = glm::clamp(value[0][sub_variable] + (normalized_amount * step), value[1][sub_variable], value[2][sub_variable]);
		}
		
		update = true;
	}
}

void ShaderVariable::adjust_animate(float normalized_amount, int &sub_variable, int &animate_variable) {
	int sub_var_count[6] = {1, 1, 1, 2, 3, 4};
	sub_variable = glm::clamp(sub_variable, 0, sub_var_count[var_type] - 1);
	animate_variable = glm::clamp(animate_variable, 0, 2);

	float step = 0.0f;
	float resolution = 100.0f;
	if (animate_variable == 0) {
		step = 1.0f / resolution;
		animate_values[sub_variable][animate_variable] = glm::clamp(animate_values[sub_variable][animate_variable] + (normalized_amount * step), 0.0f, 1.0f);
	}
	if (animate_variable == 1) {
		step = 1.0f / resolution;
		animate_values[sub_variable][animate_variable] = glm::clamp(animate_values[sub_variable][animate_variable] + (normalized_amount * step), 0.0f, 1.0f);
	}
	if (animate_variable == 2) {
		step = 2.0f / resolution;
		animate_values[sub_variable][animate_variable] = glm::clamp(animate_values[sub_variable][animate_variable] + (normalized_amount * step), -1.0f, 1.0f);
	}
}

string ShaderVariable::get_string() {
	char text[50];

	if (var_type == VAR_BOOL) {
		sprintf_s(text, ((value[0][0]) ? "true" : "false"));
	}
	if (var_type == VAR_INT) {
		sprintf_s(text, "%0.0f", value[0][0]);
	}
	if (var_type == VAR_FLOAT) {
		sprintf_s(text, "%f", value[0][0]);
	}
	if (var_type == VAR_VEC2) {
		sprintf_s(text, "%0.2f, %0.2f", value[0].x, value[0].y);
	}
	if (var_type == VAR_VEC3) {
		sprintf_s(text, "%0.2f, %0.2f, %0.2f", value[0].x, value[0].y, value[0].z);
	}
	if (var_type == VAR_VEC4) {
		sprintf_s(text, "%0.2f, %0.2f, %0.2f, %0.2f", value[0].x, value[0].y, value[0].z, value[0].w);
	}

	return string(text);
}

string ShaderVariable::get_string(int &sub_variable) {
	char text[50];

	string color_prefix[4] = {"R: ", "G: ", "B: ", ""};
	string other_prefix[4] = {"X: ", "Y: ", "Z: ", ""};

	if (var_type == VAR_BOOL) {
		sprintf_s(text, ((value[0][0]) ? "true" : "false"));
	} else if (var_type == VAR_INT) {
		sprintf_s(text, "%0.0f", value[0][0]);
	} else if (var_type == VAR_FLOAT) {
		sprintf_s(text, "%f", value[0][0]);
	} else {
		sprintf_s(text, "%s%f",((is_color) ? color_prefix[sub_variable].c_str() : other_prefix[sub_variable].c_str()), value[0][sub_variable]);
	}

	return string(text);
}

string ShaderVariable::get_string_animate(int &sub_variable, int &animate_variable) {
	char text[50];
	
	string animate_prefix[3] = {"Animate Speed: ", "Animate Scale: ", "Animate Offset: "};

	sprintf_s(text, "%s%f", animate_prefix[animate_variable].c_str(), animate_values[sub_variable][animate_variable]);

	return string(text);
}

bool ShaderVariable::is_bright() {
	if (is_color && ((value[0].r + value[0].g + value[0].b) > 2.0f)) {
		return true;
	}
	return false;
}


BulbShader::BulbShader() {
	fractal_file = "mandelbox.frag";

	// load fractal files
	string directory = "*";
	WIN32_FIND_DATA fileData; 
	HANDLE hFind;
	if ( !((hFind = FindFirstFile(directory.c_str(), &fileData)) == INVALID_HANDLE_VALUE) ) {
		while(FindNextFile(hFind, &fileData)) {
			ifstream current_file;
			current_file.open(fileData.cFileName, ios::in);

			string first_line;
			getline(current_file, first_line);

			if (first_line.substr(0, 9) == "//FRACTAL") {
				fractal_files.push_back(fileData.cFileName);
			}
			current_file.close();
		}
	}
	
	FindClose(hFind);
}

void BulbShader::load() {
	shader_variables.clear(); // maybe dont clear (try to keep similar consistent)

	ofstream frag_temp_file;
	frag_temp_file.open("bulb_temp.frag", ios::out);

	ifstream frag_orig_file;
	frag_orig_file.open("bulb.frag", ios::in);

	string current_line;
	while (getline(frag_orig_file, current_line)) {
		if (current_line.substr(0, 8) == "#include") {
			string include_file_name = current_line.substr(9);
			include_file_name.erase(0, 1);
			include_file_name.erase(include_file_name.size() - 1);

			if (include_file_name == "FRACTAL") {
				include_file_name = fractal_file;
			}

			ifstream include_file;
			include_file.open(include_file_name, ios::in);

			string include_file_line;
			while (getline(include_file, include_file_line)) {
				frag_temp_file << include_file_line << endl;
				if (include_file_line.substr(0, 7) == "uniform" && include_file_line.find("~") != std::string::npos) {
					ShaderVariable sv(include_file_line);
					shader_variables.push_back(sv);
				}
			}

			include_file.close();
		} else {
			frag_temp_file << current_line << endl;
			if (current_line.substr(0, 7) == "uniform" && current_line.find("~") != std::string::npos) {
				ShaderVariable sv(current_line);
				shader_variables.push_back(sv);
			}
		}
	}

	frag_orig_file.close();
	frag_temp_file.close();

	std::sort(shader_variables.begin(), shader_variables.end(), [](const ShaderVariable& lhs, const ShaderVariable& rhs){ return lhs.category < rhs.category; });

	// shader categories
	shader_categories.clear();
	shader_categories_indexes.clear();

	for (int i = 0; i < (int)(shader_variables.size()); i++) {
		if (std::find(shader_categories.begin(), shader_categories.end(), shader_variables[i].category) == shader_categories.end()) {
			shader_categories.push_back(shader_variables[i].category);
			vector<int> new_category;
			shader_categories_indexes.push_back(new_category);
		}

		int found_index = std::find(shader_categories.begin(), shader_categories.end(), shader_variables[i].category) - shader_categories.begin();
		shader_categories_indexes[found_index].push_back(i);
	}

	load_shader("bulb.vert", "bulb_temp.frag", &program_fp32);
}

void BulbShader::draw() {
	glUseProgram(program_fp32);

	glBegin(GL_QUADS);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();
}

void BulbShader::update_shader_variables() {
	for (int i = 0; i < (int)shader_variables.size(); i++) {
		if (shader_variables[i].needs_update()) 
			shader_variables[i].update_program_variable(program_fp32);
	}
}

void BulbShader::update_control_variables(glm::vec3 camera_eye, glm::vec3 camera_target, glm::vec3 camera_up, float camera_fov, float camera_aspect) {
	GLint prog_camera_pos = glGetUniformLocation(program_fp32, "camera_eye");
	glUniform3fv(prog_camera_pos, 1, (float*)&camera_eye);
	
	GLint prog_camera_target = glGetUniformLocation(program_fp32, "camera_target");
	glUniform3fv(prog_camera_target, 1, (float*)&camera_target);

	GLint prog_camera_up = glGetUniformLocation(program_fp32, "camera_up");
	glUniform3fv(prog_camera_up, 1, (float*)&camera_up);

	GLint prog_camera_fov = glGetUniformLocation(program_fp32, "camera_fov");
	glUniform1f(prog_camera_fov, camera_fov);
	
	GLint prog_camera_aspect = glGetUniformLocation(program_fp32, "camera_aspect");
	glUniform1f(prog_camera_aspect, camera_aspect);
}