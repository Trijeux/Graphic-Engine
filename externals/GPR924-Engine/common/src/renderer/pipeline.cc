
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

Contributors: Elias Farhan, Jemoel Ablay
*/

#include "renderer/pipeline.h"

#include <iostream>
#include <ostream>
#include <string>

namespace common {

namespace {
GLuint binded_pipeline_name = 0;
}
void Pipeline::Load(const Shader& vertex_shader,
                    const Shader& fragment_shader) {
  auto& pipelineInfo = get();
  pipelineInfo.pipeline_name = glCreateProgram();
  if (vertex_shader->shader_name == 0) {
    throw std::runtime_error("Vertex shader was not loaded");
  }
  if (fragment_shader->shader_name == 0) {
    throw std::runtime_error("Fragment shader was not loaded");
  }
  glAttachShader(pipelineInfo.pipeline_name, vertex_shader->shader_name);
  glAttachShader(pipelineInfo.pipeline_name, fragment_shader->shader_name);
  glLinkProgram(pipelineInfo.pipeline_name);
  GLint program_linked;
  glGetProgramiv(pipelineInfo.pipeline_name, GL_LINK_STATUS, &program_linked);
  if (program_linked != GL_TRUE)
  {
    GLsizei log_length = 0;
    std::string message( 1024, 0);
    glGetProgramInfoLog(pipelineInfo.pipeline_name,
      static_cast<GLsizei>(message.size()),
      &log_length,
      message.data());
    std::cerr << message << std::endl;
    throw std::runtime_error("Error trying to link pipeline");
  }
}
void Pipeline::Bind() {
  if (binded_pipeline_name != get().pipeline_name) {
    glUseProgram(get().pipeline_name);
    binded_pipeline_name = get().pipeline_name;
  }
}

void Pipeline::SetBool(const std::string_view name, const bool value)
{
  const GLint loc = GetUniformLocation(name);
  glUniform1i(loc, static_cast<GLint>(value));
}

void Pipeline::SetInt(const std::string_view  name, const int value)
{
  const GLint loc = GetUniformLocation(name);
  glUniform1i(loc, value);
}

void Pipeline::SetFloat(const std::string_view  name, const float value)
{
  const GLint loc = GetUniformLocation(name);
  glUniform1f(loc, value);
}

void Pipeline::SetVec3(const char* name, const float x, const float y, const float z)
{
  const GLint loc = GetUniformLocation(name);
  glUniform3f(loc, x, y, z);
}
void Pipeline::SetTexture(std::string_view name, const Texture& texture,
                          int texture_unit) {
  SetInt(name.data(), texture_unit);
  glActiveTexture(GL_TEXTURE0+texture_unit);
  texture.Bind();
}

void Pipeline::SetMat4(const char* name, const float* mat)
{
  const GLint loc = GetUniformLocation(name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}
GLint Pipeline::GetUniformLocation(std::string_view name) {

  auto it = uniform_location_map_.find(name);
  if (it == uniform_location_map_.end()) {
    const GLint loc = glGetUniformLocation(get().pipeline_name, name.data());
    uniform_location_map_.insert({name.data(), loc});
    return loc;
  }
  return it->second;
}
}  // namespace common