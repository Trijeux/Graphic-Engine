#ifndef GPR924_COMPGRAPH_MODEL_INSTANCE_H
#define GPR924_COMPGRAPH_MODEL_INSTANCE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Graphics {
    class ModelTransform {
    public:
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 position{0, 0, 0};
        glm::vec3 scale{1, 1, 1};
        glm::vec3 rotation_axis{0, 1, 0};
        float rotation_angle = 0.0f;
        float opacity = 1.0f;

        bool is_rotating = false;

        void Update(float dt);
    };
}
#endif //GPR924_COMPGRAPH_MODEL_INSTANCE_H
