#include "bulb_shader.h"

glm::vec4 rgb2hsv(glm::vec4 c) {
	glm::vec4 K = glm::vec4(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
	glm::vec4 p = glm::mix(glm::vec4(c.b, c.g, K.w, K.z), glm::vec4(c.g, c.b, K.x, K.y), (c.g < c.b) ? 0.0f : 1.0f);
	glm::vec4 q = glm::mix(glm::vec4(p.x, p.y, p.w, c.r), glm::vec4(c.r, p.y, p.z, p.x), (c.r < p.x) ? 0.0f : 1.0f);

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
	var_type = VAR_VEC4;

	is_color = false;
	hsv_mode = false;
	update = true;

	for (int i = 0; i < 4; i++) {
		value[i] = glm::vec4(0);
		animate_enable[i] = false;
		animate_values[i] = glm::vec4(0,1,0,0);
	}
}

ShaderVariable::ShaderVariable(string code) {
	this->ShaderVariable::ShaderVariable();

	if (code.find("~") != string::npos) load_from_shader_comment(code); else load_from_bulb_save_string(code);
}

void ShaderVariable::load_from_shader_comment(string code) {
	if (code.substr(0, 7) == "uniform") {
		vector<string> split_space = split_string(code, " ");
		vector<string> split_comment = split_string(code.substr(code.find("~") + 1), "|");

		string type_strings[] = {"bool", "int", "float", "vec2", "vec3", "vec4"};
		var_type = (SHADER_VAR_TYPE)std::distance(type_strings, std::find(type_strings, type_strings + 6, split_space[1]));

		name = split_space[2];
		category = split_comment[0];
		type = to_upper(split_comment[1]);
		is_color = (type == "COLOR" || type == "COLOR4");

		for (int i = 0; i < 3; i++) {
			if (var_type == VAR_BOOL) value[i][0] = (float)(to_upper(split_comment[2+i]) == "TRUE");
			if (var_type != VAR_BOOL) stovec(split_comment[2+i], value[i]);
		}
		value[3] = value[2] - value[1];
	}
}

void ShaderVariable::load_from_bulb_save_string(string code) {
	vector<string> code_split = split_string(code, "|");

	if ((int)code_split.size() >= 16) {
		if (code_split[0] == "SV") {
			name = code_split[1];
			category = code_split[2];
			type = to_upper(code_split[3]);
			var_type = (SHADER_VAR_TYPE)stoi(code_split[4]);
			
			is_color = (stoi(code_split[5]) == 1);
			hsv_mode = (stoi(code_split[6]) == 1);
			update = true;

			for (int i = 0; i < 4; i++) {
				stovec(code_split[7+i], value[i]);

				animate_enable[i] = (code_split[11][i] == '1');
				stovec(code_split[12+i], animate_values[i]);
			}
		}
	}
}

string ShaderVariable::get_bulb_save_string() {
	char save_chars[1000];
	int j = 0;

	j += sprintf_s(save_chars+j, 1000-j, "SV|%s|%s|%s|%d|", name.c_str(), category.c_str(), type.c_str(), var_type);
	j += sprintf_s(save_chars+j, 1000-j, "%d|%d|", is_color, hsv_mode);
	j += sprintf_s(save_chars+j, 1000-j, "%d%d%d%d|", animate_enable[0], animate_enable[1], animate_enable[2], animate_enable[3]);

	for (int i=0; i<4; i++) {
		j += sprintf_s(save_chars+j, 1000-j, "%f,%f,%f,%f|", value[i][0], value[i][1], value[i][2], value[i][3]);
	}
	for (int i=0; i<4; i++) {
		j += sprintf_s(save_chars+j, 1000-j, "%f,%f,%f,%f|", animate_values[i][0], animate_values[i][1], animate_values[i][2], animate_values[i][3]);
	}

	return string(save_chars);
}

bool ShaderVariable::needs_update() {
	return (update + animate_enable[0] + animate_enable[1] + animate_enable[2] + animate_enable[3]) != 0;
}

void ShaderVariable::update_program_variable(GLuint program) {
	for (int i = 0; i < 4; i++) {
		if (animate_enable[i]) {
			animate_values[i][3] = glm::mod(animate_values[i][3] + 0.025f * animate_values[i][0], M_PI*2.0f);
			float animate_wave = (((sin(animate_values[i][3]) * animate_values[i][1]) + 1.0f) / 2.0f) + (animate_values[i][2] / 2.0f);
			value[0][i] = (animate_wave * value[3][i]) + value[1][i];
		}
	}

	const char *var_name = name.c_str();
	GLint var_pointer = glGetUniformLocation(program, var_name);
	
	if (var_type == VAR_BOOL) glUniform1i(var_pointer, (GLint)value[0][0]);
	if (var_type == VAR_INT) glUniform1i(var_pointer, (GLint)value[0][0]);
	if (var_type == VAR_FLOAT) glUniform1f(var_pointer, (GLfloat)value[0][0]);
	if (var_type == VAR_VEC2) glUniform2fv(var_pointer, 1, (float*)&value[0]);
	
	if (hsv_mode) {
		glm::vec4 color_rgb = hsv2rgb(value[0]);
		if (var_type == VAR_VEC3) glUniform3fv(var_pointer, 1, (float*)&color_rgb);
		if (var_type == VAR_VEC4) glUniform4fv(var_pointer, 1, (float*)&color_rgb);
	} else {
		if (var_type == VAR_VEC3) glUniform3fv(var_pointer, 1, (float*)&value[0]);
		if (var_type == VAR_VEC4) glUniform4fv(var_pointer, 1, (float*)&value[0]);
	}

	update = false;
}

void ShaderVariable::adjust_variable(float normalized_amount, int &sub_variable) {
	int sub_var_count[6] = {1, 1, 1, 2, 3, 4};
	sub_variable = glm::clamp(sub_variable, 0, sub_var_count[var_type] - 1);
	
	if (!animate_enable[sub_variable]) {
		if (var_type == VAR_BOOL) {
			if (abs(normalized_amount) == 1.0f) {
				value[0][sub_variable] = (value[0][sub_variable] == 1.0f) ? 0.0f : 1.0f;
			}
		} else {
			float resolution = 100.0f;
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

	glm::vec3 animate_ranges[] = {glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec3(-1, 1, 2)};
	float resolution = 100.0f;
	float step = animate_ranges[animate_variable][2] / resolution;
	animate_values[sub_variable][animate_variable] = glm::clamp(animate_values[sub_variable][animate_variable] + (normalized_amount * step), animate_ranges[animate_variable][0], animate_ranges[animate_variable][1]);
}

void ShaderVariable::set_hsv_mode(bool enable) {
	if (is_color && hsv_mode != enable) {
		hsv_mode = enable;
		value[0] = (enable) ? rgb2hsv(value[0]) : hsv2rgb(value[0]);
	}
}

glm::vec4 ShaderVariable::get_color() {
	return (hsv_mode) ? hsv2rgb(value[0]) : value[0];
}

string ShaderVariable::get_string() {
	char text[50];

	if (var_type == VAR_BOOL) sprintf_s(text, ((value[0][0]) ? "true" : "false"));
	if (var_type == VAR_INT) sprintf_s(text, "%0.0f", value[0][0]);
	if (var_type == VAR_FLOAT) sprintf_s(text, "%f", value[0][0]);
	if (var_type == VAR_VEC2) sprintf_s(text, "%0.2f, %0.2f", value[0].x, value[0].y);
	if (var_type == VAR_VEC3) sprintf_s(text, "%0.2f, %0.2f, %0.2f", value[0].x, value[0].y, value[0].z);
	if (var_type == VAR_VEC4) sprintf_s(text, "%0.2f, %0.2f, %0.2f, %0.2f", value[0].x, value[0].y, value[0].z, value[0].w);

	return string(text);
}

string ShaderVariable::get_string(int &sub_variable) {
	char text[50];
	string prefixes[3][4] = {{"X: ", "Y: ", "Z: ", ""}, {"R: ", "G: ", "B: ", ""}, {"H: ", "S: ", "V: ", ""}};

	if (var_type == VAR_BOOL) sprintf_s(text, ((value[0][0]) ? "true" : "false"));
	if (var_type == VAR_INT) sprintf_s(text, "%0.0f", value[0][0]);
	if (var_type == VAR_FLOAT) sprintf_s(text, "%f", value[0][0]);
	if (var_type >= VAR_VEC2) {
		int prefix_index = (int)is_color + (int)hsv_mode;
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
	glm::vec4 variable_color = get_color();
	return (is_color && ((variable_color.r + variable_color.g + variable_color.b) > 2.0f));
}

bool ShaderVariable::operator==(const ShaderVariable& rhs) {
	return ((rhs.category == category) && (rhs.name == name));
}


BulbShader::BulbShader() {
	fractal_file = "BulbFractals\\mandelbox.frag";

	WIN32_FIND_DATA file_data; 
	HANDLE hFind;
	if (!((hFind = FindFirstFile("BulbFractals\\*", &file_data)) == INVALID_HANDLE_VALUE)) {
		while (FindNextFile(hFind, &file_data)) {
			ifstream current_file("BulbFractals\\" + string(file_data.cFileName), ios::in);

			string first_line;
			getline(current_file, first_line);
			if (first_line.substr(0, 9) == "//FRACTAL") fractal_files.push_back("BulbFractals\\" + string(file_data.cFileName));
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
		if (save_file_line.substr(0, 2) == "SF") fractal_file = save_file_line.substr(3);
		if (save_file_line.substr(0, 2) == "SV") shader_variables_new.push_back(ShaderVariable(save_file_line));
	}

	this->load();

	for (int i = 0; i < (int)shader_variables_new.size(); i++) {
		vector<ShaderVariable>::iterator found = std::find(shader_variables.begin(), shader_variables.end(), shader_variables_new[i]);
		if (found != shader_variables.end()) shader_variables[found - shader_variables.begin()] = shader_variables_new[i];
	}
}

void BulbShader::load() {
	ofstream frag_temp_file("BulbShaders\\bulb_temp.frag", ios::out);
	ifstream frag_orig_file("BulbShaders\\bulb.frag", ios::in);

	vector<ShaderVariable> shader_variables_old(shader_variables);
	vector<ShaderVariable> shader_variables_new;

	string current_line;
	while (getline(frag_orig_file, current_line)) {
		if (current_line.substr(0, 8) == "#include") {
			string include_file_name = split_string(current_line, """")[1];
			if (include_file_name == "FRACTAL") include_file_name = fractal_file;

			ifstream include_file(include_file_name, ios::in);

			string include_file_line;
			while (getline(include_file, include_file_line)) {
				frag_temp_file << include_file_line << endl;
				if (include_file_line.find("//~") != string::npos) shader_variables_new.push_back(ShaderVariable(include_file_line));
			}

			include_file.close();
		} else {
			frag_temp_file << current_line << endl;
			if (current_line.find("//~") != string::npos) shader_variables_new.push_back(ShaderVariable(current_line));
		}
	}

	frag_orig_file.close();
	frag_temp_file.close();

	shader_variables.clear();
	for (int i = 0; i < (int)shader_variables_new.size(); i++) {
		if (to_upper(shader_variables_new[i].category) == "FRACTAL") {
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

	load_shader("BulbShaders\\bulb.vert", "BulbShaders\\bulb_temp.frag", &program_fp32);
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
		if (shader_variables[i].needs_update()) shader_variables[i].update_program_variable(program_fp32);
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