#include "../headers/Cube.h"

inline GLuint Cube::load(const char* filename, GLenum shader_type)
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

inline void Cube::loadShaders()
{
    GLuint vertex_shader;
    GLuint fragment_shader;

    vertex_shader = load("shaders/cube.vs.glsl", GL_VERTEX_SHADER);
    fragment_shader = load("shaders/cube.fs.glsl", GL_FRAGMENT_SHADER);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

inline void Cube::startup() {
    loadShaders();

    mv_location = glGetUniformLocation(program, "mv_matrix");
    proj_location = glGetUniformLocation(program, "proj_matrix");

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    static const GLfloat vertex_positions[] =
    {
        -0.25f,  0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
        0.25f, -0.25f, -0.25f,

        0.25f, -0.25f, -0.25f,
        0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,

        0.25f, -0.25f, -0.25f,
        0.25f, -0.25f,  0.25f,
        0.25f,  0.25f, -0.25f,

        0.25f, -0.25f,  0.25f,
        0.25f,  0.25f,  0.25f,
        0.25f,  0.25f, -0.25f,

        0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f,  0.25f,
        0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
        0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        0.25f, -0.25f,  0.25f,
        0.25f, -0.25f, -0.25f,

        0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f,  0.25f,

        -0.25f,  0.25f, -0.25f,
        0.25f,  0.25f, -0.25f,
        0.25f,  0.25f,  0.25f,

        0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f, -0.25f
    };

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertex_positions),
        vertex_positions,
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

inline void Cube::shutdown() {
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}

inline void Cube::render(double currentTime) {
    static const GLfloat background_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const GLfloat one = 1.0f;

    glViewport(0, 0, windowWidth, windowHeight);
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClearBufferfv(GL_DEPTH, 0, &one);

    glUseProgram(program);

    glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

    float f = (float)currentTime * 0.3f;
    vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
        /*vmath::translate(sinf(2.1f * f) * 0.5f,
        cosf(1.7f * f) * 0.5f,
        sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);*/// *
        vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
        vmath::rotate((float)currentTime * 45.0f, 1.0f, 0.0f, 0.0f);

    glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Cube::RunGameLoop(GLFWwindow* window)
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