/*
Copyright 2025 SAE Institute Switzerland SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Contributors: Elias Farhan
*/

#include "renderer/vertex_input.h"

namespace common {

namespace {
GLuint current_binded_vao = 0;
}

// void VertexBuffer::Load() {
//   glGenBuffers(1, &get().name);
// }
// void VertexBuffer::Bind() const {
//   glBindBuffer(GL_ARRAY_BUFFER, get().name);
// }
// void IndexBuffer::Load() {
//   glGenBuffers(1, &get().name);
// }
// void IndexBuffer::Bind() const {
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, get().name);
// }
void VertexInput::Load() {
  auto& info = get();
  glGenVertexArrays(1, &info.vao);
}
void VertexInput::BindVertexBuffer(
    const VertexBuffer& vbo,
    std::span<const VertexBufferAttribute> attributes) {
  Bind();
  vbo.Bind();
  for (const auto& attribute : attributes) {
    glVertexAttribPointer(attribute.location, attribute.size, attribute.type, GL_FALSE, attribute.stride,
                          reinterpret_cast<void*>(attribute.offset));
    glEnableVertexAttribArray(attribute.location);
  }
}
void VertexInput::BindIndexBuffer(const IndexBuffer& ebo) {
  Bind();
  ebo.Bind();
}
void VertexInput::Bind() {
  if (current_binded_vao != get().vao) {
    glBindVertexArray(get().vao);
    current_binded_vao = get().vao;
  }
}
}  // namespace common