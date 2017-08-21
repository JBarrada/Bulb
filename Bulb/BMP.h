#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <string>
#include <cstring>

#include <iostream>
#include <fstream>

using namespace std;

class BMP {
public:
	int width, height;
	uint8_t* image_data;
	
	BMP();
	BMP(int, int);
	
	void save(string);
	
	uint8_t* get_pixel(int, int);
	
	void set_pixel(int, int, uint8_t*);
};

#endif