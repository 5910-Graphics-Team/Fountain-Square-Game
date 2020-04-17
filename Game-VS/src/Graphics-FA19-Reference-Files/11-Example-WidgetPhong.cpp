// WidgetPhong.cpp: Phong-shade a cube with movable lights

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Text.h"
#include "VecMat.h"
#include "Widgets.h"

struct Vertex {
	vec3 point, color, normal;
	Vertex() { }
	Vertex(vec3 p, vec3 c, vec3 n) : point(p), color(c), normal(n) { }
};

std::vector<Vertex> vertices;

// app parameters
int				winWidth = 800, winHeight = 800;
GLuint			vBuffers[] = {0, 0, 0, 0}, program = 0;
Camera			camera((float) winWidth/winHeight, vec3(0,0,0), vec3(0,0,-10));
float			tA = 1.8f, tB = tA/sqrt(2.f); // factors for regular tetrahedron
vec3			lights[] = {vec3(-tA,0,-tB), vec3(tA,0,-tB), vec3(0,-tA,tB), vec3(0,tA,tB)};
int				nlights = sizeof(lights)/sizeof(vec3);
int				diffuseOnly = 1, localLights = 1, facetedShade = 1, constantShade = 1;
int				xCursorOffset = -7, yCursorOffset = -3; // for GLFW

// Shaders

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
	uniform vec3 lights[4];											\n\
	uniform int diffuseOnly = 1;									\n\
	uniform int localLights = 1;									\n\
	float PhongIntensity(vec3 pos, vec3 nrm) {						\n\
        float a = .15f, d = 0, s = 0;								\n\
		vec3 N = normalize(nrm);			   // surface normal	\n\
        vec3 E = normalize(pos);			   // eye vertex		\n\
		for (int i = 0; i < 4; i++) {								\n\
			vec3 L = normalize(localLights == 1?					\n\
						lights[i]-pos :	       // local light		\n\
						lights[i]);            // infinite light 	\n\
			d += max(0, dot(N, L));		       // one-sided diffuse	\n\
			if (diffuseOnly == 0) {									\n\
				vec3 R = reflect(L, N);        // highlight vector	\n\
				float h = max(0, dot(R, E));   // highlight term	\n\
				s += pow(h, 50);			   // specular term		\n\
			}														\n\
		}															\n\
		return clamp(a+d+s, 0, 1);									\n\
	}																\n\
    void main() {													\n\
		float intensity = PhongIntensity(vPoint, vNormal);			\n\
		pColor = vec4(intensity*vColor, 1);							\n\
	}";

// Interaction

Mover	lightMover;
void   *picked = NULL;

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
		float minDSq = 150;			// seems ok for large mouse cursor
		for (int i = 0; i < nlights; i++) {
			float dsq = ScreenDistSq(x+xCursorOffset, y+yCursorOffset, lights[i], camera.fullview);
			if (dsq < minDSq) {
				minDSq = dsq;
				picked = &lightMover;
				lightMover.Down(&lights[i], x, y, camera.modelview, camera.persp);
			}
		}
		if (picked == NULL) {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
	if (action == GLFW_RELEASE)
		camera.MouseUp();
}

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	}

void MouseMove(GLFWwindow *w, double x, double y) {
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		y = WindowHeight(w)-y; // invert y for upward-increasing screen space
		if (picked == &lightMover)
			lightMover.Drag(x, y, camera.modelview, camera.persp);
		else
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
	// activate shader, set vertex fetch
	int mode = (facetedShade? 0: 2) + (constantShade? 0 : 1);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffers[mode]);
    glUseProgram(program);
	VertexAttribPointer(program, "point", 3,  sizeof(Vertex), (void *) 0);
	VertexAttribPointer(program, "color", 3, sizeof(Vertex), (void *) sizeof(vec3));
	VertexAttribPointer(program, "normal", 3, sizeof(Vertex), (void *) (2*sizeof(vec3)));
	// transform lights
	vec3 xlights[4];
	for (int i = 0; i < nlights; i++) {
		vec4 l(lights[i], localLights == 1? 1.f : 0.f), xl(camera.modelview*l);
		xlights[i] = vec3(xl.x, xl.y, xl.z);
	}
	// set uniforms, draw cube
	SetUniform(program, "modelview", camera.modelview);
	SetUniform(program, "persp", camera.persp);
	SetUniform3v(program, "lights", nlights, &xlights[0].x);
	SetUniform(program, "diffuseOnly", diffuseOnly);
	SetUniform(program, "localLights", localLights);
	glDrawArrays(GL_QUADS, 0, vertices.size());
	// draw light sources
	UseDrawShader(camera.fullview);
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < nlights; i++) {
		vec3 l = lights[i];
		bool visible = IsVisible(l, camera.fullview);
		bool incube = l.x > -1 && l.x < 1 && l.y > -1 && l.y < 1 && l.z > -1 && l.z < 1;
		Disk(l, 12.f, incube? vec3(0,0,1) : vec3(1,0,0), visible? 1 : .5f);
	}
	Text(20, 20, vec3(), .7f, "d: ");
	Text(40, 20, vec3(), .7f, diffuseOnly ? "diffuse" : "diffuse & specular");
	Text(20, 45, vec3(), .7f, "l: "); Text(40, 45, vec3(), .7f, localLights? "local lights" : "infinite lights");
	Text(20, 70, vec3(), .7f, "f: "); Text(40, 70, vec3(), .7f, facetedShade? "faceted" : "smooth");
	Text(20, 95, vec3(), .7f, "c: "); Text(40, 95, vec3(), .7f, constantShade? "constant" : "blend");
	glFlush();
}

