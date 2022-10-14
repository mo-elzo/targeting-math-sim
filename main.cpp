/*
 * Garbage code written by Xaavian for SAE Advanced Targeting Pod Math Testing
 * Plagiarized from COMP 371 ;)
 *
 * WASD moves object
 * Arrow Keys move Camera
 * Right click mouse and move to change camera Yaw
 * Middle click mouse to change camera Pitch
 * N and M keys change camera Roll
 *
 * Space bar gives you Location and Angle data for everything
 * Z will advance sim by <timeSimInterval> seconds
 *
 * Other buttons do funny things (idk what)
 *
 * Feed this output into OpenCV and test Targeting pod math accuracy
 *
 * Dont Ask me how anything else works cause I dont know
 * DO NOT MODIFY SHADERS
 *
 * Compile with:
 *      g++ shader.h main.cpp $(pkg-config glfw3 glew --cflags --libs) -lglut
 * on linux
 * Should just work on windows
 *
 */

#include <iostream>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "shader.h"
using namespace std;

glm::mat4 view;
glm::mat4 projection;

glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float mockGroundSpeedX = 5.0f; // m/s
float mockGroundSpeedZ = 0.0f; // m/s
float timeSimInterval = 0.5f; // s

float cameraSpeed = 10.0f;
float sensitivity = 0.03f;
float gridUnitLength = 0.5f; // LETS ASSUME 1.0f IS EQUAL TO ONE METER :)
double deltaTime = 0.0f;
double lastFrame = 0.0f;
float roll = 0.0f;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;
double mousePosX, mousePosY;
double lastMousePosX, lastMousePosY;
double mouseXChange;
double mouseYChange;
GLenum renderModeOne = GL_TRIANGLES;

float translateOneX = 0.0f;
float translateOneZ = 0.0f;
float scaleOne = 1.0f;
float rotateOne = 0.0f;

bool spacePressed = false;
bool pPressed = false;

GLuint createCubeVao()
{
    float vertices[] =
            {
                        //Back
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
                        // Front
                    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                        //Left
                    -0.5f,  0.5f,  0.5f,  0.0f, 0.9f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 0.9f, 0.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.9f, 0.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.9f, 0.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.9f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 0.9f, 0.0f,
                        //Right
                    0.5f,  0.5f,  0.5f,  0.0f, 0.9f, 0.0f,
                    0.5f,  0.5f, -0.5f,  0.0f, 0.9f, 0.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, 0.9f, 0.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, 0.9f, 0.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, 0.9f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f, 0.9f, 0.0f,
                        //Bottom
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.8f, 0.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, 0.8f, 0.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, 0.8f, 0.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, 0.8f, 0.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.8f, 0.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.8f, 0.0f,
                        //Top
                    -0.5f,  0.5f, -0.5f,  0.0f, 0.8f, 0.0f,
                    0.5f,  0.5f, -0.5f,  0.0f, 0.8f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f, 0.8f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f, 0.8f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 0.8f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 0.8f, 0.0f,
            };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer
            (
                    0,                   // attribute 0 matches aPos in Vertex Shader
                    3,                   // size
                    GL_FLOAT,            // type
                    GL_FALSE,            // normalized?
                    6 * sizeof(float), // stride - each vertex contain 2 vec3 (position, color)
                    (void*)0             // array buffer offset
            );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer
            (
                    1,                            // attribute 1 matches aColor in Vertex Shader
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    6 * sizeof(float),
                    (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
            );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // VAO already stored the state we just defined, safe to unbind buffer
    glBindVertexArray(0); // Unbind to not modify the VAO

    return vertexArrayObject;
}

GLuint createAxisVao() {
    {
        float vertices[] =
                {
                        -1.0f * gridUnitLength, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                        1.0f * gridUnitLength, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, -1.0f * gridUnitLength, 0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f,  1.0f * gridUnitLength, 0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, -1.0f * gridUnitLength, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f,  1.0f * gridUnitLength, 1.0f, 0.0f, 0.0f,
                };

        // Create a vertex array
        GLuint vertexArrayObject;
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);


        // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
        GLuint vertexBufferObject;
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer
                (
                        0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        6 * sizeof(float),
                        (void*)0
                );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer
                (
                        1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        6 * sizeof(float),
                        (void*)sizeof(glm::vec3)
                );
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // VAO already stored the state we just defined, safe to unbind buffer
        glBindVertexArray(0); // Unbind to not modify the VAO

        return vertexArrayObject;
    }
}

