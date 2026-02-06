#ifndef GPR924_ENGINE_SHADER_H
#define GPR924_ENGINE_SHADER_H

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

#include <string_view>

#include "resource/resource.h"
#include "third_party/gl_include.h"

namespace common {

struct ShaderInfo {
  GLuint shader_name;
  GLenum shader_stage;
};

class ShaderDestructor {
 public:
  explicit ShaderDestructor(ShaderInfo& shader_info)
      : shader_info_(shader_info) {};
  void operator()() const {
    glDeleteShader(shader_info_.shader_name);
    shader_info_ = {};
  }

 private:
  ShaderInfo& shader_info_;
};

class Shader : public core::Resource<ShaderInfo, ShaderDestructor> {
 public:
  void Load(std::string_view shader_path, GLenum shader_stage);
  void LoadFromMemory(const GLchar * string, GLenum shader_stage);
};

}  // namespace common

#endif  // GPR924_ENGINE_SHADER_H
