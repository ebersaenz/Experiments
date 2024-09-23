#include "../headers/Cube.h"
#include <string>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Cube c;
    c.windowWidth = 800;
    c.windowHeight = 600;

    // TODO: Remove
    std::string s = "\n\ntest\n";
    OutputDebugStringA(s.c_str());

    GLFWwindow* window = ES::CreateAppWindow(c.windowWidth, c.windowHeight, "Platformer");

    if (window == NULL)
    {
        fprintf(stdout, "No window\n");
        return 0;
    }

    c.RunGameLoop(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}