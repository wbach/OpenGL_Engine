#pragma once
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
const int MAX_LIGHTS = 10;

class DefferedLighting : public PostprocessingRenderer
{
private:
    struct LightPass
    {
        AlignWrapper<vec4> skyColor;
        AlignWrapper<vec2> screenSize;
        AlignWrapper<float> viewDistance;
        AlignWrapper<int> numberOfLights;

        AlignWrapper<vec3> position_[MAX_LIGHTS];
        AlignWrapper<vec3> color_[MAX_LIGHTS];
        AlignWrapper<vec3> attenuation_[MAX_LIGHTS];
        AlignWrapper<vec3> direction[MAX_LIGHTS];
        AlignWrapper<float> cutOff_[MAX_LIGHTS];
        AlignWrapper<int> type_[MAX_LIGHTS];
    };

public:
    DefferedLighting(RendererContext&);
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;

private:
    void LoadLights(const Scene& scene);
    void PrepareApiStateToRender();
    void RetriveChanges();
    void Convert(const Light& light, int index);
    void bindShadowMapCascades() const;

private:
    ShaderProgram shader_;
    GraphicsApi::ID lightPassID_;
    LightPass lightPass_;
    bool lightsInGpu_ = false;
};
}  // namespace GameEngine
