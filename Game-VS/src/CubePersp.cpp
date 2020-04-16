// CubePersp.cpp
#include <glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <time.h>
#include <VecMat.h>
#include <stdlib.h>
#include "GLXtras.h" 
#include "Vertex.h"
#include "Camera.h"

// GPU identifiers
GLuint vBuffer = 0;  
GLuint program = 0;   

int initialWinWidth =  500, initialWinHeight = 500;
Camera camera(initialWinWidth / 2, initialWinHeight, vec3(0, 0, 0), vec3(0, 0, -1), (float)30);

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

// Cube
float l = -1, r = 1, b = -1, t = 1, n = -1, f = 1;
float points[][3] = { {l,b,n},{l,b,f},{l,t,n},{l,t,f},{r,b,n},{r,b,f},{r,t,n},{r,t,f} };
float colors[][3] = { {0,0,1},{0,1,0},{0,1,1},{1,0,0},{1,0,1},{1,1,0},{0,0,0},{1,1,1} };
int faces[][4] = { {1,3,2,0},{6,7,5,4},{4,5,1,0},{3,7,6,2},{2,6,4,0},{5,7,3,1} };
float cubeSize = .05f, cubeStretch = cubeSize;

void Display(GLFWwindow *w) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	int screenWidth, screenHeight;
	glfwGetWindowSize(w, &screenWidth, &screenHeight);
	int halfWidth = screenWidth / 2;
	camera.setWindowDim(halfWidth, screenHeight);
	mat4 m = camera.fullview() * Scale(cubeSize, cubeSize, cubeStretch);
	SetUniform(program, "view", m);
    // clear to gray, use app's shader
    glClearColor(.5, .5, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	VertexAttribPointer(program, "point", 3, 0, (void*)0);
	VertexAttribPointer(program, "color", 3, 0, (void*)0);
    
	glViewport(0, 0, halfWidth, screenHeight);
	glDrawElements(GL_QUADS, sizeof(faces) / sizeof(int), GL_UNSIGNED_INT, faces);
	glViewport(halfWidth, 0, halfWidth, screenHeight);
	glLineWidth(5);
	for (int i = 0; i < 6; i++)
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, &faces[i]);
	glFlush();
}

void InitVertexBuffer() {
    // make GPU buffer for points & colors, set it active buffer
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

bool InitShader() { 
    program = LinkProgramViaFile("res/shaders/CubePersp-Vertex.shader", 
                                 "res/shaders/CubePersp-Pixel.shader");
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
