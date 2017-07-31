#include <iostream>
#include <fstream>

#include "shader_utils.h"

int load_shader(char *vtxShdName, char *frgShdName, GLuint *lpProg)
{
	GLuint vtxShader;
	GLuint frgShader;
	GLuint prog;
	GLint linked;
    
	vtxShader = glCreateShader(GL_VERTEX_SHADER);
	frgShader = glCreateShader(GL_FRAGMENT_SHADER);

	if (compile_shader(vtxShader, vtxShdName) < 0)
	{
		return -1;
	}

	if (compile_shader(frgShader, frgShdName) < 0)
	{
		return -1;
	}

	prog = glCreateProgram();
    
	glAttachShader(prog, vtxShader);
	glAttachShader(prog, frgShader);
    
	glDeleteShader(vtxShader);
	glDeleteShader(frgShader);
    
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	print_program_log(prog);
	if (linked == GL_FALSE)
	{
        std::cout << "Link error of " 
        << vtxShdName <<  " & " << frgShdName << " !!" << std::endl;
		return -1;
	} else {
        std::cout << "Link successful: " << vtxShdName <<  " & " << frgShdName << std::endl;
    }
    
	*lpProg = prog;
    
	return 0;
}

int compile_shader(GLuint shader, char *name)
{
	GLint compiled;
    
    std::ifstream inputFile2(name);
	if (!inputFile2.is_open())
	{
        std::cout << name << "does not exist!" << std::endl;
		return -1;
	} else {
        std::cout << "Successfully loaded shader file " << name << std::endl;
    }
    
    std::istreambuf_iterator<char> fdataBegin(inputFile2);
    std::istreambuf_iterator<char> fdataEnd;
    
    std::string fileData2(fdataBegin,fdataEnd);
    const char * file = fileData2.c_str();
    
	glShaderSource(shader, 1, &file, NULL);
    
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	print_shader_log(shader);
	if (compiled == GL_FALSE)
	{
        std::cout << "Compile error in " << name << std::endl;
		return -1;
	} else {
        std::cout << "Successfully compiled shader " << name << std::endl;
    }
    
	return 0;
}

void print_shader_log(GLuint shader)
{
	int logSize;
	int length;
    
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &logSize);
    
	if (logSize > 1)
	{
        char s_logBuffer[MAX_SHADER_LOG_SIZE];
		glGetShaderInfoLog(shader, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
        std::cout << "Shader Info Log " << s_logBuffer << std::endl;
	}
}

void print_program_log(GLuint program)
{
	int logSize;
	int length;
    
	glGetProgramiv(program, GL_INFO_LOG_LENGTH , &logSize);
    
	if (logSize > 1)
	{
        char s_logBuffer[MAX_SHADER_LOG_SIZE];
		glGetProgramInfoLog(program, MAX_SHADER_LOG_SIZE, &length, s_logBuffer);
        std::cout << "Program Info Log " << s_logBuffer << std::endl;
	}
}