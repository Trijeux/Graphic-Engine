#include "7-cube_light_deferred_rendering.h"
#include <stb_image.h>
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "engine/engine.h"
#include "engine/window.h"

namespace graphics {

// Variables pour le Quad (Passe d'éclairage)
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void CubeLightRendering::Begin() {
    glEnable(GL_DEPTH_TEST);

    // 1. CONFIGURATION DU G-BUFFER
    glGenFramebuffers(1, &g_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

    auto createTex = [&](unsigned int& id, GLint internalFormat, GLenum format, GLenum type) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, common::GetWindowSize().x, common::GetWindowSize().y, 0, format, type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    };

    createTex(g_position, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);

    createTex(g_normal, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);

    createTex(albedo, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, common::GetWindowSize().x, common::GetWindowSize().y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. SHADERS
    Cube.Init("7-g_buffer.vert", "7-g_buffer.frag");
    light.Init("7-deferred_rendering.vert", "7-deferred_rendering.frag");

    // 3. TEXTURES DU CUBE
    auto loadTex = [&](const std::string& filename) {
        unsigned int id;
        int w, h, c;
        unsigned char* data = stbi_load((path_texture + filename).c_str(), &w, &h, &c, 0);
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        GLenum fmt = (c == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return id;
    };

    textureDiffuse = loadTex("logo_diffuse.jpg");
    textureSpecular = loadTex("logo_specular.jpg");
    textureNormal = loadTex("logo_normal.jpg");
    // --- GEOMETRIE CUBE ---
    float cubeVertices[] = {
        // Back face (Normale 0,0,-1)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        // Front face (Normale 0,0,1)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    };

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  int stride = 11 * sizeof(float);
  glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(3); glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
}

void CubeLightRendering::Update(float dt) {
    static float t = 0; t += dt;
    model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.5f, 1.0f, 0.0f));
    lightPos = glm::vec3(sin(t) * 2.5f, 1.0f, cos(t) * 2.5f);
}

void CubeLightRendering::FixedUpdate() {
    // Laisser vide ou ajouter physique
}

void CubeLightRendering::End() {
    glDeleteFramebuffers(1, &g_buffer);
    glDeleteTextures(1, &g_position);
    glDeleteTextures(1, &g_normal);
    glDeleteTextures(1, &albedo);
}

void CubeLightRendering::Draw() {
    // PASSE 1 : GEOMETRY
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Cube.use();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)common::GetWindowSize().x/(float)common::GetWindowSize().y, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
    Cube.setMat4("projection", projection);
    Cube.setMat4("view", view);
    Cube.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureDiffuse);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, textureSpecular);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, textureNormal);
    Cube.setInt("material_diffuse", 0);
    Cube.setInt("material_specular", 1);
    Cube.setInt("material_normal", 2);
    Cube.setBool("useNormalMap", true);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // PASSE 2 : LIGHTING
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    light.use();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, g_position);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, g_normal);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, albedo);
    light.setInt("gPosition", 0);
    light.setInt("gNormal", 1);
    light.setInt("gAlbedoSpec", 2);

    light.setVec3("viewPos", camPos);
    light.setInt("pointLightCount", 1);
    light.setVec3("pointLights[0].position", lightPos);
    light.setVec3("pointLights[0].color", glm::vec3(1.0f));
    light.setFloat("pointLights[0].constant", 1.0f);
    light.setFloat("pointLights[0].linear", 0.09f);
    light.setFloat("pointLights[0].quadratic", 0.032f);
    light.setFloat("specularPow", 32.0f);

    renderQuad();
}
} // namespace graphics