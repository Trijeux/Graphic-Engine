#ifndef GPR924_COMPGRAPH_CUBE_INSTANCE_H
#define GPR924_COMPGRAPH_CUBE_INSTANCE_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/pipeline.h"

class CubeTransform {

public:
    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 position{0, 0, 0};
    glm::vec3 scale{1, 1, 1};
    float rotationAngle = 0;
    float opacity = 1.0f;

    bool rotates = false;

    void Update(float dt);

    void Draw(common::Pipeline &pipeline);
};
#endif //GPR924_COMPGRAPH_CUBE_INSTANCE_H
