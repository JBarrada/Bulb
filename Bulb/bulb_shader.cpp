#include "bulb_shader.h"

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
		save_file << shader_variables[i].get_bulb_save_string("SV") << endl;
	}
}

void BulbShader::read_from_save_file(ifstream &save_file) {
	save_file.clear();
	save_file.seekg(0, ios::beg);
	
	vector<BulbVariable> shader_variables_new;

	string save_file_line;
	while (getline(save_file, save_file_line)) {
		if (save_file_line.substr(0, 2) == "SF") fractal_file = save_file_line.substr(3);
		if (save_file_line.substr(0, 2) == "SV") shader_variables_new.push_back(BulbVariable(save_file_line));
	}

	this->load();

	for (int i = 0; i < (int)shader_variables_new.size(); i++) {
		vector<BulbVariable>::iterator found = std::find(shader_variables.begin(), shader_variables.end(), shader_variables_new[i]);
		if (found != shader_variables.end()) shader_variables[found - shader_variables.begin()] = shader_variables_new[i];
	}
}

void BulbShader::load() {
	ofstream frag_temp_file("BulbShaders\\bulb_temp.frag", ios::out);
	ifstream frag_orig_file("BulbShaders\\bulb.frag", ios::in);

	vector<BulbVariable> shader_variables_old(shader_variables);
	vector<BulbVariable> shader_variables_new;

	string current_line;
	while (getline(frag_orig_file, current_line)) {
		if (current_line.substr(0, 8) == "#include") {
			string include_file_name = split_string(current_line, "\"")[1];
			if (include_file_name == "FRACTAL") include_file_name = fractal_file;

			ifstream include_file(include_file_name, ios::in);

			string include_file_line;
			while (getline(include_file, include_file_line)) {
				frag_temp_file << include_file_line << endl;
				if (include_file_line.find("//~") != string::npos) shader_variables_new.push_back(BulbVariable(include_file_line));
			}

			include_file.close();
		} else {
			frag_temp_file << current_line << endl;
			if (current_line.find("//~") != string::npos) {
				shader_variables_new.push_back(BulbVariable(current_line));
			}
		}
	}

	frag_orig_file.close();
	frag_temp_file.close();

	shader_variables.clear();
	for (int i = 0; i < (int)shader_variables_new.size(); i++) {
		if (to_upper(shader_variables_new[i].category) == "FRACTAL") {
			shader_variables.push_back(shader_variables_new[i]);
		} else {
			vector<BulbVariable>::iterator found = std::find(shader_variables_old.begin(), shader_variables_old.end(), shader_variables_new[i]);
			
			if (found != shader_variables_old.end()) {
				shader_variables.push_back(shader_variables_old[found - shader_variables_old.begin()]);
			} else {
				shader_variables.push_back(shader_variables_new[i]);
			}
		}
	}

	std::sort(shader_variables.begin(), shader_variables.end(), [](const BulbVariable& lhs, const BulbVariable& rhs){ return lhs.category < rhs.category; });

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

void BulbShader::update_control_variables(BulbControlSettings *control_settings, float camera_aspect) {
	control_settings->camera_eye.update_program_variable(program_fp32);
	control_settings->camera_target.update_program_variable(program_fp32);
	control_settings->camera_up.update_program_variable(program_fp32);
	control_settings->camera_fov.update_program_variable(program_fp32);

	GLint prog_camera_aspect = glGetUniformLocation(program_fp32, "camera_aspect");
	glUniform1f(prog_camera_aspect, camera_aspect);
}