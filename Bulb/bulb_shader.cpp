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

vector<string> split_string(string input, string target) {
	vector<string> split_output;

	int input_pos = 0;
	while (input_pos < (int)input.size()) {
		string current_output = parse_to_next(input, target, input_pos); input_pos++;

		split_output.push_back(current_output);
	}

	return split_output;
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

glm::vec4 rgb2hsv(glm::vec4 c) {
	glm::vec4 K = glm::vec4(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
	glm::vec4 p = glm::mix(glm::vec4(c.b, c.g, K.w, K.z), glm::vec4(c.g, c.b, K.x, K.y), (c.g < c.b) ? 0.0f : 1.0f); // glm::step(c.b, c.g)
	glm::vec4 q = glm::mix(glm::vec4(p.x, p.y, p.w, c.r), glm::vec4(c.r, p.y, p.z, p.x), (c.r < p.x) ? 0.0f : 1.0f); // glm::step(p.x, c.r)

	float d = q.x - glm::min(q.w, q.y);
	float e = 1.0e-10f;
	return glm::vec4(glm::abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x, c.w);
}

glm::vec4 hsv2rgb(glm::vec4 c) {
	glm::vec4 K = glm::vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
    glm::vec3 p = glm::abs(glm::fract(glm::vec3(c.x) + glm::vec3(K.x, K.y, K.z)) * 6.0f - glm::vec3(K.w));
    return glm::vec4(c.z * glm::mix(glm::vec3(K.x), glm::clamp(p - glm::vec3(K.x), 0.0f, 1.0f), c.y), c.w);
}

ShaderVariable::ShaderVariable() {
	name = "";
	category = "";
	type = "";

	hsv_mode = false;
	is_color = false;

	update = true;

	for (int i = 0; i < 4; i++) {
		animate_enable[i] = false;
		animate_values[i][0] = 0.0f; // speed
		animate_values[i][1] = 1.0f; // scale
		animate_values[i][2] = 0.0f; // offset
		animate_values[i][3] = 0.0f; // pos
	}
}

void ShaderVariable::load_from_shader_comment(string code) {
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

void ShaderVariable::load_from_bulb_save_string(string code) {
	vector<string> code_split = split_string(code, "|");

	if ((int)code_split.size() >= 16) {
		if (code_split[0] == "SV") {
			name = code_split[1];
			category = code_split[2];
			type = code_split[3];

			// flags
			var_type = (SHADER_VAR_TYPE)stoi(code_split[4]);
			hsv_mode = (stoi(code_split[5]) == 1);
			is_color = (stoi(code_split[6]) == 1);

			// values
			for (int i = 0; i < 4; i++) {
				stovec(code_split[7+i], value[i]);
			}

			// animate enable
			vector<string> animate_enable_split = split_string(code_split[11], ",");
			for (int i = 0; i < 4; i++) {
				animate_enable[i] = (stoi(animate_enable_split[i]) == 1);
			}

			// animate values
			for (int i = 0; i < 4; i++) {
				stovec(code_split[12+i], animate_values[i]);
			}

			update = true;
		}
	}
}

string ShaderVariable::get_bulb_save_string() {
	string save_string = "SV|" + name + "|" + category + "|" + type + "|";

	// flags
	char flags_chars[50];
	sprintf_s(flags_chars, "%d|%d|%d|", var_type, hsv_mode, is_color);
	save_string += string(flags_chars);

	// values
	for (int i = 0; i < 4; i++) {
		char value_chars[100];
		sprintf_s(value_chars, "%f,%f,%f,%f|", value[i][0], value[i][1], value[i][2], value[i][3]);
		save_string += string(value_chars);
	}

	// animate enable
	char animate_enable_chars[50];
	sprintf_s(animate_enable_chars, "%d,%d,%d,%d|", animate_enable[0], animate_enable[1], animate_enable[2], animate_enable[3]);
	save_string += string(animate_enable_chars);

	// animate values
	for (int i = 0; i < 4; i++) {
		char animate_value_chars[100];
		sprintf_s(animate_value_chars, "%f,%f,%f,%f|", animate_values[i][0], animate_values[i][1], animate_values[i][2], animate_values[i][3]);
		save_string += string(animate_value_chars);
	}

	return save_string;
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
		if (hsv_mode) {
			glm::vec4 color_rgb = hsv2rgb(value[0]);
			glUniform3fv(var_pointer, 1, (float*)&color_rgb);
		} else {
			glUniform3fv(var_pointer, 1, (float*)&value[0]);
		}
	}
	if (var_type == VAR_VEC4) {
		if (hsv_mode) {
			glm::vec4 color_rgb = hsv2rgb(value[0]);
			glUniform3fv(var_pointer, 1, (float*)&color_rgb);
		} else {
			glUniform4fv(var_pointer, 1, (float*)&value[0]);
		}
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

void ShaderVariable::set_hsv_mode(bool enable) {
	if (is_color) {
		if (hsv_mode != enable) {
			hsv_mode = enable;
			if (enable) {
				value[0] = rgb2hsv(value[0]);
			} else {
				value[0] = hsv2rgb(value[0]);
			}
		}
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

	string prefixes[3][4] = {{"X: ", "Y: ", "Z: ", ""}, {"R: ", "G: ", "B: ", ""}, {"H: ", "S: ", "V: ", ""}};

	if (var_type == VAR_BOOL) {
		sprintf_s(text, ((value[0][0]) ? "true" : "false"));
	} else if (var_type == VAR_INT) {
		sprintf_s(text, "%0.0f", value[0][0]);
	} else if (var_type == VAR_FLOAT) {
		sprintf_s(text, "%f", value[0][0]);
	} else {
		int prefix_index = 0;
		if (is_color) {
			if (hsv_mode) {
				prefix_index = 2;
			} else {
				prefix_index = 1;
			}
		}
		sprintf_s(text, "%s%f", prefixes[prefix_index][sub_variable].c_str(), value[0][sub_variable]);
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

bool ShaderVariable::operator==(const ShaderVariable& rhs) {
	if ((rhs.category == category) && (rhs.name == name)) {
		return true;
	} else {
		return false;
	}
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

void BulbShader::write_to_save_file(ofstream &save_file) {
	save_file << "SF|" << fractal_file << endl; 
	for (int i = 0; i < (int)shader_variables.size(); i++) {
		save_file << shader_variables[i].get_bulb_save_string() << endl;
	}
}

void BulbShader::read_from_save_file(ifstream &save_file) {
	save_file.clear();
	save_file.seekg(0, ios::beg);
	
	vector<ShaderVariable> shader_variables_new;

	string save_file_line;
	while (getline(save_file, save_file_line)) {
		if (save_file_line.substr(0, 2) == "SF") {
			fractal_file = save_file_line.substr(3);
		}
		if (save_file_line.substr(0, 2) == "SV") {
			ShaderVariable sv;
			sv.load_from_bulb_save_string(save_file_line);
			shader_variables_new.push_back(sv);
		}
	}

	load();

	for (int i = 0; i < (int)shader_variables_new.size(); i++) {
		vector<ShaderVariable>::iterator found = std::find(shader_variables.begin(), shader_variables.end(), shader_variables_new[i]);
			
		if (found != shader_variables.end()) {
			shader_variables[found - shader_variables.begin()] = shader_variables_new[i];
		}
	}
}

void BulbShader::load() {
	ofstream frag_temp_file;
	frag_temp_file.open("bulb_temp.frag", ios::out);

	ifstream frag_orig_file;
	frag_orig_file.open("bulb.frag", ios::in);

	vector<ShaderVariable> shader_variables_old(shader_variables);
	vector<ShaderVariable> shader_variables_new;

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
					ShaderVariable sv;
					sv.load_from_shader_comment(include_file_line);
					shader_variables_new.push_back(sv);
				}
			}

			include_file.close();
		} else {
			frag_temp_file << current_line << endl;
			if (current_line.substr(0, 7) == "uniform" && current_line.find("~") != std::string::npos) {
				ShaderVariable sv;
				sv.load_from_shader_comment(current_line);
				shader_variables_new.push_back(sv);
			}
		}
	}

	frag_orig_file.close();
	frag_temp_file.close();

	shader_variables.clear();
	for (int i = 0; i < (int)shader_variables_new.size(); i++) {
		if (shader_variables_new[i].category == "Fractal") {
			shader_variables.push_back(shader_variables_new[i]);
		} else {
			vector<ShaderVariable>::iterator found = std::find(shader_variables_old.begin(), shader_variables_old.end(), shader_variables_new[i]);
			
			if (found != shader_variables_old.end()) {
				shader_variables.push_back(shader_variables_old[found - shader_variables_old.begin()]);
			} else {
				shader_variables.push_back(shader_variables_new[i]);
			}
		}
	}

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