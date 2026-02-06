#ifndef GPR924_ENGINE_PIPELINE_H
#define GPR924_ENGINE_PIPELINE_H


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

#include <string>
#include <string_view>
#include <unordered_map>

#include "maths/vec2.h"
#include "maths/vec3.h"
#include "resource/resource.h"
#include "shader.h"
#include "texture.h"
#include "third_party/gl_include.h"

namespace common {

struct PipelineInfo {
  GLuint pipeline_name = 0;
};

class PipelineDestructor {
public:
  explicit PipelineDestructor(PipelineInfo& pipeline_info): pipeline_info_(pipeline_info) {}
  void operator()() const {
    glDeleteProgram(pipeline_info_.pipeline_name);
    pipeline_info_.pipeline_name = 0;
  }
private:
  PipelineInfo& pipeline_info_;
};

class Pipeline final : public core::Resource<PipelineInfo, PipelineDestructor> {
public:
  void Load(const Shader& vertex_shader, const Shader& fragment_shader);
  void Bind();

  void SetBool(std::string_view name, bool value);
  void SetInt(std::string_view name, int value);
  void SetFloat(std::string_view  name, float value);
  void SetVec3(const char* name, float x, float y, float z);

  void SetTexture(std::string_view name, const Texture& texture, int texture_unit = 0);

  template<typename T>
requires core::IsVector2<T, float>
void SetVec2(const std::string_view uniform_name, const T& v) {
    const GLint loc = GetUniformLocation(uniform_name);
    glUniform2f(loc, v.x, v.y);
  }

  template<typename T>
requires core::IsVector2<T, int>
void SetVec2(const std::string_view uniform_name, const T& v) {
    const GLint loc = GetUniformLocation(uniform_name);
    glUniform2f(loc, v.x, v.y);
  }

  template<typename T>
  requires core::IsVector3<T, float>
  void SetVec3(const std::string_view uniform_name, const T& v) {
    const GLint loc = GetUniformLocation(uniform_name);
    glUniform3f(loc, v.x, v.y, v.z);
  }

  template<typename T>
  requires core::IsVector3<T, int>
  void SetVec3(const std::string_view uniform_name, const T& v) {
    const GLint loc = GetUniformLocation(uniform_name);
    glUniform3i(loc, v.x, v.y, v.z);
  }
  void SetMat4(const char* name, const float* mat);
private:
  struct StringHash {
    using is_transparent = void;  // Enable heterogeneous lookup

    [[nodiscard]] size_t operator()(const char* txt) const {
      return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(std::string_view txt) const {
      return std::hash<std::string_view>{}(txt);
    }
    [[nodiscard]] size_t operator()(const std::string& txt) const {
      return std::hash<std::string>{}(txt);
    }
  };

  struct StringEqual {
    using is_transparent = void;  // Enable heterogeneous lookup

    [[nodiscard]] bool operator()(std::string_view lhs, std::string_view rhs) const {
      return lhs == rhs;
    }
  };
  GLint GetUniformLocation(std::string_view name);
  std::unordered_map<std::string, GLint, StringHash, StringEqual> uniform_location_map_;
};
}
#endif  // GPR924_ENGINE_PIPELINE_H
