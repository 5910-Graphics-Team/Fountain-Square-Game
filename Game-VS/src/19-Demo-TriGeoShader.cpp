// TriGeoShader.cpp: use geometry shader to subdivide and outline triangle

#include <glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Text.h"
#include "Widgets.h"

// shader ids
GLuint		vBuffer = 0, program = 0;

// equilateral triangle
float		h = 1/sqrt(3), points[][3] = {{.5f, -h/2, 0}, {0, h, 0}, {-.5, -h/2, 0}};

// display
int			winWidth = 800, winHeight = 800;
Camera		camera(winWidth, winHeight, vec3(70,0,0), vec3(0,0,-5));

// display params
enum Mode	{ M_Shade = 0, M_Line, M_HLE } mode = M_Shade;
float		lineWidth = 5, outlineTransition = 2; // in pixels
vec3		light(.3, .2, -.2);

// interaction
void	   *picked = NULL, *hover = NULL;
Mover 		mover;

const char *vertexShader = "\
	#version 130																			\n\
	uniform mat4 modelview;																	\n\
	in vec3 pt;																				\n\
	out vec3 vPt;																			\n\
	void main() {																			\n\
		vPt = (modelview*vec4(pt, 1)).xyz;													\n\
	}";

const char *geometryShader = "\
	#version 330 core																		\n\
	layout (triangles) in;																	\n\
	layout (triangle_strip, max_vertices = 12) out;											\n\
	uniform mat4 persp;																		\n\
	uniform mat4 viewpt;																	\n\
	in vec3 vPt[];																			\n\
	out vec3 gPt;																			\n\
	out vec3 gNrm;																			\n\
	out vec3 gColor;																		\n\
	noperspective out vec3 gEdgeDistance;													\n\
	vec2 ScreenPoint(vec4 h) {																\n\
		// perspective divide and viewport transform:										\n\
		return (viewpt*vec4(h.xyz/h.w, 1)).xy;												\n\
	}																						\n\
	void EmitTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 color) {								\n\
		// transform each vertex into perspective and into screen space						\n\
		vec4 x0 = persp*vec4(v0, 1), x1 = persp*vec4(v1, 1), x2 = persp*vec4(v2, 1);		\n\
		vec2 p0 = ScreenPoint(x0), p1 = ScreenPoint(x1), p2 = ScreenPoint(x2);				\n\
		// find altitudes ha, hb, hc (courtesy OpenGL4 Cookbook by Wolff)					\n\
		float a = length(p2-p1), b = length(p2-p0), c = length(p1-p0);						\n\
		float alpha = acos((b*b+c*c-a*a)/(2*b*c));											\n\
		float beta = acos((a*a+c*c-b*b)/(2*a*c));											\n\
		float ha = abs(c*sin(beta)), hb = abs(c*sin(alpha)), hc = abs(b*sin(alpha));		\n\
		// tetrahedron is faceted, so output same normal for triangle vertices				\n\
		gNrm = normalize(cross(v2-v1, v1-v0));												\n\
		// output edge dist, color, and homog & non-homog loc for three vertices			\n\
		for (int i = 0; i < 3; i++) {														\n\
    		gEdgeDistance = i==0? vec3(ha, 0, 0) : i==1? vec3(0, hb, 0) : vec3(0, 0, hc);	\n\
			gColor = color;																	\n\
			gl_Position = i == 0? x0 : i == 1? x1 : x2;										\n\
			gPt = i == 0? v0 : i == 1? v1 : v2;												\n\
			EmitVertex();																	\n\
		}																					\n\
		EndPrimitive();																		\n\
	}																						\n\
	void main() {																			\n\
		// compute fourth vertex															\n\
		vec3 center = (vPt[0]+vPt[1]+vPt[2])/3;												\n\
		vec3 n = normalize(cross(vPt[2]-vPt[1], vPt[1]-vPt[0]));							\n\
		float sLen = length(vPt[1]-vPt[0]);													\n\
		vec3 pt3 = center+sLen*sqrt(2)/sqrt(3)*n;											\n\
		// output tetrahedron, each face different color									\n\
		EmitTriangle(vPt[2], vPt[1], vPt[0], vec3(1,0,0));									\n\
		EmitTriangle(vPt[0], vPt[1], pt3, vec3(0,1,0));										\n\
		EmitTriangle(vPt[1], vPt[2], pt3, vec3(0,0,1));										\n\
		EmitTriangle(vPt[2], vPt[0], pt3, vec3(1,1,0));										\n\
	}";

