#include "0-triangle_sans_vbo.h"

#include <string>
#include "third_party/gl_include.h"

namespace graphics {
namespace {
std::string path = "data/shaders/";
}

void TriangleSansVbo::Begin() {
  common::Shader vertex;

  common::Shader fragment;

  vertex.Load(path + "0-triangle_sans_vbo.vert", GL_VERTEX_SHADER);
  fragment.Load(path + "0-triangle_sans_vbo.frag", GL_FRAGMENT_SHADER);

  pipeline.Load(vertex, fragment);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
}

void TriangleSansVbo::End() {
  pipeline.Clear();
  glDeleteVertexArrays(1, &vao);
}

void TriangleSansVbo::Update(float dt) {
}

void TriangleSansVbo::FixedUpdate() {
}

void TriangleSansVbo::Draw() {
  pipeline.Bind();
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}
}