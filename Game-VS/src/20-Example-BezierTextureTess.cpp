// BezierTextureTess.cpp - textured patch using tessellation shader
// copyright (c) Jules Bloomenthal, 2016, all rights reserved

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Misc.h"
#include "Text.h"
#include "Widgets.h"
#include "VecMat.h"

// display parameters
GLFWwindow	   *window;
int				winWidth = 800, winHeight = 600;
Camera			camera(winWidth, winHeight, vec3(0,0,0), vec3(0,0,-5));

// Bezier patch
vec3			ctrlPts[4][4];							// 16 control points, indexed [s][t]

// movable light
vec3			lightSource(-.2f, .4f, .3f);

// selection
int				xCursorOffset = -7, yCursorOffset = -3; // for GLFW
void		   *picked = NULL, *hover = NULL;

// UI
bool			viewMesh = true;
Toggler			viewMeshTog(&viewMesh, "control mesh", 20, 50, 14);
Mover			mover;
int				res = 25;

// shading
GLuint			shader = 0;								// valid if > 0
GLuint          textureId = 0;							// valid if > 0

// vertex shader
const char *vShaderCode = "void main() { gl_Position = vec4(0); }"; // no-op

// tessellation evaluation
const char *teShaderCode = "\
	#version 400 core																				\n\
	layout (quads, equal_spacing, ccw) in;															\n\
	uniform vec3 ctrlPts[16];																		\n\
    uniform mat4 modelview;																			\n\
	uniform mat4 persp;																				\n\
	out vec3 normal;																				\n\
	out vec3 point;																					\n\
	out vec2 uv;																					\n\
	vec3 BezTangent(float t, vec3 b1, vec3 b2, vec3 b3, vec3 b4) {									\n\
		float t2 = t*t;																				\n\
		return (-3*t2+6*t-3)*b1+(9*t2-12*t+3)*b2+(6*t-9*t2)*b3+3*t2*b4;								\n\
	}																								\n\
	vec3 BezPoint(float t, vec3 b1, vec3 b2, vec3 b3, vec3 b4) {									\n\
		float t2 = t*t, t3 = t*t2;																	\n\
		return (-t3+3*t2-3*t+1)*b1+(3*t3-6*t2+3*t)*b2+(3*t2-3*t3)*b3+t3*b4;							\n\
	}																								\n\
	void main() {																					\n\
		vec3 spts[4], tpts[4];																		\n\
		uv = gl_TessCoord.st;																		\n\
		for (int i = 0; i < 4; i++) {																\n\
			spts[i] = BezPoint(uv.s, ctrlPts[4*i], ctrlPts[4*i+1], ctrlPts[4*i+2], ctrlPts[4*i+3]);	\n\
			tpts[i] = BezPoint(uv.t, ctrlPts[i], ctrlPts[i+4], ctrlPts[i+8], ctrlPts[i+12]);		\n\
		}																							\n\
		vec3 p = BezPoint(uv.t, spts[0], spts[1], spts[2], spts[3]);								\n\
		vec3 tTan = BezTangent(uv.t, spts[0], spts[1], spts[2], spts[3]);							\n\
		vec3 sTan = BezTangent(uv.s, tpts[0], tpts[1], tpts[2], tpts[3]);							\n\
		vec3 n = normalize(cross(sTan, tTan));														\n\
		normal = (modelview*vec4(n, 0)).xyz;														\n\
		point = (modelview*vec4(p, 1)).xyz;															\n\
		gl_Position = persp*vec4(point, 1);															\n\
	}";

// pixel shader
const char *pShaderCode = "\
    #version 130 core																				\n\
	in vec3 point;																					\n\
	in vec3 normal;																					\n\
	in vec2 uv;																						\n\
	out vec4 pColor;																				\n\
	uniform sampler2D textureMap;																	\n\
	uniform vec3 light;																				\n\
	uniform int flatShade = 0;                  // 0: Phong, 1: flat								\n\
	uniform int facing = 0;						// 0: both, 1: fwd-facing, 2: back-facing			\n\
	uniform vec4 color = vec4(1, 1, 1, 1);		// opaque white										\n\
	void PhongIntensity(vec3 pos, vec3 nrm, out float dif, out float spec) {						\n\
		vec3 N = normalize(nrm);				// surface normal									\n\
        vec3 L = normalize(light-pos);			// light vector										\n\
        vec3 E = normalize(pos);				// eye vertex										\n\
        vec3 R = reflect(L, N);					// highlight vector									\n\
		dif = abs(dot(N, L));                   // one-sided diffuse								\n\
		spec = pow(max(0, dot(E, R)), 50);															\n\
	}																								\n\
    void main() {																					\n\
		if ((facing == 1 && normal.z < 0) || (facing == 2 && normal.z > 0))							\n\
			discard;							// for two-pass transparency						\n\
		if (flatShade == 1)																			\n\
			pColor = color;						// two-sided flat shading							\n\
			else {																					\n\
				float amb = .15f, dif = 0, spec = 0;												\n\
				// which side of plane defined by point and normal are viewer, light?				\n\
				bool sideLight = (dot(normal, light)-dot(point, normal)) < 0;						\n\
				bool sideViewer = normal.z < 0;														\n\
				if (sideLight == sideViewer)														\n\
					PhongIntensity(point, normal, dif, spec);										\n\
				vec4 texColor = texture(textureMap, uv);											\n\
				float ad = clamp(amb+dif, 0, 1);													\n\
				pColor = vec4(ad*texColor.rgb, 1)+vec4(spec*color.rgb, 1);							\n\
			}																						\n\
	}";

