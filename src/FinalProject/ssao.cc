#include "FinalProject/ssao.h"
#include <glm/gtc/type_ptr.hpp>

namespace Graphics {
    float lerp(float a, float b, float f) { return a + f * (b - a); }

    void SSAO::Init(int width, int height) {
        // 1. Setup Framebuffers
        glGenFramebuffers(1, &ssaoFbo);
        glGenFramebuffers(1, &ssaoBlurFbo);

        // SSAO Color Buffer
        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

        // Blur Color Buffer
        glGenTextures(1, &ssaoBlurBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurBuffer, 0);

        GenerateKernel();
        GenerateNoise();

        // Load Shaders
        common::Shader v, fS, fB;
        v.Load("data/shaders/FinalProject/screen.vert", GL_VERTEX_SHADER);
        fS.Load("data/shaders/FinalProject/ssao.frag", GL_FRAGMENT_SHADER);
        fB.Load("data/shaders/FinalProject/ssao_blur.frag", GL_FRAGMENT_SHADER);
        ssaoPipeline.Load(v, fS);
        blurPipeline.Load(v, fB);
    }

    void SSAO::GenerateKernel() {
        std::uniform_real_distribution<float> randomValues(0.0, 1.0);
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 64; ++i) {
            glm::vec3 sample(randomValues(generator) * 2.0 - 1.0,
                             randomValues(generator) * 2.0 - 1.0,
                             randomValues(generator));
            sample = glm::normalize(sample);
            sample *= randomValues(generator);
            float scale = (float)i / 64.0f;
            scale = lerp(0.1f, 1.0f, scale * scale);
            ssaoKernel.push_back(sample * scale);
        }
    }

    void SSAO::GenerateNoise() {
        std::uniform_real_distribution<float> randomValues(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++) {
            glm::vec3 noise(randomValues(generator) * 2.0 - 1.0,
                            randomValues(generator) * 2.0 - 1.0, 0.0f);
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void SSAO::Render(GLuint gPosition, GLuint gNormal, const glm::mat4& projection, const glm::mat4& view, GLuint quadVAO, float radius, float bias) {
        // 1. SSAO Pass
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFbo);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoPipeline.Bind();

        for (unsigned int i = 0; i < 64; ++i) {
            ssaoPipeline.SetVec3(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i].x, ssaoKernel[i].y, ssaoKernel[i].z);
        }
        ssaoPipeline.SetMat4("view", glm::value_ptr(view));
        ssaoPipeline.SetMat4("projection", glm::value_ptr(projection));
        ssaoPipeline.SetFloat("radius", radius);
        ssaoPipeline.SetFloat("bias", bias);

        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, noiseTexture);
        ssaoPipeline.SetInt("gPosition", 0);
        ssaoPipeline.SetInt("gNormal", 1);
        ssaoPipeline.SetInt("texNoise", 2);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 2. Blur Pass
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFbo);
        glClear(GL_COLOR_BUFFER_BIT);
        blurPipeline.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        blurPipeline.SetInt("ssaoInput", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO::Resize(int width, int height) {
        // Resize SSAO Buffer
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);

        // Resize Blur Buffer
        glBindTexture(GL_TEXTURE_2D, ssaoBlurBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void SSAO::Destroy() {
        glDeleteFramebuffers(1, &ssaoFbo);
        glDeleteFramebuffers(1, &ssaoBlurFbo);

        glDeleteTextures(1, &ssaoColorBuffer);
        glDeleteTextures(1, &ssaoBlurBuffer);
        glDeleteTextures(1, &noiseTexture);

        // Reset IDs
        ssaoFbo = ssaoBlurFbo = 0;
        ssaoColorBuffer = ssaoBlurBuffer = noiseTexture = 0;
    }
}