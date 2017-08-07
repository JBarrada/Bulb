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

glm::vec2 stovec2(string value_string) {
	int value_string_pos = 0;

	float x = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;
	float y = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;

	return glm::vec2(x, y);
}

glm::vec3 stovec3(string value_string) {
	int value_string_pos = 0;

	float x = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;
	float y = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;
	float z = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;

	return glm::vec3(x, y, z);
}

glm::vec4 stovec4(string value_string) {
	int value_string_pos = 0;

	float x = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;
	float y = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;
	float z = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;
	float w = stof(parse_to_next(value_string, ",", value_string_pos)); value_string_pos++;

	return glm::vec4(x, y, z, w);
}

ShaderVariable::ShaderVariable(string code) {
	name = "";
	category = "";
	type = "";

	update = true;

	animate = false;
	animate_pos = 0.0;
	animate_scale = 1.0;
	animate_offset = 0.0;
	animate_speed = 0.0;

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
			var_bool[0] = (var_default[0] == 't');
			var_bool[1] = (var_min[0] == 't');
			var_bool[2] = (var_max[0] == 't');
		}
		if (var_type == VAR_INT) {
			var_int[0] = stoi(var_default);
			var_int[1] = stoi(var_min);
			var_int[2] = stoi(var_max);
			var_int[3] = var_int[2] - var_int[1];
		}
		if (var_type == VAR_FLOAT) {
			var_float[0] = stof(var_default);
			var_float[1] = stof(var_min);
			var_float[2] = stof(var_max);
			var_float[3] = var_float[2] - var_float[1];
		}
		if (var_type == VAR_VEC2) {
			var_vec2[0] = stovec2(var_default);
			var_vec2[1] = stovec2(var_min);
			var_vec2[2] = stovec2(var_max);
			var_vec2[3] = var_vec2[2] - var_vec2[1];
		}
		if (var_type == VAR_VEC3) {
			var_vec3[0] = stovec3(var_default);
			var_vec3[1] = stovec3(var_min);
			var_vec3[2] = stovec3(var_max);
			var_vec3[3] = var_vec3[2] - var_vec3[1];
		}
		if (var_type == VAR_VEC4) {
			var_vec4[0] = stovec4(var_default);
			var_vec4[1] = stovec4(var_min);
			var_vec4[2] = stovec4(var_max);
			var_vec4[3] = var_vec4[2] - var_vec4[1];
		}
	}
}

void ShaderVariable::update_program_variable(GLuint program) {
	const char *var_name = name.c_str();
	GLint var_pointer = glGetUniformLocation(program, var_name);

	if (animate) {
		animate_pos += 0.025f * animate_speed;
		if (animate_pos > M_PI*2.0)
			animate_pos -= M_PI*2.0;

		float animate_wave = (((sin(animate_pos) * animate_scale) + 1.0f) / 2.0f) + (animate_offset / 2.0f);
		if (var_type == VAR_INT) {
			var_int[0] = (int)(animate_wave * var_int[3]) + var_int[1];
		}
		if (var_type == VAR_FLOAT) {
			var_float[0] = (animate_wave * var_float[3]) + var_float[1];
		}
		if (var_type == VAR_VEC2) {
			var_vec2[0] = (animate_wave * var_vec2[3]) + var_vec2[1];
		}
		if (var_type == VAR_VEC3) {
			var_vec3[0] = (animate_wave * var_vec3[3]) + var_vec3[1];
		}
		if (var_type == VAR_VEC4) {
			var_vec4[0] = (animate_wave * var_vec4[3]) + var_vec4[1];
		}
	}

	if (var_type == VAR_BOOL) {
		glUniform1i(var_pointer, (GLint)var_bool[0]);
	}
	if (var_type == VAR_INT) {
		glUniform1i(var_pointer, (GLint)var_int[0]);
	}
	if (var_type == VAR_FLOAT) {
		glUniform1f(var_pointer, (GLfloat)var_float[0]);
	}
	if (var_type == VAR_VEC2) {
		glUniform2fv(var_pointer, 1, (float*)&var_vec2[0]);
	}
	if (var_type == VAR_VEC3) {
		glUniform3fv(var_pointer, 1, (float*)&var_vec3[0]);
	}
	if (var_type == VAR_VEC4) {
		glUniform4fv(var_pointer, 1, (float*)&var_vec4[0]);
	}

	update = false;
}

