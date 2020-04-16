// ColorfulLetter.cpp: draw colorful letter with triangles

#include <glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include "GLXtras.h" 

// GPU identifiers
GLuint vBuffer = 0;
GLuint program = 0;

struct Vertex {
	vec2 point;
	vec3 color;
	Vertex(float x, float y, float r, float g, float b) : 
		point(x, y), color(r, g, b) {}
};

// vertices
Vertex vertices[] = {
	Vertex(-.5f,-.8f,  0, 0, 1), Vertex(-.8f,-.8f,  1, 0, 0), // V0, V1
	Vertex(-.8f, .8f,  0, 0, 1), Vertex(-.5f, .8f,  0, 1, 0),
	Vertex(-.5f, .13f, 0, 0, 1), Vertex( .5f, .13f, 0, 1, 0),
	Vertex(-.5f,-.13f, 0, 0, 1), Vertex( .5f,-.13f, 1, 0, 0), // V6, V7
	Vertex( .8f,-.8f,  0, 0, 1), Vertex( .5f,-.8f,  1, 0, 0),
	Vertex( .5f, .8f,  0, 0, 1), Vertex( .8f, .8f,  0, 1, 0),
	Vertex(-.8f,-.13f, 1, 0, 0), Vertex(-.8f, .13f, 0, 0, 1), // V12, 13 (added vertices)
	Vertex( .8f,-.13f, 1, 0, 0), Vertex( .8f, .13f, 0, 0, 1)  // (added vertices)
};


// triangles
int triangles[][3] = {
	{0,1,12},{2, 3,4},{4, 5, 6},{5,6, 7},{8, 9,7},{10,11,15},
	{0,12,6},{2,13,4},{6,12,13},{6,4,13},{8,14,7},{10,15, 5},// (added triangles)
	{7,14,15},{7,5,15}	// (added)
};

// shaders
const char *vertexShader = "\
	#version 130								\n\
	in vec2 point;								\n\
	in vec3 color;								\n\
	out vec4 vColor;							\n\
	void main() {								\n\
		gl_Position = vec4(point, 0, 1);		\n\
	    vColor = vec4(color, 1);				\n\
	}";

const char *pixelShader = "\
	#version 130								\n\
	in vec4 vColor;								\n\
	out vec4 pColor;							\n\
	void main() {								\n\
        pColor = vColor;						\n\
	}";

void Display() {
	// clear background
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);
	// access GPU vertex buffer
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	VertexAttribPointer(program, "point", 2, sizeof(Vertex), (void*) 0);
	VertexAttribPointer(program, "color", 3, sizeof(Vertex), (void*) sizeof(vec2));
	glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, triangles);
	glFlush();
}

void InitVertexBuffer() {
    // make GPU buffer for points & colors, set it active buffer
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

bool InitShader() {
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	if (!program)
		printf("can't init shader program\n");
	return program != 0;
}

// application

void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    GLFWwindow *w = glfwCreateWindow(600, 600, "Colorful Letter", NULL, NULL);
    if (!w) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    printf("GL version: %s\n", glGetString(GL_VERSION));
    PrintGLErrors();
	if (!InitShader())
        return 0;
    InitVertexBuffer();
    glfwSetKeyCallback(w, Keyboard);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
 	Close();
	glfwDestroyWindow(w);
    glfwTerminate();
}