const char *pixelShader = "\
	#version 330																			\n\
	uniform vec3 light;																		\n\
	uniform float outlineWidth = 1;															\n\
	uniform float transition = 1;															\n\
	uniform int mode = 0; // 0: Shade, 1: Lines, 2: HLE										\n\
	in vec3 gPt;																			\n\
	in vec3 gNrm;																			\n\
	in vec3 gColor;																			\n\
	noperspective in vec3 gEdgeDistance;													\n\
	out vec4 pColor;																		\n\
	void main() {																			\n\
		// compute triangle normal for faceted shading										\n\
		vec3 N = normalize(gNrm), E = -gPt;													\n\
		bool sideViewer = dot(E, N) < 0;													\n\
		// given local lights, compute total diffuse intensity								\n\
		float intensity = .2;																\n\
		vec3 L = normalize(light-gPt);														\n\
		bool sideLight = dot(L, N) < 0;														\n\
		if (sideLight == sideViewer)														\n\
			intensity += max(0, dot(N, L));													\n\
		intensity = clamp(intensity, 0, 1);													\n\
		// get dist to nearest edge and map to 0,1											\n\
		float minDist = min(min(gEdgeDistance[0], gEdgeDistance[1]), gEdgeDistance[2]);		\n\
		float t = smoothstep(outlineWidth-transition, outlineWidth+transition, minDist);	\n\
		// mix edge and surface colors (t=0: edgeColor, t=1: surfaceColor)					\n\
		pColor = mix(vec4(0,0,0,1), vec4(intensity*gColor,1), t);							\n\
		if (mode == 1)																		\n\
			pColor = vec4(0,0,0,1-t);														\n\
		if (mode == 2)																		\n\
			pColor = vec4(t,t,t,1);															\n\
	}";

void Display() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	// transform light
	vec4 xl = camera.modelview*vec4(light, 1);
	// send uniforms
	SetUniform(program, "light", (vec3 *) &xl);
	SetUniform(program, "viewpt", Viewport());
	SetUniform(program, "modelview", camera.modelview);
	SetUniform(program, "persp", camera.persp);
	SetUniform(program, "outlineWidth", lineWidth);
	SetUniform(program, "transition", outlineTransition);
	SetUniform(program, "mode", (int) mode);
	// z-buffer, blending
    if (mode == M_Line) {
		glDisable(GL_DEPTH_TEST);	// draw all lines
		glEnable(GL_BLEND);			// enable pixel shader alpha value
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
    else {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	// set vertex fetch, render
	VertexAttribPointer(program, "pt", 3, 0, (void *) 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// light
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(ScreenMode());
	Text(10,10,vec3(0,0,0),12,"light:%4.3f,%4.3f,%4.3f",light.x,light.y,light.z);
	UseDrawShader(camera.fullview);
	bool lVisible = IsVisible(light, camera.fullview);
	Disk(light, 12, hover == &light? lVisible? vec3(1,0,0) : vec3(1,0,1) : lVisible? vec3(1,1,0) : vec3(0,1,1));
	glFlush();
}

int WindowHeight(GLFWwindow *w) {
	int width, height;
	glfwGetWindowSize(w, &width, &height);
	return height;
}

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	if (action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(w, &x, &y);
		y = WindowHeight(w)-y;
		picked = NULL;
		if (MouseOver(x, y, light, camera.fullview)) {
			mover.Down(&light, x, y, camera.modelview, camera.persp);
			picked = &mover;
		}
		if (picked == NULL) {
			camera.MouseDown((int) x, (int) y);
			picked = &camera;
		}
	}
	if (action == GLFW_RELEASE)
		camera.MouseUp();
}

void MouseMove(GLFWwindow *w, double x, double y) {
	y = WindowHeight(w)-y;
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (picked == &mover)
			mover.Drag(x, y, camera.modelview, camera.persp);
		if (picked == &camera)
			camera.MouseDrag((int) x, (int) y, Shift(w));
	}
	else
		hover = MouseOver(x, y, light, camera.fullview)? (void *) &light : NULL;
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	camera.MouseWheel(yoffset, Shift(w));
}

void Resize(GLFWwindow *w, int width, int height) {
	camera.Resize(winWidth = width, winHeight = height);
	glViewport(0, 0, winWidth, winHeight);
}

void Keyboard(GLFWwindow *w, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		bool shift = mods & GLFW_MOD_SHIFT;
		if (key == 'T')
			outlineTransition *= (shift? .8f : 1.2f);
		if (key == 'W')
			lineWidth *= (shift? .8f : 1.2f);
		if (key == 'M')
			mode = (Mode) ((mode+1)%3);
	}
}

const char *usage = "\n\
	T: outline transition\n\
	W: line width\n\
	M: Shade/Line/HLE\n";

int main() {
    glfwInit();
    GLFWwindow *w = glfwCreateWindow(winWidth, winHeight, "Geometry Shader Demo", NULL, NULL);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	program = LinkProgramViaCode(&vertexShader, NULL, NULL, &geometryShader, &pixelShader);
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glfwSetCursorPosCallback(w, MouseMove);
    glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetScrollCallback(w, MouseWheel);
    glfwSetWindowSizeCallback(w, Resize);
    glfwSetKeyCallback(w, Keyboard);
    printf("Usage: %s\n", usage);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
    glfwDestroyWindow(w);
    glfwTerminate();
}

