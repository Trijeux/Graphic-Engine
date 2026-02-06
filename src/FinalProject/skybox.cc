#include "renderer/shader.h"
#include "stb_image.h"
#include "FinalProject/skybox.h"

#include <array>
#include <iostream>
#include <span>
#include <glm/gtc/type_ptr.hpp>

namespace Graphics {

    void Skybox::Init() {
        stbi_set_flip_vertically_on_load(false);

        // 1. Setup Shaders
        common::Shader vs, fs;
        vs.Load("data/shaders/FinalProject/skybox.vert", GL_VERTEX_SHADER);
        fs.Load("data/shaders/FinalProject/skybox.frag", GL_FRAGMENT_SHADER);
        pipeline.Load(vs, fs);

        // 2. Setup Cube Vertices
        // Positions only (centered at 0,0,0)
        float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        static constexpr std::array<std::string_view, 6> faces = {
            "data/textures/skybox/right.jpg",
            "data/textures/skybox/left.jpg",
            "data/textures/skybox/top.jpg",
            "data/textures/skybox/bottom.jpg",
            "data/textures/skybox/front.jpg",
            "data/textures/skybox/back.jpg"
        };

        // 3. Load Texture
        textureID = LoadCubemap(faces);

        stbi_set_flip_vertically_on_load(true);
    }

    void Skybox::Draw(const glm::mat4& view, const glm::mat4& proj) {
        // Change depth function so depth 1.0 (skybox) passes
        glDepthFunc(GL_LEQUAL);

        pipeline.Bind();

        pipeline.SetMat4("view", glm::value_ptr(glm::mat4(glm::mat3(view))));
        pipeline.SetMat4("projection", glm::value_ptr(proj));
        pipeline.SetInt("skybox", 0);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Set depth function back to default
        glDepthFunc(GL_LESS);
    }

    void Skybox::Destroy() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &textureID);
    }

    GLuint Skybox::LoadCubemap(std::span<const std::string_view, 6> faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].data(), &width, &height, &nrChannels, 0);
            if (data) {
                // Determine format
                GLint format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
}