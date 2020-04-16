// BezierPatch.cpp - interactive patch design

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "VecMat.h"
#include "Widgets.h"

// display
GLFWwindow *window;
int			winWidth = 930, winHeight = 800;
Camera		camera(winWidth, winHeight, vec3(0,0,0), vec3(0,0,-5));

// widgets
int			xCursorOffset = -7, yCursorOffset = -3;
Mover		mover;
void	   *picked = NULL, *hover = NULL;

// patch
int			res = 10;				// res*res quadrilaterals (res+1)**2 #vertices
vec3		ctrlPts[4][4];			// 16 control points, indexed [s][t]
vec3        coeffs[4][4];
// shading
GLuint		shader = 0, vBufferId = -1;
vec3        lightSource(.6f, .4f, .2f);

// Bezier operations

vec3 BezTangent(float t, vec3 b1, vec3 b2, vec3 b3, vec3 b4) {
	// return the tangent of the curve given parameter t
	// HERE
	return b1 * (6 * t - 3 * pow(t, 2) - 3) + b2 * (9 * pow(t, 2) - 12 * t + 3) + 
		b3 * (6 * t - 9 * pow(t, 2)) + 3 * pow(t, 2) * b4;
	 
}


vec3 BezPoint(float t, vec3 b1, vec3 b2, vec3 b3, vec3 b4) {
	// return the position of the curve given parameter t
	return (0 - pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) * b1 + (3 * pow(t, 3) - 6 * pow(t, 2) + 3 * t) * b2 +
		(-3 * pow(t, 3) + 3 * pow(t, 2)) * b3 + pow(t, 3) * b4;
}

void BezierPatch(float s, float t, vec3* point, vec3* normal) {
	vec3 spts[4], tpts[4];
	for (int i = 0; i < 4; i++) {
		spts[i] = BezPoint(s, ctrlPts[i][0], ctrlPts[i][1],
			ctrlPts[i][2], ctrlPts[i][3]);
		tpts[i] = BezPoint(t, ctrlPts[0][i], ctrlPts[1][i],
			ctrlPts[2][i], ctrlPts[3][i]);
	}
	*point = BezPoint(t, spts[0], spts[1], spts[2], spts[3]);
	vec3 tTan = BezTangent(t, spts[0], spts[1], spts[2], spts[3]);
	vec3 sTan = BezTangent(s, tpts[0], tpts[1], tpts[2], tpts[3]);
	*normal = normalize(cross(sTan, tTan));
};

const char *vShader = "\
	#version 150														\n\
	in vec3 point;														\n\
	in vec3 normal;														\n\
	out vec3 vPoint;													\n\
	out vec3 vNormal;													\n\
    uniform mat4 modelview;												\n\
	uniform mat4 persp;													\n\
	void main() {														\n\
		vNormal = (modelview*vec4(normal, 0)).xyz;						\n\
		vPoint = (modelview*vec4(point, 1)).xyz;						\n\
		gl_Position = persp*vec4(vPoint, 1);							\n\
	}";

const char *pShader = "\
    #version 150														\n\
	in vec3 vPoint;														\n\
	in vec3 vNormal;													\n\
	out vec4 pColor;													\n\
	uniform vec3 light;													\n\
	uniform vec4 color = vec4(0,.7,0,1);	// default					\n\
    void main() {														\n\
		vec3 N = normalize(vNormal);       	// surface normal			\n\
        vec3 L = normalize(light-vPoint);  	// light vector				\n\
        vec3 E = normalize(vPoint);        	// eye vertex				\n\
        vec3 R = reflect(L, N);            	// highlight vector			\n\
        float d = abs(dot(N, L));          	// two-sided diffuse		\n\
        float s = abs(dot(R, E));          	// two-sided specular		\n\
		float intensity = clamp(d+pow(s, 50), 0, 1);					\n\
		pColor = vec4(intensity*color.rgb, 1);							\n\
	}";

// vertex buffer

int nQuadrilaterals = 0;

void SetVertices(int res, bool init = false) {
	// activate GPU vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
	// get pointers to GPU memory for vertices, normals
	nQuadrilaterals = res*res;
	int sizeBuffer = 2*4*nQuadrilaterals*sizeof(vec3);
	if (init)
		glBufferData(GL_ARRAY_BUFFER, sizeBuffer, NULL, GL_STATIC_DRAW);
	// set the 4 vertices for each quadrilateral and save to GPU
	float vals[] = { -.75, -.25, .25, .75 };
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			ctrlPts[i][j] = vec3(vals[i], vals[j], i % 3 == 0 || j % 3 == 0 ? .5 : 0);
}

void SetCoeffs() {
	// set Bezier coefficient matrix
	mat4 m(vec4(-1, 3, -3, 1), vec4(3, -6, 3, 0), vec4(-3, 3, 0, 0), vec4(1, 0, 0, 0));
	mat4 g;
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 16; i++)
			g[i / 4][i % 4] = ctrlPts[i / 4][i % 4][k];
		mat4 c = m * g * m;
		for (int i = 0; i < 16; i++)
			coeffs[i / 4][i % 4][k] = c[i / 4][i % 4];
	}
}

// display

