#pragma once
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
constexpr int MAX_DIR_LIGHTS   = 2;
constexpr int MAX_POINT_LIGHTS = 64;
constexpr int MAX_SPOT_LIGHTS  = 32;

namespace Components
{
class DirectionalLightComponent;
class PointLightComponent;
class SpotLightComponent;
}  // namespace Components

class DefferedLighting : public PostprocessingRenderer
{
private:
    struct LightPass
    {
        AlignWrapper<vec4> skyColor;
        AlignWrapper<vec2> screenSize;
        AlignWrapper<float> viewDistance;
        AlignWrapper<vec3ui> numberOfLights;
    };

    struct DirectionalLights
    {
        AlignWrapper<vec3> direction[MAX_DIR_LIGHTS];
        AlignWrapper<vec4> color[MAX_DIR_LIGHTS];  // rgb, a-intensity
    };

    struct PointlLights
    {
        AlignWrapper<vec4> color[MAX_POINT_LIGHTS];   // rgb, a-intensity
        AlignWrapper<vec4> params[MAX_POINT_LIGHTS];  // x- intensity, y - range, z - falloffExponent
        AlignWrapper<vec4> worldPosition[MAX_POINT_LIGHTS]; // w - not used
    };

    struct SpotLights
    {
        AlignWrapper<vec4> color[MAX_SPOT_LIGHTS];   // rgb, a-intensity
        AlignWrapper<vec4> params[MAX_SPOT_LIGHTS];  // x- intensity, y - range, z - falloffExponent
        AlignWrapper<vec3> direction[MAX_SPOT_LIGHTS];
        AlignWrapper<vec4> cutoff[MAX_SPOT_LIGHTS];  // x - inter, y - outer
        AlignWrapper<vec4> worldPosition[MAX_POINT_LIGHTS]; // w - not used
    };

public:
    DefferedLighting(RendererContext&);
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;
    void CleanUp() override;

private:
    void LoadLights(const Scene& scene);
    void PrepareApiStateToRender();
    void RetriveChanges();
    void bindShadowMapCascades() const;
    void FillDirectionalLightsBuffer(const std::vector<Components::DirectionalLightComponent*>&);
    void FillPointLightsBuffer(const std::vector<Components::PointLightComponent*>&);
    void FillSpotLightsBuffer(const std::vector<Components::SpotLightComponent*>&);

private:
    ShaderProgram shader_;
    GraphicsApi::ID lightPassID_;
    LightPass lightPass_;

    GraphicsApi::ID directionalLightsId_;
    DirectionalLights directionalLights_;

    GraphicsApi::ID pointLightsId_;
    PointlLights pointLights_;

    GraphicsApi::ID spotLightsId_;
    SpotLights spotLights_;
    bool lightsInGpu_ = false;
};
}  // namespace GameEngine
