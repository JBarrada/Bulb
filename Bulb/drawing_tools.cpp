#include "drawing_tools.h"

DrawingTools::DrawingTools() {
	SCREEN_W = 1;
	SCREEN_H = 1;
}
DrawingTools::DrawingTools(float width, float height) {
	SCREEN_W = width;
	SCREEN_H = height;
}

void DrawingTools::reshape(float width, float height) {
	SCREEN_W = width;
	SCREEN_H = height;
}

float DrawingTools::screen_x(float x) {
	return (x / SCREEN_W)*2.0f - 1.0f;
}
float DrawingTools::screen_x(int x) {
	return screen_x((float)x);
}

float DrawingTools::screen_y(float y) {
	return (y / SCREEN_H)*2.0f - 1.0f;
}
float DrawingTools::screen_y(int y) {
	return screen_y((float)y);
}

/*
void DrawingTools::circle_filled(double x, double y, double r) {
	glBegin(GL_POLYGON);
	double steps = 10;
	double step = (M_PI*2.0) / steps;
	for (int i = 0; i < steps; i++) {
		double sx = screen_x((cos(step*i)*r) + x);
		double sy = screen_y((sin(step*i)*r) + y);
		glVertex2f(sx, sy);
	}
	glEnd();
}
void DrawingTools::circle_hollow(double x, double y, double r) {
	glBegin(GL_LINE_LOOP);
	double steps = 10;
	double step = (M_PI*2.0) / steps;
	for (int i = 0; i < steps; i++) {
		double sx = screen_x((cos(step*i)*r) + x);
		double sy = screen_y((sin(step*i)*r) + y);
		glVertex2f(sx, sy);
	}
	glEnd();
}
*/

void DrawingTools::rectangle_filled(int x, int y, int w, int h) {
	glBegin(GL_QUADS);
	glVertex2f(screen_x(x), screen_y(y));
	glVertex2f(screen_x(x+w), screen_y(y));
	glVertex2f(screen_x(x+w), screen_y(y+h));
	glVertex2f(screen_x(x), screen_y(y+h));
	glEnd();
}

void DrawingTools::rectangle_tex(int x, int y, int w, int h) {
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2f(screen_x(x), screen_y(y));
	glTexCoord2i(1, 0); glVertex2f(screen_x(x+w), screen_y(y));
	glTexCoord2i(1, 1); glVertex2f(screen_x(x+w), screen_y(y+h));
	glTexCoord2i(0, 1); glVertex2f(screen_x(x), screen_y(y+h));
	glEnd();
}

/*
template<typename T>
void DrawingTools::rectangle_filled(T x, T y, T w, T h) {
	rectangle_filled((float)x, (float)y, (float)w, (float)h);
}
*/

void DrawingTools::rectangle_hollow(float x, float y, float w, float h) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(screen_x(x), screen_y(y));
	glVertex2f(screen_x(x + w), screen_y(y));
	glVertex2f(screen_x(x + w), screen_y(y + h));
	glVertex2f(screen_x(x), screen_y(y + h));
	glEnd();
}

void DrawingTools::text(int x, int y, void *font, string text) {
	float offset = 0;

	for (int i = 0; i < (int)text.size(); i++) {
		glRasterPos2f(screen_x(x + offset), screen_y(y));
		glutBitmapCharacter(font, text[i]);
		offset += glutBitmapWidth(font, text[i]);
	}
}