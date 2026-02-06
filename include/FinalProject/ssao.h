#ifndef GPR924_COMPGRAPH_SSAO_H
#define GPR924_COMPGRAPH_SSAO_H

#include <GL/glew.h>
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include "renderer/pipeline.h"


namespace Graphics {
    class SSAO {
    public:
        GLuint ssaoFbo = 0, ssaoBlurFbo = 0;
        GLuint ssaoColorBuffer = 0, ssaoBlurBuffer = 0;
        GLuint noiseTexture = 0;

        std::vector<glm::vec3> ssaoKernel;
        common::Pipeline ssaoPipeline;
        common::Pipeline blurPipeline;

        void Init(int width, int height);
        void Render(GLuint gPosition, GLuint gNormal, const glm::mat4& projection, const glm::mat4& view, GLuint quadVAO, float radius, float bias);
        void Resize(int width, int height);
        void Destroy();

    private:
        void GenerateKernel();
        void GenerateNoise();
    };
}

#endif //GPR924_COMPGRAPH_SSAO_H
