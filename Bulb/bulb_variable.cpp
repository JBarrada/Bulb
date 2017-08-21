#include "bulb_variable.h"

int BULB_VAR_SIZE[] = {1, 1, 1, 2, 3, 4};

BulbVariable::BulbVariable() {
	name = "";
	category = "";
	type = "";
	var_type = VAR_VEC4;

	is_color = false;
	hsv_mode = false;
	no_clamp = false;
	update = true;

	for (int i = 0; i < 4; i++) {
		value[i] = glm::vec4(0);
		animate_enable[i] = false;
		animate_values[i] = glm::vec4(0,1,0,0);
	}
}

BulbVariable::BulbVariable(string code) {
	this->BulbVariable::BulbVariable();

	if (code.find("~") != string::npos) load_from_shader_comment(code); else load_from_bulb_save_string(code);
}

void BulbVariable::load_from_shader_comment(string code) {
	if (code.substr(0, 7) == "uniform") {
		vector<string> split_space = split_string(code, " ");
		vector<string> split_comment = split_string(code.substr(code.find("~") + 1), "|");

		string type_strings[] = {"bool", "int", "float", "vec2", "vec3", "vec4"};
		var_type = (BULB_VAR_TYPE)std::distance(type_strings, std::find(type_strings, type_strings + 6, split_space[1]));

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

void BulbVariable::load_from_bulb_save_string(string code) {
	vector<string> code_split = split_string(code, "|");

	if ((int)code_split.size() >= 16) {
		if (code_split[0].size() == 2) {
			name = code_split[1];
			category = code_split[2];
			type = to_upper(code_split[3]);
			var_type = (BULB_VAR_TYPE)stoi(code_split[4]);
			
			is_color = (stoi(code_split[5]) == 1);
			hsv_mode = (stoi(code_split[6]) == 1);
			update = true;

			for (int i = 0; i < 4; i++) {
				stovec(code_split[8+i], value[i]);

				animate_enable[i] = (code_split[7][i] == '1');
				stovec(code_split[12+i], animate_values[i]);
			}
			if ((int)code_split[8].size() == 0 || (int)code_split[9].size() == 0) no_clamp = true;
		}
	}
}

string BulbVariable::get_bulb_save_string(string prefix) {
	char save_chars[1000];
	int j = 0;

	j += sprintf_s(save_chars+j, 1000-j, "%s|%s|%s|%s|%d|", prefix.c_str(), name.c_str(), category.c_str(), type.c_str(), var_type);
	j += sprintf_s(save_chars+j, 1000-j, "%d|%d|", is_color, hsv_mode);
	j += sprintf_s(save_chars+j, 1000-j, "%d%d%d%d|", animate_enable[0], animate_enable[1], animate_enable[2], animate_enable[3]);

	for (int i=0; i<4; i++) {		
		if (no_clamp && i > 0) {
			j += sprintf_s(save_chars+j, 1000-j, "|");
		} else {
			j += sprintf_s(save_chars+j, 1000-j, "%f,%f,%f,%f|", value[i][0], value[i][1], value[i][2], value[i][3]);
		}
	}
	for (int i=0; i<4; i++) {
		j += sprintf_s(save_chars+j, 1000-j, "%f,%f,%f,%f|", animate_values[i][0], animate_values[i][1], animate_values[i][2], animate_values[i][3]);
	}

	return string(save_chars);
}

bool BulbVariable::needs_update() {
	return (update + animate_enable[0] + animate_enable[1] + animate_enable[2] + animate_enable[3]) != 0;
}

void BulbVariable::update_program_variable(GLuint program) {
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
		glm::vec4 color_rgb = glm::vec4(glm::rgbColor(glm::vec3(value[0].x*360.0f, value[0].y, value[0].z)), value[0].w);
		if (var_type == VAR_VEC3) glUniform3fv(var_pointer, 1, (float*)&color_rgb);
		if (var_type == VAR_VEC4) glUniform4fv(var_pointer, 1, (float*)&color_rgb);
	} else {
		if (var_type == VAR_VEC3) glUniform3fv(var_pointer, 1, (float*)&value[0]);
		if (var_type == VAR_VEC4) glUniform4fv(var_pointer, 1, (float*)&value[0]);
	}

	update = false;
}

void BulbVariable::adjust_variable(float analog, int digital, int &sub_variable) {
	sub_variable = glm::clamp(sub_variable, 0, BULB_VAR_SIZE[var_type] - 1);
	
	if (!animate_enable[sub_variable]) {
		if (var_type == VAR_BOOL) {
			if (abs(digital) == 1) {
				value[0][sub_variable] = ((int)value[0][sub_variable] == 1.0f) ? 0.0f : 1.0f;
			}
		} else {
			float resolution = 100.0f;
			float step = value[3][sub_variable] / resolution;
			float step_digital = (var_type == VAR_INT) ? 1.0f : max(pow(10, floor(log10(step))), step - (pow(10, floor(log10(step)))*5));
			value[0][sub_variable] = glm::clamp(value[0][sub_variable] + (analog * step) + (digital * step_digital), value[1][sub_variable], value[2][sub_variable]);
		}
		update = true;
	}
}

void BulbVariable::adjust_animate(float normalized_amount, int &sub_variable, int &animate_variable) {
	sub_variable = glm::clamp(sub_variable, 0, BULB_VAR_SIZE[var_type] - 1);
	animate_variable = glm::clamp(animate_variable, 0, 2);

	glm::vec3 animate_ranges[] = {glm::vec3(0, 1, 1), glm::vec3(0, 1, 1), glm::vec3(-1, 1, 2)};
	float resolution = 100.0f;
	float step = animate_ranges[animate_variable][2] / resolution;
	animate_values[sub_variable][animate_variable] = glm::clamp(animate_values[sub_variable][animate_variable] + (normalized_amount * step), animate_ranges[animate_variable][0], animate_ranges[animate_variable][1]);
}

void BulbVariable::set_hsv_mode(bool enable) {
	if (is_color && hsv_mode != enable) {
		hsv_mode = enable;
		value[0] = (enable) ? glm::vec4(glm::hsvColor(glm::vec3(value[0])) / glm::vec3(360.0f, 1, 1), value[0].w) : glm::vec4(glm::rgbColor(glm::vec3(value[0].x*360.0f, value[0].y, value[0].z)), value[0].w);
	}
}

glm::vec4 BulbVariable::get_color() {
	return (hsv_mode) ? glm::vec4(glm::rgbColor(glm::vec3(value[0].x*360.0f, value[0].y, value[0].z)), value[0].w) : value[0];
}

string BulbVariable::get_string() {
	char text[50];

	if (var_type == VAR_BOOL) sprintf_s(text, ((value[0][0]) ? "true" : "false"));
	if (var_type == VAR_INT) sprintf_s(text, "%0.0f", value[0][0]);
	if (var_type == VAR_FLOAT) sprintf_s(text, "%f", value[0][0]);
	if (var_type == VAR_VEC2) sprintf_s(text, "%0.2f, %0.2f", value[0].x, value[0].y);
	if (var_type == VAR_VEC3) sprintf_s(text, "%0.2f, %0.2f, %0.2f", value[0].x, value[0].y, value[0].z);
	if (var_type == VAR_VEC4) sprintf_s(text, "%0.2f, %0.2f, %0.2f, %0.2f", value[0].x, value[0].y, value[0].z, value[0].w);

	return string(text);
}

string BulbVariable::get_string(int &sub_variable) {
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

string BulbVariable::get_string_animate(int &sub_variable, int &animate_variable) {
	char text[50];
	string animate_prefix[3] = {"Animate Speed: ", "Animate Scale: ", "Animate Offset: "};
	sprintf_s(text, "%s%f", animate_prefix[animate_variable].c_str(), animate_values[sub_variable][animate_variable]);
	return string(text);
}

bool BulbVariable::is_bright() {
	glm::vec4 variable_color = get_color();
	return (is_color && ((variable_color.r + variable_color.g + variable_color.b) > 2.0f));
}

bool BulbVariable::operator==(const BulbVariable& rhs) {
	return ((rhs.category == category) && (rhs.name == name));
}
