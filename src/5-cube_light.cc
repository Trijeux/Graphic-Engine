#include "5-cube_light.h"

#include <stb_image.h>

#include <iostream>
#include <ostream>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/engine.h"
#include "engine/window.h"
#include "renderer/shader.h"
#include "renderer/vertex_input.h"

namespace graphics {
void CubeLight::Begin() {
  glEnable(GL_DEPTH_TEST);

  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CW);

  // build and compile our shader program
  // ------------------------------------
  Cube.Init("5-basique_light.vert", "5-basique_light.frag");
  cube_light.Init("4-5-cube_3d.vert", "4-5-cube_3d.frag");
  post_light.Init("5-cube_light_blur.vert", "5-cube_light_blur.frag");
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------

  post_light.use();
  post_light.setInt("screenTexture", 0);


  static float cubeVertices[] = {
      // FRONT (Z+)
      -0.5f, -0.5f, 0.5f, 0, 0, 1, 0, 0, 1, 0, 0,
      0.5f, -0.5f, 0.5f, 0, 0, 1, 1, 0, 1, 0, 0,
      0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 1, 1, 0, 0,
      0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 1, 1, 0, 0,
      -0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 1, 1, 0, 0,
      -0.5f, -0.5f, 0.5f, 0, 0, 1, 0, 0, 1, 0, 0,

      // BACK (Z-)
      0.5f, -0.5f, -0.5f, 0, 0, -1, 0, 0, -1, 0, 0,
      -0.5f, -0.5f, -0.5f, 0, 0, -1, 1, 0, -1, 0, 0,
      -0.5f, 0.5f, -0.5f, 0, 0, -1, 1, 1, -1, 0, 0,
      -0.5f, 0.5f, -0.5f, 0, 0, -1, 1, 1, -1, 0, 0,
      0.5f, 0.5f, -0.5f, 0, 0, -1, 0, 1, -1, 0, 0,
      0.5f, -0.5f, -0.5f, 0, 0, -1, 0, 0, -1, 0, 0,

      // RIGHT (X+)
      0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 0, 0, 0, -1,
      0.5f, -0.5f, -0.5f, 1, 0, 0, 1, 0, 0, 0, -1,
      0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 1, 0, 0, -1,
      0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 1, 0, 0, -1,
      0.5f, 0.5f, 0.5f, 1, 0, 0, 0, 1, 0, 0, -1,
      0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 0, 0, 0, -1,

      // LEFT (X-)
      -0.5f, -0.5f, -0.5f, -1, 0, 0, 0, 0, 0, 0, 1,
      -0.5f, -0.5f, 0.5f, -1, 0, 0, 1, 0, 0, 0, 1,
      -0.5f, 0.5f, 0.5f, -1, 0, 0, 1, 1, 0, 0, 1,
      -0.5f, 0.5f, 0.5f, -1, 0, 0, 1, 1, 0, 0, 1,
      -0.5f, 0.5f, -0.5f, -1, 0, 0, 0, 1, 0, 0, 1,
      -0.5f, -0.5f, -0.5f, -1, 0, 0, 0, 0, 0, 0, 1,

      // TOP (Y+)
      -0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 0, 1, 0, 0,
      0.5f, 0.5f, 0.5f, 0, 1, 0, 1, 0, 1, 0, 0,
      0.5f, 0.5f, -0.5f, 0, 1, 0, 1, 1, 1, 0, 0,
      0.5f, 0.5f, -0.5f, 0, 1, 0, 1, 1, 1, 0, 0,
      -0.5f, 0.5f, -0.5f, 0, 1, 0, 0, 1, 1, 0, 0,
      -0.5f, 0.5f, 0.5f, 0, 1, 0, 0, 0, 1, 0, 0,

      // BOTTOM (Y-)
      -0.5f, -0.5f, -0.5f, 0, -1, 0, 0, 0, 1, 0, 0,
      0.5f, -0.5f, -0.5f, 0, -1, 0, 1, 0, 1, 0, 0,
      0.5f, -0.5f, 0.5f, 0, -1, 0, 1, 1, 1, 0, 0,
      0.5f, -0.5f, 0.5f, 0, -1, 0, 1, 1, 1, 0, 0,
      -0.5f, -0.5f, 0.5f, 0, -1, 0, 0, 1, 1, 0, 0,
      -0.5f, -0.5f, -0.5f, 0, -1, 0, 0, 0, 1, 0, 0,
  };

  float verticesLight[] = {

      // FRONT FACE (Z+)
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

      // BACK FACE (Z-)
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

      // RIGHT FACE (X+)
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

      // Triangle 2
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

      // LEFT FACE (X-)
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

      // TOP FACE (Y+)
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

      // BOTTOM FACE (Y-)
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f
  };

