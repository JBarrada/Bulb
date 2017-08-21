#ifndef DRAWING_TOOLS_H
#define DRAWING_TOOLS_H

#include <cmath>
#include <glew.h>
#include <glut.h>

#include <string>

using namespace std;

class DrawingTools {
public:
	float SCREEN_W;
	float SCREEN_H;

	DrawingTools();
	DrawingTools(float width, float height);

	void reshape(float width, float height);

	float screen_x(float x);
	float screen_x(int x);
	float screen_y(float y);
	float screen_y(int y);

	//void circle_filled(double x, double y, double r);
	//void circle_hollow(double x, double y, double r);

	//void rectangle_filled(float x, float y, float w, float h);
	void rectangle_filled(int x, int y, int w, int h);
	void rectangle_tex(int x, int y, int w, int h);
	void rectangle_hollow(float x, float y, float w, float h);

	void text(int x, int y, void *font, string text);

};

#endif