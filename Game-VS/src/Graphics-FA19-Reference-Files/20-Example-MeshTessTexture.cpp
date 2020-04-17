// MeshTessTexture.cpp: displacement and texture mapped OBJ mesh
//						anti-alias and magnifier demos

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "CameraArcball.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Mesh.h"
#include "Misc.h"
#include "Text.h"
#include "VecMat.h"
#include "Widgets.h"

// mesh
//const char   *objName = "C:/Users/jules/SeattleUniversity/Web/Models/Lamborghini/Lamborghini_Aventador.obj";
const char	   *objName = "C:/Users/jules/SeattleUniversity/Web/Models/Teacup.obj";
int		 		nVertices = 0;
vector<vec3>	points, normals;
vector<vec2>	uvs;
vector<int3>	triangles;
vector<TriInfo>	triInfos;									// for interactive selection
float	 		displacementScale = 0;

// interactive view
int				winWidth = 800, winHeight = 800;
int				xCursorOffset = -7, yCursorOffset = -3;
CameraAB		camera(winWidth, winHeight, vec3(83.7f,-74.7f,0), vec3(0,0,-6)); // -12.4f)); // vec3(0,0,0), vec3(0,0,-5));
time_t			event = clock();
Magnifier		magnifier(int2(400, 400), int2(300, 300), 10);

// selection
void	 	   *picked = NULL, *hover = NULL;

// movable light
vec3	 		light(-.2f, .4f, .8f);
Mover	 		mover;

// rendering
GLuint	 		shaderId = 0, vBufferId = 0, textureIds[2] = {0, 0};
int				useMipmap = 1, superSample = 1, linear = 1;	// anti-aliasing
int				colorHilite = 0, specular = 0;				// coloring
int				showLines = 0;
bool			magnify = false, showVertexNormals = false, showFaceNormals = false;

// vertex shader
const char *vShaderCode = "\
	#version 330 core																		\n\
	in vec3 point;																			\n\
	in vec3 normal;																			\n\
	in vec2 uv;																				\n\
	out vec3 vPoint;																		\n\
	out vec3 vNormal;																		\n\
	out vec2 vUv;																			\n\
	void main()	{																			\n\
		vPoint = point; 																	\n\
		vNormal = normal;																	\n\
		vUv = uv;																			\n\
	}";

// tessellation evaluation: set vertex position, normal, and st parameters
const char *teShaderCode = "\
	#version 400 core																		\n\
	layout (triangles, fractional_odd_spacing, ccw) in;										\n\
	in vec3 vPoint[];																		\n\
	in vec3 vNormal[];																		\n\
	in vec2 vUv[];																			\n\
	out vec3 tePoint;																		\n\
	out vec3 teNormal;																		\n\
	out vec2 teUv;																			\n\
	uniform sampler2D heightField;															\n\
	uniform float displacementScale;														\n\
    uniform mat4 modelview;																	\n\
	uniform mat4 persp;																		\n\
	void main() {																			\n\
		// send uv, point, normal to pixel shader											\n\
		vec2 t;																				\n\
		vec3 p, n;																			\n\
		for (int i = 0; i < 3; i++) {														\n\
			float f = gl_TessCoord[i];														\n\
			p += f*vPoint[i];																\n\
			n += f*vNormal[i];																\n\
			t += f*vUv[i];																	\n\
		}																					\n\
		normalize(n);																		\n\
		float height = displacementScale*texture(heightField, t).z;							\n\
		p += height*n;																		\n\
		vec4 hPosition = modelview*vec4(p, 1);												\n\
		gl_Position = persp*hPosition;														\n\
		tePoint = hPosition.xyz;															\n\
		teNormal = (modelview*vec4(n, 0)).xyz;												\n\
		teUv = t;																			\n\
	}";

// geometry shader
const char *gShaderCode = "\
	#version 330 core																		\n\
	layout (triangles) in;																	\n\
	layout (triangle_strip, max_vertices = 3) out;											\n\
	in vec3 tePoint[];																		\n\
	in vec3 teNormal[];																		\n\
	in vec2 teUv[];																			\n\
	out vec3 gPoint;																		\n\
	out vec3 gNormal;																		\n\
	out vec2 gUv;																			\n\
	noperspective out vec3 gEdgeDistance;													\n\
	uniform mat4 viewptM;																	\n\
	vec3 ViewPoint(int i) {																	\n\
		return vec3(viewptM*(gl_in[i].gl_Position/gl_in[i].gl_Position.w));					\n\
	}																						\n\
	void main() {																			\n\
		float ha = 0, hb = 0, hc = 0;														\n\
		// transform each vertex into viewport space										\n\
		vec3 p0 = ViewPoint(0), p1 = ViewPoint(1), p2 = ViewPoint(2);						\n\
		// find altitudes ha, hb, hc														\n\
		float a = length(p2-p1), b = length(p2-p0), c = length(p1-p0);						\n\
		float alpha = acos((b*b+c*c-a*a)/(2.*b*c));											\n\
		float beta = acos((a*a+c*c-b*b)/(2.*a*c));											\n\
		ha = abs(c*sin(beta));																\n\
		hb = abs(c*sin(alpha));																\n\
		hc = abs(b*sin(alpha));																\n\
		// send triangle vertices and edge distances										\n\
		vec3 edgeDists[3] = { vec3(ha, 0, 0), vec3(0, hb, 0), vec3(0, 0, hc) };				\n\
		for (int i = 0; i < 3; i++) {														\n\
    		gEdgeDistance = edgeDists[i];													\n\
			gPoint = tePoint[i];															\n\
			gNormal = teNormal[i];															\n\
			gUv = teUv[i];																	\n\
			gl_Position = gl_in[i].gl_Position;												\n\
			EmitVertex();																	\n\
		}																					\n\
		EndPrimitive();																		\n\
	}";

