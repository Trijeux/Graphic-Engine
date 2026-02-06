#include "FinalProject/g_buffer.h"

#include <iostream>

namespace Graphics {

    void GBuffer::Init(int width, int height) {
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        // 1. Position Buffer (High Precision)
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // 2. Normal Buffer (High Precision for smooth lighting)
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        // 3. Albedo + Specular Buffer (Standard Color)
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        // 4. Attachments
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // 5. Depth Buffer
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "GBuffer Not Complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Load Shaders
        common::Shader gVert, gFrag, lVert, lFrag;
        // Geometry Pass (Renders Models)
        gVert.Load("data/shaders/FinalProject/g_buffer.vert", GL_VERTEX_SHADER);
        gFrag.Load("data/shaders/FinalProject/g_buffer.frag", GL_FRAGMENT_SHADER);
        geometryPipeline.Load(gVert, gFrag);

        // Lighting Pass (Renders Quad)
        lVert.Load("data/shaders/FinalProject/screen.vert", GL_VERTEX_SHADER); // Re-use screen quad vert
        lFrag.Load("data/shaders/FinalProject/deferred.frag", GL_FRAGMENT_SHADER);
        lightingPipeline.Load(lVert, lFrag);
    }

    void  GBuffer::Bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GBuffer::Unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void GBuffer::BindTextures() const {
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    }

    void GBuffer::Resize(int width, int height) const {
        // 1. Resize Position Buffer
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

        // 2. Resize Normal Buffer
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

        // 3. Resize Albedo + Specular Buffer
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // 4. Resize Depth Renderbuffer
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }



    void GBuffer::Destroy() {
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedoSpec);
        glDeleteRenderbuffers(1, &rboDepth);

        // Reset IDs to 0 safety
        gBuffer = gPosition = gNormal = gAlbedoSpec = rboDepth = 0;
    }

}