
#include "SharedUtilities.h"

class Triangle {
private:
    GLuint program;
    GLuint vao;

    void startup() {
        static const char* vs_source[] =
        {
            "#version 420 core                                                 \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4(-0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4( 0.25,  0.25, 0.5, 1.0)); \n"
            "                                                                  \n"
            "    gl_Position = vertices[gl_VertexID];                          \n"
            "}                                                                 \n"
        };

        static const char* fs_source[] =
        {
            "#version 420 core                                                 \n"
            "                                                                  \n"
            "out vec4 color;                                                   \n"
            "                                                                  \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    color = vec4(0.0, 0.8, 1.0, 1.0);                             \n"
            "}                                                                 \n"
        };

        program = glCreateProgram();
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);

        glAttachShader(program, vs);
        glAttachShader(program, fs);

        glLinkProgram(program);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void shutdown() {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }

    void render(double currentTime) {
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, green);

        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

public:
    void RunGameLoop(GLFWwindow* window)
    {
        startup();

        bool running = true;
        do
        {
            render(glfwGetTime());
            glfwSwapBuffers(window);
            glfwPollEvents();

            running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
            running &= (glfwWindowShouldClose(window) != GL_TRUE);
        } while (running);

        shutdown();
    }
};

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GLFWwindow* window = ES::CreateAppWindow(800, 600, "Triangle Test");

    if (window == NULL)
    {
        fprintf(stdout, "No window\n");
        return 0;
    }

    Triangle t;
    t.RunGameLoop(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}