#include "3-square_avec_texture.h"

#include <iostream>
#include <stb_image.h>

namespace graphics {
namespace {
std::string path = "data/shaders/";
}

void SquareAvecTexture::Begin() {
  common::Shader vertex;
  vertex.Load(path + "3-triangle_avec_texture.vert", GL_VERTEX_SHADER);

  common::Shader fragment;
  fragment.Load(path + "3-triangle_avec_texture.frag", GL_FRAGMENT_SHADER);

  pipeline.Load(vertex, fragment);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  constexpr float vertices[] = {
      //Positions     //TexCoords     //Colors
      -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Top-Left
      0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-Right
      0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, // Down-Right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f // Down-Left
  };

  constexpr unsigned int indices[] = {
      0, 1, 2,
      0, 2, 3
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  constexpr GLsizei stride = 7 * sizeof(float);

  int width = 0, height = 0, nrChannels = 0;
  stbi_set_flip_vertically_on_load(true);
  const std::string final_path = "data/textures/brickwall.jpg";
  unsigned char*    data = stbi_load(final_path.c_str(), &width, &height,
                                     &nrChannels, 0);
  if (!data) {
    std::cerr << "Failed to load texture " << final_path << "\n";
    return;
  }

  if (texture == 0) glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);

  GLint format = GL_RGBA;
  if (nrChannels == 3) format = GL_RGB;
  else if (nrChannels == 4) format = GL_RGBA;
  else if (nrChannels == 1) format = GL_RED;

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

  //Position attributes
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
  glEnableVertexAttribArray(0);

  //Texture attributes
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  //Color attributes
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(4 * sizeof(float)));
  glEnableVertexAttribArray(2);

  //Unbind
  glBindVertexArray(0);
}

void SquareAvecTexture::End() {
}

void SquareAvecTexture::Update(float dt) {
}

void SquareAvecTexture::FixedUpdate() {
}

void SquareAvecTexture::Draw() {
  pipeline.Bind();
  if(const GLint loc = glGetUniformLocation(pipeline.get().pipeline_name, "colorCoeff"); loc != -1)
  {
    constexpr float colorValue = 1.0f;
    glUniform1f(loc, colorValue);
  }
  glBindVertexArray(vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (const GLint texLoc = glGetUniformLocation(pipeline.get().pipeline_name,
                                                "texture0"); texLoc != -1) {
    glUniform1i(texLoc, 0);
  }
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
}