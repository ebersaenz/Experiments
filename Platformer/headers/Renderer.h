#pragma once
#include "../headers/Cube.h"
#include <string>
#include <vector>

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    unsigned int textureId;
};

class Renderer {
private:
    int windowWidth;
    int windowHeight;
    GLuint basicShaderProgram;
    GLuint texturedShaderProgram;
    GLuint vao;
    GLuint buffer;
    GLint model_location;
    GLint view_location;
    GLint proj_location;
    GLint model_location2;
    GLint view_location2;
    GLint proj_location2;
    vmath::mat4 proj_matrix;
    vmath::mat4 view_matrix;
    vmath::vec3 camera_position;
    Mesh spiderMesh;

    void loadShaders(std::string shaderName, GLuint& programId);

public:
    void startup(int width, int height);
    void shutdown();
    void render(double currentTime);
    void runGameLoop(GLFWwindow* window);
};