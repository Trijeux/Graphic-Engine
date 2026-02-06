#include "renderer/shader.h"
#include "FinalProject/light_gizmo.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Graphics {
    void LightGizmo::Init() {
        // 1. Load Shaders
        common::Shader vs, fs;
        vs.Load("data/shaders/FinalProject/gizmo.vert", GL_VERTEX_SHADER);
        fs.Load("data/shaders/FinalProject/gizmo.frag", GL_FRAGMENT_SHADER);
        pipeline.Load(vs, fs);

        // 2. Setup Cube Data (Vertices only)
        float vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            // Front face
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            // Left face
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            // Right face
             0.5f,  0.5f,  0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,

            // Bottom face
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            // Top face
            -0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    void LightGizmo::Draw(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& position, const glm::vec3& color, float scale) {
        pipeline.Bind();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(scale));

        pipeline.SetMat4("view", glm::value_ptr(view));
        pipeline.SetMat4("projection", glm::value_ptr(proj));
        pipeline.SetMat4("model", glm::value_ptr(model));
        pipeline.SetVec3("objectColor", color.x, color.y, color.z);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    void LightGizmo::Destroy() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}
