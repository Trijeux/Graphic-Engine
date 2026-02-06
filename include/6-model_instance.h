#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics {
    class ModelInstance {
    public:
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 position{0, 0, 0};
        glm::vec3 scale{1, 1, 1};
        glm::vec3 rotationAxis{0, 1, 0};
        float rotationAngle = 0.0f;
        float opacity = 1.0f;

        bool rotates = false;

        void Update(float dt);
    };
}
#endif //MODEL_INSTANCE_H
