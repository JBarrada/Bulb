#include "BMP.h"

BMP::BMP() {
}

BMP::BMP(int width, int height) {
	this->width = width;
	this->height = height;
	
	this->image_data = new uint8_t[width * height * 3];
	memset(this->image_data, 0x00, width * height * 3);
}

void BMP::save(string path) {
	int bmp_file_size =  53 + (this->width * this->height * 3);
	
	uint8_t bmp_file_header[14];
	memset(bmp_file_header, 0x00, 14);
	bmp_file_header[0] = 66;
	bmp_file_header[1] = 77;
	bmp_file_header[2] = (bmp_file_size >> 0) & 0xff;
	bmp_file_header[3] = (bmp_file_size >> 8) & 0xff;
	bmp_file_header[4] = (bmp_file_size >> 16) & 0xff;
	bmp_file_header[5] = (bmp_file_size >> 24) & 0xff;
	bmp_file_header[10] = 54;
	
	uint8_t bmp_image_header[40];
	memset(bmp_image_header, 0x00, 40);
	bmp_image_header[0] = 40;
	bmp_image_header[4] = (this->width >> 0) & 0xff;
	bmp_image_header[5] = (this->width >> 8) & 0xff;
	bmp_image_header[6] = (this->width >> 16) & 0xff;
	bmp_image_header[7] = (this->width >> 24) & 0xff;
	bmp_image_header[8] = (this->height >> 0) & 0xff;
	bmp_image_header[9] = (this->height >> 8) & 0xff;
	bmp_image_header[10] = (this->height >> 16) & 0xff;
	bmp_image_header[11] = (this->height >> 24) & 0xff;
	bmp_image_header[12] = 1;
	bmp_image_header[14] = 24;

	int bmp_padding = (4 - (this->width * 3) % 4) % 4;
	
	fstream output(path.c_str(), ios::out | ios::binary | ios::trunc);
	output.write((char*)bmp_file_header, 14);
	output.write((char*)bmp_image_header, 40);

	for (int i = 0; i < this->height; i++) {
		int line_offset = this->width * (this->height - i - 1) * 3;
		output.write((char*)&image_data[line_offset], this->width * 3);
		output.write(0x00, bmp_padding);
	}
	
	output.close();
}

uint8_t* BMP::get_pixel(int x, int y) {
	int offset = (y * this->width * 3) + (x * 3);
	return &this->image_data[offset];
}

void BMP::set_pixel(int x, int y, uint8_t* pixel) {
	int offset = (y * this->width * 3) + (x * 3);
	memcpy(&this->image_data[offset], pixel, 3);
}