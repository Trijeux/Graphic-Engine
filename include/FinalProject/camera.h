#ifndef GPR924_COMPGRAPH_CAMERA_H
#define GPR924_COMPGRAPH_CAMERA_H

#include <array>
#include <glm/glm.hpp>

namespace Graphics {
    struct Plane {
        glm::vec3 normal;
        float distance;

        void Normalize() {
            float length = glm::length(normal);
            normal /= length;
            distance /= length;
        }
    };

    class Camera {
        bool isMouseCaptured = false;

        float yaw = -90.0f;
        float pitch = 0.0f;

        std::array<Plane, 6> planes = {};

    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;

        float moveSpeed = 5.0f;
        float moveSpeedMult = 5.0f;
        float mouseSensitivity = 0.1f;
        float Fov = 45.f;

        void Init(glm::vec3 pos = {0, 0, 0}, glm::vec3 front = {0, 0, -1}, glm::vec3 up = {0, 1, 0},
                  float fov = 45);

        void Update(float dt, glm::mat4& viewProj);
        [[nodiscard]] bool IsOnFrustum(const glm::vec3& center, float radius) const;
    };
}

#endif //GPR924_COMPGRAPH_CAMERA_H
