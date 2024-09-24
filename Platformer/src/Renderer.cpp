#include "../headers/Cube.h"
#include <string>

int windowWidth;
int windowHeight;

GLuint program;
GLuint vao;
GLuint buffer;
GLint model_location;
GLint view_location;
GLint proj_location;
vmath::mat4 proj_matrix;
vmath::vec3 camera_position;

 void loadShaders()
{
    GLuint vertex_shader;
    GLuint fragment_shader;

    vertex_shader = ES::LoadShader("shaders/basic.vs.glsl", GL_VERTEX_SHADER);
    fragment_shader = ES::LoadShader("shaders/basic.fs.glsl", GL_FRAGMENT_SHADER);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

 void startup() {
    loadShaders();

    model_location = glGetUniformLocation(program, "model_matrix");
    view_location = glGetUniformLocation(program, "view_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(Cube::vertexPositions),
        Cube::vertexPositions,
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float aspect = windowWidth / windowHeight;
    proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
}

 void shutdown() {
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}

 void render(double currentTime) {
    static const GLfloat background_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const GLfloat one = 1.0f;

    glViewport(0, 0, windowWidth, windowHeight);
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClearBufferfv(GL_DEPTH, 0, &one);

    glUseProgram(program);

    glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

    vmath::mat4 model_matrix = vmath::translate(0.0f, 0.0f, -4.0f);
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model_matrix);

    vmath::vec3 cameraPos = vmath::vec3(0.0f, 0.0f, 3.0f);
    vmath::vec3 cameraTarget = vmath::vec3(0.0f, 0.0f, 0.0f);
    vmath::vec3 cameraUp = vmath::vec3(0.0f, 1.0f, 0.0f);
    vmath::mat4 view_matrix = vmath::lookat(cameraPos, cameraTarget, cameraUp);
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void runGameLoop(GLFWwindow* window)
{
    bool running = true;
    do
    {
        render(glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();

        running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
        running &= (glfwWindowShouldClose(window) != GL_TRUE);
    } while (running); 
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{    
    windowWidth = 800;
    windowHeight = 600;

    GLFWwindow* window = ES::CreateAppWindow(windowWidth, windowHeight, "Platformer");

    if (window == NULL)
    {
        fprintf(stdout, "No window\n");
        return 0;
    }

    startup();
    runGameLoop(window);
    shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}