GLuint createGridVao()
{
    std::vector<float> verticesArr;

    verticesArr.push_back(0.0f);
    verticesArr.push_back(0.0f);
    verticesArr.push_back(25.0f);

    verticesArr.push_back(0.8f);
    verticesArr.push_back(0.3f);
    verticesArr.push_back(0.0f);

    verticesArr.push_back(0.0f);
    verticesArr.push_back(0.0f);
    verticesArr.push_back(-25.0f);

    verticesArr.push_back(0.8f);
    verticesArr.push_back(0.3f);
    verticesArr.push_back(0.0f);

    verticesArr.push_back(25.0f);
    verticesArr.push_back(0.0f);
    verticesArr.push_back(0.0f);

    verticesArr.push_back(0.8f);
    verticesArr.push_back(0.3f);
    verticesArr.push_back(0.0f);

    verticesArr.push_back(-25.0f);
    verticesArr.push_back(0.0f);
    verticesArr.push_back(0.0f);

    verticesArr.push_back(0.8f);
    verticesArr.push_back(0.3f);
    verticesArr.push_back(0.0f);

    for (int i = 1; i <= 50; i++)
    {
        verticesArr.push_back(-i * gridUnitLength);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(25.0f);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(-i * gridUnitLength);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(-25.0f);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(i * gridUnitLength);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(25.0f);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(i * gridUnitLength);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(-25.0f);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(25.0f);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(-i * gridUnitLength);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(-25.0f);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(-i * gridUnitLength);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(25.0f);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(i * gridUnitLength);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);

        verticesArr.push_back(-25.0f);
        verticesArr.push_back(0.0f);
        verticesArr.push_back(i * gridUnitLength);

        verticesArr.push_back(0.8f);
        verticesArr.push_back(0.3f);
        verticesArr.push_back(0.0f);
    }

    float* vertices = &verticesArr[0];

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, 2424 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer
            (
                    0,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    6 * sizeof(float),
                    (void*)0
            );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer
            (
                    1,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    6 * sizeof(float),
                    (void*)sizeof(glm::vec3)
            );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // VAO already stored the state we just defined, safe to unbind buffer
    glBindVertexArray(0); // Unbind to not modify the VAO

    return vertexArrayObject;
}

void buildMatrices(Shader shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(fov), 1024.0f / 768.0f, 0.1f, 100.0f);

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
}

void displayGrid(Shader shader, GLuint vao)
{
    shader.use();
    buildMatrices(shader);

    glBindVertexArray(vao);
    glLineWidth(1);
    glDrawArrays(GL_LINES, 0, 404);
}

void displayAxis(Shader shader, GLuint vao)
{
    shader.use();
    buildMatrices(shader);

    glBindVertexArray(vao);

    glLineWidth(4);
    glDrawArrays(GL_LINES, 0, 6);
}

void displayFirstShape(Shader shader, GLuint vao)
{
    shader.use();
    buildMatrices(shader);

    glm::vec3 positions = glm::vec3(0.0f, 0.0f, 0.0f);

    glBindVertexArray(vao);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f + translateOneX, 0.0f, 0.0f + translateOneZ));
    model = glm::rotate(model, glm::radians(rotateOne), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f * scaleOne, 0.5f * scaleOne, 0.5f * scaleOne));
    model = glm::translate(model, positions);


    shader.setMat4("model", model);

    glDrawArrays(renderModeOne, 0, 36);

}

