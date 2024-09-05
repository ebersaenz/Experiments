#include "SharedUtilities.h"

namespace ES {      
    GLFWwindow* CreateAppWindow(int width, int height, const char* name)
    {        
        if (!glfwInit())
        {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return NULL;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_STEREO, GL_FALSE);

        GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);

        if (!window)
        {
            fprintf(stderr, "Failed to open window\n");
            return NULL;
        }

        glfwMakeContextCurrent(window);

        gl3wInit();

        return window;
    }
};