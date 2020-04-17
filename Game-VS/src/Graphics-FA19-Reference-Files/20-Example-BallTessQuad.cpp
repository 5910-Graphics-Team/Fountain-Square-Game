// BallTessQuad.cpp - textured displacement mapping (quad tessellation: unlike BallTessTri.cpp, no pole degeneracies)

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Misc.h"
#include "Text.h"
#include "VecMat.h"
#include "Widgets.h"

// view
GLFWwindow     *window;
int				winWidth = 800, winHeight = 800;
Camera			camera(winWidth, winHeight, vec3(0,0,180), vec3(0,0,-15));

// selection
void		   *picked = NULL, *hover = NULL;
int				xCursorOffset = -7, yCursorOffset = -3;

// movable lightSource
vec3			lightSource(-1.4f, -1.7f, .7f);
Mover			mover;

// controls
float			heightScale = 0;
int				res = 50;

// shader indices (for program and textures, but no vertex buffer)
GLuint			shaderId = 0, textureNames[2] = {0, 0};

// no-op vertex shader
const char *vShaderCode = "\
    #version 130 core														\n\
	void main() {															\n\
		gl_Position = vec4(0);												\n\
	}";

// tessellation evaluation: set vertex position, normal, and st parameters
const char *teShaderCode = "\
	#version 400 core														\n\
	layout (quads, equal_spacing, ccw) in;									\n\
	// BallTessTri input array 3 uvs, but here single gl_TessCoord.st		\n\
	out	vec3 point;															\n\
	out	vec3 normal;														\n\
	out	vec2 uv;															\n\
	uniform sampler2D heightField;											\n\
	uniform float heightScale;												\n\
    uniform mat4 modelview;													\n\
	uniform mat4 persp;														\n\
	vec3 PtFromSphere(float u, float v) {									\n\
		// u is longitude, v is latitude									\n\
		// latitude: PI/2 = N. pole, 0 = equator, -PI/2 = S. pole			\n\
		float _PI = 3.141592;												\n\
		float elevation = _PI/2-_PI*v;										\n\
		float eFactor = cos(elevation);										\n\
		float y = sin(elevation);											\n\
		float angle = 2*_PI*u;												\n\
		float x = eFactor*cos(angle), z = eFactor*sin(angle);				\n\
		return vec3(x, y, z);												\n\
	}																		\n\
	vec3 PtFromField(vec2 uv) {												\n\
		float h = heightScale*texture(heightField, uv).r;					\n\
		return (1+h)*PtFromSphere(uv.s, uv.t);								\n\
	}																		\n\
	vec3 PtFromField(float u, float v) {									\n\
		return PtFromField(vec2(u, v));										\n\
	}																		\n\
	vec3 NormalFromField(float u, float v) {								\n\
		// approximate normal with differencing								\n\
		float du = 1/gl_TessLevelInner[0], dv = 1/gl_TessLevelInner[1];		\n\
		vec3 texL = PtFromField(mod(u-du, 1), v);							\n\
		vec3 texR = PtFromField(mod(u+du, 1), v);							\n\
		vec3 texT = PtFromField(u, max(0, v-dv));							\n\
		vec3 texB = PtFromField(u, min(1, v+dv));							\n\
		return normalize(cross(texT-texB, texR-texL));						\n\
	}																		\n\
	void main() {															\n\
		// send uv, point, normal to pixel shader							\n\
		uv = gl_TessCoord.st;												\n\
		vec3 p = PtFromField(uv);											\n\
		vec3 n = abs(heightScale) < .01? p : NormalFromField(uv.s, uv.t);	\n\
		vec4 hPosition = modelview*vec4(p, 1);								\n\
		gl_Position = persp*hPosition;										\n\
		point = hPosition.xyz;												\n\
		normal = normalize(modelview*vec4(n, 0)).xyz;						\n\
	}";

// pixel shader
const char *pShaderCode = "\
    #version 130 core														\n\
	in	vec3 point;															\n\
	in	vec3 normal;														\n\
	in	vec2 uv;															\n\
	out vec4 fColor;														\n\
	uniform sampler2D textureImage;											\n\
	uniform vec3 light;														\n\
    void main() {															\n\
		vec3 N = normalize(normal);					// surface normal		\n\
        vec3 L = normalize(light-point);			// light vector			\n\
        vec3 E = normalize(point);					// eye vertex			\n\
        vec3 R = reflect(L, N);						// highlight vector		\n\
		float dif = clamp(dot(N, L), 0, 1);			// one-sided diffuse	\n\
		float spec = pow(max(0, dot(E, R)), 50);							\n\
		float amb = .15;													\n\
		float intensity = clamp(amb+dif+spec, 0, 1);						\n\
		vec4 texColor = texture(textureImage, uv);							\n\
		fColor = vec4(intensity*texColor.rgb+spec, 1);						\n\
	}";

// display

