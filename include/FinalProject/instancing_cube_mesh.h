#ifndef GPR924_COMPGRAPH_INSTANCING_CUBE_MESH_H
#define GPR924_COMPGRAPH_INSTANCING_CUBE_MESH_H

#include <GL/glew.h>

#include <glm/vec3.hpp>

#include "renderer/pipeline.h"

namespace Graphics {
    class InstancingCubeMesh {
        GLuint vao = 0, vbo = 0, ebo = 0, texture = 0, specular_texture = 0, normal_texture = 0;

    public:

        // Culling Data (Unit Cube centered at 0)
        glm::vec3 boundingCenter = glm::vec3(0.0f);
        float boundingRadius = 0.866f; // sqrt(0.5^2 + 0.5^2 + 0.5^2)

        [[nodiscard]] GLuint get_texture() const { return texture; }
        [[nodiscard]] GLuint get_specular_texture() const { return specular_texture; }
        [[nodiscard]] GLuint get_normal_texture() const { return normal_texture; }


        void Init();
        void Destroy() const;

        // Link the external VBO containing matrices to this mesh's VAO
        void SetupInstancing(GLuint instanceVBO);

        // Draw multiple instances
        void DrawInstanced(common::Pipeline &pipeline, int count);
    };
}

#endif //GPR924_COMPGRAPH_INSTANCING_CUBE_MESH_H
