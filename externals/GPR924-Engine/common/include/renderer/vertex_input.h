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

#ifndef GPR924_ENGINE_VERTEX_INPUT_H
#define GPR924_ENGINE_VERTEX_INPUT_H

#include <ranges>

#include "resource/resource.h"
#include "third_party/gl_include.h"

namespace common {

struct BufferInfo {
  GLuint name;
};

class BufferDestructor {
 public:
  explicit BufferDestructor(BufferInfo& buffer) : buffer_(buffer) {}
  void operator()() const {
    glDeleteBuffers(1, &buffer_.name);
    buffer_.name = 0;
  }

 private:
  BufferInfo& buffer_;
};
template<GLenum target>
class Buffer : public core::Resource<BufferInfo, BufferDestructor> {
public:
  void Bind() const {
    glBindBuffer(target, get().name);
  }
  void Load() {
    glGenBuffers(1, &get().name);
  }
  /**
 * @brief Allocate and upload the data in the buffer
 */
  template<typename T>
  void UploadRange(std::span<const T> range, GLenum usage = GL_STATIC_DRAW) {
    Bind();
    glBufferData(target, range.size_bytes(), range.data(), usage);
  }
  /**
   * @brief Update the data in the buffer (to be used after using UploadRange
   */
  template <typename T>
  void UpdateRange(std::span<const T> range, GLintptr offset = 0, GLenum usage = GL_STATIC_DRAW) {
    glBufferSubData(target, offset, range.size_bytes(), range.data(), usage);
  }

};

using VertexBuffer = Buffer<GL_ARRAY_BUFFER>;


struct VertexBufferAttribute {
  GLuint location;
  GLint size;
  GLenum type;
  GLsizei stride;
  size_t offset;
};

using IndexBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER>;

struct VertexInputInfo {
  GLuint vao = 0;
};

class VertexInputDestructor {
 public:
  explicit VertexInputDestructor(VertexInputInfo& vertex_input_info)
      : vertex_input_info_(vertex_input_info) {}
  void operator()() const {
    glDeleteVertexArrays(1, &vertex_input_info_.vao);
    vertex_input_info_.vao = 0;
  }

 private:
  VertexInputInfo& vertex_input_info_;
};

class VertexInput
    : public core::Resource<VertexInputInfo, VertexInputDestructor> {
 public:
  void Load();
  void BindVertexBuffer(const VertexBuffer& vbo, std::span<const VertexBufferAttribute> attributes);
  void BindIndexBuffer(const IndexBuffer& ebo);
  void Bind();
};

}  // namespace common
#endif  // GPR924_ENGINE_VERTEX_INPUT_H
