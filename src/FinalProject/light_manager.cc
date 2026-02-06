#include "FinalProject/light_manager.h"

#include <string>
#include <iostream>
#include <glm/gtc/constants.hpp>
#include <SDL3/SDL_timer.h>

#include "4-cube_3d.h"
#include "4-cube_3d.h"

namespace Graphics {
    void LightManager::AddPointLight(const PointLight &light) {
        if (pointLights.size() < MAX_POINT_LIGHTS) {
            pointLights.push_back(light);
        } else {
            std::cout << "Warning: Max Point Lights reached!" << std::endl;
        }
    }

    void LightManager::AddSpotLight(const SpotLight &light) {
        if (spotLights.size() < MAX_SPOT_LIGHTS) {
            spotLights.push_back(light);
        }
    }

    void LightManager::RemovePointLight(const int index) {
      if (index >= 0 && index < pointLights.size()) {
        pointLights.erase(pointLights.begin() + index);
      }
    }

    void LightManager::RemoveSpotLight(const int index) {
      if (index >= 0 && index < spotLights.size()) {
        spotLights.erase(spotLights.begin() + index);
      }
    }

    void LightManager::SetDirectionalLight(const DirectionalLight &light) {
        dirLight = light;
    }

    void LightManager::SetFlashLight(const SpotLight &light) {
        flashLight = light;
    }

    void LightManager::UpdateFlashLight(const glm::vec3 &pos, const glm::vec3 &dir) {
        flashLight.position = pos;
        flashLight.direction = dir;
    }

    void LightManager::ApplyLighting(common::Pipeline &pipeline) {
        // --- Directional Light ---
        pipeline.SetInt("hasDirLight", hasDirLight);
        if (hasDirLight) {
            pipeline.SetVec3("dirLight.direction", dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);
            pipeline.SetVec3("dirLight.color", dirLight.color.x, dirLight.color.y, dirLight.color.z);
            pipeline.SetFloat("dirLight.ambientStrength", dirLight.ambientStrength);
        }

        // --- Flash Light ---
        pipeline.SetInt("hasFlashLight", hasFlashLight);
        if (hasFlashLight) {
            pipeline.SetVec3("flashLight.position", flashLight.position.x, flashLight.position.y,
                             flashLight.position.z);
            pipeline.SetVec3("flashLight.direction", flashLight.direction.x, flashLight.direction.y,
                             flashLight.direction.z);
            pipeline.SetVec3("flashLight.color", flashLight.color.x, flashLight.color.y, flashLight.color.z);
            pipeline.SetFloat("flashLight.cutOff", flashLight.cutOff);
            pipeline.SetFloat("flashLight.outerCutOff", flashLight.outerCutOff);
            pipeline.SetFloat("flashLight.constant", flashLight.constant);
            pipeline.SetFloat("flashLight.linear", flashLight.linear);
            pipeline.SetFloat("flashLight.quadratic", flashLight.quadratic);
        }

        // --- Point Lights ---
        // Pass the actual count so the shader knows when to stop looping
        pipeline.SetInt("pointLightCount", (int) pointLights.size());
        for (size_t i = 0; i < pointLights.size(); i++) {
            std::string base = "pointLights[" + std::to_string(i) + "]";

            pipeline.SetVec3((base + ".position").c_str(), pointLights[i].position.x, pointLights[i].position.y,
                             pointLights[i].position.z);
            pipeline.SetVec3((base + ".color").c_str(), pointLights[i].color.x, pointLights[i].color.y,
                             pointLights[i].color.z);
            pipeline.SetFloat((base + ".constant").c_str(), pointLights[i].constant);
            pipeline.SetFloat((base + ".linear").c_str(), pointLights[i].linear);
            pipeline.SetFloat((base + ".quadratic").c_str(), pointLights[i].quadratic);
        }

        // --- Spot Lights ---
        pipeline.SetInt("spotLightCount", (int) spotLights.size());
        for (size_t i = 0; i < spotLights.size(); i++) {
            std::string base = "spotLights[" + std::to_string(i) + "]";

            pipeline.SetVec3((base + ".position").c_str(), spotLights[i].position.x, spotLights[i].position.y,
                             spotLights[i].position.z);
            pipeline.SetVec3((base + ".direction").c_str(), spotLights[i].direction.x, spotLights[i].direction.y,
                             spotLights[i].direction.z);
            pipeline.SetVec3((base + ".color").c_str(), spotLights[i].color.x, spotLights[i].color.y,
                             spotLights[i].color.z);
            pipeline.SetFloat((base + ".cutOff").c_str(), spotLights[i].cutOff);
            pipeline.SetFloat((base + ".outerCutOff").c_str(), spotLights[i].outerCutOff);
            pipeline.SetFloat((base + ".constant").c_str(), spotLights[i].constant);
            pipeline.SetFloat((base + ".linear").c_str(), spotLights[i].linear);
            pipeline.SetFloat((base + ".quadratic").c_str(), spotLights[i].quadratic);
        }
    }

    void LightManager::RotatesPointLightsAround(glm::vec3 rotationCenter, float radius_d) {
        // Rotate ALL point lights around the center (0,0,0)
        if (!pointLights.empty()) {
            float time = (float) SDL_GetTicks() / 1000.0f;
            float radius = 3.0f + radius_d; // Distance from center
            size_t count = pointLights.size();

            for (size_t i = 0; i < count; ++i) {
                auto &light = pointLights[i];

                float dir = (i % 2 == 0) ? -1.0f : 1.0f;

                float offset = i * (glm::two_pi<float>() / count);
                float angle = dir * (time + offset);

                light.position.x = rotationCenter.x + (sin(angle) * radius);
                light.position.z = rotationCenter.z + (cos(angle) * radius);
                light.position.y = rotationCenter.y + sin(time * 2.0f + offset);

                float r = (sin(time * 2.0f + offset) + 1.0f) * 0.5f;
                float g = (sin(time * 2.0f + offset + 2.0f) + 1.0f) * 0.5f;
                float b = (sin(time * 2.0f + offset + 4.0f) + 1.0f) * 0.5f;

                light.color = glm::vec3(r, g, b);
            }
        }
    }

    void LightManager::Clear() {
        pointLights.clear();
        spotLights.clear();
        hasDirLight = false;
        hasFlashLight = false;
    }
}
