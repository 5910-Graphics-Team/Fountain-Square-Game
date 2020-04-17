// TestGL.cpp: an application to determine GL and GLSL versions

#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int AppError(const char *msg) {
	glfwTerminate();
	printf("Error: %s\n", msg);
    getchar();
	return 1;
}

int main() {
    if (!glfwInit())
		return AppError("can't init GLFW\n");
    GLFWwindow *w = glfwCreateWindow(1, 1, "", NULL, NULL);
    if (!w)
		return AppError("can't open window\n");
//	glfwSetWindowPos(w, 0, 0);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	printf("GL vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL renderer: %s\n", glGetString(GL_RENDERER));
	printf("GL version: %s\n", glGetString(GL_VERSION));
	printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	getchar();
	return 0;
}
