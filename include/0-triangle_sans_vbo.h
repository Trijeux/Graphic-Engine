#ifndef GPR924_COMPGRAPH_TRIANGLE_SANS_VBO_H
#define GPR924_COMPGRAPH_TRIANGLE_SANS_VBO_H
#include "third_party/gl_include.h"

#include "engine/renderer.h"
#include "engine/system.h"
#include "renderer/pipeline.h"

namespace graphics {
class TriangleSansVbo final : public common::SystemInterface,
                              public common::DrawInterface {
  common::Pipeline pipeline;
  GLuint vao = 0;
public:
  void Begin() override;

  void End() override;

  void Update(float dt) override;

  void FixedUpdate() override;

  void Draw() override;
};
}

#endif //GPR924_COMPGRAPH_TRIANGLE_SANS_VBO_H