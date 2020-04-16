// Ross Hoyt and Chris Downing - Assignment 5 (Exercise 10.2) - Phong face

#include <glad.h>
#include <glfw/glfw3.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include "GLXtras.h"
#include "Camera.h"
#include "Draw.h"
#include <Misc.h>
#include <Numbers.h>
#include "VecMat.h"
#include <algorithm>

// vertices for left half of face
vec3 leftPoints[] = {
	
	vec3(722,244,434), vec3(912,269,488),  vec3(1052,318,670),											// 0-2
	vec3(722,447,284), vec3(978,450,366),  vec3(1093,496,490),  vec3(1143,586,561), vec3(1173,732,687), // 3-7
	vec3(722,660,216), vec3(1018,593,308), vec3(1147,787,516),											// 8-10
	vec3(722,1103,50), vec3(793,951,196),  vec3(833,900,307),   vec3(1065,923,409),						// 11-14
	vec3(722,1275,176),vec3(898,1134,286), vec3(1100,1095,417), vec3(1128,1219,543),					// 15-18
	vec3(904,1298,241),vec3(1024,1356,333),																// 19-20
	vec3(722,1480,162),vec3(853,1578,201), vec3(957,1594,440),  vec3(1120,1255,818),					// 21-24
	vec3(722,1741,154),vec3(722,1802,554), vec3(977,1650,782),											// 25-27
	vec3(1185,914,653)																					// 28


};

// triangles for left half of face
int leftTriangles[][3] = {
	{0,1,3}, {1,4,3}, {1,2,4}, {2,5,4}, {2,6,5}, 
	{3,4,8}, {4,9,8}, {4,5,10}, {4,9,10}, {5,6,10}, {6,7,10}, {7,28,10}, {7,24,28},
	/*{8,9,13},*/{8,13,12}, {8,12,11}, /*{9,10,14}, {9,14,13},*/ {10,28,18}, {10,18,17}, {10,17,14},
	{11,12,16}, {11,16,15}, {12,13,16}, {13,14,16}, {14,17,16}, 
	{15,16,19}, {15,19,21}, {16,17,18}, {16,18,20}, {16,20,19}, {17,10,18}, {18,28,24}, {18,24,23}, {18,23,20}, 
	{19,20,21}, {20,23,22}, {20,22,21}, 
	{21,22,25}, {22,23,25}, {23,24,27}, {23,27,26}, {23,26,25}
	
};

// entire face
const int nLeftPoints = sizeof(leftPoints)/sizeof(leftPoints[0]);
const int nLeftTriangles = sizeof(leftTriangles)/sizeof(leftTriangles[0]);
const int npoints = 2*nLeftPoints, ntriangles = 2*nLeftTriangles;
vec3 normals[npoints], points[npoints];
int triangles[ntriangles][3];
int sizePts = sizeof(points);
int midX = leftPoints[0].x; // midpoint of face

// shaders
const char *vShader = "\
	#version 130												\n\
	in vec3 point;												\n\
	in vec3 normal;												\n\
	uniform mat4 modelview;										\n\
	uniform mat4 persp;											\n\
	out vec3 vPoint;											\n\
	out vec3 vNormal;											\n\
	void main() {												\n\
		vPoint = (modelview*vec4(point, 1)).xyz;				\n\
		vNormal = (modelview*vec4(normal, 0)).xyz;				\n\
		gl_Position = persp*vec4(vPoint, 1);					\n\
	}";

const char *pShader = "\
	#version 130												\n\
	in vec3 vPoint;												\n\
	in vec3 vNormal;											\n\
	uniform float a = 0.1;										\n\
	uniform vec3 lightPos = vec3(-1, 0, -2);					\n\
	uniform vec3 color = vec3(1, 1, 1);							\n\
	out vec4 pColor;											\n\
	void main() {												\n\
		vec3 N = normalize(vNormal);							\n\
		vec3 L = normalize(lightPos-vPoint);					\n\
		vec3 R = reflect(L, N);									\n\
		vec3 E = normalize(vPoint);								\n\
		float d = abs(dot(L, N));								\n\
		float h = max(0, dot(R, E));							\n\
		float s = pow(h, 100);									\n\
		float intensity = clamp(a+d+s, 0, 1);					\n\
		pColor = vec4(intensity*color, 1);						\n\
	}";

// OpenGL identifiers
GLuint vBuffer = 0, program = 0;

// window size and camera
int winWidth = 800, winHeight = 1000;
Camera camera(winWidth, winHeight, vec3(0, 0, 0), vec3(0, 0, -5), 30, 0.001f, 500, false);

// display
bool annotate = false;

