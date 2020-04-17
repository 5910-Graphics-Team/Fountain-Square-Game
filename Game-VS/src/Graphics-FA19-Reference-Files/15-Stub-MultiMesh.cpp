// MultiMesh.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Mesh.h"
#include "Misc.h"
#include "Widgets.h"

using std::vector;
using std::string;

// display
GLuint		 shader = 0;
int			 winW = 600, winH = 600;
Camera		 camera(winW, winH, vec3(0,0,0), vec3(0,0,-5));

// interaction
int			 xCursorOffset = 2, yCursorOffset = 0;
vec3		 lightSource(-.2f, .4f, .3f);
Arcball		 arcball;
Mover		 mover;
void	    *picked = &camera;

// Mesh Class

class Mesh {
public:
	Mesh();
	int id;
	string filename;
	// vertices and triangles
	vector<vec3> points;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<int3> triangles;
	// object to world space
	mat4 xform;
	// GPU vertex buffer and texture
	GLuint vBufferId, textureId;
	// operations
	void Buffer();
	void Draw();
	bool Read(int id, char *fileame, mat4 *m = NULL);
		// read in object file (with normals, uvs) and texture map, initialize matrix, build vertex buffer
};

// Shaders

const char *vertexShader = "\
	#version 130													\n\
	in vec3 point;													\n\
	in vec3 normal;													\n\
	in vec2 uv;														\n\
	out vec3 vPoint;												\n\
	out vec3 vNormal;												\n\
	out vec2 vUv;													\n\
    uniform mat4 modelview;											\n\
	uniform mat4 persp;												\n\
	void main() {													\n\
		vPoint = (modelview*vec4(point, 1)).xyz;					\n\
		vNormal = (modelview*vec4(normal, 0)).xyz;					\n\
		gl_Position = persp*vec4(vPoint, 1);						\n\
		vUv = uv;													\n\
	}";

const char *pixelShader = "\
    #version 130													\n\
	in vec3 vPoint;													\n\
	in vec3 vNormal;												\n\
	in vec2 vUv;													\n\
	out vec4 pColor;												\n\
	uniform vec3 light;												\n\
	uniform sampler2D textureImage;									\n\
    void main() {													\n\
		vec3 N = normalize(vNormal);       // surface normal		\n\
        vec3 L = normalize(light-vPoint);  // light vector			\n\
        vec3 E = normalize(vPoint);        // eye vector			\n\
        vec3 R = reflect(L, N);            // highlight vector		\n\
        float d = abs(dot(N, L));          // two-sided diffuse		\n\
        float s = abs(dot(R, E));          // two-sided specular	\n\
		float intensity = clamp(d+pow(s, 50), 0, 1);				\n\
		vec3 color = texture(textureImage, vUv).rgb;				\n\
		pColor = vec4(intensity*color, 1);							\n\
	}";

// Scene

const char  *sceneFilename = "C:/Users/Jules/CodeBlocks/Book/Test.scene";
const char  *directory = "C:/Users/jules/SeattleUniversity/Web/Models";
const char  *defaultNames[] = {"HousePlant", "Rose", "Cat"};
vector<Mesh> meshes;

void NewMesh(char *filename, mat4 *m) {
	int nmeshes = meshes.size();
	meshes.resize(nmeshes+1);
	Mesh &mesh = meshes[nmeshes];
	if (!mesh.Read(nmeshes, filename, m))
		meshes.resize(nmeshes);
}

void SaveScene() {
	FILE *file = fopen(sceneFilename, "w");
	if (!file) {
		printf("can't write %s\n", sceneFilename);
		return;
	}
	for (size_t i = 0; i < meshes.size(); i++) {
		Mesh &mesh = meshes[i];
		mat4 &m = mesh.xform;
		fprintf(file, "%s\n", mesh.filename.c_str());
		fprintf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
	}
	fclose(file);
	printf("saved %i meshes\n", meshes.size());
}

