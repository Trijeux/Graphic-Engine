#ifndef GPR924_ENGINE_FRAMEBUFFER_H
#define GPR924_ENGINE_FRAMEBUFFER_H
#include <GL/glew.h>

#include <array>
#include <variant>

#include "maths/vec2.h"
#include "texture.h"
#include "vertex_input.h"

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

namespace common {

struct ColorAttachmentInfo {
  GLenum target = GL_TEXTURE_2D;
  GLint internal_format = GL_RGB8;
  bool is_active = false;
};

struct DepthAttachmentInfo {
  GLenum target = GL_TEXTURE_2D;
  GLenum internal_format = GL_DEPTH24_STENCIL8;
  bool is_active = false;
  bool is_rbo = true;
};

struct FramebufferCreateInfo {
  std::array<ColorAttachmentInfo, 16> color_attachment_infos{};
  DepthAttachmentInfo depth_stencil_attachment_info{};
  core::Vec2I size{};
};

class Renderbuffer {
public:
  void Load(GLenum internal_format, core::Vec2I size);
  void Clear();
private:
  GLuint name_ = 0;
};

struct FramebufferInfo {
  GLuint framebuffer_name;

  std::array<Texture, 16> color_attachments;
  std::variant<Texture, Renderbuffer> depth_stencil_attachment;
};

struct FramebufferDestructor {
  explicit FramebufferDestructor(FramebufferInfo& framebuffer_info)
      : framebuffer_info_(framebuffer_info) {}
  void operator()() const {
    glDeleteFramebuffers(1, &framebuffer_info_.framebuffer_name);
    framebuffer_info_.framebuffer_name = 0;

    for (auto& color_attachment : framebuffer_info_.color_attachments) {
      color_attachment.Clear();
    }
    std::visit([](auto& arg){ arg.Clear();}, framebuffer_info_.depth_stencil_attachment);
  }
 private:
  FramebufferInfo& framebuffer_info_;
};

class Framebuffer
    : public core::Resource<FramebufferInfo, FramebufferDestructor> {
 public:
  void Load(const FramebufferCreateInfo& info);
  void Bind();
  static void BindBackbuffer();
  void Reload(core::Vec2I newSize);

  const Texture& GetColorAttachment(int index);
  const Texture& GetDepthAttachment();
 private:
  FramebufferCreateInfo framebuffer_create_info_{};
};

}  // namespace common

#endif  // GPR924_ENGINE_FRAMEBUFFER_H
