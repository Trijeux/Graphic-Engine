#include "FinalProject/camera.h"

#include <imgui.h>

namespace Graphics {
    void Camera::Init(glm::vec3 pos, glm::vec3 front, glm::vec3 up,
                      float fov){
        Position = pos;
        Front = front;
        Up = up;
        Fov = fov;
    }

    void Camera::Update(float dt, glm::mat4& viewProj) {
        // --- Camera Movement ---
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            ImGuiIO &io = ImGui::GetIO();
            if (io.MouseDelta.x != 0 || io.MouseDelta.y != 0) {
                float offsetX = io.MouseDelta.x * mouseSensitivity;
                float offsetY = io.MouseDelta.y * mouseSensitivity;

                yaw += offsetX;
                pitch -= offsetY;

                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;

                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                Front = glm::normalize(front);
            }
        }

        float velocity;
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
            velocity = moveSpeed * moveSpeedMult * dt;
        else
            velocity = moveSpeed * dt;

        if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_Z)) Position += Front * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_S)) Position -= Front * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_Q)) Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_D)) Position += glm::normalize(glm::cross(Front, Up)) * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_Space)) Position += Up * velocity;
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) Position -= Up * velocity;

        // --- Frustrum ---
        // Gribb-Hartmann Method to extract planes

        // Left Plane
        planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
        planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
        planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
        planes[0].distance = viewProj[3][3] + viewProj[3][0];

        // Right Plane
        planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
        planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
        planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
        planes[1].distance = viewProj[3][3] - viewProj[3][0];

        // Bottom Plane
        planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
        planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
        planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
        planes[2].distance = viewProj[3][3] + viewProj[3][1];

        // Top Plane
        planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
        planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
        planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
        planes[3].distance = viewProj[3][3] - viewProj[3][1];

        // Near Plane
        planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
        planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
        planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
        planes[4].distance = viewProj[3][3] + viewProj[3][2];

        // Far Plane
        planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
        planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
        planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
        planes[5].distance = viewProj[3][3] - viewProj[3][2];

        for (auto& plane : planes) {
            plane.Normalize();
        }
    }

    [[nodiscard]] bool Camera::IsOnFrustum(const glm::vec3& center, float radius) const {
        for (const auto& plane : planes) {
            // Calculate distance from center to plane
            // If distance < -radius, the sphere is completely behind this plane
            if (glm::dot(plane.normal, center) + plane.distance < -radius) {
                return false;
            }
        }
        return true;
    }
}