bool ReadScene(const char *filename) {
	char meshName[500], buf[500];
	FILE *file = fopen(filename, "r");
	if (!file)
		return false;
	meshes.resize(0);
	while (fgets(meshName, 500, file) != NULL) {
		meshName[strlen(meshName)-1] = 0; // remove carriage-return
		if (fgets(buf, 500, file) == NULL) {
			printf("can't read line\n");
			return false;
		}
		mat4 m;
		int nitems = sscanf(buf, "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f",
			&m[0][0], &m[0][1], &m[0][2], &m[0][3],
			&m[1][0], &m[1][1], &m[1][2], &m[1][3],
			&m[2][0], &m[2][1], &m[2][2], &m[2][3],
			&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
		if (nitems != 16) {
			printf("can't read matrix\n");
			return false;
		}
		NewMesh(meshName, &m);
	}
	fclose(file);
	return true;
}

void ListScene() {
	int nmeshes = meshes.size();
	printf("%i meshes:\n", nmeshes);
	for (int i = 0; i < nmeshes; i++)
		printf("  %i: %s\n", i, meshes[i].filename.c_str());
}

void DeleteMesh() {
	char buf[500];
	int n = -1;
	printf("delete mesh number: ");
	gets(buf);
	if (sscanf(buf, "%i", &n) == 1 && n >= 0 && n < (int) meshes.size()) {
		printf("deleted mesh[%i]\n", n);
		meshes.erase(meshes.begin()+n);
	}
}

void AddMesh() {
	char buf[500], meshName[500];
	printf("name of new mesh: ");
	gets(buf);
	int nMeshes = meshes.size();
	meshes.resize(nMeshes+1);
	sprintf(meshName, "%s/%s", directory, buf);
	meshes[nMeshes].Read(nMeshes, meshName);
}

// Mesh

Mesh::Mesh() { vBufferId = textureId = id = 0; }

void Mesh::Buffer() {
	// create a vertex buffer for the mesh
	<10 lines of code>
}

void Mesh::Draw() {
	// use vertex buffer for this mesh
	glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
	// connect shader inputs to GPU buffer
	<setup vertex feeder (5 lines of code)>
	// set active textureUnit (mesh id), bind to mesh texture, set pixel shader sampler2D
	glActiveTexture(GL_TEXTURE1+id);
	glBindTexture(GL_TEXTURE_2D, textureId);
	SetUniform(shader, "textureImage", (int) textureId);
	// set custom transform
	SetUniform(shader, "modelview", camera.modelview*xform);
	SetUniform(shader, "persp", camera.persp);
	glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
}


bool Mesh::Read(int mid, char *name, mat4 *m) {
	id = mid;
	filename = string(name);
	string objectFilename = filename+".obj";
	string textureFilename = filename+".tga";
	if (!ReadAsciiObj((char *) objectFilename.c_str(), points, triangles, &normals, &uvs)) {
		printf("can't read %s\n", objectFilename.c_str());
		return false;
	}
	Normalize(points, .8f);
	Buffer();
	textureId = LoadTexture((char *) textureFilename.c_str(), id);
	if (m)
		xform = *m;
	arcball.Set(&xform);
	return true;
}

// Display

time_t mouseMoved;

void Display() {
	// clear screen, depth test, blend
    glClearColor(.5f, .5f, .5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
	// update light
	vec4 xlight = camera.modelview*vec4(lightSource, 1);
	glUseProgram(shader);
	SetUniform3(shader, "light", (float *) &xlight.x);
	// display objects
	for (size_t i = 0; i < meshes.size(); i++)
		meshes[i].Draw();
	// lights and frames
	if ((clock()-mouseMoved)/CLOCKS_PER_SEC < 1.5f) {
		glDisable(GL_DEPTH_TEST);
		UseDrawShader(camera.fullview);
		Disk(lightSource, 12, vec3(1,1,0));
		for (size_t i = 0; i < meshes.size(); i++) {
			mat4 &f = meshes[i].xform;
			vec3 base(f[0][3], f[1][3], f[2][3]);
			Disk(base, 10, vec3(1,1,1));
		}
		if (picked == &arcball)
			arcball.Draw(camera);
	}
    glFlush();
}

// Mouse

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
	if (action == GLFW_PRESS && butn == GLFW_MOUSE_BUTTON_LEFT) {
		void *newPicked = NULL;
		if (MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)) {
			newPicked = &lightSource;
			mover.Set(&lightSource, x, y, camera.modelview, camera.persp);
		}
		if (!newPicked)
			// test for arcball center hit
			for (size_t i = 0; i < meshes.size(); i++) {
				Mesh &m = meshes[i];
				vec3 base(m.xform[0][3], m.xform[1][3], m.xform[2][3]);
				if (MouseOver(x, y, base, camera.fullview, xCursorOffset, yCursorOffset)) {
					newPicked = &arcball;
					arcball.Down(x, y, &m.xform, camera);
				}
			}
		if (!newPicked && picked == &arcball)
			// test for within arcball radius
			for (size_t i = 0; i < meshes.size(); i++) {
				mat4 *m = &meshes[i].xform;
				if (arcball.Hit(x, y, *m, camera) && arcball.frame == m) {
					arcball.Down(x, y, m, camera);
					newPicked = &arcball;
				}
			}
		picked = newPicked;
		if (!picked) {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
	if (action == GLFW_RELEASE) {
		if (picked == &camera)
			camera.MouseUp();
		if (picked == &arcball)
			arcball.Up();
	}
}

void MouseMove(GLFWwindow *w, double x, double y) {
	mouseMoved = clock();
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		y = WindowHeight(w)-y;
		if (picked == &lightSource)
			mover.Drag(x, y, camera.modelview, camera.persp);
		if  (picked == &arcball)
			arcball.Drag(x, y, camera);
		if (picked == &camera)
			camera.MouseDrag(x, y, Shift(w));
	}
}

void MouseWheel(GLFWwindow *w, double xoffset, double direction) {
	if (picked == &arcball)
		arcball.Wheel(direction, Shift(w));
	if (picked == &camera)
		camera.MouseWheel(direction, Shift(w));
}

// Application

void Resize(GLFWwindow *w, int width, int height) {
	glViewport(0, 0, winW = width, winH = height);
	camera.Resize(width, height);
}

void Keyboard(GLFWwindow *w, int c, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch(c) {
			case 'R': ReadScene(sceneFilename); break;
			case 'S': SaveScene(); break;
			case 'L': ListScene(); break;
			case 'D': DeleteMesh(); break;
			case 'A': AddMesh(); break;
			default: break;
		}
}

int main(int ac, char **av) {
	// init app window and GL context
    glfwInit();
    GLFWwindow *w = glfwCreateWindow(winW, winH, "MultiMesh", NULL, NULL);
	glfwSetWindowPos(w, 100, 100);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	// build shader program, read scene file
	shader = LinkProgramViaCode(&vertexShader, &pixelShader);
	if (ReadScene(sceneFilename))
		printf("read %i meshes\n", meshes.size());
	else {
		printf("can't read %s, using default scene\n", sceneFilename);
		// read default meshes
		char meshName[100];
		int nMeshes = sizeof(defaultNames)/sizeof(char *);
		meshes.resize(nMeshes);
		for (int i = 0; i < nMeshes; i++) {
			sprintf(meshName, "%s/%s", directory, defaultNames[i]);
			if (!meshes[i].Read(i, meshName))
				meshes.resize(--nMeshes);
		}
	}
	printf("Usage:\n\tR: read scene\n\tS: save scene\n\tL: list scene\n\tD': delete mesh\n\tA: add mesh\n");
	// callbacks
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
	for (size_t i = 0; i < meshes.size(); i++)
		glDeleteBuffers(1, &meshes[i].vBufferId);
    glfwDestroyWindow(w);
    glfwTerminate();
}
