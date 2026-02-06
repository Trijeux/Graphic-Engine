#ifndef GPR924_COMPGRAPH_SHADOW_MAP_H
#define GPR924_COMPGRAPH_SHADOW_MAP_H

#include <GL/glew.h>

class ShadowMap {
public:
    GLuint fbo = 0;
    GLuint shadowMap = 0;
    int width = 4096;
    int height = 4096;

    void Init();

    void BindForWriting();

    void BindForReading(GLenum textureUnit);
};

#endif //GPR924_COMPGRAPH_SHADOW_MAP_H