void ShaderVariable::adjust_variable(float normalized_amount, int &sub_variable) {
	float step = 0.0f;
	float resolution = 100.0f;

	if (var_type == VAR_BOOL) {
		if (abs(normalized_amount) == 1.0f) var_bool[0] = !var_bool[0];
	}
	if (var_type == VAR_INT) {
		step = (float)var_int[3] / resolution;
		var_int[0] = glm::clamp(var_int[0] + (int)(normalized_amount * step), var_int[1], var_int[2]);
	}
	if (var_type == VAR_FLOAT) {
		step = var_float[3] / resolution;
		var_float[0] = glm::clamp(var_float[0] + (normalized_amount * step), var_float[1], var_float[2]);
	}
	if (var_type == VAR_VEC2) {
		sub_variable = glm::clamp(sub_variable, 0, 1);

		step = var_vec2[3][sub_variable] / resolution;
		var_vec2[0][sub_variable] = glm::clamp(var_vec2[0][sub_variable] + (normalized_amount * step), var_vec2[1][sub_variable], var_vec2[2][sub_variable]);
	}
	if (var_type == VAR_VEC3) {
		sub_variable = glm::clamp(sub_variable, 0, 2);

		step = var_vec3[3][sub_variable] / resolution;
		var_vec3[0][sub_variable] = glm::clamp(var_vec3[0][sub_variable] + (normalized_amount * step), var_vec3[1][sub_variable], var_vec3[2][sub_variable]);
	}
	if (var_type == VAR_VEC4) {
		sub_variable = glm::clamp(sub_variable, 0, 3);

		step = var_vec4[3][sub_variable] / resolution;
		var_vec4[0][sub_variable] = glm::clamp(var_vec4[0][sub_variable] + (normalized_amount * step), var_vec4[1][sub_variable], var_vec4[2][sub_variable]);
	}

	update = true;
}

void ShaderVariable::adjust_animate(float normalized_amount, int &sub_variable) {
	float step = 0.0f;
	float resolution = 100.0f;

	sub_variable = glm::clamp(sub_variable, 0, 2);

	if (sub_variable == 0) {
		step = 1.0f / resolution;
		animate_speed = glm::clamp(animate_speed + (normalized_amount * step), 0.0f, 1.0f);
	}
	if (sub_variable == 1) {
		step = 1.0f / resolution;
		animate_scale = glm::clamp(animate_scale + (normalized_amount * step), 0.0f, 1.0f);
	}
	if (sub_variable == 2) {
		step = 2.0f / resolution;
		animate_offset = glm::clamp(animate_offset + (normalized_amount * step), -1.0f, 1.0f);
	}
}

string ShaderVariable::get_string() {
	char text[50];

	if (var_type == VAR_BOOL) {
		sprintf_s(text, ((var_bool[0]) ? "true" : "false"));
	}
	if (var_type == VAR_INT) {
		sprintf_s(text, "%d", var_int[0]);
	}
	if (var_type == VAR_FLOAT) {
		sprintf_s(text, "%f", var_float[0]);
	}
	if (var_type == VAR_VEC2) {
		sprintf_s(text, "%0.2f, %0.2f", var_vec2[0].x, var_vec2[0].y);
	}
	if (var_type == VAR_VEC3) {
		sprintf_s(text, "%0.2f, %0.2f, %0.2f", var_vec3[0].x, var_vec3[0].y, var_vec3[0].z);
	}
	if (var_type == VAR_VEC4) {
		sprintf_s(text, "%0.2f, %0.2f, %0.2f, %0.2f", var_vec4[0].x, var_vec4[0].y, var_vec4[0].z, var_vec4[0].w);
	}

	string s(text);
	return s;
}

void BulbShader::load(char *vertex, char *fragment) {
	shader_variables.clear();

	ofstream frag_temp_file;
	frag_temp_file.open("bulb_temp.frag", ios::out);

	ifstream frag_orig_file;
	frag_orig_file.open(fragment, ios::in);

	string current_line;
	while (getline(frag_orig_file, current_line)) {
		if (current_line.substr(0, 8) == "#include") {
			string include_file_name = current_line.substr(9);
			include_file_name.erase(0, 1);
			include_file_name.erase(include_file_name.size() - 1);

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

	load_shader(vertex, "bulb_temp.frag", &program_fp32);
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
		if (shader_variables[i].update || shader_variables[i].animate) 
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