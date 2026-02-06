#include "FinalProject/point_shadow_map.h"

void PointShadowMap::Init() {
    glGenFramebuffers(1, &fbo);

    // 1. Create the Depth Buffer (RBO) reused for all renders
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);

    // 2. Generate Textures
    shadowCubemaps.resize(maxShadowLights);
    glGenTextures(maxShadowLights, shadowCubemaps.data());

    for (int i = 0; i < maxShadowLights; ++i) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemaps[i]);
        for (unsigned int face = 0; face < 6; ++face) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_R32F,
                         resolution, resolution, 0, GL_RED, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    // 3. Setup FBO (Just attach RBO for now, texture attached in BindForWriting)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    common::Shader sPointVert, sPointFrag;
    sPointVert.Load("data/shaders/FinalProject/shadow_point.vert", GL_VERTEX_SHADER);
    sPointFrag.Load("data/shaders/FinalProject/shadow_point.frag", GL_FRAGMENT_SHADER);
    pipeline.Load(sPointVert, sPointFrag);
}

void PointShadowMap::BindForWriting(int index, GLenum face) {
    if (index >= maxShadowLights) return;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Attach the specific face of the specific light's cubemap
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, shadowCubemaps[index], 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glViewport(0, 0, resolution, resolution);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void PointShadowMap::BindForReading(GLenum startTextureUnit) {
    for (int i = 0; i < maxShadowLights; ++i) {
        glActiveTexture(startTextureUnit + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemaps[i]);
    }
}

void PointShadowMap::Destroy() {
    if (!shadowCubemaps.empty()) {
        glDeleteTextures(static_cast<GLsizei>(shadowCubemaps.size()), shadowCubemaps.data());
        shadowCubemaps.clear();
    }
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &depthRBO);
}