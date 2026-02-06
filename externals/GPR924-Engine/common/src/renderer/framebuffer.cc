
/*
Copyright 2026 SAE Institute Switzerland SA

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

#include "renderer/framebuffer.h"

#include <imgui.h>

#include <iostream>

namespace common {

void Renderbuffer::Load(GLenum internal_format, core::Vec2I size) {
  GLenum attachment = GL_NONE;
  if (is_depth_format(internal_format)) {
    attachment = GL_DEPTH_COMPONENT;
  }
  else if (is_depth_or_depth_stencil_format(internal_format)) {
    attachment = GL_DEPTH_STENCIL;
  }
  glGenRenderbuffers(1, &name_);
  glBindRenderbuffer(GL_RENDERBUFFER, name_);
  glRenderbufferStorage(
      GL_RENDERBUFFER, internal_format,
      size.x, size.y);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment,
                            GL_RENDERBUFFER, name_);
}
void Renderbuffer::Clear() {
  glDeleteRenderbuffers(1, &name_);
  name_ = 0;
}
void Framebuffer::Load(const FramebufferCreateInfo& info) {
  framebuffer_create_info_ = info;
  glGenFramebuffers(1, &get().framebuffer_name);
  Bind();
  std::array<GLenum, 16> color_attachment_buffers{};
  int attachment_index = 0;
  for (size_t i = 0; i < std::size(get().color_attachments); ++i) {
    const GLenum color_attachment_index = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
    const auto& color_attachment_info = info.color_attachment_infos[i];
    if (!color_attachment_info.is_active) {
      continue;
    }
    if (is_depth_or_depth_stencil_format(color_attachment_info.internal_format)) {
      throw std::invalid_argument("Color attachment is not supposed to be a depth or depth-stencil format");
    }
    auto& color_attachment = get().color_attachments[i];
    color_attachment.Create(
        color_attachment_info.target, framebuffer_create_info_.size.x,
        framebuffer_create_info_.size.y,
        color_attachment_info.internal_format);
    glFramebufferTexture2D(GL_FRAMEBUFFER, color_attachment_index,
                           color_attachment_info.target,
                           color_attachment->texture_name, 0);
    color_attachment_buffers[attachment_index] = color_attachment_index;
    attachment_index++;
  }
  if (attachment_index == 0) {
    GLenum drawBuffers = GL_NONE;
    glDrawBuffers(1, &drawBuffers);
    glReadBuffer(GL_NONE);
  }
  else {
    glDrawBuffers(attachment_index, color_attachment_buffers.data());
  }
  if (framebuffer_create_info_.depth_stencil_attachment_info.is_active) {
    const auto& depth_attachment_info =
        framebuffer_create_info_.depth_stencil_attachment_info;
    if (!is_depth_or_depth_stencil_format(depth_attachment_info.internal_format)) {
      throw std::invalid_argument("Depth attachment is supposed to be a depth or depth-stencil internal format type");
    }
    if (depth_attachment_info.is_rbo) {
      Renderbuffer depth_stencil_rbo;
      depth_stencil_rbo.Load(depth_attachment_info.internal_format, framebuffer_create_info_.size);
      get().depth_stencil_attachment = std::move(depth_stencil_rbo);
    } else {
      Texture depth_stencil_texture;
      depth_stencil_texture.Create(depth_attachment_info.target, framebuffer_create_info_.size.x,
        framebuffer_create_info_.size.y, depth_attachment_info.internal_format);
      get().depth_stencil_attachment = std::move(depth_stencil_texture);
    }
  }
  const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::string log;
    switch (status) {
      case GL_FRAMEBUFFER_UNDEFINED:
        std::cerr << "Framebuffer is undefined!\n";
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cerr << "Framebuffer is unsupported!";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cerr << "Framebuffer has incomplete attachment!";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cerr << "Framebuffer has incomplete missing attachment!";
        break;
      default:
        break;
    }
  }
}

void Framebuffer::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, get().framebuffer_name);
}

void Framebuffer::BindBackbuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void Framebuffer::Reload(core::Vec2I newSize) {
  framebuffer_create_info_.size = newSize;
  Clear();
  Load(framebuffer_create_info_);
}
const Texture& Framebuffer::GetColorAttachment(int index) {
  return get().color_attachments[index];
}
const Texture& Framebuffer::GetDepthAttachment() {
  //if you throw here, it means you forgot to put the depth-stencil is_rbo as false...
  return std::get<Texture>(get().depth_stencil_attachment);
}
}  // namespace common