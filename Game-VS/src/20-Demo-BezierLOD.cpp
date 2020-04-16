// BezierLOD.cpp - faceted level of detail patch display

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Text.h"
#include "VecMat.h"
#include "Widgets.h"

// window, shader
GLFWwindow	   *window;
int				winWidth = 800, winHeight = 800;
Camera			camera(winWidth, winHeight, vec3(0,0,0), vec3(0,0,-5));
GLuint			shader = 0;

// Bezier patch to tessellate into triangles
vec3			ctrlPts[4][4];							// 16 control points, indexed [s][t]
vec3			coeffs[4][4];							// alternative 16 poly coeffs in x, y, z, indexed [s][t]

// movable light
vec3			light(-.2f, .4f, .3f);

// UI
int				xCursorOffset = -7, yCursorOffset = -3;
Mover			mover;
void		   *picked = NULL, *hover = NULL;
Magnifier		magnifier(int2(400, 400), int2(300, 300), 10);

// display options
bool			viewMesh = true, useLod = true, outline = true, magnify = true;
Toggler			magnifyTog(&magnify, "magnify", 15, 25, 12),
				viewMeshTog(&viewMesh, "control mesh", 15, 50, 12),
				useLodTog(&useLod, "use LOD", 15, 75, 12),
				outlineTog(&outline, "outline", 15, 100, 12);
int				res = 25;
float			lineWidth = 2, outlineTransition = 1;
time_t			tEvent = clock();

// vertex shader
const char *vShaderCode = "\
	#version 130 core																				\n\
	void main() {																					\n\
		gl_Position = vec4(0);																		\n\
	}";

// tessellation control
const char *tcShaderCode = "\
	#version 400 core																				\n\
	layout (vertices = 4) out;																		\n\
	uniform vec3 ctrlPts[16];																		\n\
	uniform mat4 modelview;																			\n\
	uniform mat4 persp;																				\n\
	uniform mat4 viewportMatrix;																	\n\
	uniform int pixelsPerEdge = 0;	// 0 for do not use LOD											\n\
	uniform int fixedRes = 10;		// if do not use LOD											\n\
	void main()	{																					\n\
		if (gl_InvocationID == 0) {	// only set once												\n\
			// see www.khronos.org/opengl/wiki/Tessellation											\n\
			// outerLevels: left, bottom, right, top edges											\n\
			// innerLevels: left/right, bottom/top divisions										\n\
			if (pixelsPerEdge > 0) { 																\n\
				mat4 m = viewportMatrix*persp*modelview; 											\n\
				// test distance bet pairs of corner ctrl pts										\n\
				// *** should use control mesh outer edge lengths									\n\
				vec3 quad[] = {ctrlPts[12], ctrlPts[0], ctrlPts[3], ctrlPts[15]};					\n\
				vec2 quadS[4]; 	// in pixels														\n\
				for (int i = 0; i < 4; i++) {														\n\
					vec4 h = m*vec4(quad[i], 1);													\n\
					quadS[i] = h.xy/h.w;															\n\
				}																					\n\
				// set outer res																	\n\
				for (int i = 0; i < 4; i++) {														\n\
					float d = distance(quadS[i], quadS[(i+1)%4]);									\n\
					gl_TessLevelOuter[i] = max(2, d/pixelsPerEdge);									\n\
				}																					\n\
				// set inner res as average outer res												\n\
				gl_TessLevelInner[0] = .5*(gl_TessLevelOuter[0]+gl_TessLevelOuter[2]);				\n\
				gl_TessLevelInner[1] = .5*(gl_TessLevelOuter[1]+gl_TessLevelOuter[3]);				\n\
			}																						\n\
			else																					\n\
				for (int i = 0; i < 4; i++)															\n\
					gl_TessLevelInner[i%2] = gl_TessLevelOuter[i] = fixedRes;						\n\
		}																							\n\
	}";

