#include "FinalProject/shadow_map.h"

void ShadowMap::Init() {
    glGenFramebuffers(1, &fbo);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Clamp to border to fix "dark box" outside light area
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    // We only need depth, no color
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::BindForWriting() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::BindForReading(GLenum textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}