#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "Shaders/DefferedShader.h"

namespace GameEngine
{
const int MAX_LIGHTS = 100;

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
        AlignWrapper<float> cutOff_[MAX_LIGHTS];
        AlignWrapper<int> type_[MAX_LIGHTS];
    };

public:
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    void LoadLights(const Scene& scene);
    void PrepareApiStateToRender();
    void RetriveChanges();
    void convert(const Light& light, int index);

private:
    std::unique_ptr<DefferedShader> shader_;
    GraphicsApi::ID lightPassID_;
    LightPass lightPass_;
    bool lightsInGpu_ = false;
};
}  // namespace GameEngine