// tessellation evaluation - note comparison coeffs vs control points
const char *teShaderCode = "\
	#version 400 core																				\n\
	layout (quads, equal_spacing, ccw) in;															\n\
	uniform vec3 ctrlPts[16];																		\n\
	uniform vec3 coeffs[16];																		\n\
    uniform mat4 modelview;																			\n\
	uniform mat4 persp;																				\n\
	out vec3 vPosition;																				\n\
	vec3 CurvePoint(float t, vec3 b1, vec3 b2, vec3 b3, vec3 b4) {									\n\
		float t2 = t*t, t3 = t*t2;																	\n\
		return (-t3+3*t2-3*t+1)*b1+(3*t3-6*t2+3*t)*b2+(3*t2-3*t3)*b3+t3*b4;							\n\
		// 22 ops/coord																				\n\
	}																								\n\
	vec3 PatchPoint1(float s, float t) { // form 1 curve across 4 curves via CurvePoint, ctrlPts	\n\
		vec3 spts[4];																				\n\
		for (int i = 0; i < 4; i++)																	\n\
			spts[i] = CurvePoint(s, ctrlPts[4*i], ctrlPts[4*i+1], ctrlPts[4*i+2], ctrlPts[4*i+3]);	\n\
		return CurvePoint(t, spts[0], spts[1], spts[2], spts[3]);									\n\
		// 5 calls to CurvePoint, total 110 ops/coord												\n\
	}																								\n\
	vec3 PatchPoint2(float s, float t) { // form 1 curve across 4 curves via coeffs					\n\
		float s2 = s*s, s3 = s*s2, t2 = t*t, t3 = t*t2;												\n\
		vec3 spts[4];																				\n\
		for (int i = 0; i < 4; i++)																	\n\
			spts[i] = s3*coeffs[4*i]+s2*coeffs[4*i+1]+s*coeffs[4*i+2]+coeffs[4*i+3];				\n\
		return t3*spts[0]+t2*spts[1]+t*spts[2]+spts[3];												\n\
		// 34 ops/coord																				\n\
	}																								\n\
	vec3 PatchPoint3(float s, float t) { // direct evaluation of coeffs								\n\
		float s2 = s*s, s3 = s*s2, t2 = t*t, ta[] = {t*t2, t2, t, 1};								\n\
		vec3 ret;																					\n\
		for (int i = 0; i < 4; i++)																	\n\
			ret += ta[i]*(s3*coeffs[4*i]+s2*coeffs[4*i+1]+s*coeffs[4*i+2]+coeffs[4*i+3]);			\n\
		return ret;																					\n\
		// 32 ops/coord																				\n\
	}																								\n\
	void main() {																					\n\
		vec3 p = PatchPoint3(gl_TessCoord.st.s, gl_TessCoord.st.t);									\n\
		vPosition = (modelview*vec4(p, 1)).xyz;														\n\
		gl_Position = persp*vec4(vPosition, 1);														\n\
	}";

// geometry shader
const char *gShaderCode = "\
	#version 330 core																				\n\
	layout (triangles) in;																			\n\
	layout (triangle_strip, max_vertices = 3) out;													\n\
	in vec3 vPosition[];																			\n\
	out vec3 gPosition;																				\n\
	noperspective out vec3 gEdgeDistance;															\n\
	uniform mat4 viewportMatrix;																	\n\
	vec3 ViewSpacePoint(int i) {																	\n\
		return vec3(viewportMatrix*(gl_in[i].gl_Position/gl_in[i].gl_Position.w));					\n\
	}																								\n\
	void main() {																					\n\
		float ha = 0, hb = 0, hc = 0;																\n\
        // from OpenGL4 Cookbook by Wolff, p 198 (also NVidia 2007 white paper Solid Wireframe)		\n\
		// transform each vertex into viewport space												\n\
		vec3 p0 = ViewSpacePoint(0), p1 = ViewSpacePoint(1), p2 = ViewSpacePoint(2);				\n\
		// find altitudes ha, hb, hc																\n\
		float a = length(p2-p1), b = length(p2-p0), c = length(p1-p0);								\n\
		float alpha = acos((b*b+c*c-a*a)/(2.*b*c));													\n\
		float beta = acos((a*a+c*c-b*b)/(2.*a*c));													\n\
		ha = abs(c*sin(beta));																		\n\
		hb = abs(c*sin(alpha));																		\n\
		hc = abs(b*sin(alpha));																		\n\
		// send triangle vertices and edge distances												\n\
		for (int i = 0; i < 3; i++) {																\n\
    		gEdgeDistance = i==0? vec3(ha, 0, 0) : i==1? vec3(0, hb, 0) : vec3(0, 0, hc);			\n\
			gPosition = vPosition[i];																\n\
			gl_Position = gl_in[i].gl_Position;														\n\
			EmitVertex();																			\n\
		}																							\n\
		EndPrimitive();																				\n\
	}";

