#include "4-cube_3d.h"

#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/engine.h"
#include "engine/window.h"
#include "renderer/shader.h"

namespace graphics {
namespace {
std::string path_shader = "data/shaders/";
}

void Cube::Begin() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  for (unsigned int& texture : textures) {
    glGenTextures(1, &texture);
  }

  common::Shader vertex;
  vertex.Load(path_shader + "4-5-cube_3d.vert", GL_VERTEX_SHADER);

  common::Shader fragment;
  fragment.Load(path_shader + "4-5-cube_3d.frag", GL_FRAGMENT_SHADER);
  pipeline.Load(vertex, fragment);

  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  float vertices[] = {
      // Face arrière (Z-)
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Face avant (Z+)
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,

      // Face gauche (X-)
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Face droite (X+)
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f,

      // Face dessous (Y-)
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

      // Face dessus (Y+)
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f
  };

  unsigned int indices[] = {
      // Face arrière
      0, 1, 2,
      2, 3, 0,

      // Face avant
      4, 5, 6,
      6, 7, 4,

      // Face gauche
      8, 9, 10,
      10, 11, 8,

      // Face droite
      12, 13, 14,
      14, 15, 12,

      // Face dessous
      16, 17, 18,
      18, 19, 16,

      // Face dessus
      20, 21, 22,
      22, 23, 20
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  for (int i = 0; i < 7; ++i) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path_files[i].data(), &width, &height,
                                    &nrChannels, 0);
    if (!data) {
      std::cerr << "Failed to load texture " + path_files[i] + "\n";
      return;
    }

    glGenTextures(1, &textures[i]);
    glBindTexture(GL_TEXTURE_2D, textures[i]);

    GLint format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  constexpr GLsizei stride = 5 * sizeof(float);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        static_cast<void*>(nullptr));
  glEnableVertexAttribArray(0);

  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glEnable(GL_DEPTH_TEST);

  glBindVertexArray(0);
}

void Cube::End() {
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  for (unsigned int& texture : textures) {
    glDeleteTextures(1, &texture);
  }
  pipeline.Clear();
}

void Cube::Update(float dt) {
  time += dt;
  auto view = glm::mat4(1.0f);
  auto projection = glm::mat4(1.0f);

  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  projection = glm::perspective(glm::radians(45.0f),
                                (float)common::GetWindowSize().x / (float)
                                common::GetWindowSize().x, 0.1f, 100.0f);
  // retrieve the matrix uniform locations

  const GLint viewLoc = glGetUniformLocation(pipeline.get().pipeline_name, "view");
  // pass them to the shaders (3 different ways)

  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
  // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
  glUniformMatrix4fv(glGetUniformLocation(pipeline.get().pipeline_name, "projection"), 1, GL_FALSE,
                     &projection[0][0]);
}

void Cube::FixedUpdate() {
}

void Cube::Draw() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);

  pipeline.Bind();

  glBindVertexArray(vao);

  for (unsigned int i = 0; i < 7; i++) {
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);
    if (i % 2 == 0) {
      model = glm::rotate(model, time, glm::vec3(0.5f * (i + 1), 1.0f, 0.0f));
    } else {
      model = glm::rotate(model, time, glm::vec3(-0.5f * (i + 1), 1.0f, 0.0f));
    }
    const unsigned int modelLoc = glGetUniformLocation(pipeline.get().pipeline_name, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(pipeline.get().pipeline_name, "model"), 1, GL_FALSE,
                       &model[0][0]);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  }
}
}