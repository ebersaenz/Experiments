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

    GLuint LoadShader(const char* filename, GLenum shader_type)
    {
        GLuint result = 0;
        FILE* fp;
        size_t filesize;
        char* data;

        fp = fopen(filename, "rb");

        if (!fp)
            return 0;

        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        data = new char[filesize + 1];

        if (!data)
            return result;

        fread(data, 1, filesize, fp);
        data[filesize] = 0;
        fclose(fp);

        result = glCreateShader(shader_type);

        if (!result)
            return result;

        glShaderSource(result, 1, &data, NULL);

        delete[] data;

        glCompileShader(result);

        return result;
    }
};