void Display() {
    // background, blending, zbuffer
    glClearColor(.6f, .6f, .6f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
	// invoke Phong shader
	glUseProgram(shader);

	// send matrices to vertex shader
	SetUniform(shader, "modelview", camera.modelview);
	SetUniform(shader, "persp", camera.persp);
	// set vertex feed
	glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
	// just send fixed light to fragment shader (in previous assignments we have transformed the light
	// by the modelview matrix in order that the light be part of the same scene containing the object)
	// here, the light doesn't move, even though the object may be rotated, giving the impression of
	// something be examined, rather than the camera moving around a scene
	vec4 hLight = camera.modelview*vec4(lightSource, 1);
	SetUniform(shader, "light", vec3(hLight.x, hLight.y, hLight.z));
	// connect patch points and normals to vertex shader
	VertexAttribPointer(shader, "point", 3, 2*sizeof(vec3), (void *) 0);
	VertexAttribPointer(shader, "normal", 3, 2*sizeof(vec3), (void *) sizeof(vec3));
	// shade patch
	glDrawArrays(GL_QUADS, 0, 4*nQuadrilaterals);
	UseDrawShader(camera.fullview);
	glDisable(GL_DEPTH_TEST);
	Disk(lightSource, 12, hover == (void *) &lightSource? vec3(0,1,1) : IsVisible(lightSource, camera.fullview, NULL, NULL, NULL, 0.0f)? vec3(1,0,0) : vec3(0,0,1));
	
	// TODO draw 16 movable points and lines between control mesh
	// HERE
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Disk(ctrlPts[i][j], 2.0f, 1.0f, 2.5f);
			//Line(ctrlPts[i][j], ctrlPts[(i + 1) % 4][j + 1 % 4], 1.0f, 0.0f, 1.0f);
		}
	}
	
	
	
	glFlush();
}

// mouse

vec3 *PickControlPoint(int x, int y, bool rightButton) {
	for (int k = 0; k < 16; k++)
		if (MouseOver(x, y, ctrlPts[k/4][k%4], camera.fullview, xCursorOffset, yCursorOffset))
			return &ctrlPts[k/4][k%4];
	return NULL;
}

int WindowHeight() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight()-y; // invert y for upward-increasing screen space
	hover = picked = NULL;
    if (action == GLFW_RELEASE)
		camera.MouseUp();
	if (action == GLFW_PRESS) {
		vec3 *pp = PickControlPoint(x, y, butn == GLFW_MOUSE_BUTTON_RIGHT);
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
	y = WindowHeight()-y; // invert y for upward-increasing screen space
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		bool shift = glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
					 glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
		if (picked == &mover) {
			mover.Drag(x, y, camera.modelview, camera.persp);
			SetVertices(res);
		}
		if (picked == &camera)
			camera.MouseDrag((int) x, (int) y, shift);
    }
    else
 		hover = MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)? (void *) &lightSource : NULL;
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	bool shift = glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	camera.MouseWheel((int) yoffset, shift);
}

vec3 PointFromCtrlPts(float s, float t) {

	vec3 spt[4];

	for (int i = 0; i < 4; i++)
		spt[i] = BezPoint(s, ctrlPts[i][0], ctrlPts[i][1], ctrlPts[i][2], ctrlPts[i][3]);
	return BezPoint(t, spt[0], spt[1], spt[2], spt[3]);

}


vec3 PointFromCoeffs(float s, float t) {
	vec3 p;
	
	float s2 = s * s, s3 = s * s2, t2 = t * t, ta[] = { t * t2, t2, t, 1 };

	for (int i = 0; i < 4; i++)
		p += ta[i] * (s3 * coeffs[i][0] + s2 * coeffs[i][1] + s * coeffs[i][2] + coeffs[i][3]);
	return p;
}

// patch


void DefaultControlPoints() {
	vec3 p0(.5f, -.25f, 0), p1(.5f, .25f, 0), p2(-.5f, -.25f, 0), p3(-.5f, .25f, 0);
	float vals[] = {0, 1/3.f, 2/3.f, 1.};
	for (int i = 0; i < 16; i++) {
		float ax = vals[i%4], ay = vals[i/4];
		vec3 p10 = p0+ax*(p1-p0), p32 = p2+ax*(p3-p2);
		ctrlPts[i/4][i%4] = p10+ay*(p32-p10);
	}
}

// application

static void Keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == 'R' && action == GLFW_PRESS) {
		res += mods & GLFW_MOD_SHIFT? -1 : 1;
		res = res < 1? 1 : res;
		SetVertices(res, true);
	}
}

void Resize(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int ac, char **av) {
    // init app window
    glfwInit();
    window = glfwCreateWindow(winWidth, winHeight, "Simple Bezier Patch", NULL, NULL);
	glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    // init OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glViewport(0, 0, winWidth, winHeight);
	// make shader program
	shader = LinkProgramViaCode(&vShader, &pShader);
	// init patch
	DefaultControlPoints();
	// make vertex buffer
	glGenBuffers(1, &vBufferId);
	SetVertices(res, true);
	SetCoeffs();
    // callbacks
    glfwSetCursorPosCallback(window, MouseMove);
    glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetScrollCallback(window, MouseWheel);
    glfwSetKeyCallback(window, Keyboard);
    glfwSetWindowSizeCallback(window, Resize);
    // event loop
	while (!glfwWindowShouldClose(window)) {
		Display();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
    glfwDestroyWindow(window);
    glfwTerminate();
}
