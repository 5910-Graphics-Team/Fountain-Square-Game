// MeshTessTexture.cpp: displacement and texture mapped OBJ mesh
//						anti-alias and magnifier demos

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Mesh.h"
#include "Misc.h"
#include "Text.h"
#include "VecMat.h"
#include "Widgets.h"

// mesh
int		 		nVertices = 0;
vector<vec3>	points, normals;
vector<vec2>	uvs;
vector<int3>	triangles;
vector<TriInfo>	triInfos;									// for interactive selection
float	 		displacementScale = 0;

// interactive view
int				winWidth = 800, winHeight = 800;
int				xCursorOffset = -7, yCursorOffset = -3;
GLFWwindow	   *window;
Camera			camera(winWidth, winHeight, vec3(83.7f,-74.7f,0), vec3(0,0,-12.4f)); // vec3(0,0,0), vec3(0,0,-5));
time_t			event = clock();
Magnifier		magnifier(int2(400, 400), int2(300, 300), 10);

// selection
void	 	   *picked = NULL, *hover = NULL;

// movable light
vec3	 		lightSource(-.2f, .4f, .8f);
Mover	 		lightMover;

// rendering
GLuint	 		shaderId = 0, vBufferId = 0, textureIds[2] = {0, 0};
int				useMipmap = 0, superSample = 0, linear = 0;	// anti-aliasing
int				colorHilite = 0, specular = 0;				// coloring
int				illustrate = 0;								// diagram
bool			magnify = true;

// vertex shader
const char *vShaderCode = "\
	#version 330 core															\n\
	in vec3 point;																\n\
	in vec3 normal;																\n\
	in vec2 uv;																	\n\
	out vec3 vPoint;															\n\
	out vec3 vNormal;															\n\
	out vec2 vUv;																\n\
	void main()	{																\n\
		vPoint = point; 														\n\
		vNormal = normal;														\n\
		vUv = uv;																\n\
	}";

// tessellation evaluation: set vertex position, normal, and st parameters
const char *teShaderCode = "\
	#version 400 core															\n\
	layout (triangles, fractional_odd_spacing, ccw) in;							\n\
	in vec3 vPoint[];															\n\
	in vec3 vNormal[];															\n\
	in vec2 vUv[];																\n\
	out vec3 tePoint;															\n\
	out vec3 teNormal;															\n\
	out vec2 teUv;																\n\
	uniform sampler2D heightField;												\n\
	uniform float displacementScale;											\n\
    uniform mat4 modelview;														\n\
	uniform mat4 persp;															\n\
	void main() {																\n\
		// send uv, point, normal to pixel shader								\n\
		vec2 t;																	\n\
		vec3 p, n;																\n\
		for (int i = 0; i < 3; i++) {											\n\
			float f = gl_TessCoord[i];											\n\
			p += f*vPoint[i];													\n\
			n += f*vNormal[i];													\n\
			t += f*vUv[i];														\n\
		}																		\n\
		normalize(n);															\n\
		float height = displacementScale*texture(heightField, t).z;				\n\
		p += height*n;															\n\
		vec4 hPosition = modelview*vec4(p, 1);									\n\
		gl_Position = persp*hPosition;											\n\
		tePoint = hPosition.xyz;												\n\
		teNormal = (modelview*vec4(n, 0)).xyz;									\n\
		teUv = t;																\n\
	}";

// pixel shader
const char *pShaderCode = "\
    #version 130 core															\n\
	in vec3 tePoint;															\n\
	in vec3 teNormal;															\n\
	in vec2 teUv;																\n\
	out vec4 pColor;															\n\
	uniform sampler2D textureImage;												\n\
	uniform vec3 light;															\n\
	uniform vec4 color = vec4(1, 1, 1, 1);			// default white			\n\
	uniform int useMipmap = 1;													\n\
	uniform int specular = 1;													\n\
	uniform int colorHilite = 1;												\n\
    void main() {																\n\
		// Phong shading with texture											\n\
		vec3 N = normalize(teNormal);											\n\
        vec3 L = normalize(light-tePoint);			// light vector				\n\
        vec3 E = normalize(tePoint);				// eye vertex				\n\
        vec3 R = reflect(L, N);						// highlight vector			\n\
		float dif = abs(dot(N, L));             	 // one-sided diffuse		\n\
		float spec = specular == 1? pow(max(0, dot(E, R)), 50) : 0;				\n\
		float amb = .15, ad = clamp(amb+dif, 0, 1);								\n\
		vec4 texColor = useMipmap == 0?											\n\
			textureLod(textureImage, teUv, 0) :									\n\
			texture(textureImage, teUv);										\n\
		pColor = colorHilite == 1?												\n\
			vec4(ad*texColor.rgb+spec*color.rgb, 1) :							\n\
			vec4((ad+spec)*texColor.rgb, 1);									\n\
	}";

// Display

