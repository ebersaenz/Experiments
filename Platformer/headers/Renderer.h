#pragma once
#include "../headers/Cube.h"
#include <string>
#include <vector>

class Renderer {
private:
    int windowWidth;
    int windowHeight;
    GLuint program;
    GLuint vao;
    GLuint buffer;
    GLint model_location;
    GLint view_location;
    GLint proj_location;
    vmath::mat4 proj_matrix;
    vmath::mat4 view_matrix;
    vmath::vec3 camera_position;   

    void loadShaders();

public:
    void startup(int width, int height);
    void shutdown();
    void render(double currentTime);
    void runGameLoop(GLFWwindow* window);
};