// pixel shader
const char *pShaderCode = "\
    #version 130																					\n\
	in vec3 gPosition;																				\n\
	noperspective in vec3 gEdgeDistance;															\n\
	out vec4 fColor;																				\n\
	uniform vec3 lightV;																			\n\
	uniform vec4 shadeColor = vec4(1, 1, 1, 1);														\n\
	uniform vec4 outlineColor = vec4(0, 0, 0, 1);													\n\
	uniform float outlineWidth = 1;																	\n\
	uniform float transition = 1;																	\n\
	uniform int outlineOn = 1;																		\n\
    void main() {																					\n\
		vec3 dx = dFdy(gPosition.xyz);																\n\
		vec3 dy = dFdx(gPosition.xyz);																\n\
		vec3 n = normalize(cross(dx, dy));					// faceted shading						\n\
		float intensity = clamp(abs(dot(n, normalize(lightV))), 0, 1);								\n\
		fColor = shadeColor;																		\n\
		fColor.rgb *= intensity;																	\n\
		if (outlineOn > 0) {																		\n\
			float minDist = min(gEdgeDistance.x, gEdgeDistance.y);									\n\
			minDist = min(minDist, gEdgeDistance.z);												\n\
			float t = smoothstep(outlineWidth-transition, outlineWidth+transition, minDist);		\n\
			// mix edge and surface colors(t=0: edgeColor, t=1: surfaceColor)						\n\
			fColor = mix(outlineColor, fColor, t);													\n\
		}																							\n\
	}";

// Bezier control points

void SetCoeffs() {
	// set coeffs from ctrlPts
	mat4 m(vec4(-1, 3, -3, 1), vec4(3, -6, 3, 0), vec4(-3, 3, 0, 0), vec4(1, 0, 0, 0)), g;
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 16; i++)
			g[i/4][i%4] = ctrlPts[i/4][i%4][k];
		mat4 c = m*g*m;
		for (int i = 0; i < 16; i++)
			coeffs[i/4][i%4][k] = c[i/4][i%4];
	}
}

void DefaultControlPoints() {
    float vals[] = {-.6, -.2, .2, .6};
    for (int i = 0; i < 16; i++)
		ctrlPts[i/4][i%4] = vec3(2*vals[i%4], vals[i/4], 0);
	SetCoeffs();
}

// display

void Display() {
    // background, blending, zbuffer
    glClearColor(1,1,1,1); // .6f, .6f, .6f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
	glUseProgram(shader);
	// send matrices to vertex shader
//	int w, h;
//	GetViewportSize(w, h);
//	mat4 vp = mat4(vec4(w,0,0,w), vec4(0,h,0,h), vec4(0,0,1,0), vec4(0,0,0,1)); // inverse of ScreenMode
	SetUniform(shader, "viewportMatrix", Viewport());
	SetUniform(shader, "modelview", camera.modelview);
	SetUniform(shader, "persp", camera.persp);
	// send LOD params
	SetUniform(shader, "pixelsPerEdge", useLod? 100 : 0);
	SetUniform(shader, "fixedRes", res);
	// send ctrlPts and coeffs (see comparison in tess eval shader)
	SetUniform3v(shader, "ctrlPts", 16, (float *) &ctrlPts[0][0]);
	SetUniform3v(shader, "coeffs", 16, (float *) &coeffs[0][0]);
	// transform light, send to fragment shader
	vec4 xLightV = camera.modelview*vec4(light, 0);
	SetUniform3v(shader, "lightV", 1, (float *) &xLightV.x);
	// send HLE params
	SetUniform(shader, "outlineOn", outline? 1 : 0);
	SetUniform(shader, "outlineWidth", lineWidth);
	SetUniform(shader, "transition", outlineTransition);
	// tessellate and render patch
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);
	// mesh and buttons without z-test
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(camera.fullview);
	// control mesh (disks and dashed lines)
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
	if ((float) (clock()-tEvent)/CLOCKS_PER_SEC < 1)
		Disk(light, 12, hover == (void *) &light? vec3(0,1,1) : IsVisible(light, camera.fullview)? vec3(1,0,0) : vec3(0,0,1));
	// draw controls in 2D pixel space
	UseDrawShader(ScreenMode());
	if (magnify)
		magnifier.Display(int2(10, 490));
	viewMeshTog.Draw();
	useLodTog.Draw();
	outlineTog.Draw();
	magnifyTog.Draw();
	Text(20, 20, vec3(0, 0, 0), 1, "res = %i", res);
    glFlush();
}

