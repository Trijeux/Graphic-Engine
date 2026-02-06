#include "1-triangle_avec_vbo.h"

#include <fstream>
#include <string>
#include "third_party/gl_include.h"

namespace graphics {
namespace {
std::string path = "data/shaders/";
}

void TriangleAvecVbo::Begin() {
  common::Shader vertex;

  common::Shader fragment;

  vertex.Load(path + "1-2-triangle_avec_vbo.vert", GL_VERTEX_SHADER);
  fragment.Load(path + "1-2-triangle_avec_vbo.frag", GL_FRAGMENT_SHADER);

  pipeline.Load(vertex, fragment);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  constexpr float vertices[] = {
      0.0f, 0.5f, /*Pos*/ 0.0f, 0.0f, 1.0f /*Blue*/,
      0.5f, -0.5f, /*Pos*/ 1.0f, 0.0f, 0.0f /*Red*/,
      -0.5f, -0.5f, /*Pos*/ 0.0f, 1.0f, 0.0f /*Green*/,
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void TriangleAvecVbo::End() {
  pipeline.Clear();
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
}

void TriangleAvecVbo::Update(float dt) {
}

void TriangleAvecVbo::FixedUpdate() {
}

void TriangleAvecVbo::Draw() {
  pipeline.Bind();
  if(const GLint loc = glGetUniformLocation(pipeline.get().pipeline_name, "colorCoeff"); loc != -1)
  {
    constexpr float colorValue = 1.0f;
    glUniform1f(loc, colorValue);
  }
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}
}