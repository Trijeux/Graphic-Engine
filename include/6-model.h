#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "6-mesh.h"
#include <vector>
#include <string>
#include <iostream>

namespace graphics {
    class Model {
    public:
        std::vector<Texture> textures_loaded;
        std::vector<Mesh>    meshes;
        std::string          directory;
        Model() = default;

        void SetupInstancing(GLuint instanceVBO);

        void Load(const std::string& path);

        void Draw(common::Pipeline& pipeline, int instanceCount);

    private:
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(const aiMaterial *mat, aiTextureType type, std::string typeName);
    };
}
#endif //MODEL_H
