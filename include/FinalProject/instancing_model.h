#ifndef GPR924_COMPGRAPH_INSTANCING_MODEL_H
#define GPR924_COMPGRAPH_INSTANCING_MODEL_H

#include <assimp/scene.h>

#include <iostream>
#include <string>
#include <vector>

#include "instancing_mesh.h"

namespace Graphics {
    class InstancingModel {
    public:
        std::vector<Texture> textures_loaded;
        std::vector<InstancingMesh>    meshes;
        std::string          directory;

        glm::vec3 boundingCenter = glm::vec3(0.0f);
        float boundingRadius = 0.0f;

        glm::vec3 minBounds = glm::vec3(1e9); // Start huge
        glm::vec3 maxBounds = glm::vec3(-1e9); // Start tiny

        InstancingModel() = default;

        void SetupInstancing(GLuint instanceVBO);
        void Load(const std::string& path);
        void Draw(common::Pipeline &pipeline, int instanceCount);

    private:
        void processNode(aiNode *node, const aiScene *scene);
        InstancingMesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(const aiMaterial *mat, aiTextureType type, std::string typeName);
    };
}
#endif //GPR924_COMPGRAPH_INSTANCING_MODEL_H