void displayPositions(){
    cout << "\nPitch: " << pitch << " Roll: " << roll << " Yaw: " << yaw << endl;
    cout << "CameraPos: " << cameraPos[0]*2 << "m, " << cameraPos[1]*2 << "m, " << cameraPos[2]*2 << "m" << endl;
    cout << "Mock Speed: x: " << mockGroundSpeedX << "m/s, z: " << mockGroundSpeedZ << "m/s" << endl;
    cout << "ObjectPos: " << translateOneX*2 << "m, " << "0m, " << translateOneZ*2 << "m" << endl;
}

void processInput(GLFWwindow* window)
{
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    //Camera Controls
    const float actualSpeed = cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += actualSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= actualSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * actualSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * actualSpeed;
    if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) { //HAVE TO FIX THIS IMPORTANT MAYBE IT WILL BE FIXED BY ITSELF
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        //lastX = 512.0f;
        //lastY = 384.0f;
        //firstMouse = true;
        yaw = -90.0f;
        pitch = 0.0f;
        fov = 45.0f;
        roll = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        spacePressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        if (spacePressed) {
            spacePressed = false;
            displayPositions();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        pPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
        if (pPressed) {
            pPressed = false;
            cameraPos += (mockGroundSpeedX * timeSimInterval) * cameraFront;
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (mockGroundSpeedZ * timeSimInterval);
            displayPositions();
        }
    }

    //Shape Render Controls

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        renderModeOne = GL_TRIANGLES;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        renderModeOne = GL_POINTS;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        renderModeOne = GL_LINES;
    }

    //Shape Translation Controls
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move left
    {
        translateOneX -= 0.1;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move right
    {
        translateOneX += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move Up
    {
        translateOneZ += 0.1;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move down
    {
        translateOneZ -= 0.1;
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // Scale Up
    {
        scaleOne += 0.1;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // Scale Down
    {
        scaleOne -= 0.1;
        if (scaleOne < 0.0) scaleOne = 0.0;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // Rotate clockwise
    {
        rotateOne += 5;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Rotate counterclockwise
    {
        rotateOne -= 5;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        mouseYChange *= sensitivity;
        pitch += mouseYChange;
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        roll -= 0.25f;
        if (roll > 360) roll -= 360;
        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-0.25f), cameraFront);
        cameraUp = normalize(glm::mat3(roll_mat) * cameraUp);
        cout << roll << endl;
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        roll += 0.25f;
        if (roll > 360) roll -= 360;
        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(0.25f), cameraFront);
        cameraUp = normalize(glm::mat3(roll_mat) * cameraUp);
        cout << roll << endl;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        mouseXChange *= sensitivity;
        yaw += mouseXChange;
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        fov += (float)mouseYChange;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 180.0f)
            fov = 180.0f;
    }


}

void textUpdate(){

    unsigned char arr[] = "The quick god jumps over the lazy brown fox.";
    glRasterPos3f(2.0, 2.0, 2.0);
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; arr[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, arr[i]);
    }

}

int main(int argc, char* argv[]) {
    // Initialize GLFW and OpenGL version
    glfwInit();
    glutInit(&argc, argv);

    #if defined(PLATFORM_OSX)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    #endif

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Super Cool Targeting Pod Tester!", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    // glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Compile and link shaders here ...
    Shader axisShader("shape.vertexshader", "shape.fragmentshader");
    Shader gridShader("shape.vertexshader", "shape.fragmentshader");
    Shader firstShapeShader("shape.vertexshader", "shape.fragmentshader");

    GLuint gridVao = createGridVao();
    GLuint axisVao = createAxisVao();
    GLuint cubeVao = createCubeVao();

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //MOUSE MOVEMENTS.
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        // Calculate the positional differences
        mouseXChange = mousePosX - lastMousePosX;
        mouseYChange = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Processing input
        processInput(window);

        // Display
        displayGrid(gridShader, gridVao);
        displayAxis(axisShader, axisVao);
        displayFirstShape(firstShapeShader, cubeVao);

        // End frame
        glfwSwapBuffers(window);

        // Detect inputs
        glfwPollEvents();

        // Text (Broken)
        textUpdate();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}