// display

int WindowHeight(GLFWwindow *w) {
	int width, height;
	glfwGetWindowSize(w, &width, &height); // GetFramebufferSize?
	return height;
}

void Display() {
    // background, blending, zbuffer
    glClearColor(.6f, .6f, .6f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader);
	// update control points
	GLint id = glGetUniformLocation(shader, "ctrlPts");
	glUniform3fv(id, 16, (float *) &ctrlPts[0][0]);
	// update matrices
	SetUniform(shader, "modelview", camera.modelview);
	SetUniform(shader, "persp", camera.persp);
	// transform light and send to fragment shader
	vec4 hLight = camera.modelview*vec4(lightSource, 1);
	vec3 xlight(hLight.x, hLight.y, hLight.z);
	glUniform3fv(glGetUniformLocation(shader, "light"), 1, (float *) &xlight);
	// tessellate patch
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	float r = (float) res;
	float outerLevels[] = {r, r, r, r}, innerLevels[] = {r, r};
		// defined as floats, but intended use is integer
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerLevels);
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerLevels);
	glDrawArrays(GL_PATCHES, 0, 4);
	// control mesh
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(camera.fullview);
	if (viewMesh) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(6, 0xAAAA);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 3; j++) {
				Line(ctrlPts[i][j], ctrlPts[i][j+1], 1.25f, vec3(1,1,0));
				Line(ctrlPts[j][i], ctrlPts[j+1][i], 1.25f, vec3(1,1,0));
			}
		glDisable(GL_LINE_STIPPLE);
		for (int i = 0; i < 16; i++)
			Disk(ctrlPts[i/4][i%4], 7, vec3(1,1,0));
	}
	Disk(lightSource, 12, hover == (void *) &lightSource? vec3(0,1,1) : IsVisible(lightSource, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	// butttons: draw in 2D screen space
	UseDrawShader(ScreenMode());
	viewMeshTog.Draw();
	Text(20, 20, vec3(0, 0, 0), 1, "res = %i", res);
    glFlush();
}

// mouse

vec3 *PickControlPoint(int x, int y) {
	for (int k = 0; k < 16; k++)
		if (MouseOver(x, y, ctrlPts[k/4][k%4], camera.fullview, xCursorOffset, yCursorOffset))
			return &ctrlPts[k/4][k%4];
	return NULL;
}

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight(w)-y; // invert y for upward-increasing screen space
	hover = picked = NULL;
	bool hit = viewMeshTog.UpHit(x, y, action);
    if (action == GLFW_RELEASE)
		camera.MouseUp();
	if (action == GLFW_PRESS && !hit) {
		vec3 *pp = viewMesh? PickControlPoint(x, y) : NULL;
		if (pp) {
			// pick or deselect control point
			if (butn == GLFW_MOUSE_BUTTON_LEFT) {
				mover.Down(pp, x, y, camera.modelview, camera.persp);
				picked = &mover;
			}
		}
		else if (MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)) {
			mover.Down(&lightSource, x, y, camera.modelview, camera.persp);
			picked = &mover;
		}
		else {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
}

void MouseMove(GLFWwindow *w, double x, double y) {
	y = WindowHeight(w)-y; // invert y for upward-increasing screen space
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		if (picked == &mover)
			mover.Drag(x, y, camera.modelview, camera.persp);
		if (picked == &camera)
			camera.MouseDrag((int) x, (int) y, Shift(w));
    }
    else
 		hover = MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)? (void *) &lightSource : NULL;
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	camera.MouseWheel((int) yoffset, Shift(w));
}

// patch

void DefaultControlPoints() {
	vec3 p0 = vec3(-.35f,-.35f,0), p1 = vec3(.35f,-.35f,0), p2 = vec3(-.35f,.35f,0), p3 = vec3(.35f,.35f,0);
	float vals[] = {0, 1/3.f, 2/3.f, 1.};
	for (int i = 0; i < 16; i++) {
		float ax = vals[i%4], ay = vals[i/4];
		vec3 p10 = p0+ax*(p1-p0), p32 = p2+ax*(p3-p2);
		ctrlPts[i/4][i%4] = p10+ay*(p32-p10);
	}
}

// application

void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == 'R' && action == GLFW_PRESS) {
		res += mods & GLFW_MOD_SHIFT? -1 : 1;
		res = res < 1? 1 : res;
	}
}

void Resize(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int ac, char **av) {
    // init app window
    if (!glfwInit())
        return 1;
    window = glfwCreateWindow(winWidth, winHeight, "Bezier Texture Tess", NULL, NULL);
	glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    // init OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glViewport(0, 0, winWidth, winHeight);
	// init shader programs
	shader = LinkProgramViaCode(&vShaderCode, NULL, &teShaderCode, NULL, &pShaderCode);
	// init patch, texture
	DefaultControlPoints();
	textureId = LoadTexture("C:/Users/jules/SeattleUniversity/Exe/Lily.tga", 0);
    // callbacks
    glfwSetCursorPosCallback(window, MouseMove);
    glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetScrollCallback(window, MouseWheel);
    glfwSetKeyCallback(window, Keyboard);
    glfwSetWindowSizeCallback(window, Resize);
    // event loop
    glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		Display();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
    glfwDestroyWindow(window);
    glfwTerminate();
}
