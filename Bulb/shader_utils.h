#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <glew.h>
#include <glut.h>

#define MAX_SHADER_LOG_SIZE 1024

int load_shader(char *vtxShdName, char *frgShdName, GLuint *lpProg);
int compile_shader(GLuint shader, char *file);
void print_shader_log(GLuint shader);
void print_program_log(GLuint program);

#endif