  // first, configure the cube's VAO (and VBO)
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
               GL_STATIC_DRAW);

  constexpr int stride = 11 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                        (void*)(8 * sizeof(float)));
  glEnableVertexAttribArray(3);

  glBindVertexArray(0);

  int widthD, heightD, nrChannelsD;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* dataD = stbi_load((path_texture + "logo_diffuse.jpg").c_str(),
                                   &widthD, &heightD,
                                   &nrChannelsD, 0);
  if (!dataD) {
    std::cerr << "Failed to load texture " + path_texture + "logo_diffuse.jpg" +
        "\n";
    return;
  }

  int widthS, heightS, nrChannelsS;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* dataS = stbi_load(
      (path_texture + "logo_specular.jpg").c_str(), &widthS, &heightS,
      &nrChannelsS, 0);
  if (!dataS) {
    std::cerr << "Failed to load texture " + path_texture +
        "logo_specular.jpg" + "\n";
    return;
  }

  int widthN, heightN, nrChannelsN;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* dataN = stbi_load(
      (path_texture + "logo_normal.jpg").c_str(), &widthN, &heightN,
      &nrChannelsN, 0);
  if (!dataN) {
    std::cerr << "Failed to load texture " + path_texture +
        "logo_normal.jpg" + "\n";
    return;
  }

  glGenTextures(1, &textureDiffuse);
  glGenTextures(1, &textureSpecular);
  glGenTextures(1, &textureNormal);

  glBindTexture(GL_TEXTURE_2D, textureDiffuse);
  GLint formatD = (nrChannelsD == 3) ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, formatD, widthD, heightD, 0, formatD,
               GL_UNSIGNED_BYTE, dataD);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(dataD);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, textureSpecular);
  GLint formatS = (nrChannelsD == 3) ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, formatS, widthS, heightS, 0, formatS,
               GL_UNSIGNED_BYTE, dataS);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(dataS);
  glBindTexture(GL_TEXTURE_2D, 1);

  glBindTexture(GL_TEXTURE_2D, textureNormal);
  GLint formatN = (nrChannelsN == 3) ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, formatN, widthN, heightN, 0, formatN,
               GL_UNSIGNED_BYTE, dataN);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(dataN);
  glBindTexture(GL_TEXTURE_2D, 2);

  glGenVertexArrays(1, &lightCubeVAO);
  glGenBuffers(1, &VBOLight);

  glBindBuffer(GL_ARRAY_BUFFER, VBOLight);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLight), verticesLight,
               GL_STATIC_DRAW);

  glBindVertexArray(lightCubeVAO);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions  // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

  // texture couleur (FRAMEBUFFER → PAS DE MIPMAP)
  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);

  // ⬇⬇⬇ IMPORTANT : désactiver les mipmaps ⬇⬇⬇
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               common::GetWindowSize().x, common::GetWindowSize().y, 0,
               GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, colorBuffer, 0);

  // depth
  glGenRenderbuffers(1, &depthRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, common::GetWindowSize().x, common::GetWindowSize().y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, depthRBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Framebuffer incomplete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeLight::End() {
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &lightCubeVAO);
  glDeleteBuffers(1, &VBO);
}

void CubeLight::Update(float dt) {
  static float time = 0;
  time += dt;

  glm::vec3 center = glm::vec3(0.0f); // position du cube
  float     radius = 2.0f;

  //orbite autour du cube
  lightPos.x = center.x + sin(time * 2) * radius;
  lightPos.z = center.z + cos(time * 2) * radius;
  lightPos.y = center.y + 0.5f;

  model = glm::mat4(1);
  model = glm::translate(model, center);
  model = glm::rotate(model, time / 2, glm::vec3(1, 1, 0));
}

void CubeLight::FixedUpdate() {
}

void CubeLight::Draw() {
    // --- PRÉPARATION ---
    float currentFrame = common::GetDeltaTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            (float)common::GetWindowSize().x / (float)common::GetWindowSize().y,
                                            0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);

    // --- ÉTAPE 1 : RENDU DU CUBE PRINCIPAL SUR L'ÉCRAN ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Cube.use();
    Cube.setMat4("projection", projection);
    Cube.setMat4("view", view);
    Cube.setMat4("model", model);
    Cube.setVec3("lightPos", lightPos);
    Cube.setVec3("viewPos", camPos);
    Cube.setVec3("lightColor", glm::vec3(1.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureDiffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecular);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureNormal);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // --- ÉTAPE 2 : COPIE DE LA PROFONDEUR ---
    // On copie la profondeur de l'écran vers le Framebuffer pour que la lampe
    // sache qu'elle est derrière le cube.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    glBlitFramebuffer(0, 0, common::GetWindowSize().x, common::GetWindowSize().y,
                      0, 0, common::GetWindowSize().x, common::GetWindowSize().y,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // --- ÉTAPE 3 : RENDU DE LA LAMPE DANS LE FRAMEBUFFER (ISOLATION) ---
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // On nettoie UNIQUEMENT la couleur (noir), on garde la profondeur copiée
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST); // On laisse le test de profondeur activé !

    cube_light.use();
    cube_light.setMat4("projection", projection);
    cube_light.setMat4("view", view);
    glm::mat4 lModel = glm::mat4(1.0f);
    lModel = glm::translate(lModel, lightPos);
    lModel = glm::scale(lModel, glm::vec3(0.2f));
    cube_light.setMat4("model", lModel);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // --- ÉTAPE 4 : POST-PROCESS (FLOU ADDITIF) ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // Le quad de post-process ne doit pas être bloqué par la profondeur

    // On active le Blending Additif pour que la lumière "brille" sur le cube
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    post_light.use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer); // On bind la texture de la lampe seule
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
}
}