// pixel shader
const char *pShaderCode = "\
    #version 130 core																		\n\
	in vec3 gPoint;																			\n\
	in vec3 gNormal;																		\n\
	in vec2 gUv;																			\n\
	noperspective in vec3 gEdgeDistance;													\n\
	out vec4 pColor;																		\n\
	uniform sampler2D textureImage;															\n\
	uniform vec3 light;																		\n\
	uniform vec4 color = vec4(1, 1, 1, 1);			// default white						\n\
	uniform int useMipmap = 1;																\n\
	uniform int specular = 1;																\n\
	uniform int colorHilite = 1;															\n\
	uniform int showLines = 1;																\n\
	uniform vec4 outlineColor = vec4(0, 0, 0, 1);											\n\
	uniform float outlineWidth = 1;															\n\
	uniform float transition = 1;															\n\
    void main() {																			\n\
		float t = 0;																		\n\
		if (showLines > 0) {																\n\
				float minDist = min(gEdgeDistance.x, gEdgeDistance.y);						\n\
				minDist = min(minDist, gEdgeDistance.z);									\n\
				t = smoothstep(outlineWidth-transition, outlineWidth+transition, minDist);	\n\
			if (showLines == 2) {															\n\
				pColor = mix(outlineColor, vec4(1,1,1,1), t);								\n\
				return;																		\n\
			}																				\n\
		}																					\n\
		// Phong shading with texture														\n\
		vec3 N = normalize(gNormal);														\n\
		vec3 L = normalize(light-gPoint);			// light vector							\n\
		vec3 E = normalize(gPoint);					// eye vertex							\n\
		vec3 R = reflect(L, N);						// highlight vector						\n\
		float dif = abs(dot(N, L));             	// one-sided diffuse					\n\
		float spec = specular == 1? pow(max(0, dot(E, R)), 50) : 0;							\n\
		float amb = .15, ad = clamp(amb+dif, 0, 1);											\n\
		vec4 texColor = useMipmap == 0?														\n\
			textureLod(textureImage, gUv, 0) :												\n\
			texture(textureImage, gUv);														\n\
		pColor = colorHilite == 1?															\n\
			vec4(ad*texColor.rgb+spec*color.rgb, 1) :										\n\
			vec4((ad+spec)*texColor.rgb, 1);												\n\
		if (showLines == 1)																	\n\
				pColor = mix(outlineColor, pColor, t);										\n\
	}";

// Display

