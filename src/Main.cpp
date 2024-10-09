#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "Shader.h"
#include <iostream>
#include <vector>
#include <fstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char* message, const void* userParam);

static constexpr int initialWindowWidth  = 1280;
static constexpr int initialWindowHeight = 960;

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    window = glfwCreateWindow(initialWindowWidth, initialWindowHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "WARNING::GLAD: Failed to initialize OpenGL" << std::endl;
        return -1;
    }
    else std::cout << "Succesfully initialized OpenGL" << std::endl;

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        std::cout << "Succesfully initialized Debug Output" << std::endl;
    }
    else std::cout << "WARNING::DEBUG_OUTPUT: couldn't initialize" << std::endl;

    // RENDER DATA:

    constexpr float circleRadius = 5.0f;
    constexpr int circleVerticesCount = 10;

    std::vector<float> circleVertices;

    float angle = 0.0f;
    for (int i = 0; i < circleVerticesCount; i += 1)
    {
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);
		circleVertices.push_back(circleRadius * glm::cos(glm::radians(angle)));
        circleVertices.push_back(circleRadius * glm::sin(glm::radians(angle)));
		angle += 360.0f / circleVerticesCount;
        circleVertices.push_back(circleRadius * glm::cos(glm::radians(angle)));
        circleVertices.push_back(circleRadius * glm::sin(glm::radians(angle)));
    }

    for (int i = 0; i < circleVertices.size(); i += 2)
    {
		std::cout << "Triangle (" << circleVertices[i] << ", " << circleVertices[i + 1] << ")\n";
    }

	unsigned int VAOCircle, VBOCircle;
	glGenVertexArrays(1, &VAOCircle);
	glBindVertexArray(VAOCircle);

	glGenBuffers(1, &VBOCircle);
	glBindBuffer(GL_ARRAY_BUFFER, VBOCircle);

	glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    // CIRCLE SHADER
    Shader circleShader("C:\\Users\\Anto\\source\\repos\\GraphEditor\\res\\shaders\\circle.vert"
                        , "C:\\Users\\Anto\\source\\repos\\GraphEditor\\res\\shaders\\circle.frag");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAOCircle);

		glDrawArrays(GL_TRIANGLES, 0, circleVerticesCount);

        glfwSwapBuffers(window);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        //std::cout << "screen size: (" << width << "x" << height << ")\n";
        //std::cout << "cursor at (" << xPos << ", " << yPos << ")\n";

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}