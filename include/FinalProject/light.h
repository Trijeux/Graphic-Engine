#ifndef GPR924_COMPGRAPH_LIGHT_H
#define GPR924_COMPGRAPH_LIGHT_H

#include <glm/glm.hpp>

namespace Graphics {
    struct DirectionalLight {
        DirectionalLight(glm::vec3 direction_, glm::vec3 color_) : direction(direction_),
                                                                   color(color_) {
        }
        DirectionalLight() = default;

        glm::vec3 direction = {0, 0, 0};
        glm::vec3 color = {0, 0, 0};
        float ambientStrength = 0.05f;
    };

    struct PointLight {
        PointLight(glm::vec3 position_, glm::vec3 color_) : position(position_),
                                                            color(color_) {
        }
        PointLight() = default;

        glm::vec3 position = {0, 0, 0};
        glm::vec3 color = {0, 0, 0};

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };

    struct SpotLight {
        SpotLight(glm::vec3 position_, glm::vec3 direction_, glm::vec3 color_) : position(position_),
            direction(direction_), color(color_) {
        }
        SpotLight() = default;

        glm::vec3 position = {0, 0, 0};
        glm::vec3 direction = {0, 0, 0};
        glm::vec3 color = {0, 0, 0};

        float cutOff = glm::cos(glm::radians(12.5f));
        float outerCutOff = glm::cos(glm::radians(17.5f));

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };
}
#endif //GPR924_COMPGRAPH_LIGHT_H
