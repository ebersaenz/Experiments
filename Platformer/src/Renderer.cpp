#define STB_IMAGE_IMPLEMENTATION
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../headers/Renderer.h"
#include "stb_image.h"
#include <direct.h> 

unsigned int loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        OutputDebugStringA("\nerror loading texture");
        stbi_image_free(data);
    }

    return textureID;
}

void processMesh(aiMesh* mesh, Mesh& myMesh) {
    // Iterate over the vertices of the mesh
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

        // Vertex positions
        myMesh.vertices.push_back(mesh->mVertices[i].x);
        myMesh.vertices.push_back(mesh->mVertices[i].y);
        myMesh.vertices.push_back(mesh->mVertices[i].z);

        // Normals (optional)
        if (mesh->HasNormals()) {
            myMesh.vertices.push_back(mesh->mNormals[i].x);
            myMesh.vertices.push_back(mesh->mNormals[i].y);
            myMesh.vertices.push_back(mesh->mNormals[i].z);
        }

        // Texture coordinates (optional)
        if (mesh->mTextureCoords[0]) {
            myMesh.vertices.push_back(mesh->mTextureCoords[0][i].x);
            myMesh.vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            myMesh.vertices.push_back(0.0f); // Default value if no texture
            myMesh.vertices.push_back(0.0f);
        }
    }

    // Iterate over the faces (triangles)
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            myMesh.indices.push_back(face.mIndices[j]);
        }
    }

    // Generate OpenGL buffers and arrays
    glGenVertexArrays(1, &myMesh.VAO);
    glGenBuffers(1, &myMesh.VBO);
    glGenBuffers(1, &myMesh.EBO);

    // Bind and fill data in buffers
    glBindVertexArray(myMesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, myMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, myMesh.vertices.size() * sizeof(float), &myMesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myMesh.indices.size() * sizeof(unsigned int), &myMesh.indices[0], GL_STATIC_DRAW);

    // Specify the layout of the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Vertex position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normals
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
}

Mesh loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    Mesh myMesh;

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        OutputDebugStringA("Error loading model");
        return myMesh;
    }

    aiMesh* mesh = scene->mMeshes[0]; // Load the first mesh (assuming a single mesh for simplicity)
    processMesh(mesh, myMesh);

    const size_t size = 1024;
    char buffer[size];
    
    if (_getcwd(buffer, size) != NULL) {
        std::string s(buffer);
        OutputDebugStringA(("current directory: " + s).c_str());        
    }

    // Load texture
    if (mesh->mMaterialIndex >= 0) {       
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            std::string s(texturePath.C_Str());
            OutputDebugStringA(("\ninitial path: " + std::string(texturePath.C_Str())).c_str());
            s.replace(0,2,"..\\Assets\\Models\\");
            OutputDebugStringA(("\nfinal path: " + s).c_str());
            myMesh.textureId = loadTexture(s);
        }
    }

    return myMesh;
}

void renderMesh(const Mesh& mesh) {
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);  // Activate the first texture unit
    glBindTexture(GL_TEXTURE_2D, mesh.textureId);  // Bind the texture

    // Bind the VAO and draw the mesh
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::loadShaders(std::string shaderName, GLuint& programId)
{
    GLuint vertex_shader;
    GLuint fragment_shader;

    vertex_shader = ES::LoadShader(("shaders/" + shaderName + ".vs.glsl").c_str(), GL_VERTEX_SHADER);
    fragment_shader = ES::LoadShader(("shaders/" + shaderName + ".fs.glsl").c_str(), GL_FRAGMENT_SHADER);

    programId = glCreateProgram();
    glAttachShader(programId, vertex_shader);
    glAttachShader(programId, fragment_shader);
    glLinkProgram(programId);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Renderer::startup(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    loadShaders("basic", basicShaderProgram);
    loadShaders("textured", texturedShaderProgram);

    // Setup matrices projection and view matrices
    model_location = glGetUniformLocation(basicShaderProgram, "model_matrix");
    view_location = glGetUniformLocation(basicShaderProgram, "view_matrix");
    proj_location = glGetUniformLocation(basicShaderProgram, "proj_matrix");

    model_location2 = glGetUniformLocation(texturedShaderProgram, "model_matrix");
    view_location2 = glGetUniformLocation(texturedShaderProgram, "view_matrix");
    proj_location2 = glGetUniformLocation(texturedShaderProgram, "proj_matrix");

    float aspect = windowWidth / windowHeight;
    proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

    vmath::vec3 cameraPos = vmath::vec3(0.0f, 0.0f, 3.0f);
    vmath::vec3 cameraTarget = vmath::vec3(0.0f, 0.0f, 0.0f);
    vmath::vec3 cameraUp = vmath::vec3(0.0f, 1.0f, 0.0f);
    view_matrix = vmath::lookat(cameraPos, cameraTarget, cameraUp);

    // Pass cube vertices to opengl
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

    // Model load test
    spiderMesh = loadModel("../Assets/Models/spider.obj");

    // OpenGL settings
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void Renderer::shutdown() {   
    // Delete spider test data
    glDeleteProgram(texturedShaderProgram);
    glDeleteVertexArrays(1, &spiderMesh.VAO);
    glDeleteBuffers(1, &spiderMesh.VBO);
    glDeleteBuffers(1, &spiderMesh.EBO);

    if (spiderMesh.textureId != 0) {
        glDeleteTextures(1, &spiderMesh.textureId);
    }

    spiderMesh.vertices.clear();
    spiderMesh.indices.clear();

    // Delete cube data
    glDeleteProgram(basicShaderProgram);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::render(double currentTime) {
    static const GLfloat background_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const GLfloat one = 1.0f;

    glBindVertexArray(vao);

    glViewport(0, 0, windowWidth, windowHeight);
    glClearBufferfv(GL_COLOR, 0, background_color);
    glClearBufferfv(GL_DEPTH, 0, &one);

    glUseProgram(basicShaderProgram);    

    glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view_matrix);

    vmath::mat4 model_matrix = vmath::translate(0.0f, 0.0f, -4.0f);
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model_matrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model_matrix = vmath::translate(0.0f, -2.0f, -7.0f) * vmath::scale(10.0f, 1.0f, 10.0f);
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model_matrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw spider
    glUseProgram(texturedShaderProgram);
    glUniform1i(glGetUniformLocation(texturedShaderProgram, "texture_diffuse1"), 0);  // Set the texture unit to 0

    glUniformMatrix4fv(proj_location2, 1, GL_FALSE, proj_matrix);
    glUniformMatrix4fv(view_location2, 1, GL_FALSE, view_matrix);

    float s = 0.0f;
    model_matrix = vmath::translate(s,s,-100.0f) * vmath::scale(0.5f, 0.5f, 0.5f) * vmath::rotate<float>(0.0f, 30.0f*currentTime, 0.0f);
    glUniformMatrix4fv(model_location2, 1, GL_FALSE, model_matrix);
    renderMesh(spiderMesh);
}

void Renderer::runGameLoop(GLFWwindow* window)
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