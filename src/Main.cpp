#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Application.h"

void loadOpenGL();
GLFWwindow* createWindow(unsigned int width, unsigned int height);

static constexpr int initialWindowWidth  = 1280;
static constexpr int initialWindowHeight = 960; 

int main(void)
{
    GLFWwindow* window = createWindow(initialWindowWidth, initialWindowHeight);
    loadOpenGL();
    
    Application app;
    while (!glfwWindowShouldClose(window))
    {
        app.render();
    }

    glfwTerminate();
    return 0;
}