//
// Created by unite on 04.12.2025.
//

#include "renderer/shader.h"

#include <format>
#include <stdexcept>
#include <iostream>

#include "third_party/sdl3_include.h"

namespace common {

void Shader::Load(std::string_view shader_path,
                                GLenum shader_stage) {
  size_t data_size = 0;
  auto* shader_content = static_cast<const char*>(SDL_LoadFile(shader_path.data(), &data_size));
  if (shader_content == nullptr) {
    throw std::runtime_error(std::format("Failed to load shader {}", shader_path));
  }
  LoadFromMemory(shader_content, shader_stage);
}
void Shader::LoadFromMemory(const GLchar* string,
                            GLenum shader_stage) {
  auto& info = get();
  info.shader_stage = shader_stage;
  info.shader_name = glCreateShader(shader_stage);
  glShaderSource(info.shader_name, 1, &string, nullptr);
  glCompileShader(info.shader_name);
  GLint compile_status;
  glGetShaderiv(info.shader_name, GL_COMPILE_STATUS, &compile_status);
  if (!compile_status) {
    std::string buffer{};
    buffer.resize(512);
    glGetShaderInfoLog(info.shader_name, static_cast<GLsizei>(buffer.size()), nullptr, buffer.data());
    std::cerr << "Shader compilation error: " << buffer << std::endl;
    throw std::runtime_error("Failed to compile shader");
  }
}
}  // namespace common
