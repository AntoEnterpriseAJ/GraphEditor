#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Graph.h"

void loadOpenGL();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow* createWindow(unsigned int width, unsigned int height);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char* message, const void* userParam);

static constexpr int initialWindowWidth  = 1280;
static constexpr int initialWindowHeight = 960; 

int main(void)
{
    GLFWwindow* window = createWindow(initialWindowWidth, initialWindowHeight);
    loadOpenGL();

    Graph graph;
    while (!glfwWindowShouldClose(window))
    {
        graph.render();
    }

    glfwTerminate();
    return 0;
}