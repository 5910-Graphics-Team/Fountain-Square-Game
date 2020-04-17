// BumpQuad.cpp: bump-mapped quadrilateral

#include <glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Misc.h"
#include "Text.h"
#include "Widgets.h"

const char *textureFilename = "C:/Users/jules/SeattleUniversity/Exe/Parrots.tga";
const char *normalFilename = "C:/Users/Jules/Codeblocks/Book/RipplesNormal.tga"; // CushionNormal.tga"; // ;

// GPU identifiers, app window, camera
GLuint vBuffer = 0, program = 0, textureId = 0, normalId = 0;
int textureUnit = 0, normalUnit = 1;
Camera camera(800, 800, vec3(0,0,0), vec3(0,0,-5));
time_t tEvent = clock();

// a quad
float s = .8f;
float pnts[][3] = {{-s,-s,0}, {-s,s, 0}, {s, s, 0}, {s,-s, 0}};
float nrms[][3] = {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};
float uaxs[][3] = {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}};
float vaxs[][3] = {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}};
float uvs[][2]  = {{0, 0},    {0, 1},    {1, 1},    {1, 0}};

// movable light
vec3 light(-.5f, .2f, .3f);
Mover lightMover;
void *picked = NULL, *hover = NULL;
int diffuseOnly = 1, localLights = 1, useTexture = 1;

const char *vertexShader = "\
	#version 130													\n\
	in vec3 point;													\n\
	in vec3 normal;													\n\
	in vec3 uaxis;													\n\
	in vec3 vaxis;													\n\
	in vec2 uv;														\n\
	out vec3 vPoint;												\n\
	out vec3 vNormal;												\n\
	out vec3 vUaxis;												\n\
	out vec3 vVaxis;												\n\
	out vec2 vUv;													\n\
	uniform mat4 modelview;											\n\
	uniform mat4 persp;												\n\
	void main() {													\n\
		vPoint = (modelview*vec4(point, 1)).xyz;					\n\
		vNormal = (modelview*vec4(normal, 0)).xyz;					\n\
		vUaxis = (modelview*vec4(uaxis, 0)).xyz;					\n\
		vVaxis = (modelview*vec4(vaxis, 0)).xyz;					\n\
	    vUv = uv;													\n\
		gl_Position = persp*vec4(vPoint, 1);						\n\
	}";

const char *pixelShader = "\
    #version 400													\n\
	in vec3 vPoint;													\n\
	in vec3 vNormal;												\n\
	in vec3 vUaxis;													\n\
	in vec3 vVaxis;													\n\
	in vec2 vUv;													\n\
	out vec4 pColor;												\n\
	uniform sampler2D textureMap;									\n\
	uniform sampler2D normalMap;									\n\
	uniform vec3 light;												\n\
	uniform int diffuseOnly = 1;									\n\
	uniform int localLights = 1;									\n\
	uniform int useTexture = 1;										\n\
	float PhongIntensity(vec3 pos, vec3 nrm) {						\n\
        float a = .15f, d = 0, s = 0;								\n\
		vec3 N = normalize(nrm);			// surface normal		\n\
        vec3 E = normalize(pos);			// eye vertex			\n\
		vec3 L = normalize(localLights == 1? light-pos : light); 	\n\
		d = abs(dot(N, L));					// two-sided diffuse	\n\
		if (diffuseOnly == 0) {										\n\
			vec3 R = reflect(L, N);			// highlight vector		\n\
			float h = abs(dot(R, E));		// highlight term		\n\
			s += pow(h, 50);				// specular term		\n\
		}															\n\
		return clamp(a+d+s, 0, 1);									\n\
	}																\n\
 	vec3 TransformToLocal(vec3 v, vec3 x, vec3 y, vec3 z) {			\n\
 		float xx = v.x*x.x + v.y*y.x + v.z*z.x;						\n\
 		float yy = v.x*x.y + v.y*y.y + v.z*z.y;						\n\
 		float zz = v.x*x.z + v.y*y.z + v.z*z.z;						\n\
 		return normalize(vec3(xx, yy, zz));							\n\
	}																\n\
	vec3 BumpNormal() {												\n\
    	vec4 bumpV = texture(normalMap, vUv);						\n\
		// map red, grn to [-1,1], blu to [0,1]						\n\
 		vec3 b = vec3(2*bumpV.r-1, 2*bumpV.g-1, bumpV.b);			\n\
 		return TransformToLocal(b, vUaxis, vUaxis, vNormal);		\n\
 	}																\n\
    void main() {													\n\
		vec3 b = BumpNormal();										\n\
		float intensity = PhongIntensity(vPoint, b);				\n\
		vec3 color = vec3(1,1,1);									\n\
		if (useTexture == 1) color = texture(textureMap, vUv).rgb;	\n\
		pColor = vec4(intensity*color, 1);							\n\
	}";

