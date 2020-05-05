// QuadSlide.cpp - display and manipulate texture maps

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Camera.h"
#include "GLXtras.h"
#include "Misc.h"
#include "VecMat.h"
#include "Audio-Engine/AudioEngine.h"


Camera camera;
AudioEngine audioEngine;

// asset paths
const char* SOUND_FILE      = "res/sound/Medieval Village2.5_Loop1_Layer1_54BPM.wav";
const char* TEXTURE_EARTH   = "res/objects/environment/Earth.tga";

// display
GLuint      textureNames[3] = { 0, 0, 0 };
int			textureUnits[3] = { 0, 1, 2 };
int         winW = 600, winH = 600;

// shaders
GLuint backgroundShader = 0, foregroundShader = 0;
GLuint vBuffer = 0; // GPU vertex buffer ID
vec3 points[] = { vec3(-1, 0, -1), vec3(-1, 0, 1), vec3(1, 0, 1), vec3(1, 0, -1) };
vec2 uvs[] =    { vec2(0, 0), vec2(1,0), vec2(1,1), vec2(0,1) };

// interaction
float objectX = 0, objectY = 0, objectScale = 1;
float xDown = 0, yDown = 0, oldMouseX = 0, oldMouseY = 0;

void DisplayGround() {
    const char* vShader = R"(
        #version 330
		in vec3 point;
		in vec2 uv;
		uniform mat4 view;
        out vec2 vUv;
        void main() {
            vUv = uv;
            gl_Position = view*vec4(point, 1);
        }
    )";
    const char* pShader = R"(
        #version 330
        in vec2 vUv;
        out vec4 pColor;
        uniform sampler2D textureImage;
        uniform sampler2D textureMat;
        void main() {
            pColor = texture(textureImage, vUv);
			pColor.a = texture(textureMat, vUv).r;
        }
    )";
    if (!foregroundShader)
        foregroundShader = LinkProgramViaCode(&vShader, &pShader);
    glUseProgram(foregroundShader);
    glActiveTexture(GL_TEXTURE0 + textureUnits[1]);
    glBindTexture(GL_TEXTURE_2D, textureNames[1]);
    SetUniform(foregroundShader, "textureImage", textureUnits[1]);
    glActiveTexture(GL_TEXTURE0 + textureUnits[2]);
    glBindTexture(GL_TEXTURE_2D, textureNames[2]);
    SetUniform(foregroundShader, "textureMat", textureUnits[2]);
    VertexAttribPointer(foregroundShader, "point", 2, 0, (void*)0);
    VertexAttribPointer(foregroundShader, "uv", 2, 0, (void*)sizeof(points));
    mat4 trans = Translate(objectX, objectY, 0);
    mat4 rot = RotateY(45);
    //rot = rot * RotateX(80);
    //rot = RotateZ(45);
    mat4 scale = Scale(objectScale);
    SetUniform(foregroundShader, "view", trans /* rot */ * scale);
    glDrawArrays(GL_QUADS, 0, 4);
}

// Mouse

int WindowHeight(GLFWwindow* w) {
    int width, height;
    glfwGetWindowSize(w, &width, &height);
    return height;
}

void MouseWheel(GLFWwindow* w, double xoffset, double yoffset) {
    objectScale += .1f * (float)yoffset;
}

void MouseButton(GLFWwindow* w, int butn, int action, int mods) {
    double x, y;
    glfwGetCursorPos(w, &x, &y);
    y = WindowHeight(w) - y; // invert y for upward-increasing screen space
    if (action == GLFW_PRESS) {
        xDown = (float)x;
        yDown = (float)y;
    }
    else {
        oldMouseX = objectX;
        oldMouseY = objectY;
    }
}

void MouseMove(GLFWwindow* w, double x, double y) {
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
        y = WindowHeight(w) - y;
        objectX = oldMouseX + ((float)x - xDown) / winW;
        objectY = oldMouseY + ((float)y - yDown) / winH;
    }
}
// 
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        audioEngine.playSoundFile(SOUND_FILE, true);
}


// Application

void Resize(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, winW = width, winH = height);
}

void InitVertexBuffer() {
    // define GPU buffer, make it active
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    // allocate and fill vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(uvs), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(uvs), uvs);
}

void InitAudioEngine() {
    //  pre-load audio assets
    audioEngine.cacheSoundFile(SOUND_FILE);
}

int main(int ac, char** av) {
    // init audio engine, app window and GL context
    InitAudioEngine();
    glfwInit();
    GLFWwindow* w = glfwCreateWindow(winW, winH, "Fountain Square Game", NULL, NULL);
    glfwSetWindowPos(w, 100, 100);
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // read background, foreground, and mat textures
    glGenTextures(1, textureNames);
    textureNames[0] = LoadTexture(TEXTURE_EARTH, textureUnits[0]);
    textureNames[1] = LoadTexture(TEXTURE_EARTH, textureUnits[1]);
    textureNames[2] = LoadTexture(TEXTURE_EARTH, textureUnits[2]);
    
    InitVertexBuffer();
    // callbacks
    glfwSetMouseButtonCallback(w, MouseButton);
    glfwSetCursorPosCallback(w, MouseMove);
    glfwSetScrollCallback(w, MouseWheel);
    glfwSetWindowSizeCallback(w, Resize);
    glfwSetKeyCallback(w, KeyCallback);
    // event loop
    glfwSwapInterval(1);

    std::cout << "Press Space bar to start sound!";
    
    // GL render loop
    while (!glfwWindowShouldClose(w)) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        DisplayGround();
        //DisplayForeground();
        glFlush();
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(3, textureNames);
    glfwDestroyWindow(w);
    glfwTerminate();
}
