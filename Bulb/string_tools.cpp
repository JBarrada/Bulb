#include "string_tools.h"

string parse_to_next(string input, string target, int &pos) {
	string output = "";

	while (pos < (int)input.size()) {
		for (int i = 0; i < (int)target.size(); i++) {
			if (input[pos] == target[i]) return output;
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

string to_upper(string s) {
	string upper_string = "";
	for (int i = 0; i < (int)s.size(); i++) upper_string += toupper(s[i]);
	return upper_string;
}

string random_name(int size) {
	string cons_sounds[] = {"b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "qu", "r", "s", "t", "v", "w", "x", "z"};
	string vowels[] = {"a", "e", "i", "o", "u", "y"};

	string name = "";

	for (int i = 0; i < size; i++) {
		if (i % 2 == 0) {
			name += cons_sounds[rand()%20];
		} else {
			name += vowels[rand()%6];
		}
	}

	return name;
}