void Display() {
    glClearColor(.5, .5, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// access GPU vertex buffer
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	// maps
	glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
	SetUniform(program, "textureMap", textureUnit);
	glActiveTexture(GL_TEXTURE0+normalUnit);
	glBindTexture(GL_TEXTURE_2D, normalId);
	SetUniform(program, "normalMap", normalUnit);
    // associate position input to shader with position array in vertex buffer
	VertexAttribPointer(program, "point", 3, 0, (void *) 0);
	VertexAttribPointer(program, "normal", 3,  0, (void *) sizeof(pnts));
	VertexAttribPointer(program, "uaxis", 3, 0, (void *) (2*sizeof(pnts)));
	VertexAttribPointer(program, "vaxis", 3, 0, (void *) (3*sizeof(pnts)));
	VertexAttribPointer(program, "uv", 2, 0, (void *) (4*sizeof(pnts)));
	// transform light
	vec4 xl(camera.modelview*vec4(light, localLights == 1? 1.f : 0.f));
	vec3 xlight(xl.x, xl.y, xl.z);
	// set uniforms, render quad
	SetUniform(program, "modelview", camera.modelview);
	SetUniform(program, "persp", camera.persp);
	SetUniform3v(program, "light", 1, &xlight.x);
	SetUniform(program, "diffuseOnly", diffuseOnly);
	SetUniform(program, "localLights", localLights);
	SetUniform(program, "useTexture", useTexture);
	glDrawArrays(GL_QUADS, 0, 4);
	// draw light sources
	UseDrawShader(camera.fullview);
	glDisable(GL_DEPTH_TEST);
	if ((float) (clock()-tEvent)/CLOCKS_PER_SEC < 1)
		Disk(light, 12, IsVisible(light, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	Text(20, 20, vec3(0,0,0), 9, "toggle d: now %s", diffuseOnly ? "diffuse" : "diffuse & specular");
	Text(20, 45, vec3(0,0,0), 9, "toggle l: now %s", localLights? "local lights" : "infinite lights");
	Text(20, 70, vec3(0,0,0), 9, "toggle t: now %s", useTexture? "texture" : "no texture");
    glFlush();
}

// Mouse Handlers

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

int WindowHeight(GLFWwindow *w) {
	int width, height;
	glfwGetWindowSize(w, &width, &height);
	return height;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
    y = WindowHeight(w)-y; 			// invert y for upward-increasing screen space
	picked = NULL;
	if (action == GLFW_PRESS) {
		float dsq = ScreenDistSq(x, y, light, camera.fullview);
		if (dsq < 150) {
			picked = &lightMover;
			lightMover.Down(&light, x, y, camera.modelview, camera.persp);
		}
		if (picked == NULL) {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
	if (action == GLFW_RELEASE)
		camera.MouseUp();
}

void MouseMove(GLFWwindow *w, double x, double y) {
	tEvent = clock();
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		y = WindowHeight(w)-y; // invert y for upward-increasing screen space
		if (picked == &lightMover)
			lightMover.Drag(x, y, camera.modelview, camera.persp);
		else
			camera.MouseDrag(x, y, Shift(w));
	}
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	camera.MouseWheel(yoffset, Shift(w));
}

void Resize(GLFWwindow *w, int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

void Keyboard(GLFWwindow *w, int c, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (c) {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(w, GLFW_TRUE); break;
			case 'L': localLights = 1-localLights; break;
			case 'D': diffuseOnly = 1-diffuseOnly; break;
			case 'T': useTexture = 1-useTexture; break;
			default: break;
		}
}

const char *usage = "\
	  L: toggle local lights\n\
	  D: toggle diffuse\n\
	  T: toggle texture\n\
";

int main() {
    glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4); // anti-alias
    GLFWwindow *w = glfwCreateWindow(800, 800, "Bump-mapped Quad", NULL, NULL);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
    int width, height;
	glfwGetWindowSize(w, &width, &height);
	Resize(w, width, height);
    // init vertex buffer
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	int sPnts = sizeof(pnts), sUvs = sizeof(uvs);
    glBufferData(GL_ARRAY_BUFFER, 4*sPnts+sUvs, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sPnts, pnts);
    glBufferSubData(GL_ARRAY_BUFFER, sPnts, sPnts, nrms);
    glBufferSubData(GL_ARRAY_BUFFER, 2*sPnts, sPnts, uaxs);
    glBufferSubData(GL_ARRAY_BUFFER, 3*sPnts, sPnts, vaxs);
    glBufferSubData(GL_ARRAY_BUFFER, 4*sPnts, sUvs, uvs);
    // init bump map
	textureId = LoadTexture(textureFilename, textureUnit);
	normalId = LoadTexture(normalFilename, normalUnit);
	// callbacks
    glfwSetCursorPosCallback(w, MouseMove);
    glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetScrollCallback(w, MouseWheel);
    glfwSetKeyCallback(w, Keyboard);
    glfwSetWindowSizeCallback(w, Resize);
    printf("Usage:\n%s\n", usage);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	glDeleteBuffers(1, &textureId);
    glfwDestroyWindow(w);
    glfwTerminate();
}
