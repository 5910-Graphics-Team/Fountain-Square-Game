// SpherePhong.cpp: Phong-shade a sphere

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "Camera.h"
#include "Color.h"
#include "Draw.h"
#include "Misc.h"
#include "GLXtras.h"
#include "VecMat.h"
#include "Widgets.h"

// colorful sphere
struct Vertex {
	vec3 point, color, normal;
	Vertex() { }
	Vertex(vec3 &p, vec3 &c, vec3 &n) : point(p), color(c), normal(n) { }
};
std::vector<Vertex> vertices;

// display
int				winWidth = 800, winHeight = 800;
Camera			camera((float) winWidth/winHeight, vec3(0,0,0), vec3(0,0,-10));
GLuint			vBufferPhong = 0, vBufferFaceted = 0, program = 0;
float			sphereSize = .1f;
vec3			lightSource(1.7f, 1.1f, 1.3f);
bool			facetedShade = false;

// interaction
int				xCursorOffset = -7, yCursorOffset = -3;
Mover			lightMover;
void		   *picked = NULL;

// shaders
const char *vertexShader = "\
	#version 130													\n\
	in vec3 point;													\n\
	in vec3 color;													\n\
	in vec3 normal;													\n\
	out vec3 vPoint;												\n\
	out vec3 vColor;												\n\
	out vec3 vNormal;												\n\
	uniform mat4 modelview;											\n\
	uniform mat4 persp;												\n\
	void main()	{													\n\
		vPoint = (modelview*vec4(point, 1)).xyz;					\n\
		vNormal = (modelview*vec4(normal, 0)).xyz;					\n\
		gl_Position = persp*vec4(vPoint, 1);						\n\
	    vColor = color;												\n\
	}";

const char *pixelShader = "\
    #version 130													\n\
	in vec3 vPoint;													\n\
	in vec3 vColor;													\n\
	in vec3 vNormal;												\n\
	out vec4 pColor;												\n\
	uniform vec3 lightPos = vec3(1,0,0);							\n\
	uniform float a = .05;											\n\
    void main() {													\n\
		vec3 N = normalize(vNormal);          // surface normal		\n\
        vec3 L = normalize(lightPos-vPoint);  // light vector		\n\
        vec3 E = normalize(vPoint);           // eye vertex			\n\
        vec3 R = reflect(L, N);               // highlight vector	\n\
        float d = max(0, dot(N, L));          // one-sided diffuse	\n\
        float h = max(0, dot(R, E));          // highlight term		\n\
		float s = pow(h, 100);			      // specular term		\n\
		float intensity = clamp(a+d+s, 0, 1);						\n\
		pColor = vec4(intensity*vColor, 1);							\n\
	}";

// Interaction

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
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight(w)-y; // invert y for upward-increasing screen space
	picked = NULL;
	if (action == GLFW_PRESS && butn == GLFW_MOUSE_BUTTON_LEFT) {
		if (MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)) {
			picked = &lightSource;
			lightMover.Down(&lightSource, x, y, camera.modelview, camera.persp);
		}
		else {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
	if (action == GLFW_RELEASE)
		camera.MouseUp();
}

void MouseMove(GLFWwindow *w, double x, double y) {
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		y = WindowHeight(w)-y;
		if (picked == &lightSource)
			lightMover.Drag(x, y, camera.modelview, camera.persp);
		if (picked == &camera)
			camera.MouseDrag(x, y, Shift(w));
	}
}

void MouseWheel(GLFWwindow *w, double xoffset, double direction) {
	camera.MouseWheel(direction, Shift(w));
}

// Display

void Display() {
	// clear screen, enable z-buffer
    glClearColor(.5, .5, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// update and send matrices to vertex shader
	SetUniform(program, "modelview", camera.modelview);
    glBindBuffer(GL_ARRAY_BUFFER, facetedShade? vBufferFaceted : vBufferPhong);
	// Phong shaded sphere
    glUseProgram(program);
	VertexAttribPointer(program, "point", 3, sizeof(Vertex), (void *) 0);
	VertexAttribPointer(program, "color", 3, sizeof(Vertex), (void *) sizeof(vec3));
	VertexAttribPointer(program, "normal", 3, sizeof(Vertex), (void *) (2*sizeof(vec3)));
	SetUniform(program, "modelview", camera.modelview);
	SetUniform(program, "persp", camera.persp);
	vec4 xl = camera.modelview*vec4(lightSource, 1);
	vec3 xlight(xl.x/xl.w, xl.y/xl.w, xl.z/xl.w);
	SetUniform(program, "lightPos", xlight);
	glDrawArrays(GL_QUADS, 0, vertices.size());
	UseDrawShader(camera.fullview);
	glDisable(GL_DEPTH_TEST);
	Disk(lightSource, 12, IsVisible(lightSource, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	glFlush();
}

// Vertex Buffer

Vertex SphereVertex(float u, float v, vec3 *faceNormal = NULL) {
	// compute unit-sphere vertex from (u, v)
	// set vertex normal to vertex location or (if non-null) *faceNormal
	float _PI = 3.141592;
	// u ~ longitude: 0 to 2PI
	// v ~ latitude: PI/2 = N. pole, 0 = equator, -PI/2 = S. pole
	float longitude = 2*_PI*u, latitude = _PI/2-_PI*v;
	float cosLatitude = cos(latitude);
	vec3 p(cosLatitude*cos(longitude), sin(latitude), cosLatitude*sin(longitude));
    vec3 c = RGBfromHSV(vec3(v, 1, 1)); // treat v as hue, compute r,g,b
    return Vertex(p, c, faceNormal? *faceNormal : p);
}

void InitVertexBuffer(int res, bool faceted, GLuint *vBuffer) {
	// create vertex array
	int nvertices = 4*res*res, vid = 0;
	vertices.resize(nvertices);
	float d = 1.f/(float)res;
	for (float v = 0; v <= 1-d/2; v += d)
		for (float u = 0; u <= 1-d/2; u += d) {
			vec3 faceN = SphereVertex(u+d/2, v+d/2).point, *n = faceted? &faceN : NULL;
			vertices[vid++] = SphereVertex(u, v, n);
			vertices[vid++] = SphereVertex(u+d, v, n);
			vertices[vid++] = SphereVertex(u+d, v+d, n);
			vertices[vid++] = SphereVertex(u, v+d, n);
		}
    // create and bind GPU vertex buffer, copy vertex array
    glGenBuffers(1, vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

// Application

static void Keyboard(GLFWwindow *w, int c, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (c == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(w, GLFW_TRUE);
		if (c == 'F')
			facetedShade = !facetedShade;
	}
}

void Resize(GLFWwindow *w, int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
    glfwInit();
    GLFWwindow *w = glfwCreateWindow(800, 800, "Phong-Shaded Sphere with Movable Lights", NULL, NULL);
	glfwSetWindowPos(w, 100, 100);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
    InitVertexBuffer(20, true, &vBufferFaceted);
    InitVertexBuffer(20, false, &vBufferPhong);
	printf("Usage:\n\tf:\t\ttoggle faceted\n%s\n", camera.Usage());
	glfwSetCursorPosCallback(w, MouseMove);
    glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetScrollCallback(w, MouseWheel);
    glfwSetKeyCallback(w, Keyboard);
    glfwSetWindowSizeCallback(w, Resize);
    // event loop
    glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	// unbind vertex buffer, free GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBufferPhong);
	glDeleteBuffers(1, &vBufferFaceted);
    glfwDestroyWindow(w);
    glfwTerminate();
}