// mouse

int WindowHeight(GLFWwindow *w) {
	int width, height;
	glfwGetWindowSize(w, &width, &height); // GetFramebufferSize?
	return height;
}

bool Shift(GLFWwindow *w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		   glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}

vec3 *PickControlPoint(int x, int y) {
	for (int k = 0; k < 16; k++)
		if (MouseOver(x, y, ctrlPts[k/4][k%4], camera.fullview))
			return &ctrlPts[k/4][k%4];
	return NULL;
}

void MouseButton(GLFWwindow *w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = WindowHeight(w)-y; // invert y for upward-increasing screen space
	x += xCursorOffset;
	y += yCursorOffset;
	hover = picked = NULL;
	bool hit = viewMeshTog.UpHit(x, y, action) || useLodTog.UpHit(x, y, action) || outlineTog.UpHit(x, y, action) || magnifyTog.UpHit(x, y, action);
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
		else if (MouseOver(x, y, light, camera.fullview)) {
			mover.Down(&light, x, y, camera.modelview, camera.persp);
			picked = &mover;
		}
		else if (magnifier.Hit(x, y)) {
			picked = &magnifier;
			magnifier.Down(x, y);
		}
		else {
			picked = &camera;
			camera.MouseDown(x, y);
		}
	}
}

void MouseMove(GLFWwindow *w, double x, double y) {
	tEvent = clock();
	x += xCursorOffset;
	y += yCursorOffset;
	y = WindowHeight(w)-y; // invert y for upward-increasing screen space
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		if (picked == &mover) {
			mover.Drag(x, y, camera.modelview, camera.persp);
			SetCoeffs();
		}
		if (picked == &magnifier)
			magnifier.Drag(x, y);
		if (picked == &camera)
			camera.MouseDrag((int) x, (int) y, Shift(w));
    }
    else
 		hover = MouseOver(x, y, light, camera.fullview)? (void *) &light : NULL;
}

void MouseWheel(GLFWwindow *w, double xoffset, double yoffset) {
	camera.MouseWheel((int) yoffset, Shift(w));
}

// application

void Resize(GLFWwindow *window, int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

void Keyboard(GLFWwindow *window, int c, int scancode, int action, int mods) {
	bool shift = mods & GLFW_MOD_SHIFT;
	if (action == GLFW_PRESS)
		switch (c) {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
			case 'B': magnifier.blockSize += shift? -1 : 1; break;
			case 'R': res += shift? -1 : 1; res = res < 1? 1 : res; break;
			case 'T': outlineTransition *= (shift? .8f : 1.2f); break;
			case 'W': lineWidth *= (shift? .8f : 1.2f); break;
		}
}

const char *usage = "\
	b/B: +/- magnifier block size\n\
	t/T: +/- outline transition\n\
	w/W: +/- lineWidth\n\
	r/R: +/- patch res\n\
";

int main(int ac, char **av) {
	// init app window
    if (!glfwInit())
        return 1;
    window = glfwCreateWindow(winWidth, winHeight, "Bezier Patch with LOD", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
	glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    // init OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    PrintGLErrors();
    glViewport(0, 0, winWidth, winHeight);
	// init shader programs
	shader = LinkProgramViaCode(&vShaderCode, &tcShaderCode, &teShaderCode, &gShaderCode, &pShaderCode);
	if (!shader) {
		printf("Can't link shader program\n");
		getchar();
		return 1;
	}
	// init Bezier patch
	DefaultControlPoints();
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
		Display();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
    glfwDestroyWindow(window);
    glfwTerminate();
}
