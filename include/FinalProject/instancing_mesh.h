#ifndef GPR924_COMPGRAPH_INSTANCING_MESH_H
#define GPR924_COMPGRAPH_INSTANCING_MESH_H

#include "renderer/pipeline.h"

#include <GL/glew.h>
#include <SDL3/SDL_video.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Graphics {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;

        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct Texture {
        GLuint id;
        std::string type;
        std::string path;
    };

    class InstancingMesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        InstancingMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;
            setupMesh();
        }

        //Link the Instance VBO to this Mesh's VAO
        void SetupInstancingAttributes(const GLuint instanceVBO) const {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

            for (int i = 0; i < 4; i++) {
                constexpr std::size_t vec4Size = sizeof(glm::vec4);
                glEnableVertexAttribArray(5 + i);
                glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void *>(i * vec4Size));
                glVertexAttribDivisor(5 + i, 1);
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // Draw 'count' instances of this mesh
        void Draw(common::Pipeline &pipeline, const int instanceCount) const {
            if (instanceCount <= 0) return;
            // Unbind textures first to be clean
            glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);

            // Bind Textures (Same logic as standard Draw)
            for(const auto & texture : textures) {
                std::string name = texture.type;
                if(name == "texture_diffuse") {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    pipeline.SetInt("material_diffuse", 0);
                }
                else if(name == "texture_specular") {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    pipeline.SetInt("material_specular", 1);
                }
                else if(name == "texture_normal") {
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    pipeline.SetInt("material_normal", 2);
                }
            }

            glBindVertexArray(VAO);
            //Instanced Draw Call
            glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr, instanceCount);

            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        }

    private:
        GLuint VAO{}, VBO{}, EBO{};

        void setupMesh() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // Vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(0));

            // Vertex Normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Normal)));

            // Vertex Texture Coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));

            // Vertex Tangents
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Tangent)));

            // Vertex Bitangents
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Bitangent)));

            glBindVertexArray(0);
        }
    };
}
#endif //GPR924_COMPGRAPH_INSTANCING_MESH_H
