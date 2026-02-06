#ifndef GPR924_COMPGRAPH_FRAME_BUFFER_H
#define GPR924_COMPGRAPH_FRAME_BUFFER_H

#include <GL/glew.h>
#include "renderer/pipeline.h"

namespace Graphics {
    class Framebuffer {
        GLuint colorBuffers[2] = {}; // 0=Scene, 1=Brightness
        GLuint rbo = 0;

        GLuint pingPongFBO[2] = {};
        GLuint pingPongColorBuffers[2] = {};

        common::Pipeline screenPipeline;
        common::Pipeline blurPipeline;

    public:
        GLuint fbo = 0;
        GLuint quadVAO = 0, quadVBO = 0;

        int postProcessMode = 0; // 0=None, 1=Inv, 2=Gray, 3=Sharp, 4=Blur, 5=Edge
        bool useBloom = true;
        float exposure = 1.0f;

        void Init();
        void Resize(int width, int height);

        void Bind();
        void Unbind();

        void Draw();
        void Destroy();
    };
}
#endif //GPR924_COMPGRAPH_FRAME_BUFFER_H