void Display() {
    // background, blending, zbuffer
    glClearColor(.6f, .6f, .6f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
	// use shader program, update and send matrices
	glUseProgram(shaderId);
	SetUniform(shaderId, "modelview", camera.modelview);
	SetUniform(shaderId, "persp", camera.persp);
	// texture map
//	NEEDED? glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureNames[0]);
	SetUniform(shaderId, "textureImage", 0);
	// height field
//	NEEDED? glActiveTexture(GL_TEXTURE0+0); or +1?
	glBindTexture(GL_TEXTURE_2D, textureNames[0]);
	SetUniform(shaderId, "heightField", 0);
	SetUniform(shaderId, "heightScale", heightScale);
	// transform light and send to pixel shader
	vec4 hLight = camera.fullview*vec4(lightSource, 1);
	glUniform3fv(glGetUniformLocation(shaderId, "light"), 1, (float *) &hLight);
	// establish tessellating patch and display
	float r = (float) res, outerLevels[] = {r, r, r, r}, innerLevels[] = {r, r};
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerLevels);
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerLevels);
	glDrawArrays(GL_PATCHES, 0, 4);
	// use Draw.h shader to draw light source
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(camera.fullview);
	Disk(lightSource, 12, hover == (void *) &lightSource? vec3(0,1,1) : IsVisible(lightSource, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	// text in 2D screen space
	UseDrawShader(ScreenMode());
	Text(20, 20, vec3(0,0,0), 1, "Resolution: %i (r to increase, SHIFT-r to decrease)", res);
	Text(20, 45, vec3(0,0,0), 1, "Vert scale: %3.2f (h to increase, SHIFT-h to decrease)", heightScale);
    glFlush();
}

// mouse

int WindowHeight() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight()-y; // invert y for upward-increasing screen space
	if (action == GLFW_RELEASE)
		camera.MouseUp();
	if (action == GLFW_RELEASE && butn == GLFW_MOUSE_BUTTON_RIGHT) {
		float fp1[3], fp2[3];
		ScreenLine((float) x, (float) y, camera.modelview, camera.persp, fp1, fp2);
		vec3 p1(fp1[0], fp1[1], fp1[2]), p2(fp2[0], fp2[1], fp2[2]);
		vec3 v = normalize(p2-p1);
		float alpha = RaySphere(p1, v, vec3(0,0,0), 1);
		camera.SetRotateCenter(alpha < 0? vec3(0,0,0) : p1+alpha*v);
	}
	hover = picked = NULL;
	if (action == GLFW_PRESS && butn == GLFW_MOUSE_BUTTON_LEFT) {
		if (MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)) {
			picked = &lightSource;
			mover.Down(&lightSource, x, y, camera.modelview, camera.persp);
		}
		else {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
}

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

void MouseMove(GLFWwindow *w, double x, double y) {
	y = WindowHeight()-y;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		if (picked == &lightSource)
			mover.Drag(x, y, camera.modelview, camera.persp);
		else if (picked == &camera)
			camera.MouseDrag(x, y, Shift(w));
	}
	else // mouse over
		hover = MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)? (void *) &lightSource : NULL;
}

void MouseWheel(GLFWwindow *w, double xoffset, double direction) {
	camera.MouseWheel(direction, Shift(w));
}

// application

static void Keyboard(GLFWwindow *window, int c, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		bool shift = mods & GLFW_MOD_SHIFT;
		if (c == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (c == 'H')
			heightScale += shift? -.05f : .05f;
		if (c == 'R') {
			int inc = res < 5? 1 : res < 10? 2 : res < 20? 3 : res < 50? 5 : 10;
			res += shift? -inc : inc;
			res = res < 1? 1 : res > 200? 200 : res;
		}
	}
}

void Resize(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int ac, char **av) {
	// init app window and GL context
    glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4); // anti-alias (esp. silhouette edges)
    window = glfwCreateWindow(winWidth, winHeight, "Mesh, Textured & Tessellated", NULL, NULL);
	glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	// build, use shaderId program
	if (!(shaderId = LinkProgramViaCode(&vShaderCode, NULL, &teShaderCode, NULL, &pShaderCode))) {
		printf("Can't link shader program\n");
		getchar();
		return 1;
	}
	// note: no vertices, no vertex buffer
	// init texture and height maps
	glGenTextures(2, textureNames);
		// a single texture map (default name '0') can be used with no call to glGenTextures
		// if glGenTextures is called, names other than '0' are generated
		// next comment not quite right - see documentation on ActiveTexture
		// if these names are used to bind a texture, there must be a corresponding call to glActiveTexture,
		// supplying the corresponding texture enumeration (GL_TEXTURE1 for '1', GL_TEXTURE2 for '2')
	textureNames[0] = LoadTexture("C:/Users/jules/SeattleUniversity/Exe/Earth.tga", 0);
	textureNames[1] = LoadTexture("C:/Users/jules/SeattleUniversity/Exe/EarthHeight.tga", 1);
    // callbacks
    glfwSetCursorPosCallback(window, MouseMove);
    glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetScrollCallback(window, MouseWheel);
    glfwSetKeyCallback(window, Keyboard);
    glfwSetWindowSizeCallback(window, Resize);
    // event loop
    glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Display();
		glfwSwapBuffers(window);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, textureNames);
    glfwDestroyWindow(window);
    glfwTerminate();
}
