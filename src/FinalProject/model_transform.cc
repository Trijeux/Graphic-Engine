#include "FinalProject/model_transform.h"

namespace Graphics {
    void ModelTransform::Update(float dt) {
        if (is_rotating) {
            rotation_angle += dt; // Rotate based on time
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation_angle, rotation_axis);
        model = glm::scale(model, scale);
    }
}