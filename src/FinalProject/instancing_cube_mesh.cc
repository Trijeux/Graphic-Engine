#include "FinalProject/instancing_cube_mesh.h"

#include <GL/glew.h>

#include <iostream>
#include <glm/vec4.hpp>

#include "stb_image.h"

namespace Graphics {
    void InstancingCubeMesh::Init() {
        // --- Vertex Data (Pos, Norm, Tex, Tangent, Bitangent) ---
        // Stride: 14 floats
        static constexpr float vertices[] = {
            // Front (Z+)
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            // Back (Z-)
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            // Right (X+)
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, 0.0f,
            // Left (X-)
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
            // Top (Y+)
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,-1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,-1.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,-1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,-1.0f,
            // Bottom (Y-)
            -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f
        };
        // Indices (Standard Cube)
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,       // Front
            4, 5, 6, 6, 7, 4,       // Back
            8, 9, 10, 10, 11, 8,    // Right
            12, 13, 14, 14, 15, 12, // Left
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom
        };

        // --- GL Setup ---
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        int stride = 14 * sizeof(float);
        // 0: Pos, 1: Norm, 2: Tex, 3: Tangent, 4: Bitangent
        glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
        glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(8 * sizeof(float)));
        glEnableVertexAttribArray(4); glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(11 * sizeof(float)));

        glBindVertexArray(0);

        // --- Textures ---
        auto loadTex = [](const char* path) {
            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, channels;
            unsigned char* loaded_tex = stbi_load(path, &width, &height, &channels, 0);
            if(loaded_tex) {
                GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, loaded_tex);
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(loaded_tex);
            }else {
                std::cout << "Failed to load texture: " << path << std::endl;
            }
            return id;
        };
        texture = loadTex("data/textures/logo_diffuse.jpg");
        specular_texture = loadTex("data/textures/logo_specular.jpg");
        normal_texture = loadTex("data/textures/logo_normal.jpg");
    }

    void InstancingCubeMesh::SetupInstancing(GLuint instanceVBO) {
        glBindVertexArray(vao);
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

    void InstancingCubeMesh::DrawInstanced(common::Pipeline &pipeline, int instanceCount) {
        if (instanceCount <= 0) return;
        // Bind Textures
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, specular_texture);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, normal_texture);

        pipeline.SetInt("material_diffuse", 0);
        pipeline.SetInt("material_specular", 1);
        pipeline.SetInt("material_normal", 2);

        glBindVertexArray(vao);
        //Instanced Draw Call
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, instanceCount);
        glBindVertexArray(0);
    }

    void InstancingCubeMesh::Destroy() const {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteTextures(1, &texture);
        glDeleteTextures(1, &specular_texture);
        glDeleteTextures(1, &normal_texture);
    }

}