void Display(GLFWwindow* w) {
	// clear to gray, use app's shader
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// use program, bind vertex buffer, set vertex feed, set uniforms
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	VertexAttribPointer(program, "point", 3, 0, (void *) 0);
	VertexAttribPointer(program, "normal", 3, 0, (void *) sizePts);
	SetUniform(program, "modelview", camera.modelview);
	SetUniform(program, "persp", camera.persp);
	// set color, draw shape
	SetUniform(program, "color", vec3(1, 1, 1));
	glDrawElements(GL_TRIANGLES, 3*ntriangles, GL_UNSIGNED_INT, triangles);
	// optional vertex and triangle annotation
	if (annotate) {
		glDisable(GL_DEPTH_TEST);
		SetUniform(program, "color", vec3(0, 0, 1));
		for (int i = 0; i < ntriangles; i++)
			glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, &triangles[i]);
		for (int i = 0; i < npoints; i++)
			Number(points[i], camera.fullview, i, vec3(0,0,0), 10);
	}
	glFlush();
}

// scale points to lie within +/-1
void Normalize() {
	vec3 mn(FLT_MAX), mx(-FLT_MAX);
	for (int i = 0; i < npoints; i++) {
		vec3 p = points[i];
		for (int k = 0; k < 3; k++) {
			if (p[k] < mn[k]) mn[k] = p[k];
			if (p[k] > mx[k]) mx[k] = p[k];
		}
	}
	vec3 center = .5f*(mn+mx), range = mx-mn;
	float maxrange = std::max(range.x, std::max(range.y, range.z)), s = 2/maxrange;
	for (int i = 0; i < npoints; i++)
		points[i] = s*(points[i]-center);
}

bool Mid(float x) { return fabs(x-midX) < .0001f; }
	// is x close to the middle of the face?

void Reflect() {
	// copy left face vertices and triangles to full face
	for (int i = 0; i < nLeftPoints; ++i)
		points[i] = leftPoints[i];
	for (int i = 0; i < nLeftTriangles; ++i) {
		int *tLeft = leftTriangles[i], *t = triangles[i];
		for (int k = 0; k < 3; k++)
			t[k] = tLeft[k];
	}
	// fill second half of points, reflecting points around midX
	for (int i = 0; i < nLeftPoints; ++i)
		points[i+nLeftPoints] = vec3(2*midX-leftPoints[i].x, leftPoints[i].y, leftPoints[i].z);
	// fill second half of triangles
	for (int i = 0; i < nLeftTriangles; ++i) {
		int *t = leftTriangles[i];
		// test each triangle vertex for proximity to midX; if close, use original vertex, else use reflected vertex
		for (int k = 0; k < 3; k++)
			triangles[i+nLeftTriangles][2-k] = Mid(points[t[k]].x)? t[k] : t[k]+nLeftPoints;
			// 2-k reverses order of reflected triangles to ensure still ccw
	}
}

void ComputeNormals() {
	// initialize to zero
	for (int i = 0; i < npoints; ++i)
		normals[i] = vec3(0, 0, 0);
	// for each triangle: compute surface normal and add to each corresponding vertex normal
	for (int i = 0; i < ntriangles; ++i) {
		int* t = triangles[i];
		vec3 p1(points[t[0]]), p2(points[t[1]]), p3(points[t[2]]);
		vec3 n = normalize(cross(p3-p2, p2-p1));
		for (int k = 0; k < 3; k++)
			normals[t[k]] += n;
	}
	// set normals to unit length
	for (int i = 0; i < npoints; ++i)
		normals[i] = normalize(normals[i]);
}

void Resize(GLFWwindow* w, int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

void MouseButton(GLFWwindow* w, int butn, int action, int mods) {
	if (action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(w, &x, &y);
		camera.MouseDown((int) x, (int) y);
	}
	if (action == GLFW_RELEASE)
		camera.MouseUp();
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	camera.MouseWheel((int) yoffset, Shift(w));
}

void MouseMove(GLFWwindow* w, double x, double y) {
	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera.MouseDrag((int) x, (int) y, Shift(w));
}

void Key(GLFWwindow* w, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (key) {
			case 'A':
				annotate = !annotate;
				break;
		}
}

void InitVertexBuffer() {
	// create GPU buffer, make it active
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	// allocate memory for points and normals
	glBufferData(GL_ARRAY_BUFFER, 2*sizePts, NULL, GL_STATIC_DRAW);
	// copy
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePts, &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizePts, sizePts, &normals[0]);
}

int main() {
	if (!glfwInit())
		return 1;
	GLFWwindow *w = glfwCreateWindow(winWidth, winHeight, "Face", NULL, NULL);
	if (!w) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(w);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	PrintGLErrors();
	program = LinkProgramViaCode(&vShader, &pShader);
	Reflect();				// reflect leftPoints to create entire face
	Normalize();			// set points within +/- 1.
	ComputeNormals();		// estimate vertex normals from surrounding triangles
	InitVertexBuffer();		// store in GPU
	glfwSetKeyCallback(w, Key);
	glfwSetScrollCallback(w, MouseWheel);
	glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetCursorPosCallback(w, MouseMove);
	glfwSetWindowSizeCallback(w, Resize);
    printf("Usage:\n\tA: toggle annotation\n");
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Display(w);
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBuffer);
	glfwDestroyWindow(w);
	glfwTerminate();
}
