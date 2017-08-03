#include "bulb_shader.h"

string parse_to_next(string input, string target, int &pos) {
	string output = "";

	while (pos < (int)input.size()) {
		for (int i = 0; i < (int)target.size(); i++) {
			if (input[pos] == target[i]) {
				return output;
			}
		}
		output += input[pos];
		pos++;
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
		}
		if (var_type == VAR_FLOAT) {
			var_float[0] = stof(var_default);
			var_float[1] = stof(var_min);
			var_float[2] = stof(var_max);
		}
		if (var_type == VAR_VEC2) {
			var_vec2[0] = stovec2(var_default);
			var_vec2[1] = stovec2(var_min);
			var_vec2[2] = stovec2(var_max);
		}
		if (var_type == VAR_VEC3) {
			var_vec3[0] = stovec3(var_default);
			var_vec3[1] = stovec3(var_min);
			var_vec3[2] = stovec3(var_max);
		}
		if (var_type == VAR_VEC4) {
			var_vec4[0] = stovec4(var_default);
			var_vec4[1] = stovec4(var_min);
			var_vec4[2] = stovec4(var_max);
		}
	}
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
}