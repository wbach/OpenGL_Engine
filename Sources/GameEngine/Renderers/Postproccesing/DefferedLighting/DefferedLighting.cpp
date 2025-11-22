#include "DefferedLighting.h"

#include "GameEngine/Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
DefferedLighting::DefferedLighting(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Deffered)
{
}
void DefferedLighting::Init()
{
    shader_.Init();

    const auto& projection  = rendererContext_.camera_->GetProjection();
    lightPass_.skyColor     = vec4(rendererContext_.fogColor_, 1.f);
    lightPass_.viewDistance = projection.GetViewDistance();
    lightPass_.screenSize   = vec2(projection.GetRenderingSize().x, projection.GetRenderingSize().y);

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

    const uint32 uniformSkyTextureIndex = 5;
    auto skyTextureId = rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::skyTexture).value()];
    if (skyTextureId)
    {
        rendererContext_.graphicsApi_.ActiveTexture(uniformSkyTextureIndex, *skyTextureId);
    }
    bindShadowMapCascades();

    rendererContext_.graphicsApi_.RenderQuad();
    RetriveChanges();
}

void DefferedLighting::bindShadowMapCascades() const
{
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        const uint32 uniformShadowMapIndex = 6;
        rendererContext_.graphicsApi_.ActiveTexture(uniformShadowMapIndex + cascadeIndex);
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
    lightPass_.direction[index]    = light.GetDirection();
    lightPass_.type_[index]        = static_cast<int>(light.GetType());
}
}  // namespace GameEngine
