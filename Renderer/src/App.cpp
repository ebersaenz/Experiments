
#include "../headers/Renderer.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int windowWidth = 800;
    int windowHeight = 600;

    GLFWwindow* window = ES::CreateAppWindow(windowWidth, windowHeight, "Renderer");

    if (window == NULL)
    {
        fprintf(stdout, "No window\n");
        return 0;
    }

    Renderer renderer;
    renderer.startup(windowWidth, windowHeight);
    renderer.runGameLoop(window);
    renderer.shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}