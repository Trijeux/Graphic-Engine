#ifndef GPR924_COMPGRAPH_LIGHT_GIZMO_H
#define GPR924_COMPGRAPH_LIGHT_GIZMO_H

#include "renderer/pipeline.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Graphics {
    class LightGizmo {
        GLuint VAO = 0, VBO = 0;
        common::Pipeline pipeline;

    public:
        void Init();
        void Draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& position, const glm::vec3& color, float scale);
        void Destroy();
    };
}
#endif //GPR924_COMPGRAPH_LIGHT_GIZMO_H