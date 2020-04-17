// ShadeMeshSTL.cpp: Phong shade .stl mesh

#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include "GLSL.h"
#include "MeshIO.h"

// Application Data

char			   *filename = "C:***";
vec3				lightSource(1, 1, 0);	// for Phong shading
GLuint				vBuffer = 0;			// GPU vertex buffer ID
GLuint				program = 0;			// GLSL program ID
vector<VertexSTL>	vertices;

// Shaders

char *vertexShader = "\
	#version 130														\n\
	in vec3 point;														\n\
	in vec3 normal;														\n\
	out vec3 vPoint;													\n\
	out vec3 vNormal;													\n\
    uniform mat4 view;													\n\
	uniform mat4 persp;													\n\
	void main() {														\n\
		vec4 hPosition = view*vec4(point, 1);							\n\
		vPoint = hPosition.xyz;											\n\
		gl_Position = persp*hPosition;									\n\
		vNormal = (view*vec4(normal, 0)).xyz;							\n\
	}";

char *pixelShader = "\
    #version 130														\n\
	in vec3 vPoint;														\n\
	in vec3 vNormal;													\n\
	out vec4 pColor;													\n\
	uniform vec3 light;													\n\
	uniform vec4 color = vec4(.7, .7, 0, 1);							\n\
    void main() {														\n\
		vec3 N = normalize(vNormal);       // surface normal			\n\
        vec3 L = normalize(light-vPoint);  // light vector				\n\
        vec3 E = normalize(vPoint);        // eye vertex				\n\
        vec3 R = reflect(L, N);            // highlight vector			\n\
        float d = abs(dot(N, L));          // two-sided diffuse			\n\
        float s = abs(dot(R, E));          // two-sided specular		\n\
		float intensity = clamp(d+pow(s, 50), 0, 1);					\n\
		pColor = vec4(intensity*color.rgb, color.a);					\n\
	}";

// Interactive Rotation

vec2 mouseDown;				// for each mouse down, need start point
vec2 rotOld, rotNew;	    // previous, current rotations

void MouseButton(int butn, int state, int x, int y) {
	if (state == GLUT_DOWN)
		mouseDown = vec2((float) x, (float) y);
	if (state == GLUT_UP)
		rotOld = rotNew;
}

void MouseDrag(int x, int y) {
	vec2 mouse((float) x, (float) y);
	rotNew = rotOld+.3f*(mouse-mouseDown);
	glutPostRedisplay();
}

// Application

void Display() {
	static float fov = 15, nearPlane = -.001f, farPlane = -500;
	static float aspect = (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT);
    glUseProgram(program);
	// update and send matrices to vertex shader
	mat4 view = Translate(0, 0, -5)*RotateY(rotNew.x)*RotateX(rotNew.y);
	mat4 persp = Perspective(fov, aspect, nearPlane, farPlane);
	GLSL::SetUniform(program, "view", view);
	GLSL::SetUniform(program, "persp", persp);
	// transform light and send to fragment shader
	vec4 hLight = view*vec4(lightSource, 1);
	GLSL::SetUniform(program, "light", vec3(hLight.x, hLight.y, hLight.z));
	// clear screen to grey, enable transparency, use z-buffer
    glClearColor(.5f, .5f, .5f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_BUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    // link shader inputs with vertex buffer
	*** setup vertex feeder
	// draw triangles and finish
 	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glFlush();
}

void InitVertexBuffer() {
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	*** send vertex data to GPU
}

void Close() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBuffer);
}

void main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Mesh Example");
    glewInit();
	program = GLSL::LinkProgramViaCode(vertexShader, pixelShader);
	if (!ReadSTL(filename, vertices)) {
		printf("Failed to read obj file\n");
		getchar();
		return;
	}
	printf("%i triangles\n", vertices.size()/3);
	Normalize(vertices);
    InitVertexBuffer();
    glutDisplayFunc(Display);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseDrag);
    glutCloseFunc(Close);
    glutMainLoop();
}