void Display(GLFWwindow *w) {
    // background, zbuffer
    float b = showLines == 2? 1 : .6f;
    glClearColor(b, b, b, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
	// shader, view
	glUseProgram(shaderId);
	SetUniform(shaderId, "modelview", camera. modelview);
	// texture map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureIds[0]);
	SetUniform(shaderId, "textureImage", 0);
	superSample? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
	linear == 1? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmap == 1? GL_LINEAR_MIPMAP_LINEAR : (linear == 1? GL_LINEAR : GL_NEAREST));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useMipmap == 1 || linear == 1? GL_LINEAR : GL_NEAREST);
	SetUniform(shaderId, "useMipmap", useMipmap);
	// height field
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureIds[1]);
	SetUniform(shaderId, "heightField", 1);
	SetUniform(shaderId, "displacementScale", displacementScale);
	// update matrices
	SetUniform(shaderId, "modelview", camera.modelview);
	SetUniform(shaderId, "persp", camera.persp);
	SetUniform(shaderId, "viewptM", Viewport());
	// transform light, send to fragment shader
	vec4 hLight = camera.modelview*vec4(light, 1);
	vec3 xlight(hLight.x, hLight.y, hLight.z);
	SetUniform(shaderId, "light", (vec3 *) &hLight);
	// other options
	SetUniform(shaderId, "showLines", showLines);
	SetUniform(shaderId, "specular", specular);
	SetUniform(shaderId, "colorHilite", colorHilite);
    // activate vertex buffer, establish shader links
    glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
	int sizeVertex = 2*sizeof(vec3)+sizeof(vec2);
	VertexAttribPointer(shaderId, "point",  3,  sizeVertex, (void *) 0);
	VertexAttribPointer(shaderId, "normal", 3,  sizeVertex, (void *) sizeof(vec3));
	VertexAttribPointer(shaderId, "uv",     2,  sizeVertex, (void *) (2*sizeof(vec3)));
    //							   attrib  num  type      normalize stride       offset
	// tessellation options, display patch
	float res = showLines == 0? 10 : 1, outerLevels[] = {res, res, res, res}, innerLevels[] = {res, res};
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerLevels);
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerLevels);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawArrays(GL_PATCHES, 0, nVertices);
	// normals
	if (showVertexNormals)
		for (size_t i = 0; i < points.size(); i++)
			ArrowV(points[i], .1f*normals[i], camera.modelview, camera.persp, vec3(0,.7f,0), 2, 8);
	if (showFaceNormals)
		for (size_t i = 0; i < triangles.size(); i++) {
			int3 &t = triangles[i];
			vec3 vrts[] = {points[t.i1], points[t.i2], points[t.i3]};
			vec3 cen = (vrts[0]+vrts[1]+vrts[2])/3;
			vec3 n = normalize(cross(vrts[1]-vrts[0], vrts[2]-vrts[1]));
			for (int k = 0; k < 3; k++)
				ArrowV(cen+.8f*(vrts[k]-cen), .1f*n, camera.modelview, camera.persp, vec3(.7f,0,0), 2, 8);
		}
	// light source, arcball
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(camera.fullview);
	if ((float) (clock()-event)/CLOCKS_PER_SEC < 1) {
		camera.arcball.Draw(camera.fullview);
		Disk(light, 12, hover == &light? vec3(0,1,1) : IsVisible(light, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	}
	UseDrawShader(ScreenMode());
	// magnifier
	if (magnify)
		magnifier.Display(int2(10, 490));
	// text
	Quad(vec3(10,10,0), vec3(10,100,0), vec3(560,100,0), vec3(560,10,0), true, vec3(1,1,1));
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
	printf("%i vertices, %i triangles\n", points.size(), triangles.size());
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
		if (MouseOver(x, y, light, camera.fullview, xCursorOffset, yCursorOffset)) {
			picked = &light;
			mover.Down(&light, x, y, camera.modelview, camera.persp);
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
	y = WindowHeight(w)-y;
	event = clock();
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		if (picked == &light)
			mover.Drag(x, y, camera.modelview, camera.persp);
		if (picked == &magnifier)
			magnifier.Drag(x, y);
		if (picked == &camera)
			camera.MouseDrag(x, y, Shift(w));
	}
	else {
		hover = NULL;
		if (MouseOver(x, y, light, camera.fullview, xCursorOffset, yCursorOffset))
			hover = (void *) &light;
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
			case 'L': showLines = (showLines+1)%3;				break;
			case 'V': showVertexNormals = !showVertexNormals;	break;
			case 'F': showFaceNormals = !showFaceNormals;		break;
			case 'Z': magnify = !magnify;						break;
			case 'B': magnifier.blockSize += shift? -1 : 1;		break;
			case 'N': linear = 1-linear;						break;
			case 'C': colorHilite = 1-colorHilite;				break;
			case 'P': specular = 1-specular;					break;
			case 'M': useMipmap = 1-useMipmap;					break;
			case 'S': superSample = 1-superSample;				break;
			case 'D': displacementScale += shift? -.01f : .01f;	break;
		}
}

const char *usage = "\
	L: lines-off/lines-on/HLE\n\
	V: toggle vertex normals\n\
	F: toggle face normals\n\
	N: toggle linear/nearest\n\
	M: toggle use mipmaps\n\
	S: toggle super-sampling\n\
	C: toggle colorHighlight\n\
	P: toggle specular\n\
	Z: toggle magnifier\n\
	d/D: +/- displacement\n\
	b/B: +/- magnifier block size\n\
";

int main(int argc, char **argv) {
	// init app window and GL context
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4); // anti-alias (esp. silhouette edges)
    GLFWwindow *w = glfwCreateWindow(winWidth, winHeight, "Mesh, Textured & Tessellated", NULL, NULL);
	glfwSetWindowPos(w, 100, 100);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	// build, use shaderId program
	if (!(shaderId = LinkProgramViaCode(&vShaderCode, NULL, &teShaderCode, &gShaderCode, &pShaderCode))) {
		printf("can't link shader program\n");
		getchar();
		return 1;
	}
	ReadObject(objName);
	// init texture and height maps
	glGenTextures(2, textureIds);
	textureIds[0] = LoadTexture((char *) "C:/Users/jules/SeattleUniversity/Exe/Chessbd.tga", 0);
	textureIds[1] = LoadTexture((char *) "C:/Users/jules/SeattleUniversity/Exe/GolfBall.tga", 1);
	// callbacks
    glfwSetCursorPosCallback(w, MouseMove);
    glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetScrollCallback(w, MouseWheel);
    glfwSetKeyCallback(w, Keyboard);
    glfwSetWindowSizeCallback(w, Resize);
    printf("Usage:\n%s\n", usage);
    // event loop
    glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		glfwPollEvents();
		Display(w);
		glfwSwapBuffers(w);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBufferId);
	glDeleteBuffers(2, textureIds);
    glfwDestroyWindow(w);
    glfwTerminate();
}
