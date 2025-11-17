#include "DefferedLighting.h"

#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
namespace
{
const uint32 CASCADE_INDEX0 = 0;
const uint32 CASCADE_INDEX1 = 1;
const uint32 CASCADE_INDEX2 = 2;
const uint32 CASCADE_INDEX3 = 3;
}  // namespace
DefferedLighting::DefferedLighting(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Deffered)
{
}
void DefferedLighting::Init()
{
    shader_.Init();

    lightPass_.skyColor     = vec4(rendererContext_.fogColor_, 1.f);
    lightPass_.viewDistance = rendererContext_.projection_.GetViewDistance();
    lightPass_.screenSize =
        vec2(rendererContext_.projection_.GetRenderingSize().x, rendererContext_.projection_.GetRenderingSize().y);

    if (not lightPassID_)
    {
        lightPassID_ =
            rendererContext_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(DefferedLighting::LightPass));
    }
}
void DefferedLighting::Prepare()
{
}
void DefferedLighting::Render(const Scene& scene)
{
    PrepareApiStateToRender();
    shader_.Start();
    LoadLights(scene);
    rendererContext_.graphicsApi_.BindShaderBuffer(*lightPassID_);

    bindShadowMapCascades();

    rendererContext_.graphicsApi_.RenderQuad();
    RetriveChanges();
}

void DefferedLighting::bindShadowMapCascades() const
{
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        rendererContext_.graphicsApi_.ActiveTexture(5 + cascadeIndex);
        auto shadomapIndex = magic_enum::enum_index(SharedTextures::shadowCascade0).value() + cascadeIndex;
        auto shadowmapId   = rendererContext_.sharedTextures[shadomapIndex];
        if (shadowmapId)
        {
            rendererContext_.graphicsApi_.BindTexture(*shadowmapId);
        }
    }
}

void DefferedLighting::ReloadShaders()
{
    shader_.Reload();
}
void DefferedLighting::LoadLights(const Scene& scene)
{
    lightPass_.numberOfLights = static_cast<int>(scene.GetLights().size() + 1);
    Convert(scene.GetDirectionalLight(), 0);

    int i = 1;
    for (const auto& light : scene.GetLights())
    {
        Convert(light, i++);
    }

    rendererContext_.graphicsApi_.UpdateShaderBuffer(*lightPassID_, &lightPass_);
    lightsInGpu_ = true;
}
void DefferedLighting::PrepareApiStateToRender()
{
    rendererContext_.graphicsApi_.PolygonModeRender();
    rendererContext_.graphicsApi_.DisableDepthMask();
    rendererContext_.graphicsApi_.DisableDepthTest();
    //  rendererContext_.graphicsApi_.EnableBlend();
}
void DefferedLighting::RetriveChanges()
{
    rendererContext_.graphicsApi_.EnableDepthMask();
    rendererContext_.graphicsApi_.EnableDepthTest();
    // rendererContext_.graphicsApi_.DisableBlend();
}
void DefferedLighting::Convert(const Light& light, int index)
{
    lightPass_.attenuation_[index] = light.GetAttenuation();
    lightPass_.color_[index]       = light.GetColour();
    lightPass_.cutOff_[index]      = light.GetCutoff();
    lightPass_.position_[index]    = light.GetPosition();
    lightPass_.type_[index]        = static_cast<int>(light.GetType());
}
}  // namespace GameEngine
