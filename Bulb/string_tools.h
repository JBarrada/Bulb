#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <string>
#include <vector>

#include <glm\glm.hpp>

using namespace std;

string parse_to_next(string input, string target, int &pos);
vector<string> split_string(string input, string target);
void stovec(string values_string, glm::vec4 &value);

string to_upper(string s);

string random_name(int size);

#endif