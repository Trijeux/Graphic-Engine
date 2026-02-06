#include "6-model_instance.h"

namespace graphics {
    void ModelInstance::Update(float dt) {
        if (rotates) {
            rotationAngle += dt; // Rotate based on time
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotationAngle, rotationAxis);
        model = glm::scale(model, scale);
    }
}