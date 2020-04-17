// CubePersp.cpp
#include <glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <time.h>
#include <VecMat.h>
#include <stdlib.h>
#include "GLXtras.h" 
#include "Camera.h"

// GPU identifiers
GLuint vBuffer = 0;  
GLuint program = 0;   

int initialWinWidth =  500, initialWinHeight = 500;
Camera camera(initialWinWidth, initialWinHeight, vec3(0, 0, 0), vec3(0, 0, -1), (float)30);

void MouseButton(GLFWwindow* w, int butn, int action, int mods) {
    if (action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(w, &x, &y);
		camera.MouseDown(x, y);
    } else if (action == GLFW_RELEASE)
		camera.MouseUp();
}
void MouseMove(GLFWwindow* w, double x, double y) {
	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera.MouseMove(x, y);
}
void MouseWheel(GLFWwindow* w, double xoffset, double yoffset) {
	camera.MouseWheel(xoffset, yoffset);
}
void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

struct Vertex {
	vec3 point, color, normal;
	Vertex() {}
	Vertex(vec3 p, vec3 c, vec3 n) : point(p), color(c), normal(n) {}
};
Vertex vertices[24];

// Cube Data
float l = -1, r = 1, b = -1, t = 1, n = -1, f = 1;
float points[][3] = { {l,b,n},{l,b,f},{l,t,n},{l,t,f},{r,b,n},{r,b,f},{r,t,n},{r,t,f} };
float colors[][3] = { {0,0,1},{0,1,0},{0,1,1},{1,0,0},{1,0,1},{1,1,0},{0,0,0},{1,1,1} };
int faces[][4] = { {1,3,2,0},{6,7,5,4},{4,5,1,0},{3,7,6,2},{2,6,4,0},{5,7,3,1} };
float cubeSize = .05f, cubeStretch = cubeSize;

void InitVertexBuffer() {
	// create vertex array
	int nvrts = sizeof(faces) / sizeof(int), nfaces = nvrts / 4;
	//printf("In InitVertexBuffer() nvrts = %d nfaces = %d\n", nvrts, nfaces);
	int vsize = sizeof(Vertex);
	for (int i = 0; i < nfaces; i++) {
		int* f = faces[i];
		vec3 p1(points[f[0]]), p2(points[f[1]]), p3(points[f[2]]);
		vec3 n = normalize(cross(p3 - p2, p2 - p1));
		for (int k = 0; k < 4; k++) {
			int vid = f[k];
			int vertIndex = 4 * i + k;
			vertices[vertIndex] = Vertex(vec3(points[vid]), vec3(colors[vid]), n);
			//printf("VertIndex: %d X: %f Y: %f Z: %f\n", vertIndex, vertices[vertIndex].point.x, vertices[vertIndex].point.y, vertices[vertIndex].point.z);
		}
	}
	// create and bind GPU vertex buffer, copy vertex data
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, nvrts * vsize, &vertices[0], GL_STATIC_DRAW);
}

void Display(GLFWwindow *w) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	int screenWidth, screenHeight;
	glfwGetWindowSize(w, &screenWidth, &screenHeight);
	//int halfWidth = screenWidth / 2;
	camera.setWindowDim(screenWidth, screenHeight);
	
    // clear to gray, use app's shader
    glClearColor(.5, .5, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    
	//glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	VertexAttribPointer(program, "point", 3, sizeof(Vertex), (void*) 0);
	VertexAttribPointer(program, "color", 3, sizeof(Vertex), (void*) (sizeof(vec3)));
	VertexAttribPointer(program, "normal",3, sizeof(Vertex), (void*) (2*sizeof(vec3)));
	
	mat4 modelview = camera.modelview() * Scale(cubeSize, cubeSize, cubeStretch);
	mat4 persp = camera.persp() * Scale(cubeSize, cubeSize, cubeStretch);
	SetUniform(program, "modelview", modelview);
	SetUniform(program, "persp", persp);
	SetUniform(program, "lightVec", vec3(.7f, .4f, -.2f));
	glViewport(0, 0, screenWidth, screenHeight);
	glDrawArrays(GL_QUADS, 0, sizeof(vertices) / sizeof(Vertex));
	glFlush();
}

bool InitShader() { 
    program = LinkProgramViaFile("res/shaders/FacetedCubePersp-Vertex.shader", 
                                 "res/shaders/FacetedCubePersp-Pixel.shader");
    if (!program)
        printf("can't init shader program\n");
    return program != 0;
}

void ErrorGFLW(int id, const char *reason) {
    printf("GFLW error %i: %s\n", id, reason);
}

void Close() {
    // unbind vertex buffer and free GPU memory
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vBuffer);
}

int main() {
    glfwSetErrorCallback(ErrorGFLW);
    if (!glfwInit())
        return 1;
    GLFWwindow *window = glfwCreateWindow(600, 600, "Ex 8.4: Cube Persp", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MouseMove);
	glfwSetScrollCallback(window, MouseWheel);
    glfwSetKeyCallback(window, Keyboard);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    printf("GL version: %s\n", glGetString(GL_VERSION));
    PrintGLErrors();
    if (!InitShader())
        return 0;
    InitVertexBuffer();
    while (!glfwWindowShouldClose(window)) {
        Display(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Close();
    glfwDestroyWindow(window);
    glfwTerminate();
}
