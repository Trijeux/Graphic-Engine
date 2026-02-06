#ifndef GPR924_COMPGRAPH_SKYBOX_H
#define GPR924_COMPGRAPH_SKYBOX_H

#include <span>

#include "renderer/pipeline.h"

#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Graphics {
    class Skybox {
        GLuint VAO = 0, VBO = 0;
        GLuint textureID = 0;
        common::Pipeline pipeline;

    public:
        // Pass a list of 6 file paths in this specific order:
        // Right, Left, Top, Bottom, Front, Back
        void Init();

        void Draw(const glm::mat4& view, const glm::mat4& proj);

        void Destroy();

    private:
        GLuint LoadCubemap(std::span<const std::string_view, 6> faces);
    };
}

#endif //GPR924_COMPGRAPH_SKYBOX_H
