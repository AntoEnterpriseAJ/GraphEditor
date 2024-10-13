#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Graph.h"

void loadOpenGL();
GLFWwindow* createWindow(unsigned int width, unsigned int height);

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