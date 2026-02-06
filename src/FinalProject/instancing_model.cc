#include "FinalProject/instancing_model.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>

#include "FinalProject/utils.h"

namespace Graphics {

    void InstancingModel::SetupInstancing(GLuint instanceVBO) {
        for(auto &mesh : meshes) {
            mesh.SetupInstancingAttributes(instanceVBO);
        }
    }

    void InstancingModel::Load(const std::string& path) {
        // 1. Reset bounds before loadings
        minBounds = glm::vec3(1e9);
        maxBounds = glm::vec3(-1e9);

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // --- FIX START: Handle paths without slashes correctly ---
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash == std::string::npos) {
            directory = "."; // Current directory
        } else {
            directory = path.substr(0, lastSlash);
        }
        // --- FIX END ---

        processNode(scene->mRootNode, scene);

        //Calculate Center and Radius after processing all nodes
        boundingCenter = (minBounds + maxBounds) / 2.0f;
        boundingRadius = glm::length(maxBounds - boundingCenter);
    }

    void InstancingModel::Draw(common::Pipeline &pipeline, int instanceCount) {
        for(const auto & mesh : meshes)
            mesh.Draw(pipeline, instanceCount);
    }

    void InstancingModel::processNode(aiNode *node, const aiScene *scene) {
        // Process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // Then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    InstancingMesh InstancingModel::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            glm::vec3 vector;

            // Position
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Update Min/Max bounds for the whole model
            if (vertex.Position.x < minBounds.x) minBounds.x = vertex.Position.x;
            if (vertex.Position.y < minBounds.y) minBounds.y = vertex.Position.y;
            if (vertex.Position.z < minBounds.z) minBounds.z = vertex.Position.z;

            if (vertex.Position.x > maxBounds.x) maxBounds.x = vertex.Position.x;
            if (vertex.Position.y > maxBounds.y) maxBounds.y = vertex.Position.y;
            if (vertex.Position.z > maxBounds.z) maxBounds.z = vertex.Position.z;

            // Normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // Texture Coordinates
            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            if (mesh->HasTangentsAndBitangents()) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }

            vertices.push_back(vertex);
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. Diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. Specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // 3. Normal maps (Check both HEIGHT and NORMALS types as formats differ)
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> normalMaps2 = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps2.begin(), normalMaps2.end());

        return InstancingMesh{vertices, indices, textures};
    }

    std::vector<Texture> InstancingModel::loadMaterialTextures(const aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString material_texture_path;
            mat->GetTexture(type, i, &material_texture_path);

            bool skip = false;
            for(auto & loading_texture : textures_loaded) {
                aiString loaded_texture_path(loading_texture.path);
                if(loaded_texture_path == material_texture_path) {
                    textures.push_back(loading_texture);
                    skip = true;
                    break;
                }
            }
            if(!skip) {
                Texture texture;
                texture.id = TextureFromFile(material_texture_path.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = material_texture_path.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
}
