#ifndef POINT_SHADOW_MAP_H
#define POINT_SHADOW_MAP_H

#include <GL/glew.h>
#include <vector>

#include "renderer/pipeline.h"

class PointShadowMap {
public:
    GLuint fbo = 0;
    std::vector<GLuint> shadowCubemaps; // Changed from single GLuint to vector
    GLuint depthRBO = 0;
    common::Pipeline pipeline;

    int resolution = 1024;
    int maxShadowLights = 4; // How many lights cast shadows

    void Init(); // Default to 4 shadow casters

    // index: Which light's map we are writing to (0 to maxShadowLights-1)
    // face: Which cube face (e.g., GL_TEXTURE_CUBE_MAP_POSITIVE_X)
    void BindForWriting(int index, GLenum face);

    // startTextureUnit: Where to start binding (e.g., GL_TEXTURE5)
    void BindForReading(GLenum startTextureUnit);

    void Destroy();
};

#endif