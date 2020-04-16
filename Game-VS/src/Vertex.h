#ifndef VERTEX
#define VERTEX
#include <VecMat.h>
struct Vertex {
	vec2 point;
	vec3 color;
	Vertex(float x, float y, float r, float g, float b) :
		point(x, y), color(r, g, b) {}
};
#endif