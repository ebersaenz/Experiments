#define STB_IMAGE_IMPLEMENTATION
#include "../headers/Renderer.h"
#include "stb_image.h"

void Renderer::startup(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    loadShaders("textured", texturedShaderProgram);

    // Get uniform locations
    modelLocation = glGetUniformLocation(texturedShaderProgram, "modelMatrix");
    viewLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
    projLocation = glGetUniformLocation(texturedShaderProgram, "projMatrix");
    diffuseSamplerLocation = glGetUniformLocation(texturedShaderProgram, "diffuseSampler");
    normalSamplerLocation = glGetUniformLocation(texturedShaderProgram, "normalSampler");
    matValidityCheckLocation = glGetUniformLocation(texturedShaderProgram, "isValidMaterial");
    lightDirectionLocation = glGetUniformLocation(texturedShaderProgram, "lightDir");
    lightColorLocation = glGetUniformLocation(texturedShaderProgram, "lightColor");
    viewPosLocation = glGetUniformLocation(texturedShaderProgram, "viewPos");

    // Setup matrices projection and view matrices
    float aspect = (float) windowWidth / (float) windowHeight;
    projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

    vmath::vec3 cameraPos = vmath::vec3(0.0f, 0.0f, 3.0f);
    vmath::vec3 cameraTarget = vmath::vec3(0.0f, 0.0f, 0.0f);
    vmath::vec3 cameraUp = vmath::vec3(0.0f, 1.0f, 0.0f);
    viewMatrix = vmath::lookat(cameraPos, cameraTarget, cameraUp);

    // Model load test
    gameObject = loadModel("../Assets/Models/haloSpartan2.glb");

    // OpenGL settings    
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
}

void Renderer::shutdown() {
    for (Mesh mesh : gameObject.meshes) {
        glDeleteProgram(texturedShaderProgram);
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);

        mesh.vertices.clear();
        mesh.indices.clear();
    }

    for (GLuint textureId : allUsedTextureIds) {
        glDeleteTextures(1, &textureId);
    }
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

void Renderer::render(double currentTime) {
    static const GLfloat lightDirection[] = { -0.5f, -0.5f, -0.5f };    
    static const GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f };
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(texturedShaderProgram);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, projMatrix);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, viewMatrix);

    // Draw gameObject
    for (Mesh mesh : gameObject.meshes) {
        mesh.modelMatrix *= vmath::rotate<float>(0.0f, 60.0f * currentTime, 0.0f);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, mesh.modelMatrix);

        // Set lighting uniforms
        glUniform3fv(lightDirectionLocation, 1, lightDirection);
        glUniform3fv(lightColorLocation, 1, lightColor);
        glUniform3fv(viewPosLocation, 1, cameraPosition);

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.material.diffuseTextureId);
        glUniform1i(diffuseSamplerLocation, 0);
        glUniform1i(matValidityCheckLocation, mesh.material.diffuseTextureId == -1 ? 0 : 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.material.normalTextureId);
        glUniform1i(normalSamplerLocation, 1);

        // Bind the VAO and draw the mesh
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
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

GameObject Renderer::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    GameObject gameObject;

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        OutputDebugStringA("Error loading model");
        return gameObject;
    }

    currentModelTextureIds.clear();
    processNode(scene->mRootNode, scene, gameObject);

    return gameObject;
}

void Renderer::processNode(aiNode* node, const aiScene* scene, GameObject& gameObject) {
    // The local transformation of the current node
    aiMatrix4x4 t = node->mTransformation;

    // Iterate over all the meshes that this node references
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* inputMesh = scene->mMeshes[node->mMeshes[i]];
        Mesh outputMesh;
        outputMesh.modelMatrix = getGlobalTransform(node, scene);

        float s = 28.0f;
        outputMesh.modelMatrix = vmath::translate(0.0f, -1.1f, 0.0f) * vmath::scale(s,s,s) * vmath::rotate(0.0f, 0.0f, 0.0f) * outputMesh.modelMatrix;

        processMesh(inputMesh, outputMesh);

        // Load texture
        if (inputMesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[inputMesh->mMaterialIndex];
            outputMesh.material.diffuseTextureId = loadEmbededTexture(material, scene, aiTextureType_DIFFUSE);
            outputMesh.material.normalTextureId = loadEmbededTexture(material, scene, aiTextureType_NORMALS);
        }

        gameObject.meshes.push_back(outputMesh);
    }

    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, gameObject);
    }
}

