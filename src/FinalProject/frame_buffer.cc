#include "FinalProject/frame_buffer.h"
#include "engine/window.h"

#include <iostream>

namespace Graphics {

    void Framebuffer::Init() {
        //Get Window Size
        auto size = common::GetWindowSize();
        int width = size.x;
        int height = size.y;

        // Setup Framebuffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Create Texture Attachment (Color)
        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            // Use GL_RGBA16F for HDR precision
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        // Create Renderbuffer Object (Depth & Stencil)
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // Explicitly tell OpenGL we are drawing to TWO color buffers
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        // --- 2. Ping-Pong Buffers (Blur) ---
        glGenFramebuffers(2, pingPongFBO);
        glGenTextures(2, pingPongColorBuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingPongColorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongColorBuffers[i], 0);
        }

        // Check status
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Setup Screen Quad (VAO/VBO)
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // Setup Screen Shader
        common::Shader vs, fs, bfs;
        vs.Load("data/shaders/FinalProject/screen.vert", GL_VERTEX_SHADER);
        fs.Load("data/shaders/FinalProject/screen.frag", GL_FRAGMENT_SHADER);
        bfs.Load("data/shaders/FinalProject/blur.frag", GL_FRAGMENT_SHADER);

        screenPipeline.Load(vs, fs);
        blurPipeline.Load(vs, bfs);
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // Important: Ensure we are drawing to BOTH attachments
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        glEnable(GL_DEPTH_TEST); // Enable depth testing for the scene
    }

    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // Disable depth test so quad is always drawn on top
    }

    void Framebuffer::Draw() {
        bool horizontal = true, first_iteration = true;

        // 1. Blur Pass (Only if Bloom is enabled)
        if (useBloom) {
            unsigned int amount = 10;
            blurPipeline.Bind();
            blurPipeline.SetInt("image", 0);
            for (unsigned int i = 0; i < amount; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);
                blurPipeline.SetInt("horizontal", horizontal);

                glActiveTexture(GL_TEXTURE0);
                // First iteration: Grab the Brightness Texture from Main FBO
                // Next iterations: Grab from the other PingPong FBO
                glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingPongColorBuffers[!horizontal]);

                glBindVertexArray(quadVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                horizontal = !horizontal;
                if (first_iteration) first_iteration = false;
            }
        }

        // 2. Final Combine Pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screenPipeline.Bind();
        screenPipeline.SetInt("scene", 0);
        screenPipeline.SetInt("bloomBlur", 1);
        screenPipeline.SetInt("debugRawBrightness", 2);
        screenPipeline.SetInt("bloom", useBloom);
        screenPipeline.SetFloat("exposure", exposure);
        screenPipeline.SetInt("mode", postProcessMode);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]); // Original Scene

        glActiveTexture(GL_TEXTURE1);
        // If bloom ran, use the last ping-pong buffer. If not, doesn't matter (shader ignores it)
        glBindTexture(GL_TEXTURE_2D, pingPongColorBuffers[!horizontal]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[1]); // Unit 2 = Raw Brightness

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Framebuffer::Resize(int width, int height) {
        // Regenerate texture and RBO on resize
        for(int i=0; i<2; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, pingPongColorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }

    void Framebuffer::Destroy() {
        glDeleteFramebuffers(1, &fbo);
        glDeleteFramebuffers(2, pingPongFBO);
        glDeleteRenderbuffers(1, &rbo);
        glDeleteTextures(1, colorBuffers);
        glDeleteTextures(2, pingPongColorBuffers);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}
