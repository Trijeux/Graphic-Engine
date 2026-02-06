#ifndef GPR924_COMPGRAPH_LIGHT_MANAGER_H
#define GPR924_COMPGRAPH_LIGHT_MANAGER_H

#include <vector>

#include "FinalProject/light.h"
#include "renderer/pipeline.h"

namespace Graphics {

    const int MAX_POINT_LIGHTS = 50;
    const int MAX_SPOT_LIGHTS = 50;

    class LightManager {
    public:
        DirectionalLight dirLight;
        bool hasDirLight = true;

        SpotLight flashLight;
        bool hasFlashLight = false;

        std::vector<PointLight> pointLights;
        std::vector<SpotLight>  spotLights;

        void AddPointLight(const PointLight& light);
        void AddSpotLight(const SpotLight& light);
        void RemovePointLight(int index);
        void RemoveSpotLight(int index);
        void SetDirectionalLight(const DirectionalLight& light);

        void SetFlashLight(const SpotLight& light);
        void UpdateFlashLight(const glm::vec3& pos, const glm::vec3& dir);

        void RotatesPointLightsAround(glm::vec3 rotationCenter, float radius_d = 0);

        void ApplyLighting(common::Pipeline& pipeline);

        void Clear();
    };
}
#endif //GPR924_COMPGRAPH_LIGHT_MANAGER_H
