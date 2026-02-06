#ifndef GPR924_COMPGRAPH_SQUARE_AVEC_EBO_H
#define GPR924_COMPGRAPH_SQUARE_AVEC_EBO_H
#include "third_party/gl_include.h"

#include "engine/renderer.h"
#include "engine/system.h"
#include "renderer/pipeline.h"

namespace graphics {
class SquareAvecEbo final : public common::SystemInterface,
                            public common::DrawInterface {
  common::Pipeline pipeline;
  GLuint           vao = 0, vbo = 0, ebo = 0;

public:
  void Begin() override;

  void End() override;

  void Update(float dt) override;

  void FixedUpdate() override;

  void Draw() override;
};
}

#endif //GPR924_COMPGRAPH_SQUARE_AVEC_EBO_H