void Illustrate() {
	vec3 yel(1,1,0);
	for (size_t i = 0; i < triangles.size(); i++) {
		int3 &t = triangles[i];
		int vrt[] = {t.i1, t.i2, t.i3};
		vec3 vrts[] = {points[t.i1], points[t.i2], points[t.i3]};
		vec3 nrms[] = {normals[t.i1], normals[t.i2], normals[t.i3]};
		vec3 n = normalize(cross(vrt[2]-vrt[1], vrt[1]-vrt[0]));
		UseTriangleShader(camera.fullview);
		Triangle(points[t.i1], points[t.i2], points[t.i3], yel, yel, yel, 0, true);
		UseDrawShader(camera.fullview);
		for (int k = 0; k < 3; k++)
			ArrowV(vrts[k], .1f*nrms[k], camera.modelview, camera.persp, dot(nrms[k], n) < 0? vec3(1,0,0) : vec3(0,.7f,0));
	}
}

void Display() {
	glDisable(GL_BLEND);
    // background, blending, zbuffer
    glClearColor(.6f, .6f, .6f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    // diagram object
    if (illustrate) {
		Illustrate();
		glFlush();
		return;
	}
	// use tessellation shader
	glUseProgram(shaderId);
	SetUniform(shaderId, "modelview", camera. modelview);
	// texture map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureIds[0]);
	SetUniform(shaderId, "textureImage", 0);
	superSample? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmap == 1? GL_LINEAR_MIPMAP_LINEAR : (linear == 1? GL_LINEAR : GL_NEAREST));
		// GL_NEAREST or GL_LINEAR no apparent difference
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useMipmap == 1? GL_LINEAR : GL_NEAREST); // needed?
	SetUniform(shaderId, "useMipmap", useMipmap);
	// height field
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureIds[1]);
	SetUniform(shaderId, "heightField", 1);
	SetUniform(shaderId, "displacementScale", displacementScale);
	// update matrices
	SetUniform(shaderId, "modelview", camera.modelview);
	SetUniform(shaderId, "persp", camera.persp);
	// transform light and send to fragment shader
	vec4 hLight = camera.modelview*vec4(lightSource, 1);
	vec3 xlight(hLight.x, hLight.y, hLight.z);
	glUniform3fv(glGetUniformLocation(shaderId, "light"), 1, (float *) &xlight);
	// other shading options
	SetUniform(shaderId, "specular", specular);
	SetUniform(shaderId, "colorHilite", colorHilite);
    // activate vertex buffer and establish shader links
    glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
	int sizeVertex = 2*sizeof(vec3)+sizeof(vec2);
	VertexAttribPointer(shaderId, "point",  3,  sizeVertex, (void *) 0);
	VertexAttribPointer(shaderId, "normal", 3,  sizeVertex, (void *) sizeof(vec3));
	VertexAttribPointer(shaderId, "uv",     2,  sizeVertex, (void *) (2*sizeof(vec3)));
    //							   attrib  num  type      normalize stride       offset
	// tessellation options, display patch
	float res = 10, outerLevels[] = {res, res, res, res}, innerLevels[] = {res, res};
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerLevels);
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerLevels);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawArrays(GL_PATCHES, 0, nVertices);
	// draw light source
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(camera.fullview);
	if ((float) (clock()-event)/CLOCKS_PER_SEC < 1)
		Disk(lightSource, 12, hover == &lightSource? vec3(0,1,1) : IsVisible(lightSource, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	UseDrawShader(ScreenMode());
	// magnifier
	if (magnify)
		magnifier.Display(int2(10, 490));
	// text
	Quad(vec3(0,0,0), vec3(0,100,0), vec3(600,100,0), vec3(600,0,0), true, vec3(1,1,1));
	Text(20, 20, vec3(0, 0, 0), 10, "displacement scale: %4.3f", displacementScale);
	vec3 r = camera.GetRot(), t = camera.GetTran();
	Text(20, 45, vec3(0, 0, 0), 10, "rot:(%3.2f,%3.2f,%3.2f), tran:(%3.2f,%3.2f,%3.2f)", r.x, r.y, r.z, t.x, t.y, t.z);
	Text(20, 70, vec3(0, 0, 0), 10, "%ssuper-sampling, %smip-mapping, %s, specular %s",
		superSample==0? "no " : "", useMipmap==0? "no " : "", linear==0? "nearest" : "linear", specular==0? "off" : "on");
    glFlush();
}

// Input

void ReadObject(const char *filename) {
	class Helper { public:
		char *vptr;
		void SaveVertex(int i) {
			memcpy(vptr, &points[i], sizeof(vec3)); vptr += sizeof(vec3);
			memcpy(vptr, &normals[i], sizeof(vec3)); vptr += sizeof(vec3);
			memcpy(vptr, &uvs[i], sizeof(vec2)); vptr += sizeof(vec2);
		}
	} h;
	// read Alias/Wavefront "obj" formatted mesh file
	if (!ReadAsciiObj((char *) filename, points, triangles, &normals, &uvs)) {
		printf("Failed to read %s\n", filename);
		return;
	}
	int ntriangles = triangles.size();
	nVertices = 3*ntriangles;
	Normalize(points, .8f); // scale/move model to uniform +/-1
    // allocate GPU buffer
	int sizeVertex = 2*sizeof(vec3)+sizeof(vec2);
    glGenBuffers(1, &vBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
	glBufferData(GL_ARRAY_BUFFER, nVertices*sizeVertex, NULL, GL_STATIC_DRAW);
	// write directly to GPU memory (GPU display locked until glUnmapBuffer)
	h.vptr = (char *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (size_t i = 0; i < triangles.size(); i++) {
		int3 &t = triangles[i];
		h.SaveVertex(t.i1);
		h.SaveVertex(t.i2);
		h.SaveVertex(t.i3);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	BuildTriInfos(points, triangles, triInfos);
}

// Mouse

int WindowHeight() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

bool Shift(GLFWwindow *w) {
    return glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight()-y; // invert y for upward-increasing screen space
	event = clock();
	if (action == GLFW_RELEASE)
		camera.MouseUp();
	hover = picked = NULL;
	if (action == GLFW_RELEASE && butn == GLFW_MOUSE_BUTTON_RIGHT) {
		float fp1[3], fp2[3];
		ScreenLine((float) x, (float) y, camera.modelview, camera.persp, fp1, fp2);
		vec3 p1(fp1[0], fp1[1], fp1[2]), p2(fp2[0], fp2[1], fp2[2]);
		float alpha;
		if (IntersectWithLine(p1, p2, triInfos, alpha) >= 0)
			camera.SetRotateCenter(p1+alpha*(p2-p1));
	}
	if (action == GLFW_PRESS && butn == GLFW_MOUSE_BUTTON_LEFT) {
		if (MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset)) {
			picked = &lightSource;
			lightMover.Down(&lightSource, x, y, camera.modelview, camera.persp);
		}
		if (magnifier.Hit(x, y)) {
			picked = &magnifier;
			magnifier.Down(x, y);
		}
		if (picked == NULL) {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
}

void MouseMove(GLFWwindow *w, double x, double y) {
	y = WindowHeight()-y;
	event = clock();
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		if (picked == &lightSource)
			lightMover.Drag(x, y, camera.modelview, camera.persp);
		if (picked == &magnifier)
			magnifier.Drag(x, y);
		if (picked == &camera)
			camera.MouseDrag(x, y, Shift(w));
	}
	else {
		hover = NULL;
		if (MouseOver(x, y, lightSource, camera.fullview, xCursorOffset, yCursorOffset))
			hover = (void *) &lightSource;
	}
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	event = clock();
	camera.MouseWheel(yoffset, Shift(w));
}

// Application

void Resize(GLFWwindow *window, int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

void Keyboard(GLFWwindow *w, int c, int scancode, int action, int mods) {
	bool shift = mods & GLFW_MOD_SHIFT;
	if (action == GLFW_PRESS)
		switch (c) {
			case 'Z': magnify = !magnify; break;
			case 'B': magnifier.blockSize += shift? -1 : 1; break;
			case 'I': illustrate = 1-illustrate; break;
			case 'L': linear = 1-linear; break;
			case 'C': colorHilite = 1-colorHilite; break;
			case 'P': specular = 1-specular; break;
			case 'M': useMipmap = 1-useMipmap; break;
			case 'S': superSample = 1-superSample; break;
			case 'D': displacementScale += shift? -.01f : .01f; break;
		}
}

const char *usage = "\
	Z: toggle magnifier\n\
	b/B: +/- magnifier block size\n\
	I: toggle illustration\n\
	L: toggle linear/nearest\n\
	M: toggle use mipmaps\n\
	S: toggle super-sampling\n\
	C: toggle colorHighlight\n\
	P: toggle specular\n\
	d/D: +/- displacement\n\
";

int main(int argc, char **argv) {
	// init app window and GL context
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4); // anti-alias (esp. silhouette edges)
    window = glfwCreateWindow(winWidth, winHeight, "Mesh, Textured & Tessellated", NULL, NULL);
	glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	// build, use shaderId program
	if (!(shaderId = LinkProgramViaCode(&vShaderCode, NULL, &teShaderCode, NULL, &pShaderCode))) {
		printf("can't link shader program\n");
		getchar();
		return 1;
	}
	ReadObject("C:/Users/jules/SeattleUniversity/Web/Models/Teacup.obj");
	// init texture and height maps
	glGenTextures(2, textureIds);
	textureIds[0] = LoadTexture((char *) "C:/Users/jules/SeattleUniversity/Exe/Chessbd.tga", 0);
	textureIds[1] = LoadTexture((char *) "C:/Users/jules/SeattleUniversity/Exe/GolfBall.tga", 1);
	// callbacks
    glfwSetCursorPosCallback(window, MouseMove);
    glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetScrollCallback(window, MouseWheel);
    glfwSetKeyCallback(window, Keyboard);
    glfwSetWindowSizeCallback(window, Resize);
    printf("Usage:\n%s\n", usage);
    // event loop
    glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Display();
		glfwSwapBuffers(window);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBufferId);
	glDeleteBuffers(2, textureIds);
    glfwDestroyWindow(window);
    glfwTerminate();
}