void Renderer::processMesh(aiMesh* aiInputMesh, Mesh& outputMesh) {
    // Iterate over the vertices of the mesh
    for (unsigned int i = 0; i < aiInputMesh->mNumVertices; i++) {

        // Vertex positions
        outputMesh.vertices.push_back(aiInputMesh->mVertices[i].x);
        outputMesh.vertices.push_back(aiInputMesh->mVertices[i].y);
        outputMesh.vertices.push_back(aiInputMesh->mVertices[i].z);

        // Normals
        if (aiInputMesh->HasNormals()) {
            outputMesh.vertices.push_back(aiInputMesh->mNormals[i].x);
            outputMesh.vertices.push_back(aiInputMesh->mNormals[i].y);
            outputMesh.vertices.push_back(aiInputMesh->mNormals[i].z);
        }
        else {
            outputMesh.vertices.push_back(0.0f);
            outputMesh.vertices.push_back(0.0f);
            outputMesh.vertices.push_back(0.0f);
        }

        // Tangents
        if (aiInputMesh->HasTangentsAndBitangents()) {
            outputMesh.vertices.push_back(aiInputMesh->mTangents[i].x);
            outputMesh.vertices.push_back(aiInputMesh->mTangents[i].y);
            outputMesh.vertices.push_back(aiInputMesh->mTangents[i].z);
        }
        else {
            outputMesh.vertices.push_back(0.0f);
            outputMesh.vertices.push_back(0.0f);
            outputMesh.vertices.push_back(0.0f);
        }

        // Texture coordinates
        if (aiInputMesh->mTextureCoords[0]) {
            outputMesh.vertices.push_back(aiInputMesh->mTextureCoords[0][i].x);
            outputMesh.vertices.push_back(aiInputMesh->mTextureCoords[0][i].y);
        }
        else {
            outputMesh.vertices.push_back(0.0f);
            outputMesh.vertices.push_back(0.0f);
        }
    }

    // Indices
    for (unsigned int i = 0; i < aiInputMesh->mNumFaces; i++) {
        aiFace face = aiInputMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            outputMesh.indices.push_back(face.mIndices[j]);
        }
    }

    // Generate OpenGL buffers and arrays
    glGenVertexArrays(1, &outputMesh.VAO);
    glGenBuffers(1, &outputMesh.VBO);
    glGenBuffers(1, &outputMesh.EBO);

    // Bind and fill data in buffers
    glBindVertexArray(outputMesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, outputMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, outputMesh.vertices.size() * sizeof(float), &outputMesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outputMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, outputMesh.indices.size() * sizeof(unsigned int), &outputMesh.indices[0], GL_STATIC_DRAW);

    GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);

    // Specify the layout of the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

vmath::mat4 Renderer::getGlobalTransform(aiNode* node, const aiScene* scene) {
    aiMatrix4x4 t = node->mTransformation;
    aiNode* parent = node->mParent;

    // Traverse up the hierarchy, multiplying each parent's transform
    while (parent != nullptr) {
        t = parent->mTransformation * t;
        parent = parent->mParent;
    }

    return vmath::mat4(vmath::vec4(t.a1, t.b1, t.c1, t.d1),
        vmath::vec4(t.a2, t.b2, t.c2, t.d2),
        vmath::vec4(t.a3, t.b3, t.c3, t.d3),
        vmath::vec4(t.a4, t.b4, t.c4, t.d4));
}

// To be used with glb assets only
GLuint Renderer::loadEmbededTexture(aiMaterial* material, const aiScene* scene, aiTextureType textureType) {
    aiString texturePath;

    // Try to get a texture of the specified type from the material
    if (material->GetTexture(textureType, 0, &texturePath) == AI_SUCCESS) {

        std::string path = texturePath.C_Str();
        if (currentModelTextureIds.find(path) != currentModelTextureIds.end())
        {
            return currentModelTextureIds[path];
        }

        int textureIndex = std::atoi(&path[1]); // Get texture index
        aiTexture* texture = scene->mTextures[textureIndex];   // Get embedded texture

        // Check if the texture is compressed
        if (texture->mHeight == 0) {
            unsigned char* data = reinterpret_cast<unsigned char*>(texture->pcData);
            int width, height, nrChannels;

            unsigned char* imageData = stbi_load_from_memory(
                data, texture->mWidth, &width, &height, &nrChannels, 0);

            if (imageData) {
                GLuint textureId;
                glGenTextures(1, &textureId);
                glBindTexture(GL_TEXTURE_2D, textureId);

                // Upload the texture to OpenGL
                if (nrChannels == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
                }
                else if (nrChannels == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
                }

                glGenerateMipmap(GL_TEXTURE_2D);

                stbi_image_free(imageData);

                currentModelTextureIds[path] = textureId;
                allUsedTextureIds.push_back(textureId);

                return textureId;
            }
            else {
                OutputDebugStringA("\nFailed to load embedded texture");
            }
        }
        else {
            OutputDebugStringA("\nImage is uncompressed (not currently handled)");            
        }
    }

    return -1;
}