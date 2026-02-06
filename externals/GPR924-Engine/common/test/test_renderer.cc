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

#include "engine/engine.h"
#include "engine/renderer.h"
#include "engine/system.h"
#include "engine/window.h"
#include "gtest/gtest.h"
#include "renderer/pipeline.h"
#include "renderer/vertex_input.h"

namespace test_renderer {
namespace {
constexpr std::string_view vertexShaderContent =
"#version 300 es\n"
"precision mediump float;"
"layout (location = 0) in vec2 aPos;"
"layout (location = 1) in vec2 aTexCoords;"
"out vec2 TexCoords;"
"void main()"
"{"
"  gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);"
"  TexCoords = aTexCoords;"
"}";

constexpr std::string_view fragmentShaderContent =
"#version 300 es\n"
"precision mediump float;"
"out vec4 FragColor;"
"in vec2 TexCoords;"
"void main()"
"{"
  "FragColor = vec4(TexCoords.x,TexCoords.y,1.0f, 1.0f);"
"}";

constexpr std::array vertices = {
  // pos + tex coords
  0.5f,  0.5f, 1.0f, 1.0f,
  0.5f, -0.5f, 1.0f, 0.0f,
 -0.5f, -0.5f, 0.0f, 0.0f,
 -0.5f,  0.5f, 0.0f, 1.0f,
   };
constexpr std::array indices = {
  // note that we start from 0!
  0, 1, 3,   // first triangle
  1, 2, 3    // second triangle
};

constexpr std::array attributes = {
  common::VertexBufferAttribute{.location = 0, .size = 2, .type = GL_FLOAT, .stride = sizeof(float)*4, .offset = 0},
  common::VertexBufferAttribute{.location = 1, .size = 2, .type = GL_FLOAT, .stride = sizeof(float)*4, .offset = sizeof(float)*2},
};
}

class ClosingSystem :  public common::DrawInterface , public common::SystemInterface{
public:
  void Begin() override {
    common::Shader vertexShader;
    vertexShader.LoadFromMemory(vertexShaderContent.data(), GL_VERTEX_SHADER);

    common::Shader fragmentShader;
    fragmentShader.LoadFromMemory(fragmentShaderContent.data(), GL_FRAGMENT_SHADER);

    pipeline_.Load(vertexShader, fragmentShader);

    vao_.Load();
    vbo_.Load();
    vbo_.UploadRange(vertices);
    ebo_.Load();
    ebo_.UploadRange(indices);
    vao_.BindIndexBuffer(ebo_);
    vao_.BindVertexBuffer(vbo_, attributes);

  }
  void End() override {
    pipeline_.Clear();
    vao_.Clear();
    vbo_.Clear();
    ebo_.Clear();
  }
  void Update([[maybe_unused]] float dt) override {
    ++count_;
    if (count_ > kCloseCount) {
      common::CloseWindow();
    }
  }
  void FixedUpdate() override {
    ++count_;
    if (count_ > kCloseCount) {
      common::CloseWindow();
    }
  }
  void Draw() override {
    vao_.Bind();
    pipeline_.Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

protected:
  static constexpr int kCloseCount = 100;
  common::Pipeline pipeline_;
  common::VertexInput vao_;
  common::VertexBuffer vbo_;
  common::IndexBuffer ebo_;

  int count_ = 0;
};
}
TEST(GL, Renderer) {
  test_renderer::ClosingSystem closingSystem{};

  auto windowConfig = common::GetWindowConfig();
  windowConfig.renderer = common::WindowConfig::RendererType::OPENGLES;
  common::SetWindowConfig(windowConfig);
  common::SystemObserverSubject::AddObserver(&closingSystem);
  common::DrawObserverSubject::AddObserver(&closingSystem);
  common::RunEngine();
  common::SystemObserverSubject::RemoveObserver(&closingSystem);
  common::DrawObserverSubject::RemoveObserver(&closingSystem);
}
