#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Model.h"
#include "LOGL_Camera.h"
#include "Audio-Engine/AudioEngine.h"
#include "Misc.h"
#include "GLXtras.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// audio engine
AudioEngine audioEngine;

static glm::mat4 getProjection() {
    return glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
}

static glm::mat4 getModel(glm::vec3* trans, glm::vec3* scale) {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, *trans); // translate it down so it's at the center of the scene
    m = glm::scale(m, *scale);	  // scale object
    return m;
}


const char* vertexShader = R"(
    #version 130
    in vec3 point;
    in vec3 normal;
    in vec2 uv;
    out vec3 vPoint;
    out vec3 vNormal;
    out vec2 vUv;
    uniform mat4 modelview;
    uniform mat4 persp;
    void main() {
        vPoint = (modelview*vec4(point, 1)).xyz;
        vNormal = (modelview*vec4(normal, 0)).xyz;
        gl_Position = persp*vec4(vPoint, 1);
        vUv = uv;
    }
)";

const char* pixelShader = R"(
    #version 130
    in vec3 vPoint;
    in vec3 vNormal;
    in vec2 vUv;
    out vec4 pColor;
    uniform vec3 light;
    uniform sampler2D textureImage;
    void main() {
        vec3 N = normalize(vNormal);       // surface normal
        vec3 L = normalize(light-vPoint);  // light vector
        vec3 E = normalize(vPoint);        // eye vector
        vec3 R = reflect(L, N);            // highlight vector
        float d = abs(dot(N, L));          // two-sided diffuse
        float s = abs(dot(R, E));          // two-sided specular
        float intensity = clamp(d+pow(s, 50), 0, 1);
        vec3 color = texture(textureImage, vUv).rgb;
        pColor = vec4(intensity*color, 1);
    }
)";

GLuint      shaderProgram = 0;
int			gTextureUnit = 0;
class Ground {
public:
    Ground() { };
    // GPU vertex buffer and texture
    GLuint vBufferId = 0, textureId = 0, textureUnit = 0;
    size_t sizePts = 0, sizeNrms = 0, sizeUvs = 0;
    // operations
    void Buffer() {
        float size = 5, ht = -.55f;
        glm::vec3 points[] = { glm::vec3(-size, ht, -size), glm::vec3(size, ht, -size), glm::vec3(size, ht, size), glm::vec3(-size, ht, size) };
        glm::vec3 normals[] = { glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1) };
        glm::vec2 uvs[] = { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1) };
        sizePts = sizeof(points);
        sizeNrms = sizeof(normals);
        sizeUvs = sizeof(uvs);
        // allocate and download vertices
        glGenBuffers(1, &vBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizePts + sizeNrms + sizeUvs, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizePts, points);
        glBufferSubData(GL_ARRAY_BUFFER, sizePts, sizeNrms, normals);
        glBufferSubData(GL_ARRAY_BUFFER, sizePts + sizeNrms, sizeUvs, uvs);
        textureUnit = gTextureUnit++;
        textureId = LoadTexture("res/objects/environment/ground_1024_Q3.tga", textureUnit);
        
    }
    void Draw() {
        glUseProgram(shaderProgram);

        glBindBuffer(GL_ARRAY_BUFFER, vBufferId);
        // render four vertices as a quad
        VertexAttribPointer(shaderProgram, "point", 3, 0, (void*)0);
        VertexAttribPointer(shaderProgram, "normal", 3, 0, (void*)sizePts);
        VertexAttribPointer(shaderProgram, "uv", 2, 0, (void*)(sizePts + sizeNrms));
        glActiveTexture(GL_TEXTURE1 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
        SetUniform(shaderProgram, "textureImage", (int)textureId);
        // set default trans/scale
        glm::vec3 trans(10.0f, -20.0f, 5.0f), scale(1.0f, 1.0f, 1.0f);
        SetUniform(shaderProgram, "modelview",  getModel(&trans, &scale) * camera.GetViewMatrix());
        SetUniform(shaderProgram, "persp", getProjection());
        glDrawArrays(GL_QUADS, 0, 4);
    }
};

// Container for a Model and its default translaftion and scale values. 
struct GameObject {
    Model model;
    glm::vec3 trans, scale;

    GameObject(const char* filepath, glm::vec3 defaultTrans, glm::vec3 defaultScale) : model(filepath), trans(defaultTrans), scale(defaultScale) {
        std::cout << "Created model for " << filepath << "\n";
        
    }
   void setTranslation(const glm::vec3 newTrans) {
        trans.x = newTrans.x;
        trans.y = newTrans.y;
        trans.z = newTrans.z;
    }
    void setScale(const glm::vec3 newScale) {
        scale.x = newScale.x;
        scale.y = newScale.y;
        scale.z = newScale.z;
    }
};



// Simple 'singleton' renderer (non-instantializable) 
// Performs the OpenGL calls to renter a GameObject with a provided shader.

static void renderGameObject(GameObject* gameObject, Shader* shader) {
    // enable shader before setting uniforms
    shader->use();

    // view/projection transformations
    glm::mat4 projection = getProjection();//glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);

    // render the loaded model 
    // TODO rename local var 'm' to more descriptive name (used to be 'model')

    shader->setMat4("model", getModel(&gameObject->trans, &gameObject->scale));
    gameObject->model.Draw(*shader);
}

Ground ground;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fountain Game", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW what to do with mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("res/LearnOpenGL/shaders/1.model_loading.vs", "res/LearnOpenGL/shaders/1.model_loading.fs");
    shaderProgram = LinkProgramViaCode(&vertexShader, &pixelShader);
    ground.Buffer();

    std::vector<GameObject> gameObjects;
   
    
    // create and load models with default trans/scale (TODO move object 'default' params to config file?)
    GameObject fountain("res/objects/fountains/fountainOBJ/fountain.obj", glm::vec3(-30.0f, -12.5f, -70.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    gameObjects.push_back(fountain);

    GameObject backpack("res/LearnOpenGL/objects/backpack/backpack.obj",  glm::vec3(0.5f, -1.2f, 0.0f),      glm::vec3(0.5f, 0.5f, 0.5f));
    gameObjects.push_back(backpack);



    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       // TODO update positions of game objects before next for-loop
        ground.Draw();
        // render objects
        for (int i = 0; i < gameObjects.size(); i++) {
            renderGameObject(&gameObjects[i], &ourShader);
        }
        


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

