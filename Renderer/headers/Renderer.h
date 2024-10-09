#pragma once
#include "SharedUtilities.h"
#include "vmath.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <map>

struct Material {
    int diffuseTextureId; // Using -1 for meshes that don't use this texture
    int normalTextureId;
};

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    Material material;
    vmath::mat4 modelMatrix;
};

struct GameObject {
    std::vector<Mesh> meshes;
    vmath::mat4 transformMatrix;
};

class Renderer {
private:
    int tempCounter = 0;
    int windowWidth;
    int windowHeight;
    GLuint texturedShaderProgram;
    
    // Uniform locations
    GLint modelLocation;
    GLint viewLocation;
    GLint projLocation;
    GLint diffuseSamplerLocation;
    GLint normalSamplerLocation;
    GLint matValidityCheckLocation;
    GLint lightDirectionLocation;
    GLint lightColorLocation;
    GLint viewPosLocation;

    vmath::mat4 projMatrix;
    vmath::mat4 viewMatrix;
    vmath::vec3 cameraPosition;
    GameObject car;
    std::map<std::string, GLuint> currentModelTextureIds; // Could be <int, int> if only loading glbs
    std::vector<GLuint> allUsedTextureIds;

    void loadShaders(std::string shaderName, GLuint& programId);
    GLuint loadEmbededTexture(aiMaterial* material, const aiScene* scene, aiTextureType textureType);    GameObject loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene, GameObject& gameObject);
    void processMesh(aiMesh* aiInputMesh, Mesh& outputMesh);
    vmath::mat4 getGlobalTransform(aiNode* node, const aiScene* scene);

public:
    void startup(int width, int height);
    void shutdown();
    void render(double currentTime);
    void runGameLoop(GLFWwindow* window);
};