#ifndef GPR924_COMPGRAPH_G_BUFFER_H
#define GPR924_COMPGRAPH_G_BUFFER_H

#include <GL/glew.h>

#include "renderer/pipeline.h"

namespace Graphics {
    class GBuffer {
      public:
        GLuint gBuffer = 0;
        GLuint gPosition = 0;
        GLuint gNormal = 0;
        GLuint gAlbedoSpec = 0;
        GLuint rboDepth = 0;

        common::Pipeline geometryPipeline = {};
        common::Pipeline lightingPipeline = {};

        void Init(int width, int height);
        void Resize(int width, int height) const;
        void Bind() const;

        void Unbind() const;
        void BindTextures() const;
        void Destroy();
    };
}
#endif //GPR924_COMPGRAPH_G_BUFFER_H
