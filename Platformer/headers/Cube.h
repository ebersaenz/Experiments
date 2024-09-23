#pragma once
#include "SharedUtilities.h"
#include "vmath.h"

class Cube {
private:
    GLuint program;
    GLuint vao;
    GLuint buffer;
    GLint mv_location;
    GLint proj_location;
    vmath::mat4 proj_matrix;

    GLuint load(const char* filename, GLenum shader_type);
    void loadShaders();
    void startup();
    void shutdown();
    void render(double currentTime);

public:
    int windowWidth;
    int windowHeight;

    void RunGameLoop(GLFWwindow* window);
};