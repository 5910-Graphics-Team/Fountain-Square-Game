// FontTest.cpp: test FreeType - see Text.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Draw.h"
#include "GLXtras.h"
#include "Text.h"

GLuint shaderProgram, vertexBuffer;
CharacterSet *font = NULL;
int winWidth = 720, winHeight = 720;
const char *text = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const char *vertexShader = "\
	#version 130									\n\
	in vec4 point;									\n\
	out vec2 uv;									\n\
	uniform mat4 view;								\n\
	void main() {									\n\
		gl_Position = view*vec4(point.xy, 0, 1);	\n\
		uv = point.zw;								\n\
	}												\n";

static const char *pixelShader = "\
	#version 130									\n\
	in vec2 uv;										\n\
	out vec4 pColor;								\n\
	uniform sampler2D textureImage;					\n\
	uniform vec3 color;								\n\
	void main() {									\n\
		float a = texture(textureImage, uv).r;		\n\
		pColor = vec4(color, a);					\n\
	}												\n";

vec2 Spiral(float t) {
	float alpha = t/50, mag = .3f*alpha*alpha;
	return mag*vec2(cos(alpha), sin(alpha));
}

void SpiralText(const char *text, float x, float y, vec3 color, float scale, mat4 view) {
	// create quad vertex buffer and build characters
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
	VertexAttribPointer(shaderProgram, "point", 4, 4*sizeof(float), 0);
	SetUniform(shaderProgram, "view", view);
	SetUniform(shaderProgram, "color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    for (const char *c = text; *c; c++) {
        Character ch = font->characters[(int)*c];
        float xpos = x+ch.bearing.i1*scale, w = ch.gSize.i1*scale;
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
		vec2 cen(winWidth/2, winHeight/2);
 		vec2 p1 = Spiral(xpos), p2 = Spiral(xpos+w), p3 = 1.2f*p2, p4 = 1.2f*p1;
        vec4 vertices[] = {vec4(cen+p1, 0, 0), vec4(cen+p2, 1, 0), vec4(cen+p3, 1, 1), vec4(cen+p4, 0, 1)};
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_QUADS, 0, 4);	// render glyph texture with quad
        x += (ch.advance >> 6)*scale;	// advance character position in terms of 1/64 pixel
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main(void) {
    if (!glfwInit())
        return 1;
    GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "Font Spiral", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glViewport(0, 0, winWidth, winHeight);
	shaderProgram = LinkProgramViaCode(&vertexShader, &pixelShader);
	glUseProgram(shaderProgram);
	glGenBuffers(1, &vertexBuffer);
	glClearColor(.8f, .8f, .8f, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    font = SetFont("C:/Fonts/SansSerif.ttf", 120, 60);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SpiralText(text, 0, 0, vec3(1,0,0), .5f, ScreenMode());
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    glfwDestroyWindow(window);
    glfwTerminate();
}