// Vertex Buffer

enum Mode {FacetedFlat = 0, FacetedBlend, SmoothFlat, SmoothBlend};
	// faceted: 6 normals, smooth-shaded: 8 normals
	// flat-shaded: 6 colors, blended: 8 colors

// cube
float	l = -1, r = 1, b = -1, t = 1, n = -1, f = 1;
float	points[][3] = {{l, b, n}, {l, b, f}, {l, t, n}, {l, t, f}, {r, b, n}, {r, b, f}, {r, t, n}, {r, t, f}}; // 8 points
float	colors[][3] = {{0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {0, 0, 0}, {1, 1, 1}}; // 8 colors
int		faces[][4] = {{1, 3, 2, 0}, {6, 7, 5, 4}, {4, 5, 1, 0}, {3, 7, 6, 2}, {2, 6, 4, 0}, {5, 7, 3, 1}};		// 6 faces: l,r,b,t,n,f

void InitVertexBuffer(Mode m) {
	bool flat = m == FacetedFlat || m == SmoothFlat, faceted = m == FacetedFlat || m == FacetedBlend;
	// create vertex array
	int nvertices = sizeof(faces)/sizeof(int), nfaces = nvertices/4;
	vertices.resize(nvertices);
	for (int f = 0; f < nfaces; f++) {
		int *face = faces[f];
		vec3 p1(points[face[0]]), p2(points[face[1]]), p3(points[face[2]]);
		vec3 faceNormal = normalize(cross(p2-p1, p3-p2));		// use n for all 4 face vertices
		for (int k = 0; k < 4; k++) {
			int vid = face[k];
			vec3 p(points[vid]), c(colors[flat ? f : vid]);
			vertices[4*f+k] = Vertex(p, c, faceted? faceNormal : normalize(p));
		}
	}
    // create and bind GPU vertex buffer, copy vertex data
    glGenBuffers(1, &vBuffers[m]);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffers[m]);
	glBufferData(GL_ARRAY_BUFFER, nvertices*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

// Application

void Resize(GLFWwindow *w, int width, int height) {
	glViewport(0, 0, width, height);
}

static void Keyboard(GLFWwindow *window, int c, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (c) {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
			case 'L': localLights = 1-localLights; break;
			case 'F': facetedShade = 1-facetedShade; break;
			case 'C': constantShade = 1-constantShade; break;
			case 'D': diffuseOnly = 1-diffuseOnly; break;
			default: break;
		}
}


const char *usage = "\
	  mouse-drag: rotate\n\
	     with shift: translate xy\n\
	     wheel: translate z\n\
	  L: toggle local lights\n\
	  F: toggle faceted\n\
	  C: toggle constant shade\n\
	  D: toggle diffuse only\n\
";

int main(int argc, char **argv) {
    glfwInit();
    GLFWwindow *w = glfwCreateWindow(winWidth, winHeight, "Phong-Shaded Cube with Movable Lights", NULL, NULL);
	glfwSetWindowPos(w, 100, 100);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	for (int i = 0; i < 4; i++)
		InitVertexBuffer((Mode) i);
	glfwSetCursorPosCallback(w, MouseMove);
    glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetScrollCallback(w, MouseWheel);
    glfwSetKeyCallback(w, Keyboard);
    glfwSetWindowSizeCallback(w, Resize);
    // event loop
    glfwSwapInterval(1);
    printf("Usage:\n%s\n", usage);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	// unbind vertex buffer, free GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (int i = 0; i < 4; i++)
		glDeleteBuffers(1, &vBuffers[i]);
    glfwDestroyWindow(w);
    glfwTerminate();
}
