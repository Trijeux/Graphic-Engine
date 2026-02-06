#include "FinalProject/cube_transform.h"

#include "renderer/pipeline.h"

void CubeTransform::Update(float dt) {
    if (rotates)
        rotationAngle += 0.01f;
    model = glm::mat4(1);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotationAngle, glm::vec3(0, 1, 0));
    model = glm::scale(model, scale);
}

void CubeTransform::Draw(common::Pipeline &pipeline) {
    pipeline.SetMat4("model", glm::value_ptr(model));
    pipeline.SetFloat("opacity", opacity);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}